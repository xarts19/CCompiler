#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "token.h"

typedef struct vector {
    int size;
    int allocated;
    token **elements;
} vector;

vector vector_new(unsigned int initial_size);
void vector_delete(vector *m, void (*delete)(void*));

void vector_insert(vector *m, token *value);
token* vector_get(vector *m, int index);

void print_token_vect(const char *title, vector tokens);

#endif
