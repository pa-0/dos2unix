
# Author: Erwin Waterlander

CC=gcc
CFLAGS=
PACKAGE=dos2unix
MAC2UNIX=mac2unix

EXE=
ifneq (, $(wildcard /cygdrive))
	EXE = .exe
endif
BIN=$(PACKAGE)$(EXE)
MAC2UNIX_BIN=mac2unix$(EXE)

prefix		= /usr
exec_prefix	= $(prefix)
man_prefix	= $(prefix)/share
mandir		= $(man_prefix)/man
bindir		= $(exec_prefix)/bin

BINDIR		= $(DESTDIR)$(bindir)
MANDIR		= $(DESTDIR)$(mandir)
MANDIR1		= $(MANDIR)/man1

all: $(BIN) $(MAC2UNIX_BIN) $(MAC2UNIX).1


$(BIN):
	$(CC) dos2unix.c -o dos2unix

$(MAC2UNIX_BIN) : $(BIN)
ifneq (, $(wildcard /cygdrive))
	cp -f $< $@
else
	ln -sf $< $@
endif

$(MAC2UNIX).1 : $(PACKAGE).1
ifneq (, $(wildcard /cygdrive))
	cp -f $< $@
else
	ln -sf $< $@
endif

install: all
	mkdir -p -m 755 $(BINDIR)
	install  -m 755 $(BIN) $(BINDIR)
	install  -m 755 $(MAC2UNIX_BIN) $(BINDIR)
	mkdir -p -m 755 $(MANDIR1)
	install  -m 644 $(PACKAGE).1 $(MANDIR1)
	install  -m 644 $(MAC2UNIX).1 $(MANDIR1)

clean:
	rm -f $(BIN) $(MAC2UNIX_BIN)
	rm -f *.bak *~

maintainer-clean: clean

realclean: maintainer-clean
