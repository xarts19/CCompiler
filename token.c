#include "token.h"

token* token_new(token_type id, const char* data) {
    token* t = (token*)safe_malloc(sizeof(token));
    t->id = id;
    char* str = (char*)safe_malloc( 1 + strlen(data) * sizeof(char) );
    strcpy(str, data);
    t->data = str;
    return t;
}

void token_delete(token *t) {
    free(t->data);
    free(t);
}

token* token_copy(const token *t) {
    return token_new(t->id, t->data);
}

void token_print(token *t) {
    printf("[%s: '%s' ] ", token_type_str(t->id), t->data);

}

char* token_type_str(token_type id) {
    switch (id) {
        case keyword:
            return "key";
        case type:
            return "type";
        case int_num:
            return "int";
        case float_num:
            return "float";
        case literal:
            return "literal";
        case identifier:
            return "ident";
        case operator:
            return "op";
        case logic_operator:
            return "logic_op";
        case cmp_operator:
            return "cmp_op";
        case assign_operator:
            return "assign_op";
        default:
            return "undefined operator type";
    }
    return NULL;
}

