IDIR=include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =lib

LIBS=-lm

_DEPS=lexer.h parser.h list.h hashmap.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o lexer.o parser.o list.o hashmap.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
