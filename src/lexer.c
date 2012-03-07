#include "lexer.h"

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static token *error(const char *message, const char *param);

/* Scan input string, emit array of lexical tokens */
void lexer(buffered_reader *file_reader, vector *tokens, map *words) {
    /* identifiers and reserved keywords */
    reserve_keywords(words);

    int num_tokens = 0;
    int line = 0;
    printf("\n%3d ", line++);

    token *cur_token = NULL;
    /* read first character */
    char *forward = br_get_start(file_reader);
    bool finished = false;
    while (!finished) {

        switch (*forward) {
            case UNIX_EOF:
                vector_push(tokens, token_new(e_eof, "$"));
                finished = true;
                break;

            case ' ': case '\t': case '\r':
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
                if (cur_token == NULL) {
                    br_reset_to_base(file_reader);
                    cur_token = scan_operator(file_reader);
                }
                assert(cur_token != NULL);
                vector_push(tokens, cur_token);
                break;

            case '"': case '\'':
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
                    token *word = (token*)map_find(words, cur_token->data);
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
            token *t = (token*)tokens->elements[tokens->size-1];
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
    map_insert(words, "while", token_new(e_while, "while"));
    map_insert(words, "for", token_new(e_for, "for"));
    map_insert(words, "if", token_new(e_if, "if"));
    map_insert(words, "then", token_new(e_then, "then"));
    map_insert(words, "else", token_new(e_else, "else"));
    map_insert(words, "do", token_new(e_do, "do"));
    map_insert(words, "switch", token_new(e_switch, "switch"));
    map_insert(words, "case", token_new(e_case, "case"));
    map_insert(words, "typedef", token_new(e_typedef, "typedef"));
    map_insert(words, "struct", token_new(e_struct, "struct"));
    map_insert(words, "enum", token_new(e_enum, "enum"));
    map_insert(words, "return", token_new(e_return, "return"));
    map_insert(words, "break", token_new(e_break, "break"));
    map_insert(words, "continue", token_new(e_continue, "continue"));
    map_insert(words, "sizeof", token_new(e_sizeof, "sizeof"));

    map_insert(words, "int", token_new(e_type, "int"));
    map_insert(words, "char", token_new(e_type, "char"));
    map_insert(words, "long", token_new(e_type, "long"));
    map_insert(words, "float", token_new(e_type, "float"));
    map_insert(words, "double", token_new(e_type, "double"));
    map_insert(words, "void", token_new(e_type, "void"));
}

/*
 * Scan string for int and float numbers.
 * Return pointer to the last symbol of the number.
 */
token *scan_number(buffered_reader *file_reader) {
    char buf[MAX_NUMBER_LENGTH+1];
    int size = 0;
    int state = 0;
    char *forward = br_get_base(file_reader);
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
                return token_new(e_number, buf);
            case 8:
                br_set_base(file_reader);
                buf[size-1] = '\0';
                return token_new(e_number, buf);
            case 9:
                br_set_base(file_reader);
                buf[size-1] = '\0';
                return token_new(e_number, buf);
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
token *scan_word(buffered_reader *file_reader) {
    char buf[MAX_ID_LENGTH+1];
    int size = 0;
    int state = 0;
    char *forward = br_get_base(file_reader);
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
                return token_new(e_identifier, buf);
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
token *scan_literal(buffered_reader *file_reader) {
    char buf[MAX_LITERAL_LENGTH+1];
    int size = 0;
    int state = 0;
    char *forward = br_get_base(file_reader);
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
                return token_new(e_literal, buf);

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
                return token_new(e_literal, buf);

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
                return token_new(e_literal, buf);
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
token *scan_operator(buffered_reader *file_reader) {
    char buf[OPERATOR_LENGTH+1];
    token_type token_t = e_operator;
    char *forward = br_get_base(file_reader);

    switch (*forward) {
        case '+':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '+':
                    br_get_next_char(file_reader);
                    strcpy(buf, "++");
                    token_t = e_incr_pre;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "+=");
                    token_t = e_plus_assign;
                    break;
                default:
                    strcpy(buf, "+");
                    token_t = e_plus;
                    break;
            }
            break;
        case '-':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '-':
                    br_get_next_char(file_reader);
                    strcpy(buf, "--");
                    token_t = e_decr_pre;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "-=");
                    token_t = e_minus_assign;
                    break;
                case '>':
                    forward = br_get_next_char(file_reader);
                    if (*forward == '*') {
                        br_get_next_char(file_reader);
                        strcpy(buf, "->*");
                        token_t = e_arrow_star;
                    } else {
                        strcpy(buf, "->");
                        token_t = e_arrow;
                    }
                    break;
                default:
                    strcpy(buf, "-");
                    token_t = e_minus;
                    break;
            }
            break;
        case '*':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "*=");
                    token_t = e_mult_assign;
                    break;
                default:
                    strcpy(buf, "*");
                    token_t = e_mult;
                    break;
            }
            break;
        case '/':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "/=");
                    token_t = e_div_assign;
                    break;
                default:
                    strcpy(buf, "/");
                    token_t = e_div;
                    break;
            }
            break;
        case '%':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "%=");
                    token_t = e_mod_assign;
                    break;
                default:
                    strcpy(buf, "%");
                    token_t = e_mod;
                    break;
            }
            break;
        case '=':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "==");
                    token_t = e_eq;
                    break;
                default:
                    strcpy(buf, "=");
                    token_t = e_assign;
                    break;
            }
            break;
        case '!':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "!=");
                    token_t = e_noteq;
                    break;
                default:
                    strcpy(buf, "!");
                    token_t = e_not;
                    break;
            }
            break;
        case '>':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, ">=");
                    token_t = e_moreeq;
                    break;
                case '>':
                    forward = br_get_next_char(file_reader);
                    switch (*forward) {
                        case '=':
                            br_get_next_char(file_reader);
                            strcpy(buf, ">>=");
                            token_t = e_shift_r_assign;
                            break;
                        default:
                            strcpy(buf, ">>");
                            token_t = e_shift_r;
                            break;
                    }
                    break;
                default:
                    strcpy(buf, ">");
                    token_t = e_more;
                    break;
            }
            break;
        case '<':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "<=");
                    token_t = e_lesseq;
                    break;
                case '<':
                    forward = br_get_next_char(file_reader);
                    switch (*forward) {
                        case '=':
                            br_get_next_char(file_reader);
                            strcpy(buf, "<<=");
                            token_t = e_shift_l_assign;
                            break;
                        default:
                            strcpy(buf, "<<");
                            token_t = e_shift_l;
                            break;
                    }
                    break;
                default:
                    strcpy(buf, "<");
                    token_t = e_less;
                    break;
            }
            break;
        case '&':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '&':
                    br_get_next_char(file_reader);
                    strcpy(buf, "&&");
                    token_t = e_and;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "&=");
                    token_t = e_and_assign;
                    break;
                default:
                    strcpy(buf, "&");
                    token_t = e_addr;
                    break;
            }
            break;
        case '|':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '|':
                    br_get_next_char(file_reader);
                    strcpy(buf, "||");
                    token_t = e_or;
                    break;
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "|=");
                    token_t = e_or_assign;
                    break;
                default:
                    strcpy(buf, "|");
                    token_t = e_or_bitwise;
                    break;
            }
            break;
        case '^':
            forward = br_get_next_char(file_reader);
            switch (*forward) {
                case '=':
                    br_get_next_char(file_reader);
                    strcpy(buf, "^=");
                    token_t = e_xor_assign;
                    break;
                default:
                    strcpy(buf, "^");
                    token_t = e_xor_bitwise;
                    break;
            }
            break;
        case '~':
            br_get_next_char(file_reader);
            strcpy(buf, "~");
            token_t = e_not_bitwise;
            break;
        case '.':
            forward = br_get_next_char(file_reader);
            if (*forward == '*') {
                br_get_next_char(file_reader);
                strcpy(buf, ".*");
                token_t = e_dot_star;
            } else {
                strcpy(buf, ".");
                token_t = e_dot;
            }
            break;
        case ',':
            br_get_next_char(file_reader);
            strcpy(buf, ",");
            token_t = e_comma;
            break;
        case '[':
            br_get_next_char(file_reader);
            strcpy(buf, "[");
            token_t = e_open_bracket;
            break;
        case ']':
            br_get_next_char(file_reader);
            strcpy(buf, "]");
            token_t = e_close_bracket;
            break;
        case ';':
            br_get_next_char(file_reader);
            strcpy(buf, ";");
            token_t = e_semicolon;
            break;
        case ':':
            br_get_next_char(file_reader);
            strcpy(buf, ":");
            token_t = e_colon;
            break;
        case '?':
            br_get_next_char(file_reader);
            token_t = e_question;
            strcpy(buf, "?");
            break;
        case '(':
            br_get_next_char(file_reader);
            strcpy(buf, "(");
            token_t = e_open_paren;
            break;
        case ')':
            br_get_next_char(file_reader);
            strcpy(buf, ")");
            token_t = e_close_paren;
            break;
        case '{':
            br_get_next_char(file_reader);
            strcpy(buf, "{");
            token_t = e_open_curly;
            break;
        case '}':
            br_get_next_char(file_reader);
            strcpy(buf, "}");
            token_t = e_close_curly;
            break;
        case '#':
            br_get_next_char(file_reader);
            strcpy(buf, "#");
            token_t = e_preproc_operator;
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
token *scan_comment(buffered_reader *file_reader) {
    int state = 0;
    char *forward = br_get_base(file_reader);
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
                return token_new(e_comment, "multiline comment");
            case 5:
                forward = br_get_next_char(file_reader);
                if (*forward == '\n') state = 6;
                else state = 5;
                break;
            case 6:
                forward = br_get_next_char(file_reader);
                br_set_base(file_reader);
                /* end of single-line comment */
                return token_new(e_comment, "single-line comment");
        }
        if (*forward == UNIX_EOF)
            return error("EOF while scanning comment", "");
        else if (*forward == '\n')
            ++current_line;
    }
    return NULL;
}

token *error(const char *message, const char *param) {
    printf("In file \"%s\": line %d\n", current_file, current_line-1);
    printf("Compile error: %s%s\n", message, param);
    exit(EXIT_FAILURE);
}
