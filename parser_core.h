#ifndef PARSER_CORE_H_
#define PARSER_CORE_H

typedef struct action {
    /* a - accept, e - error, s - shift, r - reduce, g- goto */
    char action;
    /* if shift - which state to push onto stack, if reduce - by which production */
    int attr;
} action;

#endif
