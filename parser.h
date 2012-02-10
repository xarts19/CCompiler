#ifndef _PARSER_H
#define _PARSER_H

#include "vector.h"
#include "ast.h"

stmt* parse_bottomup(vector* tokens);
stmt* parse_topdown(vector* tokens);

#endif
