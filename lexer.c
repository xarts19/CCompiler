#include "lexer.h"

const char* KEYWORDS[] = {"true", "false", "for", "while", "and", "or", "not", "xor", "if", "then", "else"};
const char* TYPES[] = {"int", "float", "bool", "char"};

/* Scan input string, emit array of lexical tokens */
void lexer(buffered_reader *file_reader, vector* tokens, map* words) {
    /* identifiers and reserved keywords */
    reserve_keywords(words);

    int num_tokens = 0;

    /* read first character */
    char *lexeme_begin = br_get_start(file_reader);
    char *forward = lexeme_begin;
    bool finished = false;
    while (!finished) {

        switch (*forward) {
            case UNIX_EOF:
                finished = true;
                break;

            case ' ':
            case '\t':
                /* if it's a whitespace */
                break;

            case '\n':
                /* if it's a newline */
                ++current_line;
                printf("\n%3d ", current_line);
                break;

            case '/': {
                char prev = '/';
                forward = br_get_next_char(file_reader, forward);
                if ( *forward == '/' || *forward == '*' ) {
                    /* if it's a comment */
                    forward = scan_comment(forward, file_reader);
                } else {
                    /* if it's other operator */
                    forward = scan_operator(forward, prev, file_reader, tokens);
                }
                break;
            }

            case '"':
                /* it's a literal */
                forward = scan_literal(forward, file_reader, tokens);
                break;

            default:
                if ( isdigit(*forward) || *forward == '.' ) {
                    /* if it's a number (int or float) */
                    forward = scan_number(forward, file_reader, tokens);
                }
                else if ( isalpha(*forward) || *forward == '_' ) {
                    /* if it's a letter */
                    forward = scan_word(forward, file_reader, tokens, words);
                } else {
                    /* if it's some other character */
                    forward = scan_operator(forward, '\0', file_reader, tokens);
                }
                break;
        }
        forward = br_get_next_char(file_reader, forward);

        /* print tokens as they are parsed */
        if (tokens->size > num_tokens) {
            token_print(tokens->elements[tokens->size-1]);
            ++num_tokens;
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
char* scan_number(char *forward, buffered_reader *file_reader, vector *tokens) {
    bool has_dot = false;
    char buf[MAX_NUMBER_LENGTH];
    int size = 0;
    do {
        if (*forward == '.') {
            if (has_dot) {
                buf[size] = '.';
                buf[size+1] = '\0';
                error("Too many dots in a float:", buf);
            }
            has_dot = true;
        }
        buf[size++] = *forward;
        forward = br_get_next_char(file_reader, forward);
        if (size >= MAX_NUMBER_LENGTH) {
            buf[MAX_NUMBER_LENGTH-1] = '\0';
            error("Too large number: ", buf);
        }
    } while ( isdigit(*forward) || *forward == '.');
    buf[size] = '\0';
    token_type token_t = int_num;
    if (has_dot) token_t = float_num;
    vector_insert(tokens, token_new(token_t, buf));
    return --forward;
}

/*
 * Scan string for identifiers and reserved keywords.
 * Return pointer to the last symbol of the word.
 */
char* scan_word(char *forward, buffered_reader *file_reader, vector *tokens, map *words) {
    /* read following word */
    char buf[MAX_ID_LENGTH];
    int size = 0;
    do {
        buf[size++] = *forward;
        forward = br_get_next_char(file_reader, forward);
        if (size >= MAX_ID_LENGTH) {
            buf[MAX_ID_LENGTH-1] = '\0';
            error("Too large id: ", buf);
        }
    } while ( isalnum(*forward) || *forward == '_');
    buf[size] = '\0';

    /* search for matching word in map */
    token* word = (token*)map_find(words, buf);
    /* if word that was read is reserved */
    if (word != NULL) {
        /* add it to tokens */
        vector_insert(tokens, word);
    } else {
        /* save it as identifier and add it to tokens */
        token *t = token_new(identifier, buf);
        map_insert(words, buf, token_copy(t));
        vector_insert(tokens, t);
    }
    return --forward;
}

/*
 * Scan for constant strings of text.
 * Return pointer to the last symbol of the literal.
 */
char* scan_literal(char *forward, buffered_reader *file_reader, vector *tokens) {
    char buf[MAX_LITERAL_LENGTH];
    int size = 0;
    do {
        buf[size++] = *forward;
        forward = br_get_next_char(file_reader, forward);
        if (*forward == UNIX_EOF)
            error("Missing terminating \" character", "");
        if (size >= MAX_LITERAL_LENGTH)
            error("Too large literal", "");
    } while ( *forward != '"' );
    buf[size++] = *forward;
    buf[size] = '\0';
    vector_insert(tokens, token_new(literal, buf));
    return forward;
}

/*
 * Scan for various operators.
 * Return pointer to the last symbol of the operator.
 */
char* scan_operator(char *forward, char prev, buffered_reader *file_reader, vector *tokens) {
    char buf[3];
    token_type token_t = operator;
    if (prev == '\0') {
        buf[0] = *forward;
        forward = br_get_next_char(file_reader, forward);
    } else
        buf[0] = prev;
    buf[1] = '\0';
    buf[2] = '\0';
    switch (buf[0]) {
        case '<':
            switch (*forward) {
                case '>':
                    /* not equals */
                    buf[0] = '!';
                    buf[1] = '=';
                    token_t = cmp_operator;
                    break;
                case '=':
                    /* less then or equal to */
                    buf[1] = '=';
                    token_t = cmp_operator;
                    break;
                case '<':
                    /* shift left */
                    buf[1] = '<';
                    break;
                default:
                    /* less then */
                    token_t = cmp_operator;
                    break;
            }
            break;
        case '>':
            switch (*forward) {
                case '=':
                    /* greater then or equal to */
                    buf[1] = '=';
                    token_t = cmp_operator;
                    break;
                case '>':
                    /* shift right */
                    buf[1] = '>';
                    break;
                default:
                    /* greater then */
                    token_t = cmp_operator;
                    break;
            }
            break;
        case '!':
            switch (*forward) {
                case '=':
                    /* not equals */
                    buf[1] = '=';
                    token_t = cmp_operator;
                    break;
                default:
                    /* not */
                    token_t = logic_operator;
                    break;
            }
            break;
        case '=':
            switch (*forward) {
                case '=':
                    /* equals */
                    buf[1] = '=';
                    token_t = cmp_operator;
                    break;
                default:
                    /* assignment */
                    token_t = assign_operator;
                    break;
            }
            break;
        default:
            break;
    }
    vector_insert(tokens, token_new(token_t, buf));
    if (buf[1] == '\0')
        return --forward;
    else
        return forward;
}

/* returns pointer to the last symbol of the comment */
char* scan_comment(char *forward, buffered_reader *file_reader) {
    if (*forward == '/') {
        /* single-line comment */
        while (*forward != '\n') {
            forward = br_get_next_char(file_reader, forward);
            if (*forward == UNIX_EOF)
                return --forward;
        }
    } else {
        /* multiline comment */
        char prev = *forward;
        forward = br_get_next_char(file_reader, forward);
        while ( !(prev == '*' && *forward == '/') ) {
            prev = *forward;
            forward = br_get_next_char(file_reader, forward);
            if (*forward == UNIX_EOF)
                return --forward;
        }
        forward += 1;
    }
    return forward;
}
