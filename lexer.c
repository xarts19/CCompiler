#include "lexer.h"

const char* KEYWORDS[] = {"true", "false", "for", "while", "and", "or", "not", "xor", "if", "then", "else"};
const char* TYPES[] = {"int", "float", "bool", "char"};

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static token* error(const char* message, const char* param);

/* Scan input string, emit array of lexical tokens */
void lexer(buffered_reader *file_reader, vector* tokens, map* words) {
    /* identifiers and reserved keywords */
    reserve_keywords(words);

    int num_tokens = 0;
    int line = 0;
    printf("\n%3d ", line++);

    token* cur_token = NULL;
    /* read first character */
    char *forward = br_get_start(file_reader);
    bool finished = false;
    while (!finished) {

        switch (*forward) {
            case UNIX_EOF:
                finished = true;
                break;

            case ' ':
            case '\t':
                /* if it's a whitespace */
                forward = br_get_next_char(file_reader);
                br_set_base(file_reader);
                break;

            case '\n':
                ++current_line;
                forward = br_get_next_char(file_reader);
                br_set_base(file_reader);
                break;

            case '/':
                /* comment or operator */
                cur_token = scan_comment(file_reader);
                if (cur_token == NULL)
                    cur_token = scan_operator(file_reader);
                assert(cur_token != NULL);
                vector_push(tokens, cur_token);
                break;

            case '"':
            case '\'':
                /* literal (const string) */
                cur_token = scan_literal(file_reader);
                assert(cur_token != NULL);
                vector_push(tokens, cur_token);
                break;

            default:
                if ( isdigit(*forward) ) {
                    /* if it's a number (int or float) */
                    cur_token = scan_number(file_reader);
                    assert(cur_token != NULL);
                    vector_push(tokens, cur_token);
                }
                else if ( isalpha(*forward) || *forward == '_' ) {
                    /* if it's a letter */
                    cur_token = scan_word(file_reader);
                    assert(cur_token != NULL);
                    /* search for matching word in map */
                    token* word = (token*)map_find(words, cur_token->data);
                    /* if word that was read is reserved */
                    if (word != NULL) {
                        /* add it to tokens */
                        vector_push(tokens, token_copy(word));
                        token_delete(cur_token);
                    } else {
                        /* save it as identifier and add it to tokens */
                        map_insert(words, cur_token->data, cur_token);
                        vector_push(tokens, token_copy(cur_token));
                    }
                } else {
                    /* if it's some other character */
                    cur_token = scan_operator(file_reader);
                    assert(cur_token != NULL);
                    vector_push(tokens, cur_token);
                }
                break;
        }
        forward = br_get_base(file_reader);

        /* print tokens as they are parsed */
        if (tokens->size > num_tokens) {
            token* t = (token*)tokens->elements[tokens->size-1];
            token_print(t);
            ++num_tokens;
        }
        while (current_line > line) {
            printf("\n%3d ", line);
            ++line;
        }

    }

}

/* Save reserved keywords in words map */
void reserve_keywords(map *words) {
    /* keywords */
    for (unsigned int i=0; i<(sizeof(KEYWORDS)/sizeof(KEYWORDS[0])); i++) {
        map_insert(words, KEYWORDS[i], token_new(keyword, KEYWORDS[i]));
    }
    /* types */
    for (unsigned int i=0; i<(sizeof(TYPES)/sizeof(TYPES[0])); i++) {
        map_insert(words, TYPES[i], token_new(type, TYPES[i]));
    }
}

/*
 * Scan string for int and float numbers.
 * Return pointer to the last symbol of the number.
 */
