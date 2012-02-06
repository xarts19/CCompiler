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

/*
 * Simulation of Nondeterministic Finite Automata
 * Aho - Compilers - Principles, Techniques, and Tools 2e
 * Chapter 3.7.2, p. 156
 *
 * Construction of an NFA from a Regular Expression
 * The McNaughton-Yamada-Thompson algorithm
 * Aho - Compilers - Principles, Techniques, and Tools 2e
 * Chapter 3.7.4, p. 159
 *

 There is, however, a mixed strategy that is about as good as the better of
the NFA and the DFA strategy for each expression r and string x. Start off
simulating the NFA, but remember the sets of NFA states (i.e., the DFA states)
and their transitions, as we compute them. Before processing the current set of
NFA states and the current input symbol, check to see whether we have already
computed this transition, and use the information if so.

 */

#endif
