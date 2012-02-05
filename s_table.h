#ifndef S_TABLE_H_
#define S_TABLE_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "symbol.h"

typedef struct s_pair {
    char *key;
    symbol *value;
} s_pair;

typedef struct s_table {
    struct s_table *parent;
    int size;
    int allocated;
    s_pair *pairs;
} s_table;

s_table s_table_new(s_table *parent);
void s_table_delete(s_table *t);

void s_table_insert(s_table *t, const char *key, symbol *value);
symbol* s_table_find(s_table *t, const char *key);

#endif
