#include "ast.h"
#include "utils.h"

expr* new_value_expr(token* value) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = e_value_exp;
    e->content.value = value;
    return e;
}

expr* new_binary_op_expr(token* operator) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = e_binary_op;
    e->content.binary.oper = operator;
    e->content.binary.left = NULL;
    e->content.binary.right = NULL;
    return e;
}

expr* new_unary_op_expr(token* operator) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = e_unary_op;
    e->content.unary.oper = operator;
    e->content.unary.operand = NULL;
    return e;
}

expr* new_ternary_op_expr(token* operator) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = e_ternary_op;
    e->content.ternary.oper = operator;
    return e;
}


expr* new_fnc_call_expr() {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = e_fnc_call;
    e->content.fnc_call.fnc = NULL;
    e->content.unary.operand = NULL;
    return e;
}

expr_list* new_expr_list(expr* elem) {
    expr_list* e = (expr_list*)safe_malloc( sizeof(expr_list) );
    e->elem = elem;
    e->next = NULL;
    return e;
}

stmt* new_expr_stmt(expr* elem) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_expr_stmt;
    e->content._expr = elem;
    return e;
}

stmt* new_block_stmt(block* elem) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_block_stmt;
    e->content._block = elem;
    return e;
}

stmt* new_while_stmt(expr* cond, stmt* body) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_while_stmt;
    e->content._while.cond = cond;
    e->content._while.stmt = body;
    return e;
}

stmt* new_if_stmt(expr* cond, stmt* body, stmt* alter) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_if_stmt;
    e->content._if.cond = cond;
    e->content._if.stmt = body;
    if (alter != NULL)
        e->content._if.alter = alter;
    return e;
}

stmt* new_declare_stmt(token* type, token* var) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_declare_stmt;
    e->content._declare.type = type;
    e->content._declare.var = var;
    return e;
}

stmt* new_return_stmt(expr* elem) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_return_stmt;
    e->content._return = elem;
    return e;
}

stmt* new_break_stmt() {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_break_stmt;
    return e;
}

stmt* new_continue_stmt() {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_continue_stmt;
    return e;
}

block* new_block(stmt* elem) {
    block* e = (block*)safe_malloc( sizeof(block) );
    e->elem = elem;
    e->next = NULL;
    return e;
}

void expr_delete(expr* tree) {
    if (tree == NULL) return;
    switch (tree->tag) {
        case e_value_exp:
            break;
        case e_binary_op:
            expr_delete(tree->content.binary.left);
            expr_delete(tree->content.binary.right);
            free(tree);
            break;
        case e_unary_op:
            expr_delete(tree->content.unary.operand);
            free(tree);
            break;
        case e_ternary_op:
            expr_delete(tree->content.ternary.left);
            expr_delete(tree->content.ternary.middle);
            expr_delete(tree->content.ternary.right);
            free(tree);
            break;
        case e_fnc_call:
            expr_delete(tree->content.fnc_call.fnc);
            expr_list_delete(tree->content.fnc_call.params);
            free(tree);
            break;
    }
}

void expr_list_delete(expr_list* tree){
     while (tree != NULL) {
        expr_delete(tree->elem);
        tree = tree->next;
    }
}

void stmt_delete(stmt* tree) {
    if (tree == NULL) return;
    switch (tree->tag) {
        case e_expr_stmt:
            expr_delete(tree->content._expr);
            free(tree);
            break;
        case e_block_stmt:
            block_delete(tree->content._block);
            free(tree);
            break;
        case e_while_stmt:
            expr_delete(tree->content._while.cond);
            stmt_delete(tree->content._while.stmt);
            free(tree);
            break;
        case e_if_stmt:
            expr_delete(tree->content._if.cond);
            stmt_delete(tree->content._if.stmt);
            stmt_delete(tree->content._if.alter);
            free(tree);
            break;
        case e_declare_stmt:
            free(tree);
            break;
        case e_return_stmt:
            expr_delete(tree->content._return);
            free(tree);
            break;
        case e_break_stmt:
            free(tree);
            break;
        case e_continue_stmt:
            free(tree);
            break;
    }
}

