cmake_minimum_required(VERSION 3.20)
project(basiCompiler C)

set(CMAKE_C_STANDARD 11)

add_executable(basiCompiler
        main.c
        file.c
        lexer.c
        ast.c
        parser.c
        eval.c)