#ifndef AST_H_
#define AST_H_

#include "token.h"
#include <stdbool.h>

typedef enum { e_value_exp, e_binary_op, e_unary_op, e_ternary_op, e_fnc_call } enum_expr_tag;

typedef struct expr_tag {
    enum_expr_tag tag;
    union {
        token                                 *value;
        struct { token            *oper;
                 struct expr_tag  *left;
                 struct expr_tag  *right; }     binary;
        struct { token            *oper;
                 struct expr_tag  *operand; }   unary;
        struct { token            *oper;
                 struct expr_tag  *left;
                 struct expr_tag  *middle;
                 struct expr_tag  *right; }     ternary;
        struct { struct expr_tag  *fnc;
                 struct expr_list *params; }    fnc_call;
        } content;
} expr;

typedef struct expr_list {
    expr *elem;
    struct expr_list *next;
} expr_list;

typedef enum { e_simple_type, e_array_type } enum_type_type;

typedef struct type_tree_tag {
    enum_type_type tag;
    union {
        struct { token       *type;
                 int          indirect_num;  }  type_expr;
        struct { expr                 *size;
                 struct type_tree_tag *type; }  array_expr;
    } content;
} type_tree;

typedef enum { e_block_stmt, e_while_stmt, e_if_stmt, e_expr_stmt, e_fnc_stmt, 
               e_declare_stmt, e_return_stmt, e_break_stmt, e_continue_stmt } enum_stmt_tag;

typedef struct stmt_tag {
    enum_stmt_tag tag;
    union {
        expr                                *_expr;
        struct block                        *_block;
        struct { expr            *cond;
                 struct stmt_tag *stmt; }   _while;
        struct { expr            *cond;
                 struct stmt_tag *stmt;
                 struct stmt_tag *alter; }  _if;
        struct { type_tree       *type;
                 token           *var; }    _declare;
        struct { token           *name;
                 type_tree       *type;
                 struct block    *args;
                 struct block    *body; }   _fnc;
        expr                                *_return;
    } content;
} stmt;

typedef struct block {
    stmt *elem;
    struct block *next;
} block;

expr *new_value_expr(token *value);
expr *new_binary_op_expr(token *operator);
expr *new_unary_op_expr(token *operator);
expr *new_ternary_op_expr(token *operator);
expr *new_fnc_call_expr();
expr_list *new_expr_list(expr *elem);

stmt *new_expr_stmt(expr *elem);
stmt *new_block_stmt(block *elem);
stmt *new_while_stmt(expr *cond, stmt *body);
stmt *new_if_stmt(expr *cond, stmt *body, stmt *alter);
stmt *new_declare_stmt(type_tree *type, token *var);
type_tree *new_type_tree(enum_type_type type);
stmt *new_fnc_stmt(token *name, type_tree *type, block *args, block *body);
stmt *new_return_stmt(expr *elem);
stmt *new_break_stmt();
stmt *new_continue_stmt();
block *new_block(stmt *elem);

void expr_delete(expr *tree);
void expr_list_delete(expr_list *tree);
void stmt_delete(stmt *tree);
void block_delete(block *tree);
void type_tree_delete(type_tree *tree);

void expr_print(expr *tree, int depth);
void expr_list_print(expr_list *tree, int depth);
void stmt_print(stmt *tree, int depth);
void block_print(block *tree, int depth);
void type_tree_print(type_tree *tree, int depth);

#endif
