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
#define MAX_NUMBER_LENGTH 100
#define MAX_ID_LENGTH 100
#define MAX_LITERAL_LENGTH 255

void lexer(buffered_reader *file_reader, vector* tokens, map* words);
void reserve_keywords(map *words);

/* DFAs for each token type */
token* scan_number(buffered_reader *file_reader);
token* scan_word(buffered_reader *file_reader);
token* scan_literal(buffered_reader *file_reader);
token* scan_operator(buffered_reader *file_reader);
token* scan_comment(buffered_reader *file_reader);

token* error(const char* message, const char* param);

#endif
