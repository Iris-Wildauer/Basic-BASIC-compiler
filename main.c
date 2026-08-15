#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// (˶>⩊<˶)

enum TokenType {
    EOF_TOK,
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
        case EOF_TOK: return "EOF";
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
            int start = i;
            while (isdigit(output[i])) i++;
            token[n].kind  = NUMBER;
            token[n].start = &output[start];
            token[n].len   = i - start;
            n++;
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

    token[n].kind = EOF_TOK;
    token[n].len = 0;
    token[n].start = &output[len];
    n++;
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

typedef struct Expr Expr;
struct Expr {
    enum {NUM, STRING, OPERATOR} kind;
    union {
        int Number;
        struct {const char *start; int len;} String;
        struct {enum TokenType operation; Expr *left, *right;} Operator;
    } as ;
};

static Expr *newNumber(const int num) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = NUM;
    e->as.Number = num;

    return e;
}

static Expr *newString(const char *start, int len) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = STRING;
    e->as.String.start = start;
    e->as.String.len = len;

    return e;
}

static Expr *newOp(enum TokenType op, Expr *left, Expr *right) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = OPERATOR;
    e->as.Operator.left = left;
    e->as.Operator.right = right;
    e->as.Operator.operation = op;

    return e;
}

struct Token peek(struct Parser *parser) {
    return parser->token[parser->pos];
}

struct Token advance(struct Parser *parser) {
    return parser->token[parser->pos++];
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


static Expr *parseExpr(struct Parser *parser);

static Expr *parseStatement(struct Parser *parser) {
    if (match(PRINT_TOK, parser)) {
        return parseExpr(parser);
    }

    struct Token t = peek(parser);
    fprintf(stderr, "Syntaxfehler: %s ('%.*s')\n",
            tokenName(t.kind), t.len, t.start);
    return NULL;
}

static Expr *parseFactor(struct Parser *parser) {
    struct Token t = peek(parser);

    if (t.kind == NUMBER) {
        advance(parser);
        return newNumber((int)strtol(t.start, NULL, 10));
    }

    if (t.kind == STRING_LITERAL) {
        advance(parser);
        return newString(t.start, t.len);
    }

    fprintf(stderr, "Syntaxfehler: %s ('%.*s')\n",
            tokenName(t.kind), t.len, t.start);
    return NULL;
}

static Expr *parseTerm(struct Parser *parser) {
    Expr *left = parseFactor(parser);
    if (left == NULL) return NULL;

    while (match(MULT, parser)) {
        Expr *right = parseFactor(parser);
        if (right == NULL) return NULL;
        left = newOp(MULT, left, right);
    }
    return left;
}

static Expr *parseExpr(struct Parser *parser) {
    Expr *left = parseTerm(parser);
    if (left == NULL) return NULL;

    while (match(ADD, parser)) {
        Expr *right = parseTerm(parser);
        if (right == NULL) return NULL;
        left = newOp(ADD, left, right);
    }
    return left;
}

static void printExpr(const Expr *e, int indent) {
    if (e == NULL) return;
    for (int i = 0; i < indent; i++) printf("  ");

    switch (e->kind) {
        case NUM:
            printf("Number(%d)\n", e->as.Number);
            break;
        case STRING:
            printf("String(\"%.*s\")\n", e->as.String.len, e->as.String.start);
            break;
        case OPERATOR:
            printf("Operator(%s)\n", tokenName(e->as.Operator.operation));
            printExpr(e->as.Operator.left,  indent + 1);
            printExpr(e->as.Operator.right, indent + 1);
            break;
    }
}

static void freeExpr(Expr *e) {
    if (e == NULL) return;
    if (e->kind == OPERATOR) {
        freeExpr(e->as.Operator.left);
        freeExpr(e->as.Operator.right);
    }
    free(e);
}
// MAIN :3
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <datei>\n", argv[0]);
        return 1;
    }

    char *src = NULL;
    int   len = 0;
    if (parseFile(argv[1], &src, &len) != 0) return 1;

    printArray(len, src);

    struct Token *toks = NULL;
    int ntoks = 0;
    lexer(src, len, &toks, &ntoks);

    printTokens(toks, ntoks);

    struct Parser p = { toks, ntoks, 0 };
    Expr *baum = parseStatement(&p);
    printExpr(baum, 0);

    freeExpr(baum);
    free(toks);
    free(src);
    return 0;
}