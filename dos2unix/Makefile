
# Author: Erwin Waterlander
# Copyright (C) 2009-2010 Erwin Waterlander
# This file is distributed under the same license as the dos2unix package.

include version.mk

CC=gcc
PACKAGE=dos2unix
UNIX2DOS=unix2dos
MAC2UNIX=mac2unix

ENABLE_NLS = 1

EXE=
ifneq (, $(wildcard /cygdrive))
	EXE = .exe
endif
BIN=$(PACKAGE)$(EXE)
UNIX2DOS_BIN=$(UNIX2DOS)$(EXE)
MAC2UNIX_BIN=mac2unix$(EXE)

LINK = ln -sf

prefix		= /usr
exec_prefix	= $(prefix)
bindir		= $(exec_prefix)/bin
datarootdir	= $(prefix)/share
datadir		= $(datarootdir)

docdir		= $(datarootdir)/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)
localedir	= $(datarootdir)/locale
mandir		= $(datarootdir)/man
man1dir		= $(mandir)/man1
manext		= .1
man1ext		= .1

ifdef ENABLE_NLS
DOS2UNIX_POT		= po/dos2unix/$(PACKAGE).pot
DOS2UNIX_POFILES	= $(wildcard po/dos2unix/*.po)
DOS2UNIX_MOFILES	= $(patsubst %.po,%.mo,$(DOS2UNIX_POFILES))
UNIX2DOS_POT		= po/unix2dos/$(PACKAGE).pot
UNIX2DOS_POFILES	= $(wildcard po/unix2dos/*.po)
UNIX2DOS_MOFILES	= $(patsubst %.po,%.mo,$(UNIX2DOS_POFILES))
MOFILES			= $(DOS2UNIX_MOFILES) $(UNIX2DOS_MOFILES)
endif
DOCFILES	= $(PACKAGE).txt $(PACKAGE).ps $(PACKAGE).pdf $(UNIX2DOS).txt $(UNIX2DOS).ps $(UNIX2DOS).pdf
INSTALL_OBJS_DOC = README.txt NEWS.txt ChangeLog.txt COPYING.txt TODO.txt $(DOCFILES)

# On some systems (e.g. FreeBSD 4.10) GNU install is installed as `ginstall'.
INSTALL		= install
# On some systems (e.g. GNU Win32) GNU mkdir is installed as `gmkdir'.
MKDIR           = mkdir

ifdef ENABLE_NLS
	DOS2UNIX_NLSDEFS = -DENABLE_NLS -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
	UNIX2DOS_NLSDEFS = -DENABLE_NLS -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(UNIX2DOS)\"
endif

# ......................................................... OS flags ...

OS =

ifndef OS
ifneq (, $(wildcard /boot/vmlinuz*))
	OS = linux
endif
endif

ifndef OS
ifneq (, $(wildcard /cygdrive))
	OS = cygwin
	LINK = cp -f
endif
endif

ifeq (cygwin,$(OS))
ifdef ENABLE_NLS
	LDFLAGS_EXTRA = -lintl -liconv -Wl,--enable-auto-import
endif
endif


ifndef OS
ifeq (FreeBSD, $(shell uname -s))
	OS = freebsd
endif
endif

ifeq (freebsd,$(OS))
	# Running under FreeBSD
ifdef ENABLE_NLS
	CFLAGS_OS     = -I/usr/local/include
	LDFLAGS_EXTRA = -lintl -L/usr/local/lib
endif
endif

ifndef OS
ifneq (, $(wildcard /opt/csw))
	OS = sun
endif
endif

ifeq (sun,$(OS))
	# Running under SunOS/Solaris
	LDFLAGS_EXTRA = -lintl
endif

ifndef OS
ifeq (HP-UX, $(shell uname -s))
	OS = hpux
endif
endif

ifeq (hpux,$(OS))
	# Running under HP-UX
	EXTRA_DEFS += -Dhpux -D_HPUX_SOURCE
endif

ifndef OS
	OS = unknown
endif

# ............................................................ flags ...

CFLAGS	= -O2 -Wall -D_LARGEFILE_SOURCE $(RPM_OPT_FLAGS)

EXTRA_CFLAGS	= -DVER_REVISION=\"$(DOS2UNIX_VERSION)\" \
		  -DVER_DATE=\"$(DOS2UNIX_DATE)\" \
		  $(CFLAGS_OS)

ifdef STATIC
	EXTRA_CFLAGS += -static
endif

LDFLAGS		= $(LDFLAGS_EXTRA)

DEFS		= $(EXTRA_DEFS)

# .......................................................... targets ...


all: $(BIN) $(MAC2UNIX_BIN) $(UNIX2DOS_BIN) $(MAC2UNIX).1 $(DOCFILES) $(MOFILES)

dos2unix.o : dos2unix.c
	$(CC) $(DEFS) $(EXTRA_CFLAGS) $(DOS2UNIX_NLSDEFS) $(CFLAGS) -c $< -o $@

unix2dos.o : unix2dos.c
	$(CC) $(DEFS) $(EXTRA_CFLAGS) $(UNIX2DOS_NLSDEFS) $(CFLAGS) -c $< -o $@

$(BIN): dos2unix.o
	$(CC) $< $(LDFLAGS) -o $@

$(UNIX2DOS_BIN): unix2dos.o
	$(CC) $< $(LDFLAGS) -o $@

$(MAC2UNIX_BIN) : $(BIN)
	$(LINK) $< $@

$(MAC2UNIX).1 : $(PACKAGE).1
	$(LINK) $< $@


mofiles: $(MOFILES)

docfiles: $(DOCFILES)

tags: $(DOS2UNIX_POT)

merge: $(DOS2UNIX_POFILES)

po/dos2unix/%.po : $(DOS2UNIX_POT)
	msgmerge -U $@ $(DOS2UNIX_POT) --backup=numbered

po/unix2dos/%.po : $(UNIX2DOS_POT)
	msgmerge -U $@ $(UNIX2DOS_POT) --backup=numbered

%.mo : %.po
	msgfmt -c $< -o $@

$(DOS2UNIX_POT) : dos2unix.c
	xgettext -C --keyword=_ $+ -o $(DOS2UNIX_POT)

$(UNIX2DOS_POT) : unix2dos.c
	xgettext -C --keyword=_ $+ -o $(UNIX2DOS_POT)

%.txt : %.1
	LC_ALL=C nroff -man -c $< | col -bx > $@

%.ps : %.1
	groff -man $< -T ps > $@

%.pdf: %.ps
	ps2pdf $< $@

install: all
	$(MKDIR) -p -m 755 $(DESTDIR)$(bindir)
	$(INSTALL)  -m 755 $(BIN) $(DESTDIR)$(bindir)
	$(INSTALL)  -m 755 $(MAC2UNIX_BIN) $(DESTDIR)$(bindir)
	$(INSTALL)  -m 755 $(UNIX2DOS_BIN) $(DESTDIR)$(bindir)
	$(MKDIR) -p -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL)  -m 644 $(PACKAGE).1 $(DESTDIR)$(man1dir)
	$(INSTALL)  -m 644 $(MAC2UNIX).1 $(DESTDIR)$(man1dir)
	$(INSTALL)  -m 644 $(UNIX2DOS).1 $(DESTDIR)$(man1dir)
ifdef ENABLE_NLS
	@echo "-- install-mo"
	$(foreach mofile, $(MOFILES), $(MKDIR) -p -m 755 $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES ;)
	$(foreach mofile, $(DOS2UNIX_MOFILES), $(INSTALL) -m 644 $(mofile) $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
	$(foreach mofile, $(UNIX2DOS_MOFILES), $(INSTALL) -m 644 $(mofile) $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(UNIX2DOS).mo ;)
endif
	@echo "-- install-doc"
	$(MKDIR) -p -m 755 $(DESTDIR)$(docdir)
	$(INSTALL) -m 644 $(INSTALL_OBJS_DOC) $(DESTDIR)$(docdir)

uninstall:
	@echo "-- target: uninstall"
	-rm -f $(DESTDIR)$(bindir)/$(BIN)
	-rm -f $(DESTDIR)$(bindir)/$(MAC2UNIX_BIN)
	-rm -f $(DESTDIR)$(bindir)/$(UNIX2DOS_BIN)
ifdef ENABLE_NLS
	$(foreach mofile, $(DOS2UNIX_MOFILES), rm -f $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
	$(foreach mofile, $(UNIX2DOS_MOFILES), rm -f $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(UNIX2DOS).mo ;)
endif
	-rm -f $(DESTDIR)$(mandir)/man1/$(PACKAGE).1
	-rm -f $(DESTDIR)$(mandir)/man1/$(MAC2UNIX).1
	-rm -rf $(DESTDIR)$(docdir)

clean:
	rm -f *.o
	rm -f $(BIN) $(UNIX2DOS_BIN) $(MAC2UNIX_BIN) $(MAC2UNIX).1
	rm -f *.bak *~
	rm -f */*.bak */*~

maintainer-clean: clean
	rm -f $(DOCFILES)
	rm -f $(MOFILES)

realclean: maintainer-clean


ZIPOBJ	= bin/$(BIN) \
	  bin/$(MAC2UNIX_BIN) \
	  bin/$(UNIX2DOS_BIN) \
	  share/man/man1/$(PACKAGE).1 \
	  share/man/man1/$(MAC2UNIX).1 \
	  share/man/man1/$(UNIX2DOS).1 \
	  share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION) \
	  $(ZIPOBJ_EXTRA)

ifdef ENABLE_NLS
ZIPOBJ += share/locale/*/LC_MESSAGES/$(PACKAGE).mo
endif

VERSIONSUFFIX	= -bin
ZIPFILE = $(PACKAGE)-$(DOS2UNIX_VERSION)$(VERSIONSUFFIX).zip
TGZFILE = $(PACKAGE)-$(DOS2UNIX_VERSION)$(VERSIONSUFFIX).tar.gz
TBZFILE = $(PACKAGE)-$(DOS2UNIX_VERSION)$(VERSIONSUFFIX).tar.bz2

dist-zip:
	rm -f $(prefix)/$(ZIPFILE)
	cd $(prefix) ; unix2dos share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1 share/man/man1/$(UNIX2DOS).1
	cd $(prefix) ; zip -r $(ZIPFILE) $(ZIPOBJ)
	mv -f $(prefix)/$(ZIPFILE) ..

dist-tgz:
	cd $(prefix) ; dos2unix share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1 share/man/man1/$(UNIX2DOS).1
	cd $(prefix) ; tar cvzf $(TGZFILE) $(ZIPOBJ)
	mv $(prefix)/$(TGZFILE) ..

dist-tbz:
	cd $(prefix) ; dos2unix share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1 share/man/man1/$(UNIX2DOS).1
	cd $(prefix) ; tar cvjf $(TBZFILE) $(ZIPOBJ)
	mv $(prefix)/$(TBZFILE) ..

dist: dist-tgz

strip:
	strip $(BIN)
ifeq ($(LINK),cp -f)
	strip $(MAC2UNIX_BIN)
endif
	strip $(UNIX2DOS_BIN)
