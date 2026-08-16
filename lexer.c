#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"

const char *tokenName(enum TokenType k) {
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
