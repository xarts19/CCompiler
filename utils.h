#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#define START_STRUCTURES_SIZE 32
#define MAX_FILE_LEN 100000
#define MAX_FILE_NAME_LENGTH 255

#define FILE_BUFFER_SIZE 4096
#define UNIX_EOF 4    /* ASCII 0x04 (Control-D) */

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

void* safe_malloc(size_t size);
void* safe_realloc(void *old, size_t size);
void error(const char* message, const char* param);
void FatalError(const char* message);
void InputError(const char* message, const char* filename);

#endif