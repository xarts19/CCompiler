R = """
PROGRAM STMT
STMT e_open_curly STMT e_close_curly
STMT e_if e_open_paren EXPR e_close_paren STMT
STMT e_while e_open_paren EXPR e_close_paren STMT
STMT e_identifier e_assign EXPR e_semicolon
STMT EXPR e_semicolon
STMT e_type e_identifier e_semicolon
EXPR EXPR e_eq E
EXPR EXPR e_noteq E
EXPR EXPR e_lesseq E
EXPR EXPR e_moreeq E
EXPR EXPR e_less E
EXPR EXPR e_more E
EXPR E
E E e_plus T
E E e_minus T
E T
T T e_mult F
T T e_div F
T T e_mod F
T F
F e_open_paren E e_close_paren
F e_number
F e_identifier
"""
R = """
E T E_
E_ e_plus T E_
E_ e
T F T_
T_ e_mult F T_
T_ e
F e_open_paren E e_close_paren
F e_number
"""
rules = tuple([tuple([token for token in line.strip().split(" ") if token])
               for line in R.strip().split("\n") if line])
# rules = (
#     ("E", "R"),
#     ("E", "L", "e_assign", "R"),
#     ("R", "L"),
#     ("L", "e_mult", "R"),
#     ("L", "e_number"),
# )


symbols = set()
terminals = set()
nonterminals = set()
for rule in rules:
    for term in rule:
        symbols.add(term)
        if term.startswith("e_"):
            terminals.add(term)
        else:
            nonterminals.add(term)
terminals.add("$")
symbols.add("$")
nonterminals.add("START")
symbols.add("START")

rules = tuple( [("START", rules[0][0])] + list(rules) )
