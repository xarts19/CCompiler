cmake_minimum_required(VERSION 2.8.5)
project(Ccompiler)
ADD_DEFINITIONS("-Wall -std=c99 -Wextra -pedantic -fPIC -g")
include_directories(.)
add_executable(main.out utils.c preprocessor.c reader.c token.c vector.c map.c s_table.c ast.c lexer.c parser.c main.c)

