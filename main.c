#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// (˶>⩊<˶)

enum TokenType {
    UNKNOWN,
    //keywords
    PRINT_TOK,
    ADD,
    MULT,
    //literals
    STRING_LITERAL,
    NUMBER,
};


struct Token {
    enum TokenType kind;
    int len;
    char *start;
} Token;

struct Parser {const struct Token *token; int n; int pos;} Parser;

// LEXER

static const char *tokenName(enum TokenType k) {
    switch (k) {
        case PRINT_TOK: return "PRINT_TOK";
        case STRING_LITERAL: return "STRING_LITERAL";
        case ADD: return "ADD";
        case MULT: return "MULT";
        case NUMBER: return "NUMBER";
        case UNKNOWN:
            break;
    }
    return "???";
}

void printTokens(const struct Token *toks, int n) {
    for (int i = 0; i < n; i++) {
        printf("%-15s len=%d     start=%p\n", tokenName(toks[i].kind), toks[i].len, toks[i].start);
    }
}

static enum TokenType findCmd(const char *keyword) {
    if (strcasecmp("PRINT", keyword) == 0) {
        return PRINT_TOK;
    };
    return UNKNOWN;
}

int lexer(char *output, int len, struct Token **lex, int *numtokens) {
    int i = 0;
    int n = 0;

    struct Token *token = malloc((len+1)* sizeof *token);

    while (i < len) {
        if (isspace(output[i])) {
            i++;
            continue;
        }
        if (i < len && output[i] == '"') {
            int size = 0;
            i++;
            token[n].start = &output[i];
            while (i < len && output[i] != '"') {
                i++;
                size++;
            }
            token[n].kind = STRING_LITERAL;
            token[n].len = size;
            n++;
            i++;
        }
        else if (isalpha(output[i])) {
            int size = 0;
            int j = 0;
            char buffer[20];
            token[n].start = &output[i];
            while (i < len && output[i] != ' ') {
                buffer[j] = output[i];
                j++;
                i++;
                size++;
            }
            buffer[j] = '\0';
            token[n].kind = findCmd(buffer);
            token[n].len = size;
            n++;
        }
        else if (isdigit(output[i])) {
            token[n].kind = NUMBER;
            token[n].len = 1;
            token[n].start = &output[i];
            n++;
            i++;
        }
        else if (output[i] == '*') {
            token[n].kind = MULT;
            token[n].len = 1;
            token[n].start = &output[i];
            n++;
            i++;
        }
        else if (output[i] == '+') {
            token[n].kind = ADD;
            token[n].len = 1;
            token[n].start = &output[i];
            n++;
            i++;
        }
    }

    *lex = token;
    *numtokens = n;

    return 0;
}

int printArray(int size, char* array) {
    for (int i = 0; i < size; i++) {
        printf("%c", array[i]);
    }
    printf("\n");
    return 0;
}

int parseFile(const char *file,char **output,int *len) {
    FILE* fptr = fopen(file, "r");
    if (fptr == NULL) {
        perror("Could not open File");
        return 1;
    }
    int seek = fseek(fptr, 0, SEEK_END);
    if (seek == 1) {
        perror("Seek failed");
        fclose(fptr);
        return -1;
    }

    long size = ftell(fptr);
    char *buffer = malloc(size * sizeof(char));

    fseek(fptr, 0, SEEK_SET);

    size_t newLen = fread(buffer, sizeof(char), size, fptr);
        if ( ferror( fptr ) != 0 ) {
            perror("Error reading file");
        }
    buffer[size] = '\n';
    *output = buffer;
    *len = (int)newLen;
    fclose(fptr);
    return 0;
}

// PARSER

//program   => statement* EOF
//statement => PRINT expr
//expr      => term ("+" term)*
//term      => factor ("*" factor)*
//factor    => NUMBER | STRING
//factor    => NUMBER | STRING

struct Token peek(struct Parser *parser) {
    return parser->token[parser->pos];
}

struct Token advance(struct Parser *parser) {
    parser->pos++;
    return parser->token[parser->pos];
}

int match(enum TokenType type, struct Parser *parser) {
    if (type != parser->token[parser->pos].kind) return 0;
    parser->pos++;
    return 1;
};

int expect(enum TokenType type, struct Parser *parser) {
    if (match(type, parser)) return 1;

    fprintf(stderr, "Syntaxfehler: erwartet %s, gefunden %s\n",
            tokenName(type),
            tokenName(parser->token[parser->pos].kind));
    return 0;
};

// MAIN :3
int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("No File given");
        return 1;
    }
    int len;
    char *parse;
    struct Token *lex;
    int numtokens;

    parseFile(argv[1],&parse,&len);
    printArray(len, parse);

    lexer(parse, len, &lex, &numtokens);

    printTokens(lex, numtokens);
    free(parse);
    return 0;
}