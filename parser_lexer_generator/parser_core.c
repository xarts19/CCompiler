#include "parser_core.h"
#include <stdio.h>
#include "utils.h"
#include "parser.h"

static int cur_token();
void error(const char *message, const char *param);

static const char* TABLE_FILENAME = "table";
static const char* MAPPING_FILENAME = "mapping";
static const char* RULES_FILENAME = "rules";

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

#define MAX_SYMBOLS 100
#define MAX_STATES 100
#define MAX_RULES 100
#define MAX_RULE_LEN 10

/* for state and symbol: a - accept, e - error, s - shift, r - reduce, g - goto */
static action table[MAX_STATES][MAX_SYMBOLS];

/* enum_symbol_value */
static int mapping[MAX_SYMBOLS];

/* rule_size rule_head rule_token1 rule_token2 ... rule_tokenN */
static int rules[MAX_RULES][MAX_RULE_LEN];

static int num_symbols;
static int num_states;
static int num_rules;

static vector* tokens;
static int index;

void read_mapping() {
    FILE* pfile = fopen(MAPPING_FILENAME, "r");
    if (pfile == NULL) {
        InputError("Unable to open file", MAPPING_FILENAME);
    }
    fscanf(pfile, "%d", &num_symbols);
    int index, symbol_id;
    int read = fscanf(pfile, "%d %d", &index, &symbol_id);
    while (read == 2) {
        mapping[index] = symbol_id;
        read = fscanf(pfile, "%d %d", &index, &symbol_id);
    }
}

void read_rules() {
    FILE* pfile = fopen(RULES_FILENAME, "r");
    if (pfile == NULL) {
        InputError("Unable to open file", RULES_FILENAME);
    }
    fscanf(pfile, "%d", &num_rules);
    for (int rule_num=0; rule_num<num_rules; rule_num++) {
        int size;
        fscanf(pfile, "%d", &size);
        assert(size < MAX_RULE_LEN-1);
        rules[rule_num][0] = size;
        for (int sym_num=1; sym_num<=size; sym_num++) {
            int symbol;
            fscanf(pfile, "%d", &symbol);
            rules[rule_num][sym_num] = symbol;
        }
    }
}

void read_table() {
    FILE* pfile = fopen(TABLE_FILENAME, "r");
    if (pfile == NULL) {
        InputError("Unable to open file", TABLE_FILENAME);
    }
    fscanf(pfile, "%d", &num_states);

    // clear array
    for (int i=0; i<num_states; i++)
        for (int j=0; j<num_symbols; j++)
            table[i][j].action = 'e';
    int read, state, symbol, attr;
    char act;
    read = fscanf(pfile, "%d %d %c %d", &state, &symbol, &act, &attr);
    while (read == 4) {
        table[state][symbol].action = act;
        table[state][symbol].attr = attr;
        read = fscanf(pfile, "%d %d %c %d", &state, &symbol, &act, &attr);
    }
}

void init() {
    read_mapping();
    read_table();
    read_rules();
    assert(num_rules<MAX_RULES);
    assert(num_symbols<MAX_SYMBOLS);
    assert(num_states<MAX_STATES);
}

void run() {
    int t = 0;
    vector* stack = vector_new(100);
    vector_push(stack, (void*)0);
    while (1) {
        t = cur_token();
        action a = table[(int)vector_peek(stack)][t];
        if (a.action == 's') {
            vector_push(stack, (void*)a.attr);
            index++;
            printf("shift %d\n", a.attr);
        } else if (a.action == 'r') {
            for (int i=0; i<rules[a.attr][0]-1; i++)
                vector_pop(stack);
            int head_of_production = rules[a.attr][1];
            int top_of_stack = (int)vector_peek(stack);
            int new_state = table[top_of_stack][head_of_production].attr;
            vector_push(stack, (void*)new_state);
            printf("Reduce by rule %d\n", a.attr);
            printf("Stack top: %d\n", (int)vector_peek(stack));
        } else if (a.action == 'a') {
            printf("Accept\n");
            break;
        } else {
            error("Parsing failed", "");
        }
    }
}

stmt* parse_bottomup(vector* t) {
    tokens = t;
    init();
    run();
    return NULL;
}

int cur_token() {
    if (index < tokens->size) {
        token* t = (token*)tokens->elements[index];
        for (int i=0; i<num_symbols; i++)
            if (mapping[i] == t->id)
                return i;
    }
    return -1;
}

void error(const char *message, const char *param) {
    token* t = (token*)tokens->elements[index];
    printf("In file \"%s\": line %d; token: %s(%s)\n", current_file,
        t->line-1, token_type_str(t->id), t->data);
    printf("Parser error: %s%s\n", message, param);
    exit(EXIT_FAILURE);
}
