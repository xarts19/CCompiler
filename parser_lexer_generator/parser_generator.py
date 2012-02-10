#!/usr/bin/env python

##
## Generates LR(0) parsing tables for list of rules (see file grammar.py).
## Gets token's numbers from token.h file (they are enums there).
## Numbers, along with manually given numbers for nonterminals are
## compacted to form continuous sequence.
##
## Writes 3 files:
##   table    action for each state and symbol:
##            a - accept, s STATE - shift STATE to stack, r RULE_NUM -
##            reduce by rule RULE_NUM, g STATE - goto STATE, missing cells are errors.
##   mapping  maps compacted numbers to real tokens and teminals numbers.
##   rules    rules from grammar.py with converted symbols to
##            compacted numbers.
##
## All algorithms are from the book "Aho - Compilers - Principles,
## Techniques, and Tools 2e", chapter 4.6.
##

import re
from pprint import pprint
import grammar

NONTERM = grammar.nonterminals
TERM = grammar.terminals
SYMBOLS = grammar.symbols
RULES = grammar.rules
FIRST = {}
FOLLOW = {}

def scan_tokens():
    token_type_re = re.compile("(e_[A-Za-z_]*)\W*=\W*(\d+)")
    tokens = {}
    inside = False
    for line in open("../token.h"):
        if "token_type_enum" in line:
            inside = True
            continue
        if inside:
            res = token_type_re.search(line)
            if res:
                tokens[res.group(1)] = int(res.group(2))
        if "typedef struct" in line:
            break
    tokens['$'] = 0
    for i, n in enumerate(NONTERM):
        tokens[n] = 1000 + i
    return tokens

def derive_e(nonterm):
    for rule in [rule for rule in RULES if rule[0] == nonterm]:
        if "e" in rule:
            return True
    return False

def build_first():
    first = {}
    for s in SYMBOLS:
        first[s] = set()
    for term in TERM:
        first[term].add(term)
    size1 = sum(map(len, first.values()))
    size2 = size1 + 1
    while size1 < size2:
        size1 = size2
        for nonterm in NONTERM:
            for rule in [rule for rule in RULES if rule[0] == nonterm]:
                contain_e = True
                for t in rule[1:]:
                    first[nonterm].update( first[t].difference(set(['e',])) )
                    if t in NONTERM and derive_e(t):
                        continue
                    else:
                        contain_e = False
                        break
                if contain_e:
                    first[nonterm].add("e")
            if derive_e(nonterm):
                first[nonterm].add("e")
        size2 = sum(map(len, first))
    return first

def build_follow(first):
    follow = {}
    for s in SYMBOLS:
        follow[s] = set()
    follow["START"].add("$")
    size1 = sum(map(len, follow.values()))
    size2 = size1 + 1
    while size1 < size2:
        size1 = size2
        for rule in RULES:
            for i in range(1, len(rule)):
                t = rule[i]
                if i < len(rule)-1:
                    follow[t].update( first[ rule[i+1] ].difference( set(['e',]) ) )
                contain_e = True
                for j in range(i+1, len(rule)):
                    if 'e' not in first[ rule[j] ]:
                        contain_e = False
                        break
                if contain_e:
                    follow[t].update( follow[ rule[0] ] )
        size2 = sum(map(len, follow))
    return follow

def closure(I):
    J = I.copy()
    added = True
    while added:
        added = False
        for item in J.copy():
            dot_pos = item.index(".")
            if dot_pos < len(item) - 1:
                B = item[dot_pos+1]
                if B in NONTERM:
                    for rule in [list(rule) for rule in RULES if rule[0] == B]:
                        new_item = tuple(rule[:1] + ["."] + rule[1:])
                        if new_item not in J:
                            J.add(new_item)
                            added = True
    return J

