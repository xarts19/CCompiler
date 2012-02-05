#include "parser.h"

int Parser::parse() {
    return 0;
}

string Parser::expr() {
    term();
    while (true) {
        if (lookahead==NULL) {
            return result;
        } else if (lookahead->id == "oper") {
            string oper = lookahead->to_string();
            match(lookahead->id);
            term();
            result += oper;
        } else {
            error("Syntax error: " + lookahead->id + ": " + lookahead->to_string());
        }
    }
    return "While (true) finished. 0_o";
}

void Parser::term() {
    if (lookahead->id == "int") {
        string value = lookahead->to_string();
        match(lookahead->id);
        result += value;
    }
}

void Parser::match(string t) {
    if (lookahead->id == t) {
        current += 1;
        if (current < tokens.size()) {
            lookahead = tokens[current];
        } else {
            lookahead = NULL;
        }
    } else {
        error("Syntax error");
    }
}
