#include "parser_core.h"
#include <stdio.h>

TABLE_FILENAME = "table";
MAPPING_FILENAME = "mapping";

int read_mapping() {
    FILE* pfile = fopen(MAPPING_FILENAME, "r");
    if (pfile == NULL) {
        InputError("Unable to open file", filename);
    }
    int num_symbols;
    fscanf(pfile, "%d", &num_states);

    return num_symbols;
}

int read_table(int num_symbols) {
    FILE* pfile = fopen(TABLE_FILENAME, "r");
    if (pfile == NULL) {
        InputError("Unable to open file", filename);
    }
    int num_states;
    fscanf(pfile, "%d", &num_states);

    // create and clear array
    action table[num_states][num_symbols];
    for (int i=0; i<num_states; i++)
        for (int j=0; j<num_symbols; j++)
            table[i][j].action = 'e';
    int read;
    int state;
    int symbol;
    char act;
    int attr;
    read = fscanf(pfile, "%d %d %c %d", &state, &symbol, &act, &attr);
    while (read == 3) {
        table[state][symbol].action = act;
        table[state][symbol].attr = attr;
        read = fscanf(pfile, "%d %d %c %d", &state, &symbol, &act, &attr);
    }
    return num_states;
}

int init() {
    num_symbols = read_mapping();
    read_table(num_symbols);
}

int main() {
    return 0;
}
