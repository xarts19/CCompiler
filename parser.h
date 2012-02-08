#ifndef _PARSER_H
#define _PARSER_H

#include <string.h>
#include "token.h"
#include "vector.h"
#include "utils.h"

typedef enum { e_value_exp, e_binary_op, e_unary_op } enum_expr_tag;

typedef struct expr_tag {
    enum_expr_tag tag;
    union {
        token*                                  value;
        struct { token*            oper;
                 struct expr_tag*  left;
                 struct expr_tag*  right; }     binary;
        struct { token*            oper;
                 struct expr_tag*  operand; }   unary;
        } content;
} expr;

typedef struct expr_list {
    expr* elem;
    struct expr_list* next;
} expr_list;

typedef enum { e_block_stmt, e_while_stmt, e_if_stmt,
               e_assign_stmt, e_expr_stmt, e_declare_stmt } enum_stmt_tag;

typedef struct stmt_tag {
    enum_stmt_tag tag;
    union {
        expr*                                _expr;
        struct block*                        _block;
        struct { expr*            cond;
                 struct stmt_tag* stmt; }    _while;
        struct { expr*            cond;
                 struct stmt_tag* stmt; }    _if;
        struct { token*           l_value;
                 token*           oper;
                 expr*            r_value; } _assign;
        struct { token*           type;
                 token*           var; }     _declare;
    } content;
} stmt;

typedef struct block {
    stmt* elem;
    struct block* next;
} block;

expr* parse(vector* tokens);

#endif
