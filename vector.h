/*
 * My simple dynamic vector implementation.
 * Can be used as stack.
 * Uses cumulative reallocation which increases size by factor of 2 each time.
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "assert.h"
//#define NDEBUG

typedef struct vector {
    int size;
    int allocated;
    void** elements;
} vector;

/*                                  */
/* Creating and destroying vectors. */
/*                                  */

vector* vector_new(unsigned int initial_size);

/* Calls 'delete' for each element of vector before self-destruction
 */
void vector_delete(vector* v, void (*delete)(void*));


/*                          */
/* Usual vector operations. */
/*                          */

void* vector_get(vector* v, int index);

/* Be sure to properly delete element in the position you are inserting to.
 * Returns previous element from this cell.
 / Can't be used to increase vector size.
 */
void* vector_insert(vector* v, void* value, int index);


/*                          */
/* Stack operations.        */
/*                          */

void vector_push(vector* v, void* value);
void* vector_pop(vector* v);

#endif
