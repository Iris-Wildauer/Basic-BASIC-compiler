#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

struct Parser {const struct Token *token; int n; int pos;};

struct Token peek(struct Parser *parser);

struct Token advance(struct Parser *parser);

int match(enum TokenType type, struct Parser *parser);

int expect(enum TokenType type, struct Parser *parser);

Prin *parseStatement(struct Parser *parser);

#endif
