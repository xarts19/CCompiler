#ifndef S_TABLE_H_
#define S_TABLE_H_

#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "utils.h"

typedef struct symbol {
    char* data;
} symbol;

typedef struct s_table {
    struct s_table* parent;
    map* table;
} s_table;


symbol* symbol_new(const char* data);
void symbol_delete(symbol* s);
symbol* symbol_copy(const symbol* s);

s_table* s_table_new(s_table* parent);
void s_table_delete(s_table* t);
void s_table_insert(s_table* t, const char* key, symbol* value);
symbol* s_table_find(s_table* t, const char* key);

#endif
