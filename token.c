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
        case e_semicolon:
            return ";";
        case e_colon:
            return ":";
        case e_comma:
            return ",";
        case e_while:
            return "while";
        case e_for:
            return "for";
        case e_if:
            return "if";
        case e_then:
            return "then";
        case e_else:
            return "else";
        case e_do:
            return "do";
        case e_switch:
            return "switch";
        case e_case:
            return "case";
        case e_typedef:
            return "typedef";
        case e_struct:
            return "struct";
        case e_enum:
            return "enum";
        case e_return:
            return "return";
        case e_break:
            return "break";
        case e_continue:
            return "continue";
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
            return "=";
        case e_plus:
            return "+";
        case e_minus:
            return "-";
        case e_mult:
            return "*";
        case e_div:
            return "/";
        case e_mod:
            return "%";
        case e_incr_pre:
            return "++(prefix)";
        case e_decr_pre:
            return "--(prefix)";
        case e_incr_post:
            return "++(postfix)";
        case e_decr_post:
            return "--(postfix)";
        case e_not:
            return "!";
        case e_and:
            return "&&";
        case e_or:
            return "||";
        case e_eq:
            return "==";
        case e_noteq:
            return "!=";
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
