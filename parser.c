#pragma GCC diagnostic ignored "-Wswitch"
#include "parser.h"

#include <stdbool.h>

#include "utils.h"
#include "s_table.h"

static expr *Expr();
static expr *Expr_lvl16();
static expr *Expr_lvl16_(expr *tree_left);
static expr *Expr_lvl15();
static expr *Expr_lvl15_(expr *tree_left);
static expr *Expr_lvl14();
static expr *Expr_lvl14_(expr *tree_left);
static expr *Expr_lvl13();
static expr *Expr_lvl13_(expr *tree_left);
static expr *Expr_lvl9();
static expr *Expr_lvl9_(expr *tree_left);
static expr *Expr_lvl8();
static expr *Expr_lvl8_(expr *tree_left);
static expr *Expr_lvl6();
static expr *Expr_lvl6_(expr *tree_left);
static expr *Expr_lvl5();
static expr *Expr_lvl5_(expr *tree_left);
static expr *Expr_lvl4();
static expr *Expr_lvl4_(expr *tree_left);
static expr *Expr_lvl3_();
static expr *Expr_lvl2();
static expr *Expr_lvl2_(expr *tree_left);
static expr *Expr_lvl2_unary(expr *tree_left);
static expr *Expr_lvl0();
static expr_list *Expr_list();

static stmt *Stmt();
static block *Stmt_block();
static stmt *Stmt_while();
static stmt *Stmt_if();

static stmt *Stmt_declare(bool in_fnc_prototype);
type_tree *TYPE();
int PTR();
type_tree *ARRAY_DECL(type_tree *type);
static block *Decl_list();

static token *cur_token();
static void advance_token();
static void rollback_token();
static bool match(token_type type);
static expr *error(const char *message, const char *param);

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static vector *tokens;
static int token_index;
static s_table *symtable = NULL;

stmt *parse_topdown(vector *t) {
    tokens = t;
    token_index = 0;
    stmt *s = Stmt(tokens);
    block *list = new_block(s);
    block *head = list;
    while (s != NULL && token_index+1 < tokens->size) {
        s = Stmt(tokens);
        list->next = new_block(s);
        list = list->next;
    }
    printf(">>> Parse tree =====================================\n");
    block_print(head, 0);
    block_delete(head);
    return NULL;
}

/* choose production based on next token */
stmt *Stmt() {
    stmt *s;
    token *t = cur_token();
    if ( t->id == e_eof) {
        return NULL;
    } else if ( t->id == e_open_curly ) {
        /* statement block */
        return new_block_stmt( Stmt_block() );
    } else if ( t->id == e_while ) {
        /* while loop */
        return Stmt_while();
    } else if ( t->id == e_if ) {
        /* if stmt */
        return Stmt_if();
    } else if ( t->id == e_type ) {
        /* declaration */
        return Stmt_declare(false);
    } else if ( t->id == e_return ) {
        advance_token();
        s = new_return_stmt( Expr() );
        return s;
    } else if ( t->id == e_break ) {
        advance_token();
        s = new_break_stmt();
        match(e_semicolon);
        return s;
    } else if ( t->id == e_continue ) {
        advance_token();
        s = new_continue_stmt();
        match(e_semicolon);
        return s;
    } else {
        /* STMT -> EXPR; */
        s = new_expr_stmt( Expr() );
        match(e_semicolon);
        return s;
    }
}

