#include "s_table.h"

s_table s_table_new(s_table *parent) {
    s_pair* pairs = (s_pair*)safe_malloc(sizeof(s_pair)*START_STRUCTURES_SIZE);
    s_table t;
    t.parent = parent;
    t.size = 0;
    t.allocated = START_STRUCTURES_SIZE;
    t.pairs = pairs;
    return t;
}

void s_table_delete(s_table *t) {
    for (int i=0; i<t->size; i++) {
        free(t->pairs[i].key);
        symbol_delete(t->pairs[i].value);
    }
    free(t->pairs);
}

void s_table_insert(s_table *t, const char *key, symbol *value) {
    if (t->size == t->allocated) {
        t->allocated *= 2;
        t->pairs = (s_pair*)safe_realloc(t->pairs, sizeof(s_pair)*t->allocated);
    }
    s_pair p;
    char* str = (char*)safe_malloc(sizeof(char)*strlen(key)+1);
    strcpy(str, key);
    p.key = str;
    p.value = symbol_copy(value);
    t->pairs[t->size++] = p;
}

symbol* s_table_find(s_table *t, const char *key) {
    /* search parent scopes if not found in current scope */
    for (s_table *env = t; env != NULL; env = env->parent) {
        /* simple linear search for now */
        for (int i=0; i<env->size; i++) {
            if (strcmp(env->pairs[i].key, key) == 0) {
                return env->pairs[i].value;
            }
        }
    }
    return NULL;
}
