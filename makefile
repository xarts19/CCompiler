#ODIR=obj
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -fPIC --std=c99 -g

MAIN=main.out
SRC=utils.c reader.c token.c vector.c map.c symbol.c s_table.c lexer.c parser.c main.c
#_OBJ=utils.o token.o vector.o map.o symbol.o s_table.c lexer.o main.o
#OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(ODIR) $(MAIN)

$(MAIN): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

#$(ODIR)/%.o: %.c
#	$(CC) $(CFLAGS) -c $^ -o $@

#$(ODIR):
#	mkdir -p $@

.PHONY: clean
clean:
#	rm -r $(ODIR)
	rm -f *~ $(MAIN)
