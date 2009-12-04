
# Author: Erwin Waterlander

CC=gcc
CFLAGS= 
PACKAGE=unix2dos

EXE=
ifneq (, $(wildcard /cygdrive))
	EXE = .exe
endif
BIN=$(PACKAGE)$(EXE)

prefix		= /usr
exec_prefix	= $(prefix)
man_prefix	= $(prefix)/share
mandir		= $(man_prefix)/man
bindir		= $(exec_prefix)/bin

BINDIR		= $(DESTDIR)$(bindir)
MANDIR		= $(DESTDIR)$(mandir)
MANDIR1		= $(MANDIR)/man1

all: $(BIN)

$(BIN): unix2dos.c
	$(CC) $< -o $@ 

install: all
	mkdir -p -m 755 $(BINDIR)
	install  -m 755 $(BIN) $(BINDIR)
	mkdir -p -m 755 $(MANDIR1)
	install  -m 644 $(PACKAGE).1 $(MANDIR1)

clean:
	rm -f $(BIN)
	rm -f *.bak *~

maintainer-clean: clean

realclean: maintainer-clean
