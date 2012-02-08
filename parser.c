#pragma GCC diagnostic ignored "-Wswitch"
#include "parser.h"

#include <stdbool.h>

static expr* new_value_expr(token* value);
static expr* new_binary_op_expr(token* operator);
static expr_list* new_expr_list(expr* elem);

static stmt* new_expr_stmt(expr* elem);
static stmt* new_block_stmt(block* elem);
static stmt* new_while_stmt(expr* cond, stmt* body);
static stmt* new_if_stmt(expr* cond, stmt* body);
static stmt* new_assign_stmt(token* lvalue, token* oper, expr* rvalue);
static stmt* new_declare_stmt(token* type, token* var);
static block* new_block(stmt* elem);

static expr* Expr();
static expr* Expr_();
static expr* Expr_lvl5();
static expr* Expr_lvl5_();
static expr* Expr_lvl10();
static expr* Expr_lvl10_();
static expr* Expr_lvl_max();

static stmt* Stmt();
static block* Stmt_block();
static stmt* Stmt_while();
static stmt* Stmt_if();
static stmt* Stmt_assign();
static stmt* Stmt_declare();

static void expr_delete(expr* tree);
static void expr_list_delete(expr_list* tree);
static void stmt_delete(stmt* tree);
static void block_delete(block* tree);

static void expr_print(expr* tree);
static void expr_list_print(expr_list* tree);
static void stmt_print(stmt* tree);
static void block_print(block* tree);

static void expr_print_work(expr* tree, int depth);
static void stmt_print_work(stmt* tree, int depth);
static void block_print_work(block* tree, int depth);

static token* cur_token();
static token* next_token();
static bool match(token_type type, const char* data);
static expr* error(const char *message, const char *param);

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static vector* tokens;
static int index;

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

stmt* new_if_stmt(expr* cond, stmt* body) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_if_stmt;
    e->content._if.cond = cond;
    e->content._if.stmt = body;
    return e;
}

stmt* new_assign_stmt(token* lvalue, token* oper, expr* rvalue) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_assign_stmt;
    e->content._assign.l_value = lvalue;
    e->content._assign.oper = oper;
    e->content._assign.r_value = rvalue;
    return e;
}

static stmt* new_declare_stmt(token* type, token* var) {
    stmt* e = (stmt*)safe_malloc( sizeof(stmt) );
    e->tag = e_declare_stmt;
    e->content._declare.type = type;
    e->content._declare.var = var;
    return e;
}

block* new_block(stmt* elem) {
    block* e = (block*)safe_malloc( sizeof(block) );
    e->elem = elem;
    e->next = NULL;
    return e;
}



expr* parse(vector* t) {
    tokens = t;
    index = 0;
    stmt* s = Stmt(tokens);
    block* list = new_block(s);
    block* head = list;
    while (s != NULL && index < tokens->size) {
        s = Stmt(tokens);
        list->next = new_block(s);
        list = list->next;
    }
    block_print(head);
    block_delete(head);
    return NULL;
}

stmt* Stmt() {
    stmt* s;
    token* t = cur_token();
    if (t->id == e_open_curly) {
        return new_block_stmt( Stmt_block() );
    } else if ( strcmp(t->data,"while") == 0 ) {
        return Stmt_while();
    } else if ( strcmp(t->data,"if") == 0 ) {
        return Stmt_if();
    } else if ( next_token()->id == e_assign_operator ) {
        return Stmt_assign();
    } else if ( t->id == e_type ) {
        return Stmt_declare();
    } else {
        s = new_expr_stmt( Expr() );
        match( e_colon, NULL );
        return s;
    }
}

block* Stmt_block() {
    match(e_open_curly, NULL);
    stmt* s = Stmt();
    block* list = new_block(s);
    block* head = list;
    token* t = cur_token();
    while (t->id != e_close_curly) {
        s = Stmt();
        list->next = new_block(s);
        list = list->next;
        if ( index >= tokens->size )
            return (block*)error("No closing curly brace", "");
        t = cur_token();
    }
    match(e_close_curly, NULL);
    return head;
}

stmt* Stmt_while() {
    stmt* body;
    expr* cond;
    match(e_keyword, "while");
    match(e_open_paren, NULL);
    cond = Expr();
    match(e_close_paren, NULL);
    body = Stmt();
    return new_while_stmt(cond, body);
}

stmt* Stmt_if() {
    stmt* body;
    expr* cond;
    match(e_keyword, "if");
    match(e_open_paren, NULL);
    cond = Expr();
    match(e_close_paren, NULL);
    body = Stmt();
    return new_if_stmt(cond, body);
}

stmt* Stmt_assign() {
    token* lvalue = cur_token();
    match(e_identifier, NULL);
    token* oper = cur_token();
    match(e_assign_operator, NULL);
    expr *rvalue = Expr();
    match( e_colon, NULL );
    return new_assign_stmt(lvalue, oper, rvalue);
}

