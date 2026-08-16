#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"

Expr *newNumber(const int num) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = NUM;
    e->as.Number = num;

    return e;
}

Expr *newString(const char *start, int len) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = STRING;
    e->as.String.start = start;
    e->as.String.len = len;

    return e;
}

Prin *newPrint(Expr *expr) {
    Prin *e = malloc(sizeof *e);
    if (e == NULL) return NULL;
    e->kind = PRINT_STM;
    e->expr = expr;

    return e;
}

Expr *newOp(enum TokenType op, Expr *left, Expr *right) {
    Expr *e = malloc(sizeof *e);
    if (e == NULL) return NULL;

    e->kind = OPERATOR;
    e->as.Operator.left = left;
    e->as.Operator.right = right;
    e->as.Operator.operation = op;

    return e;
}

void printExpr(const Expr *e, int indent) {
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

void printPrin(const Prin *prin) {
    if (prin == NULL) return;

    switch (prin->kind) {
        case PRINT_STM:
            printf("Print\n");
            printExpr(prin->expr, 1);
            break;
    }
}

void freeExpr(Expr *e) {
    if (e == NULL) return;
    if (e->kind == OPERATOR) {
        freeExpr(e->as.Operator.left);
        freeExpr(e->as.Operator.right);
    }
    free(e);
}

void freeStmt(Prin *prin) {
    if (prin == NULL) return;
    freeExpr(prin->expr);
    free(prin);
}
