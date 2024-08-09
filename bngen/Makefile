CFLAGS=-g -Wall
LIBS=-lm

ODIR=obj
CDIR=../common

$(ODIR)/%.o: %.c $(CDIR)/$(ODIR)/list.o
	$(CC) -c -o $@ $< $(CFLAGS)

bngen: main.o $(CDIR)/$(ODIR)/list.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
