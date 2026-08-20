#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"

const char *tokenName(enum TokenType k) {
    switch (k) {
        case NEWLINE: return "NEWLINE";
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

static enum TokenType findCmd(const char *keyword, int len) {
    if (len == 5 && strncasecmp(keyword, "PRINT", 5) == 0) {
        return PRINT_TOK;
    }
    return UNKNOWN;
}

int lexer(char *output, int len, struct Token **lex, int *numtokens) {
    int i = 0;
    int n = 0;

    struct Token *token = malloc((len+1)* sizeof *token);

    while (i < len) {
        if (output[i] == ' ' || output[i] == '\t' || output[i] == '\r') {
            i++;
            continue;
        }
        if (output[i] == '"') {
            i++;
            int start = i;
            while (i < len && output[i] != '"') i++;
            token[n].kind  = STRING_LITERAL;
            token[n].start = &output[start];
            token[n].len   = i - start;
            n++;
            i++;
        }
        else if (isalpha((unsigned char)output[i])) {
            int start = i;
            while (i < len && (isalnum((unsigned char)output[i]) || output[i] == '_')) i++;
            token[n].kind  = findCmd(&output[start], i - start);
            token[n].start = &output[start];
            token[n].len   = i - start;
            n++;
        }
        else if (isdigit((unsigned char)output[i])) {
            int start = i;
            while (i < len && isdigit((unsigned char)output[i])) i++;
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
        else if (output[i] == '\n') {
            token[n].kind = NEWLINE;
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