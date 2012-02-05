#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"

typedef struct symbol {
    char* data;
} symbol;

symbol* symbol_new(const char *data);
void symbol_delete(symbol *s);
symbol* symbol_copy(const symbol *s);

#endif
