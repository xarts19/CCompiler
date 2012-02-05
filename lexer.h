#ifndef LEXER_H_
#define LEXER_H_

#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"
#include "token.h"
#include "map.h"
#include "vector.h"
#include "reader.h"

// MAX 64-bit int number length
// TODO: check for floats
#define MAX_NUMBER_LENGTH FILE_BUFFER_SIZE
#define MAX_ID_LENGTH FILE_BUFFER_SIZE
#define MAX_LITERAL_LENGTH FILE_BUFFER_SIZE

void lexer(buffered_reader *file_reader, vector* tokens, map* words);
void reserve_keywords(map *words);
char* scan_number(char *forward, buffered_reader *file_reader, vector *tokens);
char* scan_word(char *forward, buffered_reader *file_reader, vector *tokens, map *words);
char* scan_literal(char *forward, buffered_reader *file_reader, vector *tokens);
char* scan_operator(char *forward, char prev, buffered_reader *file_reader, vector *tokens);
char* scan_comment(char *forward, buffered_reader *file_reader);

#endif
