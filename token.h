#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* defined token types */
enum token_type_enum {keyword, type, int_num, float_num, exp_num, literal, identifier, operator,
                      logic_operator, cmp_operator, assign_operator, comment};
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
