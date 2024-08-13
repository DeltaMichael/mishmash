CFLAGS=-g -Wall
LIBS=-lm

ODIR=obj
CDIR=../common

$(ODIR)/%.o: %.c %.h $(CDIR)/$(ODIR)/list.o
	$(CC) -c -o $@ $< $(CFLAGS)

bngen: main.o $(ODIR)/token_mapper.o $(CDIR)/$(ODIR)/list.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
