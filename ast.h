#ifndef AST_H
#define AST_H

#include "lexer.h"

//program   => statement* EOF
//statement => PRINT expr
//expr      => term ("+" term)*
//term      => factor ("*" factor)*
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

typedef struct Prin {
    enum {PRINT_STM} kind;
    Expr *expr;
} Prin;

Expr *newNumber(const int num);

Expr *newString(const char *start, int len);

Prin *newPrint(Expr *expr);

Expr *newOp(enum TokenType op, Expr *left, Expr *right);

void printExpr(const Expr *e, int indent);

void printPrin(const Prin *prin);

void freeExpr(Expr *e);

void freeStmt(Prin *prin);

#endif
