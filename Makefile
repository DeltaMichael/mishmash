IDIR=include
CC=gcc
CFLAGS=-I$(IDIR) -g

ODIR=obj
LDIR=lib

LIBS=-lm

_DEPS=lexer.h parser.h list.h hashmap.h ast_printer.h tac_generator.h string_builder.h asm_generator.h asm_templates.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_TEST_DEPS=list.h
TEST_DEPS=$(patsubst %,$(IDIR)/%,$(_TEST_DEPS))

_OBJ=main.o lexer.o parser.o list.o hashmap.o ast_printer.o tac_generator.o string_builder.o asm_generator.o asm_templates.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

_TEST_OBJ=test.o list.o
TEST_OBJ=$(patsubst %,$(ODIR)/%,$(_TEST_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(TEST_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mishmash: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

testmash: $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
