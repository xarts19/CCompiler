#include "vector.h"

vector *vector_new(unsigned int initial_size) {
    void **elements = (void**)safe_malloc( initial_size * sizeof(void*) );
    vector *v = (vector*)safe_malloc( sizeof(vector) );
    v->size = 0;
    v->allocated = initial_size;
    v->elements = elements;
    return v;
}

void vector_delete(vector *v, void (*delete)(void*)) {
    if (delete != NULL) {
        for (int i=0; i<v->size; i++) {
            delete(v->elements[i]);
        }
    }
    free(v->elements);
}

void vector_insert(vector *v, void *value, int index) {
    assert( 0 < index || index < v->size );
    vector_push(v, v->elements[v->size - 1]);
    for (int i = v->size-1; i > index; i--) {
        v->elements[i] = v->elements[i-1];
    }
    v->elements[index] = value;
}

void vector_erase(vector *v, int index, void (*delete)(void*)) {
    assert( 0 < index && index < v->size );
    delete(v->elements[index]);
    for (int i=index+1; i < v->size; i++) {
        v->elements[i-1] = v->elements[i];
    }
    --(v->size);
}

void *vector_get(vector *v, int index) {
    return v->elements[index];
}

void vector_push(vector *v, void *value) {
    if (v->size == v->allocated) {
        v->allocated *= 2;
        v->elements = (void**)safe_realloc(v->elements, sizeof(void*)*v->allocated);
    }
    v->elements[v->size++] = value;
}

void *vector_pop(vector *v) {
    return v->elements[ --(v->size) ];
}

void *vector_peek(vector *v) {
    return v->elements[v->size-1];
}
