#include <stdio.h>
#include <stdlib.h>
// (˶>⩊<˶)

#include "file.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "eval.h"

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

    //printTokens(toks, ntoks);

    struct Parser p = { toks, ntoks, 0 };
    Prin *baum = parseStatement(&p);
    //printPrin(baum);
    execPrin(baum);

    free(toks);
    free(src);

    return 0;
}
