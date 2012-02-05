#include "symbol.h"

symbol* symbol_new(const char *data) {
    symbol *s = (symbol*)safe_malloc(sizeof(symbol));
    char *str = (char*)safe_malloc(sizeof(char)*strlen(data)+1);
    strcpy(str, data);
    s->data = str;
    return s;
}

void symbol_delete(symbol *s) {
    free(s->data);
    free(s);
}

symbol* symbol_copy(const symbol *s) {
    symbol *new_s = (symbol*)safe_malloc(sizeof(symbol));
    char *str = (char*)safe_malloc(sizeof(char)*strlen(s->data)+1);
    strcpy(str, s->data);
    new_s->data = str;
    return new_s;
}
