#include <stdio.h>

#include "eval.h"
#include "ast.h"
#include "lexer.h"

int execExpr(Expr *e) {
    switch (e->kind) {
        case NUM: return e->as.Number;
        case OPERATOR: {
            int l = execExpr(e->as.Operator.left);
            int r = execExpr(e->as.Operator.right);
            return e->as.Operator.operation == ADD ? l + r : l * r;
        }
        case STRING:
            perror("Cannot calculate with Strings u dummy :p");
            return 0;
    }
    return 0;
}

void execPrin(const Prin *prin) {
    if (prin == NULL) return;
    switch (prin->kind) {
        case PRINT_STM:
            if (prin->expr->kind == STRING) {
                printf("%.*s\n", prin->expr->as.String.len, prin->expr->as.String.start);
            } else {
                printf("%d\n", execExpr(prin->expr));
            }
            break;
    }
}

void execProgram(const Program *prog) {
    if (prog == NULL) return;
    for (int i = 0; i < prog->count; i++) {
        execPrin(prog->stmts[i]);
    }
}