stmt* Stmt_declare() {
    token* type = cur_token();
    match(e_type, NULL);
    token* var = cur_token();
    match(e_identifier, NULL);
    match( e_colon, NULL );
    return new_declare_stmt(type, var);
}

expr* Expr() {
    expr *tree1, *tree2;
    token* t = (token*)tokens->elements[index];
    if ( t->id != e_number && t->id != e_identifier && strcmp(t->data, "op(") != 0 ) {
        return error("Unknown symbol in expression: ", t->data);
    }
    tree1 = Expr_lvl5();
    tree2 = Expr_();
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* Expr_() {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if (t->id == e_cmp_operator) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        expr* tail = Expr_lvl5();
        tree->content.binary.right = tail;
        return tree;
    } else
        return NULL;
}

expr* Expr_lvl5() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl10();
    tree2 = Expr_lvl5_();
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* Expr_lvl5_() {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if ( strcmp(t->data, "op+") == 0 || strcmp(t->data, "op-") == 0 ) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        expr* middle = Expr_lvl10();
        expr* tail = Expr_lvl5_();
        if (tail != NULL) {
            tail->content.binary.left = middle;
            tree->content.binary.right = tail;
            return tree;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

expr* Expr_lvl10() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl_max();
    tree2 = Expr_lvl10_();
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* Expr_lvl10_() {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if ( strcmp(t->data, "op*") == 0 || strcmp(t->data, "op/") == 0 || strcmp(t->data, "op%") == 0 ) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        expr* middle = Expr_lvl_max();
        expr* tail = Expr_lvl10_();
        if (tail != NULL) {
            tail->content.binary.left = middle;
            tree->content.binary.right = tail;
            return tree;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

expr* Expr_lvl_max() {
    expr* tree;
    if (index >= tokens->size) return error("Wrong expression", "");
    token* t = (token*)tokens->elements[index];
    if (t->id == e_number || t->id == e_identifier) {
        ++index;
        tree = new_value_expr(t);
        return tree;
    } else if ( strcmp(t->data, "op(") == 0 ) {
        index +=1;
        tree = Expr();
        t = (token*)tokens->elements[index];
        if (strcmp(t->data, "op)") != 0)
            return error("No closing parentethis", "");
        ++index;
        return tree;
    } else
        return error("Wrong expression", "");
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
            free(tree);
            break;
        case e_assign_stmt:
            expr_delete(tree->content._assign.r_value);
            free(tree);
            break;
        case e_declare_stmt:
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

void expr_print(expr* tree) {
    int depth = 0;
    expr_print_work(tree, depth);
}

void expr_print_work(expr* tree, int depth) {
    if (tree == NULL) return;
    for (int i=0; i<depth; i++) printf("| ");
    switch (tree->tag) {
        case e_value_exp:
            token_print(tree->content.value);
            printf("\n");
            break;
        case e_binary_op:
            token_print(tree->content.value);
            printf("\n");
            expr_print_work(tree->content.binary.left, depth+1);
            expr_print_work(tree->content.binary.right, depth+1);
            break;
        case e_unary_op:
            token_print(tree->content.value);
            printf("\n");
            expr_print_work(tree->content.unary.operand, depth+1);
            break;
    }
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
            break;
        case e_assign_stmt:
            for (int i=0; i<depth; i++) printf("| ");
            printf("Assign: ");
            token_print(tree->content._assign.oper);
            token_print(tree->content._assign.l_value);
            printf(" to \n");
            expr_print_work(tree->content._assign.r_value, depth+1);
            break;
        case e_declare_stmt:
            for (int i=0; i<depth; i++) printf("| ");
            printf("Declare: ");
            token_print(tree->content._declare.type);
            token_print(tree->content._declare.var);
            printf("\n");
            break;
    }
}

static token* cur_token() {
    if (index < tokens->size)
        return (token*)tokens->elements[index];
    else
        return NULL;
}
static token* next_token() {
    if (index + 1 < tokens->size)
        return (token*)tokens->elements[index+1];
    else
        return NULL;
}

bool match(token_type type, const char* data) {
    token* t = cur_token();
    if (data != NULL) {
        if ( strcmp(t->data, data) != 0 ) {
            char buf[MAX_ERROR_LENTH];
            error(buf, "");
            return false;
        }
    }
    if (t->id != type) {
        char buf[MAX_ERROR_LENTH];
        sprintf(buf, "Expected \"%s\", but got \"%s\"(\"%s\") instead",
                token_type_str(type), token_type_str(t->id), t->data);
        error(buf, "");
        return false;
    }
    ++index;
    return true;
}

expr* error(const char *message, const char *param) {
    printf("In file \"%s\": line %d; token: %s(%s)\n", current_file,
           cur_token()->line-1, token_type_str(cur_token()->id), cur_token()->data);
    printf("Parser error: %s%s\n", message, param);
    exit(EXIT_FAILURE);
    return NULL;
}
