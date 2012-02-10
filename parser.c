#pragma GCC diagnostic ignored "-Wswitch"
#include "parser.h"

#include <stdbool.h>

#include "utils.h"

static expr* Expr();
static expr* Expr_();
static expr* Expr_lvl5();
static expr* Expr_lvl5_(expr* tree_left);
static expr* Expr_lvl10();
static expr* Expr_lvl10_(expr* tree_left);
static expr* Expr_lvl_max();

static stmt* Stmt();
static block* Stmt_block();
static stmt* Stmt_while();
static stmt* Stmt_if();
static stmt* Stmt_assign();
static stmt* Stmt_declare();

static token* cur_token();
static token* next_token();
static bool match(token_type type);
static expr* error(const char *message, const char *param);

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static vector* tokens;
static int index;

stmt* parse_topdown(vector* t) {
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

/* choose production based on next token */
stmt* Stmt() {
    stmt* s;
    token* t = cur_token();
    if ( t->id == e_eof) {
        return NULL;
    } else if (t->id == e_open_curly) {
        return new_block_stmt( Stmt_block() );
    } else if ( t->id == e_while ) {
        return Stmt_while();
    } else if ( t->id == e_if ) {
        return Stmt_if();
    } else if ( next_token()->id == e_assign ) {
        return Stmt_assign();
    } else if ( t->id == e_type ) {
        return Stmt_declare();
    } else {
        /* STMT -> EXPR; */
        s = new_expr_stmt( Expr() );
        match(e_semicolon);
        return s;
    }
}

/* STMT -> { BLOCK } */
block* Stmt_block() {
    match(e_open_curly);
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
    match(e_close_curly);
    return head;
}

/* STMT -> while ( EXPR ) STMT */
stmt* Stmt_while() {
    stmt* body;
    expr* cond;
    match(e_while);
    match(e_open_paren);
    cond = Expr();
    match(e_close_paren);
    body = Stmt();
    return new_while_stmt(cond, body);
}

/* STMT -> if ( EXPR ) STMT */
stmt* Stmt_if() {
    expr* cond;
    stmt* body;
    stmt* alter;
    match(e_if);
    match(e_open_paren);
    cond = Expr();
    match(e_close_paren);
    body = Stmt();
    if ( cur_token()->id == e_else ) {
        match(e_else);
        alter = Stmt();
    }
    return new_if_stmt(cond, body, alter);
}

/* STMT -> id = EXPR ; */
stmt* Stmt_assign() {
    token* lvalue = cur_token();
    match(e_identifier);
    token* oper = cur_token();
    match(e_assign);
    expr *rvalue = Expr();
    match(e_semicolon);
    return new_assign_stmt(lvalue, oper, rvalue);
}

/* STMT -> type id ; */
stmt* Stmt_declare() {
    token* type = cur_token();
    match(e_type);
    token* var = cur_token();
    match(e_identifier);
    match(e_semicolon);
    return new_declare_stmt(type, var);
}

/* EXPR -> B EXPR' */
expr* Expr() {
    expr *tree1, *tree2;
    token* t = (token*)tokens->elements[index];
    if ( t->id != e_number && t->id != e_identifier && t->id != e_open_paren ) {
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

/* EXPR' -> [> < >= <= != ==] B | e */
expr* Expr_() {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if (t->id == e_eq || t->id == e_noteq || t->id == e_more ||
        t->id == e_moreeq || t->id == e_less || t->id == e_lesseq) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        expr* tail = Expr_lvl5();
        tree->content.binary.right = tail;
        return tree;
    } else
        return NULL;
}

/* B -> C B' */
expr* Expr_lvl5() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl10();
    tree2 = Expr_lvl5_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* B' -> [+ -] C B' | e */
expr* Expr_lvl5_(expr* tree_left) {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if ( t->id == e_plus || t->id == e_minus ) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl10();
        expr* tail = Expr_lvl5_(tree);
        if (tail != NULL) {
            /* Rebalance tree to the left (left-associative operator) */
            tail->content.binary.left = tree;
            tree->content.binary.right = middle;
            return tail;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

/* C -> D C' */
expr* Expr_lvl10() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl_max();
    tree2 = Expr_lvl10_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* C' -> [* / %] D C' | e */
expr* Expr_lvl10_(expr* tree_left) {
    token* t = (token*)tokens->elements[index];
    if (index >= tokens->size) return NULL;
    if (t->id == e_mult || t->id == e_div || t->id == e_mod) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl_max();
        expr* tail = Expr_lvl10_(tree);
        if (tail != NULL) {
            /* Rebalance tree to the left (left-associative operator) */
            tail->content.binary.left = tree;
            tree->content.binary.right = middle;
            return tail;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

/* D -> ( EXPR ) | id | num */
expr* Expr_lvl_max() {
    expr* tree;
    if (index >= tokens->size) return error("Wrong expression", "");
    token* t = (token*)tokens->elements[index];
    if (t->id == e_number || t->id == e_identifier) {
        ++index;
        tree = new_value_expr(t);
        return tree;
    } else if ( t->id == e_open_paren ) {
        index +=1;
        tree = Expr();
        t = (token*)tokens->elements[index];
        if (t->id != e_close_paren)
            return error("No closing parentethis", "");
        ++index;
        return tree;
    } else
        return error("Wrong expression", "");
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
    return NULL;
}

bool match(token_type type) {
    token* t = cur_token();
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
