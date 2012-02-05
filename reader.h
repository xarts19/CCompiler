#ifndef READER_H_
#define READER_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"

typedef struct buffered_reader {
    enum {e_buffer1, e_buffer2} active;
    FILE *pfile;
    char *filename;
    char *buffer;      /* circular input buffer */
    char *buffer1;     /* pointer to first half of the buffer */
    char *buffer2;     /* pointer to second half of the buffer */
    int buffer_size;
} buffered_reader;

buffered_reader* br_open_file(char *filename);
char* br_get_start(buffered_reader *br);
char* br_get_next_char(buffered_reader *br, char *current);
char* br_get_next_buffer(buffered_reader *br);
void br_read_buffer(buffered_reader *br, char* buffer);
bool br_file_end(buffered_reader *br, char *forward);
void br_close_file(buffered_reader *br);
int get_file_size(char* filename);

#endif
