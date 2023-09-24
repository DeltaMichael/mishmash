IDIR=include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =lib

LIBS=-lm

_DEPS=lexer.h parser.h list.h hashmap.h ast_printer.h tac_generator.h string_builder.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o lexer.o parser.o list.o hashmap.o ast_printer.o tac_generator.o string_builder.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
