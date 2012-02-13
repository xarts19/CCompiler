#include "map.h"

/* private helper functions */

static int compute_hash(const char* key, int total_buckets);
static bucket* new_bucket(const char* key, void* value);


/* Create new map. */
map* map_new(unsigned int initial_size) {
    map* m = (map*)safe_malloc( sizeof(map) );;

    bucket** buckets = (bucket**)safe_malloc( initial_size * sizeof(bucket*) );
    memset(buckets, 0, sizeof(bucket*) * initial_size);

    m->total_buckets = initial_size;
    m->bucket_array = buckets;
    return m;
}

/* Free memory of the map. Call delete function for each value in the map. */
void map_delete(map* m, void (*delete)(void*)) {
    for (int i=0; i<m->total_buckets; i++) {
        bucket* b = m->bucket_array[i];
        while (b != NULL) {
            delete(b->value);
            b = b->next;
        }
    }
    free(m->bucket_array);
    free(m);
}

/*
 * Insert new pair key,value into the map.
 * Map copies both key and value and stores this copies.
 */
void map_insert(map* m, const char* key, void* value) {
    /*
     * TODO: Think of expanding strategy.
     */
    int offset = compute_hash(key, m->total_buckets);
    if (m->bucket_array[offset] == NULL) {
        m->bucket_array[offset] = new_bucket(key, value);
    } else {
        bucket* b = m->bucket_array[offset];
        while (b->next != NULL)
            b = b->next;
        b->next = new_bucket(key, value);
    }
}

bucket* new_bucket(const char* key, void* value) {
    bucket* b = (bucket*)safe_malloc( sizeof(bucket) );
    b->key = (char*)safe_malloc( 1 + strlen(key) * sizeof(char) );
    strcpy(b->key, key);
    b->value = value;
    b->next = NULL;
    return b;
}

void* map_find(map *m, const char *key) {
    /* Simple linear search for now */
    int offset = compute_hash(key, m->total_buckets);
    bucket* b = m->bucket_array[offset];
    while (b != NULL) {
        if ( strcmp(b->key, key) == 0 )
            return b->value;
        b = b->next;
    }
    return NULL;
}

int compute_hash(const char* key, int total_buckets) {
    unsigned long hash = 5381;
    int c;

    while ( (c = *key++) )
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % total_buckets;
}
