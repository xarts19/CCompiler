#include "reader.h"

#include <string.h>
#include <stdbool.h>

/* private helper functions */
static void br_read_buffer(buffered_reader *br, char *buffer, int size);
static int get_file_size(const char *filename);


buffered_reader *br_open_file(const char *filename) {
    buffered_reader *br = (buffered_reader*)safe_malloc(sizeof(buffered_reader));
    br->pfile = fopen(filename, "r");
    if (br->pfile == NULL) {
        InputError("Unable to open file", filename);
    }
    br->filename = (char*)safe_malloc( 1 + strlen(filename) * sizeof(char) );
    strcpy(br->filename, filename);
    int size = get_file_size(filename);
    br->buffer = (char*)safe_malloc( 1 + size * sizeof(char) );
    br_read_buffer(br, br->buffer, size);
    br->buffer_size = size;
    return br;
}

char *br_get_start(buffered_reader *br) {
    br->base_pointer = br->buffer;
    br->forward_pointer = br->base_pointer;
    return br->buffer;
}

char *br_get_base(buffered_reader *br) {
    return br->base_pointer;
}

void br_set_base(buffered_reader *br) {
    br->base_pointer = br->forward_pointer;
}

char *br_reset_to_base(buffered_reader *br) {
    br->forward_pointer = br->base_pointer;
    return br->forward_pointer;
}

/* Returns EOF on end of file, next character otherwise */
char *br_get_next_char(buffered_reader *br) {
    return ++(br->forward_pointer);
}

char *br_get_prev_char(buffered_reader *br) {
    return --(br->forward_pointer);
}

void br_read_buffer(buffered_reader *br, char *buffer, int size) {
    int elems_read = fread(buffer, sizeof(char), size, br->pfile);
    buffer[elems_read] = UNIX_EOF;
    if ( elems_read != size && !feof(br->pfile))
        InputError("Couldn't read source file", br->filename);
}

void br_close_file(buffered_reader *br) {
    free(br->filename);
    free(br->buffer);
    fclose(br->pfile);
    free(br);
}

/* determine file size */
int get_file_size(const char *filename) {
    FILE *pfile = fopen(filename, "r");
    if (pfile == NULL) {
        InputError("Unable to open file.", "");
    }
    fseek(pfile, 0L, SEEK_END);
    int size = ftell(pfile);
    fclose(pfile);
    return size;
}
