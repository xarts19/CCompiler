#include "vector.h"

vector* vector_new(unsigned int initial_size) {
    void** elements = (void**)safe_malloc( initial_size * sizeof(void*) );
    vector* v = (vector*)safe_malloc( sizeof(vector) );
    v->size = 0;
    v->allocated = initial_size;
    v->elements = elements;
    return v;
}

void vector_delete(vector* v, void (*delete)(void*)) {
    for (int i=0; i<v->size; i++) {
        delete(v->elements[i]);
    }
    free(v->elements);
}

void* vector_insert(vector* v, void* value, int index) {
    assert( 0 < index || index < v->size );
    void* prev = v->elements[index];
    v->elements[index] = value;
    return prev;
}

void* vector_get(vector* v, int index) {
    return v->elements[index];
}

void vector_push(vector* v, void* value) {
    if (v->size == v->allocated) {
        v->allocated *= 2;
        v->elements = (void**)safe_realloc(v->elements, sizeof(void*)*v->allocated);
    }
    v->elements[v->size++] = value;
}

void* vector_pop(vector* v) {
    return v->elements[ --(v->size) ];
}
