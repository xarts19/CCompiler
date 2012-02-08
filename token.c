#include "token.h"

#include "assert.h"

extern int current_line;

token* token_new(token_type id, const char* data) {
    token* t = (token*)safe_malloc(sizeof(token));
    t->id = id;
    char* str = (char*)safe_malloc( 1 + strlen(data) * sizeof(char) );
    strcpy(str, data);
    t->data = str;
    t->line = current_line;
    return t;
}

void token_delete(token* t) {
    assert(t != NULL);
    free(t->data);
    free(t);
}

token* token_copy(const token* t) {
    assert(t != NULL);
    return token_new(t->id, t->data);
}

char* token_type_str(token_type id) {
    switch (id) {
        case e_keyword:
            return "key";
        case e_type:
            return "type";
        case e_number:
            return "num";
        case e_literal:
            return "literal";
        case e_open_paren:
            return "(";
        case e_close_paren:
            return ")";
        case e_open_curly:
            return "{";
        case e_close_curly:
            return "}";
        case e_open_bracket:
            return "[";
        case e_close_bracket:
            return "]";
        case e_colon:
            return ":";
        case e_comma:
            return ",";
        case e_identifier:
            return "ident";
        case e_operator:
            return "op";
        case e_logic_operator:
            return "logic_op";
        case e_cmp_operator:
            return "cmp_op";
        case e_assign_operator:
            return "assign_op";
        case e_bitwise_operator:
            return "bitwise";
        case e_pointer_operator:
            return "pointer";
        case e_preproc_operator:
            return "preproc";
        case e_comment:
            return "comment";
        default:
            return "undefined operator type";
    }
    return NULL;
}

void token_print(token* t) {
    assert(t != NULL);
    printf("[%s: '%s' ] ", token_type_str(t->id), t->data);
}
