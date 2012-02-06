#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* defined token types */
enum token_type_enum {keyword, type, int_num, float_num, exp_num, literal,
                      identifier, operator, logic_operator, cmp_operator,
                      assign_operator, bitwise_operator, pointer_operator,
                      preproc_operator, comment};
typedef enum token_type_enum token_type;

typedef struct token {
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
   op[
   op]
   o->
   op.

 */

