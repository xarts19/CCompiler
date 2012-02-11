#pragma GCC diagnostic ignored "-Wswitch"
#include "parser.h"

#include <stdbool.h>

#include "utils.h"

static expr* Expr();
static expr* Expr_lvl16();
static expr* Expr_lvl16_(expr* tree_left);
static expr* Expr_lvl14();
static expr* Expr_lvl14_(expr* tree_left);
static expr* Expr_lvl13();
static expr* Expr_lvl13_(expr* tree_left);
static expr* Expr_lvl9();
static expr* Expr_lvl9_(expr* tree_left);
static expr* Expr_lvl8();
static expr* Expr_lvl8_(expr* tree_left);
static expr* Expr_lvl6();
static expr* Expr_lvl6_(expr* tree_left);
static expr* Expr_lvl5();
static expr* Expr_lvl5_(expr* tree_left);
static expr* Expr_lvl4();
static expr* Expr_lvl4_(expr* tree_left);
static expr* Expr_lvl3_();
static expr* Expr_lvl2();
static expr* Expr_lvl2_(expr* tree_left);
static expr* Expr_lvl0();
static expr_list* Expr_list();

static stmt* Stmt();
static block* Stmt_block();
static stmt* Stmt_while();
static stmt* Stmt_if();
static stmt* Stmt_declare();

static token* cur_token();
static bool match(token_type type);
static expr* error(const char *message, const char *param);

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static vector* tokens;
static int index;

enum association {as_left, as_right};

static int precedence_table[2][3] =
    {
        {as_left, e_number, e_identifier},
        {as_right, e_assign, -1}
    };




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
        if ( t->id == e_eof )
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

/* STMT -> type id ; */
stmt* Stmt_declare() {
    token* type = cur_token();
    match(e_type);
    token* var = cur_token();
    match(e_identifier);
    match(e_semicolon);
    return new_declare_stmt(type, var);
}

/* EXPR -> lvl16' */
expr* Expr() {
    return Expr_lvl16();
}

/* lvl16 -> lvl14 lvl16' */
expr* Expr_lvl16() {
    expr* tree1 = Expr_lvl14();
    expr* tree2 = Expr_lvl16_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* right-associative */
/* lvl16' -> [= += -= ...] lvl14 lvl16' | e */
expr* Expr_lvl16_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_assign) {
        match(e_assign);
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl14();
        expr* tail = Expr_lvl16_(middle);
        if (tail != NULL) {
            tree->content.binary.right = tail;
            return tree;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

/* lvl14 -> lvl13 lvl14' */
expr* Expr_lvl14() {
    expr* tree1 = Expr_lvl13();
    expr* tree2 = Expr_lvl14_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* lvl14' -> || lvl13 lvl14' | e */
expr* Expr_lvl14_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_or) {
        match(e_or);
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl13();
        tree->content.binary.right = middle;
        expr* tail = Expr_lvl14_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* lvl13 -> lvl9 lvl13' */
expr* Expr_lvl13() {
    expr* tree1 = Expr_lvl9();
    expr* tree2 = Expr_lvl13_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* lvl13' -> && lvl9 lvl13' | e */
expr* Expr_lvl13_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_and) {
        match(e_and);
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl9();
        tree->content.binary.right = middle;
        expr* tail = Expr_lvl13_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* lvl9 -> lvl8 lvl9' */
expr* Expr_lvl9() {
    expr* tree1 = Expr_lvl8();
    expr* tree2 = Expr_lvl9_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* non-associative */
/* lvl9' -> [== !=] lvl8 | e */
expr* Expr_lvl9_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_eq || t->id == e_noteq) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl8();
        tree->content.binary.right = middle;
        return tree;
    } else
        return NULL;
}

/* lvl8 -> lvl6 lvl8' */
expr* Expr_lvl8() {
    expr* tree1 = Expr_lvl6();
    expr* tree2 = Expr_lvl8_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* non-associative */
/* lvl8' -> [> < <= >=] lvl6 | e */
expr* Expr_lvl8_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_less || t->id == e_more || t->id == e_lesseq || t->id == e_moreeq) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl6();
        tree->content.binary.right = middle;
        return tree;
    } else
        return NULL;
}

/* lvl6 -> lvl5 lvl6' */
expr* Expr_lvl6() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl5();
    tree2 = Expr_lvl6_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* lvl6' -> [+ -] lvl5 lvl6' | e */