/* STMT -> { BLOCK } */
block *Stmt_block() {
    match(e_open_curly);
    stmt *s = Stmt();
    block *list = new_block(s);
    block *head = list;
    token *t = cur_token();
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
stmt *Stmt_while() {
    stmt *body;
    expr *cond;
    match(e_while);
    match(e_open_paren);
    cond = Expr();
    match(e_close_paren);
    body = Stmt();
    return new_while_stmt(cond, body);
}

/* STMT -> if ( EXPR ) STMT */
stmt *Stmt_if() {
    expr *cond;
    stmt *body;
    stmt *alter;
    match(e_if);
    match(e_open_paren);
    cond = Expr();
    match(e_close_paren);
    body = Stmt();
    if ( cur_token()->id == e_else ) {
        advance_token();
        alter = Stmt();
    }
    return new_if_stmt(cond, body, alter);
}

/* STMT -> TYPE [ id ] ARRAY_DECL [ ; ] 
 * STMT -> TYPE [ id ] ( DECLAR, DECLAR, ... ) FNC_TAIL
 */
stmt *Stmt_declare(bool in_fnc_prototype) {
    /* fnc prototype allows no identifier and has no semicolons */
    stmt *tree = NULL;
    type_tree *type = TYPE();
    token *var = NULL;
    if (in_fnc_prototype) {
        if (cur_token()->id == e_identifier) {
            var = cur_token();
            advance_token();
        }
    } else {
        var = cur_token();
        match(e_identifier);
    }
    if (cur_token()->id == e_open_paren) {
        /* fucntion declaration */
        block *args = Decl_list();
        block *body = NULL;
        if (cur_token()->id == e_open_curly) {
            body = Stmt_block();
        }
        tree = new_fnc_stmt(var, type, args, body);
    } else {
        type_tree *array_type = ARRAY_DECL(type);
        type = (array_type ? array_type : type);
        tree = new_declare_stmt(type, var);
    }
    if (!in_fnc_prototype)
        match(e_semicolon);
    return tree;
}

/* TYPE -> e_type PTR */
type_tree *TYPE() {
    token *type = cur_token();
    match(e_type);
    int indirect_num = PTR();
    type_tree *tree = new_type_tree(e_simple_type);
    tree->content.type_expr.type = type;
    tree->content.type_expr.indirect_num = indirect_num;
    return tree;
}

/* PTR -> * PTR | e */
int PTR() {
    int indirect_num = 0;
    while (cur_token()->id == e_mult) {
        advance_token();
        ++indirect_num;
    }
    return indirect_num;
}

/* ARRAY_DECL -> [ EXPR ] ARRAY_DECL | e */
type_tree *ARRAY_DECL(type_tree *type) {
    if (cur_token()->id == e_open_bracket) {
        advance_token();
        type_tree *tree = new_type_tree(e_array_type);
        if (cur_token()->id == e_close_bracket) {
            /* no array size */
            tree->content.array_expr.size = NULL;
        } else {
            tree->content.array_expr.size = Expr();
        }
        tree->content.array_expr.type = type;
        match(e_close_bracket);
        type_tree *tail = ARRAY_DECL(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* EXPR -> lvl16' */
expr *Expr() {
    if (cur_token()->id == e_semicolon)
        return NULL;
    return Expr_lvl16();
}

/* lvl16 -> lvl14 lvl16' */
expr *Expr_lvl16() {
    expr *tree1 = Expr_lvl15();
    expr *tree2 = Expr_lvl16_(tree1);
    return tree2 ? tree2 : tree1;
}

/* right-associative */
/* lvl16' -> [= += -= ...] lvl14 lvl16' | e */
expr *Expr_lvl16_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_assign) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl15();
        expr *tail = Expr_lvl16_(middle);
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

/* lvl15 -> lvl14 lvl15' */
expr *Expr_lvl15() {
    expr *tree1 = Expr_lvl14();
    expr *tree2 = Expr_lvl15_(tree1);
    return tree2 ? tree2 : tree1;
}

/* ternary conditional */
/* lvl15' -> ? lvl14 : lvl14 | e */
expr *Expr_lvl15_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_question) {
        advance_token();
        expr *tree = new_ternary_op_expr(t);
        tree->content.ternary.left = tree_left;
        tree->content.ternary.middle = Expr_lvl14();
        match(e_colon);
        tree->content.ternary.right = Expr_lvl14();
        return tree;
    } else
        return NULL;
}

/* lvl14 -> lvl13 lvl14' */
expr *Expr_lvl14() {
    expr *tree1 = Expr_lvl13();
    expr *tree2 = Expr_lvl14_(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl14' -> || lvl13 lvl14' | e */
expr *Expr_lvl14_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_or) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl13();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl14_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* lvl13 -> lvl9 lvl13' */
expr *Expr_lvl13() {
    expr *tree1 = Expr_lvl9();
    expr *tree2 = Expr_lvl13_(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl13' -> && lvl9 lvl13' | e */
expr *Expr_lvl13_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_and) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl9();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl13_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* lvl9 -> lvl8 lvl9' */
expr *Expr_lvl9() {
    expr *tree1 = Expr_lvl8();
    expr *tree2 = Expr_lvl9_(tree1);
    return tree2 ? tree2 : tree1;
}

/* non-associative */
/* lvl9' -> [== !=] lvl8 | e */
expr *Expr_lvl9_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_eq || t->id == e_noteq) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl8();
        tree->content.binary.right = middle;
        return tree;
    } else
        return NULL;
}

