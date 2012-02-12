#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* defined token types */
typedef enum token_type_enum {
    e_eof              = 0,

    /* punctuation */
    e_open_paren       = 1,       /* ( */
    e_close_paren      = 2,       /* ) */
    e_open_curly       = 3,       /* { */
    e_close_curly      = 4,       /* } */
    e_open_bracket     = 5,       /* [ */
    e_close_bracket    = 6,       /* ] */
    e_semicolon        = 7,       /* ; */
    e_colon            = 8,       /* : */
    e_comma            = 9,       /* , */
    e_question         =10,       /* ? */

    /* keywords */
    e_while            =20,
    e_for              =21,
    e_if               =22,
    e_then             =23,
    e_else             =24,
    e_do               =25,
    e_switch           =26,
    e_case             =27,
    e_typedef          =28,
    e_struct           =29,
    e_enum             =30,
    e_return           =31,
    e_break            =32,
    e_continue         =33,
    e_sizeof           =34,

    /* semantic */
    e_number           =40,     /* 14, 4.6, 3.5E-23, ... */
    e_literal          =41,     /* "asfaera", 'b', ... */
    e_identifier       =42,     /* size, ball, ... */
    e_comment          =43,     /* //this is a comment, ... */

    /* type */
    e_type             =50,
    e_signed           =51,
    e_unsigned         =52,
    e_char             =53,
    e_short            =54,
    e_int              =55,
    e_long             =56,
    e_float            =57,
    e_double           =58,
    e_void             =59,

    /* operators */
    e_assign           =70,
    e_plus             =71,
    e_minus            =72,
    e_mult             =73,
    e_div              =74,
    e_mod              =75,
    e_incr_pre         =76,
    e_decr_pre         =77,
    e_not              =78,
    e_and              =79,
    e_or               =80,
    e_eq               =81,
    e_noteq            =82,
    e_more             =83,
    e_less             =84,
    e_moreeq           =85,
    e_lesseq           =86,
    e_dot              =87,
    e_arrow            =88,
    e_incr_post        =89,
    e_decr_post        =90,
    e_dot_star         =91,
    e_arrow_star       =92,
    e_addr             =93,

    e_operator         =101,    /* placeholder */

    /* other */
    e_preproc_operator =150,    /* # */
} token_type;

typedef struct token {
    int line;
    token_type id;
    char *data;
} token;

token* token_new(token_type id, const char* data);
void token_delete(token* t);
token* token_copy(const token* t);
char* token_type_str(token_type id);
void token_print(token* t);

#endif

