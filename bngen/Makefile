CFLAGS=-g -Wall
LIBS=-lm

main.o: main.c
	$(CC) -c -o $@ $< $(CFLAGS)

bngen: main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
