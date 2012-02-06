/*
 * My hash table implementation with string key.
 * djb2 hash functions for strings is used for hashing.
 * Collisions are resolved as linked lists of nodes.
 *
 * hash      buckets
 *  0         [ ] -> [ ]
 *  1
 *  2         [ ]
 *  3         [ ]
 *  4         [ ] -> [ ] -> [ ]
 *  ...
 */

#ifndef MAP_H_
#define MAP_H_

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "token.h"

typedef struct pair {
    char* key;
    void* value;
    struct pair* next;
} bucket;

typedef struct map {
    int total_buckets;            /* number of allocated buckets */
    bucket** bucket_array;        /* array of pointers to buckets */
} map;

/* creating and destroying the map */
map* map_new(unsigned int initial_size);
void map_delete(map *m, void (*delete)(void*));

/* usual map operations */
void map_insert(map* m, const char* key, void* value);
void* map_find(map* m, const char* key);
void map_remove(map* m, const char* key);


#endif