expr* Expr_lvl6_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if ( t->id == e_plus || t->id == e_minus ) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl5();
        tree->content.binary.right = middle;
        expr* tail = Expr_lvl6_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* lvl5 -> lvl4 lvl5' */
expr* Expr_lvl5() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl4();
    tree2 = Expr_lvl5_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* lvl5' -> [* / %] lvl4 lvl5' | e */
expr* Expr_lvl5_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_mult || t->id == e_div || t->id == e_mod) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl4();
        tree->content.binary.right = middle;
        expr* tail = Expr_lvl5_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* lvl4 -> lvl3 lvl4' */
expr* Expr_lvl4() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl3_();
    tree2 = Expr_lvl4_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* lvl4' -> [.* ->*] lvl3 lvl4' | e */
expr* Expr_lvl4_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_dot_star || t->id == e_arrow_star) {
        ++index;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr* middle = Expr_lvl3_();
        tree->content.binary.right = middle;
        expr* tail = Expr_lvl4_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* right-associative */
/* prefix unary operators */
/* lvl3' -> [* + - ! ++ -- &] lvl3' | lvl2 | e */
expr* Expr_lvl3_() {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_mult || t->id == e_plus || t->id == e_minus || t->id == e_not ||
        t->id == e_incr_pre || t->id == e_decr_pre || t->id == e_addr) {
        ++index;
        expr* tree = new_unary_op_expr(t);
        expr* middle = Expr_lvl3_();
        if (middle == NULL)
            return error("Wrong expression", "");
        tree->content.unary.operand = middle;
        return tree;
    } else
        return Expr_lvl2();
}

/* lvl2 -> lvl0 lvl2' */
expr* Expr_lvl2() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl0();
    tree2 = Expr_lvl2_(tree1);
    if (tree2 != NULL) {
        return tree2;
    } else
        return tree1;
}

/* left-associative */
/* postfix unary operators, array subscripting, function calls */
/* lvl2' -> [-- ++ -> . [EXPR] EXPR_LIST] lvl2' | e */
expr* Expr_lvl2_(expr* tree_left) {
    token* t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_incr_pre || t->id == e_decr_pre || t->id == e_arrow || t->id == e_dot) {
        ++index;
        if (t->id == e_incr_pre) t->id = e_incr_post;
        if (t->id == e_decr_pre) t->id = e_decr_post;
        expr* tree = new_unary_op_expr(t);
        tree->content.unary.operand = tree_left;
        expr* tail = Expr_lvl2_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else if (t->id == e_open_paren) {
        expr* tree = new_fnc_call_expr();
        tree->content.fnc_call.fnc = tree_left;
        tree->content.fnc_call.params = Expr_list();
        expr* tail = Expr_lvl2_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else if (t->id == e_open_bracket) {
        match(e_open_bracket);
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        tree->content.binary.right = Expr();
        match(e_close_bracket);
        expr* tail = Expr_lvl2_(tree);
        if (tail != NULL) {
            return tail;
        } else {
            return tree;
        }
    } else
        return NULL;
}

/* lvl0 -> ( EXPR ) | id | num */
expr* Expr_lvl0() {
    expr* tree;
    token* t = cur_token();
    if (t->id == e_eof) return error("Wrong expression", "");
    if (t->id == e_number || t->id == e_identifier) {
        ++index;
        tree = new_value_expr(t);
        return tree;
    } else if ( t->id == e_open_paren ) {
        match(e_open_paren);
        tree = Expr();
        t = cur_token();
        match(e_close_paren);
        return tree;
    } else
        return error("Wrong expression", "");
}

/* EXPR_LIST -> (EXPR, EXPR, ...) */
expr_list* Expr_list() {
    match(e_open_paren);
    expr* e = Expr();
    expr_list* list = new_expr_list(e);
    expr_list* head = list;
    token* t = cur_token();
    while (t->id != e_close_paren) {
        match(e_comma);
        e = Expr();
        list->next = new_expr_list(e);
        list = list->next;
        if ( t->id == e_eof )
            return (expr_list*)error("No closing curly brace", "");
        t = cur_token();
    }
    match(e_close_paren);
    return head;
}

static token* cur_token() {
    if (index < tokens->size)
        return (token*)tokens->elements[index];
    else
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
