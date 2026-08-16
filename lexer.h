#ifndef LEXER_H
#define LEXER_H

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
};

const char *tokenName(enum TokenType k);

void printTokens(const struct Token *toks, int n);

int lexer(char *output, int len, struct Token **lex, int *numtokens);

#endif
