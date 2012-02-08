#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* defined token types */
typedef enum token_type_enum {e_keyword,          /* while, if, ... */
                      e_type,             /* int, float, ... */
                      e_number,           /* 14, 4.6, 3.5E-23, ... */
                      e_literal,          /* "asfaera", 'b', ... */
                      e_open_paren,       /* ( */
                      e_close_paren,      /* ) */
                      e_open_curly,       /* { */
                      e_close_curly,      /* } */
                      e_open_bracket,     /* [ */
                      e_close_bracket,    /* ] */
                      e_colon,            /* ; */
                      e_comma,            /* , */
                      e_identifier,       /* size, ball, ... */
                      e_operator,         /* +, -, ... */
                      e_logic_operator,   /* ||, &&, ... */
                      e_cmp_operator,     /* <, >, ==, ... */
                      e_assign_operator,  /* =, +=, *=, &=, ... */
                      e_bitwise_operator, /* <<, &, |, ...*/
                      e_pointer_operator, /* ->, . */
                      e_preproc_operator, /* # */
                      e_comment           /* //this is a comment, ... */
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

/* operator mnemonics

   Arithmetic:
   op+    o+=    o++
   op-    o-=    o--
   op*    o*=
   op/    o/=
   op%    o%=

   Bitwise:
   o<<    <<=
   o>>    >>=
   op|    o|=
   op&    o&=
   op^    o^=
   op~

   Logical:
   op!
   o&&
   o||

   Comparison:
   o==
   o!=
   op>
   op<
   o>=
   o<=

   Pointer:
   o->
   op.

 */

