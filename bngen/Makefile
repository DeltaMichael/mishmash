CFLAGS=-g -Wall
LIBS=-lm

ODIR=obj
CDIR=../common

$(CDIR)/$(ODIR)/list.o: $(CDIR)/list.c
	$(CC) -c -o $@ $< $(CFLAGS)

main.o: main.c $(CDIR)/$(ODIR)/list.o
	$(CC) -c -o $@ $< $(CFLAGS)

bngen: main.o $(CDIR)/$(ODIR)/list.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