token* scan_number(buffered_reader *file_reader) {
    char buf[MAX_NUMBER_LENGTH+1];
    int size = 0;
    int state = 0;
    char* forward = br_get_base(file_reader);
    while (1) {
        switch (state) {
            case 0:
                if ( isdigit(*forward) ) state = 1;
                else return NULL;
                buf[size++] = *forward;
                break;

            /* integral part */
            case 1:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) { ; }
                else if (*forward == '.') state = 2;
                else if (*forward == 'E') state = 4;
                else state = 7;
                buf[size++] = *forward;
                break;
            case 2:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) state = 3;
                else {
                    buf[size] = '\0';
                    return error("Incorrect number: ", buf);
                }
                buf[size++] = *forward;
                break;
            case 3:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) { ; }
                else if (*forward == 'E') state = 4;
                else state = 8;
                buf[size++] = *forward;
                break;
            case 4:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) state = 6;
                else if (*forward == '+' || *forward == '-') state = 5;
                else {
                    buf[size] = '\0';
                    return error("Incorrect number: ", buf);
                }
                buf[size++] = *forward;
                break;
            case 5:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) state = 6;
                else {
                    buf[size] = '\0';
                    return error("Incorrect number: ", buf);
                }
                buf[size++] = *forward;
                break;
            case 6:
                forward = br_get_next_char(file_reader);
                if ( isdigit(*forward) ) { ; }
                else state = 9;
                buf[size++] = *forward;
                break;
            case 7:
                br_set_base(file_reader);
                buf[size-1] = '\0';
                return token_new(int_num, buf);
            case 8:
                br_set_base(file_reader);
                buf[size-1] = '\0';
                return token_new(float_num, buf);
            case 9:
                br_set_base(file_reader);
                buf[size-1] = '\0';
                return token_new(exp_num, buf);
        }
        if (size >= MAX_NUMBER_LENGTH) {
            buf[size] = '\0';
            return error("Too large number: ", buf);
        }
    }
    return NULL;
}

/*
 * Scan string for identifiers and reserved keywords.
 * Return pointer to the last symbol of the word.
 */
token* scan_word(buffered_reader *file_reader) {
    char buf[MAX_ID_LENGTH+1];
    int size = 0;
    int state = 0;
    char* forward = br_get_base(file_reader);
    while (1) {
        switch (state) {
            case 0:
                if ( isalpha(*forward) || *forward == '_' ) state = 1;
                else return NULL;
                buf[size++] = *forward;
                break;
            case 1:
                forward = br_get_next_char(file_reader);
                if ( isalpha(*forward) || *forward == '_' || isdigit(*forward) ) { ; }
                else state = 2;
                buf[size++] = *forward;
                break;
            case 2:
                buf[size-1] = '\0';
                br_set_base(file_reader);
                return token_new(identifier, buf);
        }
        if (size >= MAX_ID_LENGTH) {
            buf[size] = '\0';
            return error("Too large identifier: ", buf);
        }
    }
    return NULL;
}

/*
 * Scan for constant strings of text.
 * Return pointer to the last symbol of the literal.
 */
