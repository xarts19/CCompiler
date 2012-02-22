#include "s_table.h"

symbol *symbol_new(const char *data) {
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

symbol *symbol_copy(const symbol *s) {
    symbol *new_s = (symbol*)safe_malloc(sizeof(symbol));
    char *str = (char*)safe_malloc(sizeof(char)*strlen(s->data)+1);
    strcpy(str, s->data);
    new_s->data = str;
    return new_s;
}


s_table *s_table_new(s_table *parent) {
    s_table *t = (s_table*)safe_malloc(sizeof(s_table));
    t->parent = parent;
    t->table = map_new(10);
    return t;
}

static void hlp_delete_symbol(void *sym) {
    symbol_delete( (symbol*)sym );
}
void s_table_delete(s_table *t) {
    map_delete(t->table, hlp_delete_symbol);
    free(t);
}

void s_table_insert(s_table *t, const char *key, symbol *value) {
    map_insert(t->table, key, value);
}

symbol *s_table_find(s_table *t, const char *key) {
    /* search parent scopes if not found in current scope */
    return (symbol*)map_find(t->table, key);
}