/* lvl8 -> lvl6 lvl8' */
expr *Expr_lvl8() {
    expr *tree1 = Expr_lvl6();
    expr *tree2 = Expr_lvl8_(tree1);
    return tree2 ? tree2 : tree1;
}

/* non-associative */
/* lvl8' -> [> < <= >=] lvl6 | e */
expr *Expr_lvl8_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_less || t->id == e_more || t->id == e_lesseq || t->id == e_moreeq) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl6();
        tree->content.binary.right = middle;
        return tree;
    } else
        return NULL;
}

/* lvl6 -> lvl5 lvl6' */
expr *Expr_lvl6() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl5();
    tree2 = Expr_lvl6_(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl6' -> [+ -] lvl5 lvl6' | e */
expr *Expr_lvl6_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if ( t->id == e_plus || t->id == e_minus ) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl5();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl6_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* lvl5 -> lvl4 lvl5' */
expr *Expr_lvl5() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl4();
    tree2 = Expr_lvl5_(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl5' -> [ * / % ] lvl4 lvl5' | e */
expr *Expr_lvl5_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_mult || t->id == e_div || t->id == e_mod) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl4();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl5_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* lvl4 -> lvl3 lvl4' */
expr *Expr_lvl4() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl3_();
    tree2 = Expr_lvl4_(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl4' -> [.* ->*] lvl3 lvl4' | e */
expr *Expr_lvl4_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_dot_star || t->id == e_arrow_star) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl3_();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl4_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* right-associative */
/* prefix unary operators */
/* lvl3' -> [* + - ! ++ -- &] lvl3' | lvl2 | sizeof ( TYPE ) | sizeof lvl2 */
expr *Expr_lvl3_() {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_mult || t->id == e_plus || t->id == e_minus || t->id == e_not ||
            t->id == e_incr_pre || t->id == e_decr_pre || t->id == e_addr) {
        advance_token();
        expr *tree = new_unary_op_expr(t);
        expr *middle = Expr_lvl3_();
        if (middle == NULL)
            return error("Wrong expression", "");
        tree->content.unary.operand = middle;
        return tree;
    } else if (t->id == e_sizeof) {
        advance_token();
        expr *tree = new_unary_op_expr(t);
        t = cur_token();
        if (t->id == e_open_paren) {
            advance_token();
            t = cur_token();
            if (t->id == e_type) {
                tree->content.unary.operand = new_value_expr(cur_token());
                advance_token();
                match(e_close_paren);
            } else {
                // go back to open paren
                rollback_token();
                tree->content.unary.operand = Expr_lvl2();
            }
        } else {
            tree->content.unary.operand = Expr_lvl2();
        }
        return tree;
    } else
        return Expr_lvl2();
}

/* lvl2 -> lvl0 lvl2' */
expr *Expr_lvl2() {
    expr *tree1, *tree2;
    tree1 = Expr_lvl0();
    token *t = cur_token();
    if (t->id == e_arrow || t->id == e_dot)
        tree2 = Expr_lvl2_(tree1);
    else
        tree2 = Expr_lvl2_unary(tree1);
    return tree2 ? tree2 : tree1;
}

/* left-associative */
/* lvl2' -> [-> ,] lvl0 lvl2' | e */
expr *Expr_lvl2_(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    if (t->id == e_arrow || t->id == e_dot) {
        advance_token();
        expr *tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        expr *middle = Expr_lvl0();
        tree->content.binary.right = middle;
        expr *tail = Expr_lvl2_(tree);
        return tail ? tail : tree;
    } else
        return NULL;
}