void block_delete(block* tree){
     while (tree != NULL) {
        stmt_delete(tree->elem);
        tree = tree->next;
    }
}

void expr_print_work(expr* tree, int depth) {
    if (tree == NULL) return;
    for (int i=0; i<depth; i++) printf("| ");
    printf("->");
    switch (tree->tag) {
        case e_value_exp:
            token_print(tree->content.value);
            printf("\n");
            break;
        case e_binary_op:
            token_print(tree->content.binary.oper);
            printf("\n");
            expr_print_work(tree->content.binary.left, depth+1);
            expr_print_work(tree->content.binary.right, depth+1);
            break;
        case e_unary_op:
            token_print(tree->content.unary.oper);
            printf("\n");
            expr_print_work(tree->content.unary.operand, depth+1);
            break;
        case e_ternary_op:
            token_print(tree->content.ternary.oper);
            printf("\n");
            expr_print_work(tree->content.ternary.left, depth+1);
            expr_print_work(tree->content.ternary.middle, depth+1);
            expr_print_work(tree->content.ternary.right, depth+1);
            break;
        case e_fnc_call:
            printf("CALL:\n");
            expr_print_work(tree->content.fnc_call.fnc, depth+1);
            expr_list_print_work(tree->content.fnc_call.params, depth+1);
            break;
    }
}

void expr_list_print_work(expr_list* tree, int depth) {
    if (tree == NULL) return;
    for (int i=0; i<depth; i++) printf("| ");
    printf("(\n");
    while (tree != NULL) {
        expr_print_work(tree->elem, depth+1);
        tree = tree->next;
    }
    for (int i=0; i<depth; i++) printf("| ");
    printf(")\n");
}

void block_print(block* tree) {
    printf(">>> Parse tree =====================================\n");
    block_print_work(tree, 0);
}

void block_print_work(block* tree, int depth) {
    if (tree == NULL) return;
    for (int i=0; i<depth; i++) printf("| ");
    printf("{\n");
    while (tree != NULL) {
        stmt_print_work(tree->elem, depth+1);
        tree = tree->next;
    }
    for (int i=0; i<depth; i++) printf("| ");
    printf("}\n");
}

void stmt_print_work(stmt* tree, int depth) {
    if (tree == NULL) return;
    switch (tree->tag) {
        case e_expr_stmt:
            expr_print_work(tree->content._expr, depth);
            break;
        case e_block_stmt:
            block_print_work(tree->content._block, depth);
            break;
        case e_while_stmt:
            for (int i=0; i<depth; i++) printf("| ");
            printf("while:\n");
            expr_print_work(tree->content._while.cond, depth+1);
            for (int i=0; i<depth; i++) printf("| ");
            printf("do:\n");
            stmt_print_work(tree->content._while.stmt, depth+1);
            break;
        case e_if_stmt:
            for (int i=0; i<depth; i++) printf("| ");
            printf("if:\n");
            expr_print_work(tree->content._if.cond, depth+1);
            for (int i=0; i<depth; i++) printf("| ");
            printf("then:\n");
            stmt_print_work(tree->content._if.stmt, depth+1);
            for (int i=0; i<depth; i++) printf("| ");
            printf("else:\n");
            stmt_print_work(tree->content._if.alter, depth+1);
            break;
        case e_declare_stmt:
            for (int i=0; i<depth; i++) printf("| ");
            printf("Declare: ");
            token_print(tree->content._declare.type);
            token_print(tree->content._declare.var);
            printf("\n");
            break;
        case e_return_stmt:
            printf("Return:\n");
            expr_print_work(tree->content._expr, depth);
            break;
        case e_break_stmt:
            printf("Break\n");
            break;
        case e_continue_stmt:
            printf("Continue\n");
            break;
    }
}
