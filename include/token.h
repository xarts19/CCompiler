#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* defined token types */
typedef enum token_type_enum {
    e_eof,

    /* punctuation */
    e_open_paren,     /*  (  */
    e_close_paren,    /*  )  */
    e_open_curly,     /*  {  */
    e_close_curly,    /*  }  */
    e_open_bracket,   /*  [  */
    e_close_bracket,  /*  ]  */
    e_semicolon,      /*  ;  */
    e_colon,          /*  :  */
    e_comma,          /*  ,  */
    e_question,       /*  ?  */

    /* keywords */
    e_while,
    e_for,
    e_if,
    e_then,
    e_else,
    e_do,
    e_switch,
    e_case,
    e_typedef,
    e_struct,
    e_enum,
    e_return,
    e_break,
    e_continue,
    e_sizeof,

    /* semantic */
    e_number,       /* 14, 4.6, 3.5E-23, ... */
    e_literal,      /* "asfaera", 'b', ... */
    e_identifier,   /* size, ball, ... */
    e_comment,      /* //this is a comment, ... */

    /* type */
    e_type,
    e_signed,
    e_unsigned,
    e_char,
    e_short,
    e_int,
    e_long,
    e_float,
    e_double,
    e_void,

    /* operators */
    /* assignment */
    e_assign,
    e_plus_assign,
    e_minus_assign,
    e_mult_assign,
    e_div_assign,
    e_mod_assign,
    e_shift_r_assign,
    e_shift_l_assign,
    e_and_assign,
    e_or_assign,
    e_xor_assign,
    /* arithmetic */
    e_plus,
    e_minus,
    e_mult,
    e_div,
    e_mod,
    e_incr_pre,
    e_decr_pre,
    e_incr_post,
    e_decr_post,
    /* logic */
    e_not,
    e_not_bitwise,
    e_and,
    e_or,
    e_or_bitwise,
    e_xor_bitwise,
    e_shift_r,
    e_shift_l,
    /* comparison */
    e_eq,
    e_noteq,
    e_more,
    e_less,
    e_moreeq,
    e_lesseq,
    /* pointer operations */
    e_dot,
    e_dot_star,
    e_arrow,
    e_arrow_star,
    e_addr,

    e_operator,    /* placeholder */

    /* other */
    e_preproc_operator,    /* # */
} token_type;

typedef struct token {
    int line;
    token_type id;
    char *data;
} token;

token *token_new(token_type id, const char *data);
void token_delete(token *t);
token *token_copy(const token *t);
char *token_type_str(token_type id);
void token_print(token *t);

#endif