token* scan_literal(buffered_reader *file_reader) {
    char buf[MAX_LITERAL_LENGTH+1];
    int size = 0;
    int state = 0;
    char* forward = br_get_base(file_reader);
    while (1) {
        switch (state) {
            case 0:
                if (*forward == '\'') state = 1;
                else if (*forward == '"') state = 5;
                //else if (*forward == '<') state = 8;
                else return NULL;
                buf[size++] = *forward;
                break;

            /* single-quote (short) literal (character) */
            case 1:
                forward = br_get_next_char(file_reader);
                if (*forward == '\\') state = 2;
                else if (*forward == '\'') return error("Empty const character", "");
                else state = 3;
                buf[size++] = *forward;
                break;
            case 2:
                forward = br_get_next_char(file_reader);
                buf[size++] = *forward;
                state = 3;
                break;
            case 3:
                forward = br_get_next_char(file_reader);
                if (*forward == '\'') state = 4;
                else return error("Not closed const character", "");
                buf[size++] = *forward;
                break;
            case 4:
                buf[size] = '\0';
                br_get_next_char(file_reader);
                br_set_base(file_reader);
                return token_new(literal, buf);

            /* double-quote (long) literal (string) */
            case 5:
                forward = br_get_next_char(file_reader);
                if (*forward == '\\') state = 6;
                else if (*forward == '"') state = 7;
                buf[size++] = *forward;
                break;
            case 6:
                forward = br_get_next_char(file_reader);
                buf[size++] = *forward;
                state = 5;
                break;
            case 7:
                buf[size] = '\0';
                br_get_next_char(file_reader);
                br_set_base(file_reader);
                return token_new(literal, buf);

            /* angle-quote literal (string) */
            /*
              case 8:
                forward = br_get_next_char(file_reader);
                if (*forward == '\\') state = 9;
                else if (*forward == '>') state = 10;
                buf[size++] = *forward;
                break;
            case 9:
                forward = br_get_next_char(file_reader);
                buf[size++] = *forward;
                state = 8;
                break;
            case 10:
                buf[size] = '\0';
                br_get_next_char(file_reader);
                br_set_base(file_reader);
                return token_new(literal, buf);
            */
        }
        if (*forward == UNIX_EOF)
            return error("EOF while scanning comment", "");
        else if (*forward == '\n') {
            if (buf[0] == '<') return NULL;
            else
                return error("New line in literal", "");
        }
        if (size >= MAX_LITERAL_LENGTH) {
            buf[size] = '\0';
            return error("Too large literal: ", buf);
        }
    }
    return NULL;
}

/*
 * Scan for various operators.
 * Return pointer to the last symbol of the operator.
 */
