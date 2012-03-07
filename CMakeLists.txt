cmake_minimum_required(VERSION 2.8.5)
project(ccompiler)
SET(CMAKE_C_FLAGS "-std=c99 -g -fPIC -Wall -Wextra -pedantic")
include_directories(${PROJECT_SOURCE_DIR}/include)

SET(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(SOURCES ${SRC_DIR}/main.c
            ${SRC_DIR}/ast.c
            ${SRC_DIR}/lexer.c
            ${SRC_DIR}/map.c
            ${SRC_DIR}/parser.c
            ${SRC_DIR}/preprocessor.c
            ${SRC_DIR}/reader.c
            ${SRC_DIR}/s_table.c
            ${SRC_DIR}/token.c
            ${SRC_DIR}/utils.c
            ${SRC_DIR}/vector.c)

add_executable(cc ${SOURCES})

