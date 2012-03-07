#ifndef READER_H_
#define READER_H_

#include <stdio.h>

#include "utils.h"

typedef struct buffered_reader {
    FILE *pfile;
    char *filename;
    char *buffer;      /* circular input buffer */
    char *base_pointer;
    char *forward_pointer;
    int buffer_size;
} buffered_reader;

buffered_reader *br_open_file(const char *filename);
void br_close_file(buffered_reader *br);

char *br_get_start(buffered_reader *br);

char *br_get_base(buffered_reader *br);

/* set base pointer to forward pointer */
void br_set_base(buffered_reader *br);

/* set forward pointer to base and return it */
char *br_reset_to_base(buffered_reader *br);

char *br_get_next_char(buffered_reader *br);
char *br_get_prev_char(buffered_reader *br);

#endif