/* left-associative */
/* postfix unary, function call */
/* lvl2'unary -> [-- ++ EXPR_LIST] lvl2'unary | [ EXPR ] lvl2'unary | e */
expr *Expr_lvl2_unary(expr *tree_left) {
    token *t = cur_token();
    if (t->id == e_eof) return NULL;
    expr *tree, *tail;
    if (t->id == e_incr_pre || t->id == e_decr_pre) {
        advance_token();
        if (t->id == e_incr_pre) t->id = e_incr_post;
        if (t->id == e_decr_pre) t->id = e_decr_post;
        tree = new_unary_op_expr(t);
        tree->content.unary.operand = tree_left;
    } else if (t->id == e_open_paren) {
        tree = new_fnc_call_expr();
        tree->content.fnc_call.fnc = tree_left;
        tree->content.fnc_call.params = Expr_list();
    } else if (t->id == e_open_bracket) {
        advance_token();
        tree = new_binary_op_expr(t);
        tree->content.binary.left = tree_left;
        tree->content.binary.right = Expr();
        match(e_close_bracket);
    } else
        return NULL;
    tail = Expr_lvl2_unary(tree);
    return tail ? tail : tree;
}

/* lvl0 -> ( EXPR ) | id | num */
expr *Expr_lvl0() {
    expr *tree;
    token *t = cur_token();
    if (t->id == e_eof) return error("Wrong expression", "");
    if (t->id == e_number || t->id == e_identifier) {
        advance_token();
        tree = new_value_expr(t);
        return tree;
    } else if ( t->id == e_open_paren ) {
        advance_token();
        tree = Expr();
        match(e_close_paren);
        return tree;
    } else
        return error("Wrong expression", "");
}

/* EXPR_LIST -> (EXPR, EXPR, ...) */
expr_list *Expr_list() {
    match(e_open_paren);
    if (cur_token()->id == e_close_paren) {
        advance_token();
        return NULL;
    }
    expr *e = Expr();
    expr_list *list = new_expr_list(e);
    expr_list *head = list;
    token *t = cur_token();
    while (t->id != e_close_paren) {
        match(e_comma);
        e = Expr();
        list->next = new_expr_list(e);
        list = list->next;
        t = cur_token();
        if ( t->id == e_eof )
            return (expr_list*)error("No closing parenthesis", "");
    }
    match(e_close_paren);
    return head;
}

/* DECL_LIST -> (DECL, DECL, ...) */
block *Decl_list() {
    match(e_open_paren);
    if (cur_token()->id == e_close_paren) {
        advance_token();
        return NULL;
    }
    stmt *e = Stmt_declare(true);
    block *list = new_block(e);
    block *head = list;
    token *t = cur_token();
    while (t->id != e_close_paren) {
        match(e_comma);
        e = Stmt_declare(true);
        list->next = new_block(e);
        list = list->next;
        t = cur_token();
        if ( t->id == e_eof )
            return (block*)error("No closing parenthesis", "");
    }
    match(e_close_paren);
    return head;
}

token *cur_token() {
    if (token_index < tokens->size)
        return (token*)tokens->elements[token_index];
    else
        return NULL;
}

void advance_token() {
    ++token_index;
}

void rollback_token() {
    --token_index;
}

bool match(token_type type) {
    token *t = cur_token();
    if (t->id != type) {
        char buf[MAX_ERROR_LENTH];
        sprintf(buf, "Expected \"%s\", but got \"%s\" (\"%s\") instead",
                token_type_str(type), token_type_str(t->id), t->data);
        error(buf, "");
        return false;
    }
    advance_token();
    return true;
}

expr *error(const char *message, const char *param) {
    printf("In file \"%s\": line %d; token: %s (%s)\n", current_file,
            cur_token()->line, token_type_str(cur_token()->id), cur_token()->data);
    printf("Parser error: %s %s\n", message, param);
    exit(EXIT_FAILURE);
    return NULL;
}