# save results for later calls
GOTO = {}
def goto(I, X):
    if (I, X) in GOTO:
        return GOTO[(I, X)]
    J = set()
    for item in I:
        dot_pos = item.index(".")
        if X in item and dot_pos < len(item) - 1 and X == item[dot_pos+1]:
            new_item = list(item)
            new_item.pop(dot_pos)
            new_item.insert(dot_pos+1, ".")
            J.add(tuple(new_item))
    GOTO[(I, X)] = closure(J)
    return GOTO[(I, X)]

def build_canonical_collection_of_sets():
    C = []
    first_item = list(RULES[0])
    first_item.insert(1, '.')
    start_state = closure(set( [tuple(first_item),] ))
    C.append(frozenset(start_state))
    added = True
    while added:
        added = False
        for set_of_items in C[:]:
            for symbol in SYMBOLS:
                g = frozenset(goto(set_of_items, symbol))
                if len(g) > 0 and g not in C:
                    C.append(g)
                    added = True
    return C

def set_action(action, item, key, value):
    if not action.get(key):
        action[key] = value
    else:
        print "Conflict on with rule ", item, ": ", key, "=>", value
        print action[key], " already set"

def build_LR_parse_table(tokens):
    C = build_canonical_collection_of_sets()
    action = {}
    states = {}
    for num, I in enumerate(C):
        states[I] = num
    for I, num in states.items():
        for item in I:
            dot_pos = item.index('.')
            if dot_pos == len(item) - 1:
                if item[0] == "START":
                    set_action(action, item, (num, "$"), "a 0")
                else:
                    for sym in FOLLOW[item[0]]:
                        set_action(action, item, (num, sym), "r " + str(RULES.index(tuple(item[:-1]))) )
            else:
                sym = item[dot_pos+1]
                a = 's '
                if sym in NONTERM:
                    a = 'g '
                action[(num, sym)] = a + str(states[ frozenset(goto(I, sym)) ])

    print(grammar.R)
    print_table(action, states)
    print_details(action, C)
    save_to_file(action, C, tokens)

def save_to_file(action, C, tokens):
    # create continuous numeration for symbols
    # and print in to file for parser
    mapping = []
    mapping_dict = {}
    for i, sym in enumerate(list(TERM) + list(NONTERM)):
        mapping.append(str(i)+' '+str(tokens[sym]))
        mapping_dict[sym] = i
    f = file("../mapping", 'w')
    f.write(str(len(mapping))+"\n")
    f.write("\n".join(mapping))
    f.write('\n')

    content = []
    for (state, sym), res in action.items():
        content.append(str(state)+" "+str(mapping_dict[sym])+" "+res)
    f = file("../table", 'w')
    f.write(str(len(C))+"\n")
    f.write("\n".join(content))
    f.write("\n")
    f.close()

    rules = []
    for rule in RULES:
        rules.append(str(len(rule))+' ')
        for sym in rule:
            rules.append(str(mapping_dict[sym])+' ')
        rules.append('\n')
    f = file("../rules", 'w')
    f.write(str(len(RULES))+"\n")
    f.write("".join(rules))
    f.write("\n")
    f.close()

def print_table(action, states):
    header = list(TERM) + list(NONTERM)
    print "symbol".rjust(6),
    for sym in header:
        print sym[0:6].rjust(6),
    print
    for num in range(len(states)):
        print str(num).rjust(6),
        for sym in header:
            print action.get((num, sym), "   ").rjust(6),
        print

def print_details(action, C):
    for num in range(len(C)):
        print C[num]
        for sym in list(TERM) + list(NONTERM):
            print sym.rjust(15), action.get((num, sym), "err").rjust(10)

def build_LL_parse_table(tokens):
    action = {}
    for rule in RULES:
        A = rule[0]
        for a in [term for term in FIRST[A] if term in TERM]:
            action[(A, a)] = rule
        if 'e' in FIRST[A]:
            for b in FOLLOW[A]:
                action[(A, b)] = rule
    pprint(action)

def main():
    tokens = scan_tokens()
    FIRST = build_first()
    FOLLOW = build_follow(FIRST)
    build_LL_parse_table(tokens)

if __name__=="__main__":
    main()
