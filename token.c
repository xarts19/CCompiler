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
        case e_eof:
            return "End of file";
        case e_open_paren:
            return "e_open_paren";
        case e_close_paren:
            return "e_close_paren";
        case e_open_curly:
            return "e_open_curly";
        case e_close_curly:
            return "e_close_curly";
        case e_open_bracket:
            return "e_open_bracket";
        case e_close_bracket:
            return "e_close_bracket";
        case e_semicolon:
            return "e_semicolon";
        case e_colon:
            return "e_colon";
        case e_comma:
            return "e_comma";
        case e_while:
            return "e_while";
        case e_for:
            return "e_for";
        case e_if:
            return "e_if";
        case e_then:
            return "e_then";
        case e_else:
            return "e_else";
        case e_do:
            return "e_do";
        case e_switch:
            return "e_switch";
        case e_case:
            return "e_case";
        case e_typedef:
            return "e_typedef";
        case e_struct:
            return "e_struct";
        case e_enum:
            return "e_enum";
        case e_number:
            return "e_number";
        case e_literal:
            return "e_literal";
        case e_identifier:
            return "e_identifier";
        case e_comment:
            return "e_comment";
        case e_type:
            return "e_type";
        case e_signed:
            return "e_signed";
        case e_unsigned:
            return "e_unsigned";
        case e_char:
            return "e_char";
        case e_short:
            return "e_short";
        case e_int:
            return "e_int";
        case e_long:
            return "e_long";
        case e_float:
            return "e_float";
        case e_double:
            return "e_double";
        case e_void:
            return "e_void";
        case e_assign:
            return "e_assign";
        case e_plus:
            return "e_plus";
        case e_minus:
            return "e_minus";
        case e_mult:
            return "e_mult";
        case e_div:
            return "e_div";
        case e_mod:
            return "e_mod";
        case e_incr_pre:
            return "e_incr_pre";
        case e_decr_pre:
            return "e_decr_pre";
        case e_incr_post:
            return "e_incr_post";
        case e_decr_post:
            return "e_decr_post";
        case e_not:
            return "e_not";
        case e_and:
            return "e_and";
        case e_or:
            return "e_or";
        case e_eq:
            return "e_eq";
        case e_noteq:
            return "e_noteq";
        case e_more:
            return ">";
        case e_less:
            return "<";
        case e_moreeq:
            return ">=";
        case e_lesseq:
            return "<=";
        case e_dot:
            return ".";
        case e_arrow:
            return "->";
        case e_dot_star:
            return ".*";
        case e_arrow_star:
            return "->*";
        case e_addr:
            return "&";
        case e_operator:
            return "e_operator";
        case e_preproc_operator:
            return "#";
        default:
            return "Unknown token";
        }
}

void token_print(token* t) {
    assert(t != NULL);
    printf("['%s'] ", t->data);
}
