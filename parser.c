#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "ast.h"

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

Prin *parseStatement(struct Parser *parser) {
    if (match(PRINT_TOK, parser)) {
        Expr *e = parseExpr(parser);
        if (e == NULL) return NULL;
        return newPrint(e);
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
