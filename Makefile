LDIR=lang
CDIR=common
GDIR=lang/generated

CC=gcc
CFLAGS=-I$(LDIR) -I$(CDIR) -I$(GDIR) -g

ODIR=obj

LIBS=-lm

_LDEPS=lexer.h
_LOBJ=lexer.o
_GDEPS=token.h

_CDEPS=fileio.h list.h hashmap.h string_builder.h
_COBJ=fileio.o list.o hashmap.o string_builder.o
_GOBJ=token.o
_MOBJ=main.o

LDEPS=$(patsubst %,$(LDIR)/%,$(_LDEPS))
CDEPS=$(patsubst %,$(CDIR)/%,$(_CDEPS))
GDEPS=$(patsubst %,$(GDIR)/%,$(_GDEPS))

COBJ=$(patsubst %,$(CDIR)/$(ODIR)/%,$(_COBJ))
LOBJ=$(patsubst %,$(LDIR)/$(ODIR)/%,$(_LOBJ))
GOBJ=$(patsubst %,$(GDIR)/$(ODIR)/%,$(_GOBJ))
MOBJ=$(patsubst %,$(ODIR)/%,$(_MOBJ))

$(CDIR)/$(ODIR)/%.o: $(CDIR)/%.c $(CDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(LDIR)/$(ODIR)/%.o: $(LDIR)/%.c $(LDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(GDIR)/$(ODIR)/%.o: $(GDIR)/%.c $(GDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(GDEPS) $(CDEPS) $(LDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mishmash: $(MOBJ) $(COBJ) $(GOBJ) $(LOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(MOBJ) $(COBJ) $(GOBJ) $(LOBJ)
