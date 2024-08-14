CFLAGS=-g -Wall
LIBS=-lm

ODIR=obj
CDIR=../common

$(ODIR)/%.o: %.c %.h $(CDIR)/$(ODIR)/list.o $(CDIR)/$(ODIR)/hashmap.o $(CDIR)/$(ODIR)/fileio.o
	$(CC) -c -o $@ $< $(CFLAGS)

bngen: main.o $(ODIR)/token_mapper.o $(ODIR)/parser.o $(ODIR)/lexer.o $(CDIR)/$(ODIR)/list.o $(CDIR)/$(ODIR)/hashmap.o $(CDIR)/$(ODIR)/fileio.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
