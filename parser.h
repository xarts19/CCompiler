#ifndef _PARSER_H
#define _PARSER_H

#include <string.h>
#include "token.h"
#include "vector.h"
#include "utils.h"

typedef enum { value_exp, binary_op, unary_op} tag_enum;

typedef struct expr_tag {
    tag_enum tag;
    union { token*                                    value;
            struct { token*            oper;
                     struct expr_tag*  left;
                     struct expr_tag*  right; }       binary;
            struct { token*            oper;
                     struct expr_tag*  operand; }     unary;
        } content;
} expr;

typedef struct expr_list {
  expr*             elem;
  struct expr_list* next;
} expr_list;

expr* new_value_expr(token* value);

expr* parse(vector* tokens);
expr* E(vector* tokens, int* index);
expr* E_(vector* tokens, int* index);
expr* B(vector* tokens, int* index);
expr* B_(vector* tokens, int* index);
expr* C(vector* tokens, int* index);
expr* C_(vector* tokens, int* index);
expr* D(vector* tokens, int* index);

void tree_delete(expr* tree);

void tree_print(expr* tree);

#endif

