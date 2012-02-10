rules = (
    ("E", "E", "e_plus", "T"),
    ("E", "T"),
    ("T", "T", "e_mult", "F"),
    ("T", "F"),
    ("F", "e_open_paren", "E", "e_close_paren"),
    ("F", "e_number"),
)


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