token* scan_operator(buffered_reader *file_reader) {
    char buf[OPERATOR_LENGTH+1];
    token_type token_t = operator;
    char* forward = br_get_base(file_reader);

    switch (*forward) {
        case '+':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '+':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o++");
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o+=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op+");
                    token_t = operator;
                    break;
            }
            break;
        case '-':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '-':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o--");
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o-=");
                    token_t = assign_operator;
                    break;
                case '>':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o->");
                    token_t = pointer_operator;
                    break;
                default:
                    strcpy(buf, "op-");
                    token_t = operator;
                    break;
            }
            break;
        case '*':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o*=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op*");
                    token_t = operator;
                    break;
            }
            break;
        case '/':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o/=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op/");
                    token_t = operator;
                    break;
            }
            break;
        case '%':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o%=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op%");
                    token_t = operator;
                    break;
            }
            break;
        case '=':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o==");
                    token_t = cmp_operator;
                    break;
                default:
                    strcpy(buf, "op=");
                    token_t = assign_operator;
                    break;
            }
            break;
        case '!':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o!=");
                    token_t = cmp_operator;
                    break;
                default:
                    strcpy(buf, "op!");
                    token_t = logic_operator;
                    break;
            }
            break;
        case '>':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o>=");
                    token_t = cmp_operator;
                    break;
                case '>':
                    forward = br_get_next_char(file_reader);
                    switch (*forward) {
                        case '=':
                            br_get_next_char(file_reader);
                            strcpy(buf, ">>=");
                            token_t = assign_operator;
                            break;
                        default:
                            strcpy(buf, "o>>");
                            token_t = bitwise_operator;
                            break;
                    }
                    break;
                default:
                    strcpy(buf, "op>");
                    token_t = cmp_operator;
                    break;
            }
            break;
        case '<':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o<=");
                    token_t = cmp_operator;
                    break;
                case '<':
                    forward = br_get_next_char(file_reader);
                    switch (*forward) {
                        case '=':
                            br_get_next_char(file_reader);
                            strcpy(buf, "<<=");
                            token_t = assign_operator;
                            break;
                        default:
                            strcpy(buf, "o<<");
                            token_t = bitwise_operator;
                            break;
                    }
                    break;
                default:
                    strcpy(buf, "op<");
                    token_t = cmp_operator;
                    break;
            }
            break;
        case '&':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '&':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o&&");
                    token_t = logic_operator;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o&=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op&");
                    token_t = bitwise_operator;
                    break;
            }
            break;
        case '|':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '|':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o||");
                    token_t = logic_operator;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o|=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op|");
                    token_t = bitwise_operator;
                    break;
            }
            break;
        case '^':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "o^=");
                    token_t = assign_operator;
                    break;
                default:
                    strcpy(buf, "op^");
                    token_t = bitwise_operator;
                    break;
            }
            break;
        case '~':
            br_get_next_char(file_reader);
            strcpy(buf, "op~");
            token_t = bitwise_operator;
            break;
        case '.':
            br_get_next_char(file_reader);
            strcpy(buf, "op.");
            token_t = pointer_operator;
            break;
        case ',':
            br_get_next_char(file_reader);
            strcpy(buf, "op,");
            token_t = pointer_operator;
            break;
        case '[':
            br_get_next_char(file_reader);
            strcpy(buf, "op[");
            token_t = pointer_operator;
            break;
        case ']':
            br_get_next_char(file_reader);
            strcpy(buf, "op]");
            token_t = pointer_operator;
            break;
        case ';':
            br_get_next_char(file_reader);
            strcpy(buf, "op;");
            token_t = operator;
            break;
        case ':':
            br_get_next_char(file_reader);
            strcpy(buf, "op;");
            token_t = operator;
            break;
        case '?':
            br_get_next_char(file_reader);
            strcpy(buf, "op;");
            token_t = operator;
            break;
        case '(':
            br_get_next_char(file_reader);
            strcpy(buf, "op(");
            token_t = operator;
            break;
        case ')':
            br_get_next_char(file_reader);
            strcpy(buf, "op)");
            token_t = operator;
            break;
        case '{':
            br_get_next_char(file_reader);
            strcpy(buf, "op{");
            token_t = operator;
            break;
        case '}':
            br_get_next_char(file_reader);
            strcpy(buf, "op}");
            token_t = operator;
            break;
        case '#':
            br_get_next_char(file_reader);
            strcpy(buf, "op#");
            token_t = preproc_operator;
            break;
        default:
            buf[0] = *forward;
            buf[1] = '\0';
            return error("Unknows operator: ", buf);
            break;
    }
    br_set_base(file_reader);
    return token_new(token_t, buf);
}


/* returns pointer to the comment token */
token* scan_comment(buffered_reader *file_reader) {
    int state = 0;
    char* forward = br_get_base(file_reader);
    while (1) {
        switch (state) {
            case 0:
                if (*forward == '/') state = 1;
                else return NULL;
                break;
            case 1:
                forward = br_get_next_char(file_reader);
                if (*forward == '*') state = 2;
                else if (*forward == '/') state = 5;
                else return NULL;
                break;
            case 2:
                forward = br_get_next_char(file_reader);
                if (*forward == '*') state = 3;
                break;
            case 3:
                forward = br_get_next_char(file_reader);
                if (*forward == '/') state = 4;
                else state = 2;
                break;
            case 4:
                br_get_next_char(file_reader);
                br_set_base(file_reader);
                /* end of multiline comment */
                return token_new(comment, "multiline");
            case 5:
                forward = br_get_next_char(file_reader);
                if (*forward == '\n') state = 6;
                else state = 5;
                break;
            case 6:
                forward = br_get_next_char(file_reader);
                br_set_base(file_reader);
                /* end of single-line comment */
                return token_new(comment, "single-line");
        }
        if (*forward == UNIX_EOF)
            return error("EOF while scanning comment", "");
        else if (*forward == '\n')
            ++current_line;
    }
    return NULL;
}

token* error(const char *message, const char *param) {
    printf("In file \"%s\": line %d\n", current_file, current_line);
    printf("Compile error: %s%s\n", message, param);
    exit(EXIT_FAILURE);
}
