#include "vector.h"

vector vector_new(unsigned int initial_size) {
    token** elements = (token**)safe_malloc( initial_size * sizeof(token*) );
    vector v;
    v.size = 0;
    v.allocated = initial_size;
    v.elements = elements;
    return v;
}

void vector_delete(vector *v, void (*delete)(void*)) {
    for (int i=0; i<v->size; i++) {
        delete(v->elements[i]);
    }
    free(v->elements);
}

void vector_insert(vector *v, token *value) {
    if (v->size == v->allocated) {
        v->allocated *= 2;
        v->elements = (token**)safe_realloc(v->elements, sizeof(token*)*v->allocated);
    }
    v->elements[v->size++] = token_copy(value);
}

token* vector_get(vector *v, int index) {
    return v->elements[index];
}

void print_token_vect(const char *title, vector tokens) {
    printf(">>> %s ===========================\n", title);
    for (int i=0; i<tokens.size; i++) {
        token_print(tokens.elements[i]);
    }
    printf("\n");
}
