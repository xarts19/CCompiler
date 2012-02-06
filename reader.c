#include "reader.h"

buffered_reader* br_open_file(char *filename) {
    buffered_reader *br = (buffered_reader*)safe_malloc(sizeof(buffered_reader));
    br->pfile = fopen(filename, "r");
    if (br->pfile == NULL) {
        InputError("Unable to open file", filename);
    }
    br->filename = (char*)safe_malloc(sizeof(char)*strlen(filename));
    strcpy(br->filename, filename);
    br->buffer = (char*)safe_malloc(sizeof(char)*2*(FILE_BUFFER_SIZE+1));
    br->buffer1 = br->buffer;
    br->buffer2 = br->buffer + FILE_BUFFER_SIZE + 1;
    br->buffer_size = FILE_BUFFER_SIZE;
    return br;
}

char* br_get_start(buffered_reader *br) {
    br->active = e_buffer1;
    br_read_buffer(br, br->buffer1);
    br->base_pointer = br->buffer1;
    br->forward_pointer = br->base_pointer;
    return br->buffer1;
}

/* Returns EOF on end of file, next character otherwise */
char* br_get_next_char(buffered_reader *br, char *current) {
    if ( (*(++current) == UNIX_EOF) && ( !br_file_end(br, current) ) )
        current = br_get_next_buffer(br);
    return current;
}

char* br_get_next_buffer(buffered_reader *br) {
    char* next_buffer;
    switch (br->active) {
        case e_buffer1:
            next_buffer = br->buffer2;
            br->active = e_buffer2;
            break;
        case e_buffer2:
            next_buffer = br->buffer1;
            br->active = e_buffer1;
            break;
    }
    br_read_buffer(br, next_buffer);
    return next_buffer;
}

void br_read_buffer(buffered_reader *br, char* buffer) {
    int elems_read = fread(buffer, sizeof(char), FILE_BUFFER_SIZE, br->pfile);
    buffer[elems_read] = UNIX_EOF;
    if ( elems_read != FILE_BUFFER_SIZE && !feof(br->pfile))
        InputError("Couldn't read source file", br->filename);
}

bool br_file_end(buffered_reader *br, char *forward) {
    if (*forward != UNIX_EOF)
        return false;
    if (br->active == e_buffer1)
        return forward != br->buffer1 + FILE_BUFFER_SIZE;
    else
        return forward != br->buffer2 + FILE_BUFFER_SIZE;
}

void br_close_file(buffered_reader *br) {
    free(br->filename);
    free(br->buffer1);
    free(br->buffer2);
    fclose(br->pfile);
    free(br);
}

/* determine file size */
int get_file_size(char* filename) {
    FILE* pfile = fopen(filename, "r");
    if (pfile == NULL) {
        error("Unable to open file.", "");
    }
    fseek(pfile, 0L, SEEK_END);
    int size = ftell(pfile);
    fclose(pfile);
    return size;
}
