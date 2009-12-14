
# Author: Erwin Waterlander
# Copyright (C) 2009 Erwin Waterlander
# This file is distributed under the same license as the unix2dos package.

include version.mk

CC=gcc
PACKAGE=unix2dos

ENABLE_NLS = 1

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
sharedir	= $(prefix)/share

BINDIR		= $(DESTDIR)$(bindir)
DOCDIR		= $(DESTDIR)$(sharedir)/doc/$(PACKAGE)-$(UNIX2DOS_VERSION)
LOCALEDIR	= $(DESTDIR)$(sharedir)/locale
MANDIR		= $(DESTDIR)$(mandir)
MANDIR1		= $(MANDIR)/man1

ifdef ENABLE_NLS
POT		= po/$(PACKAGE).pot
POFILES		= $(wildcard po/*.po)
MOFILES		= $(patsubst %.po,%.mo,$(POFILES))
endif
DOCFILES	= $(PACKAGE).txt $(PACKAGE).ps $(PACKAGE).pdf
INSTALL_OBJS_DOC = README.txt ChangeLog.txt COPYING.txt TODO.txt $(DOCFILES)

# On some systems (e.g. FreeBSD 4.10) GNU install is installed as `ginstall'.
INSTALL		= install
# On some systems (e.g. GNU Win32) GNU mkdir is installed as `gmkdir'.
MKDIR           = mkdir

ifdef ENABLE_NLS
	NLSDEFS = -DENABLE_NLS -DLOCALEDIR=\"$(sharedir)/locale\" -DPACKAGE=\"$(PACKAGE)\"
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

GCCFLAGS	= -O2 -Wall

ifdef STATIC
	GCCFLAGS += -static
endif

CFLAGS		= -DVER_REVISION=\"$(UNIX2DOS_VERSION)\" \
		  -DVER_DATE=\"$(UNIX2DOS_DATE)\" \
		  $(GCCFLAGS) \
		  $(CFLAGS_OS) \
		  $(NLSDEFS)

LDFLAGS		= $(LDFLAGS_EXTRA)

DEFS		= $(EXTRA_DEFS)

# .......................................................... targets ...


all: $(BIN) $(DOCFILES) $(MOFILES)

%.o : %.c
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

$(BIN): unix2dos.o
	$(CC) $< $(LDFLAGS) -o $@

mofiles: $(MOFILES)

docfiles: $(DOCFILES)

tags: $(POT)

merge: $(POFILES)

%.po : $(POT)
	msgmerge -U $@ $(POT) --backup=numbered

%.mo : %.po
	msgfmt -c $< -o $@

$(POT) : unix2dos.c
	xgettext -C --keyword=_ $+ -o $(POT)

$(PACKAGE).txt : unix2dos.1
	LC_ALL=C nroff -man -c $< | col -bx > $@

$(PACKAGE).ps : unix2dos.1
	groff -man $< -T ps > $@

%.pdf: %.ps
	ps2pdf $< $@

install: all
	$(MKDIR) -p -m 755 $(BINDIR)
	$(INSTALL)  -m 755 $(BIN) $(BINDIR)
	$(MKDIR) -p -m 755 $(MANDIR1)
	$(INSTALL)  -m 644 $(PACKAGE).1 $(MANDIR1)
ifdef ENABLE_NLS
	@echo "-- install-mo"
	$(foreach mofile, $(MOFILES), $(MKDIR) -p -m 755 $(LOCALEDIR)/$(basename $(notdir $(mofile)))/LC_MESSAGES ;)
	$(foreach mofile, $(MOFILES), $(INSTALL) -m 644 $(mofile) $(LOCALEDIR)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
endif
	@echo "-- install-doc"
	$(MKDIR) -p -m 755 $(DOCDIR)
	$(INSTALL) -m 644 $(INSTALL_OBJS_DOC) $(DOCDIR)

uninstall:
	@echo "-- target: uninstall"
	-rm -f $(BINDIR)/$(BIN)
ifdef ENABLE_NLS
	$(foreach mofile, $(MOFILES), rm -f $(LOCALEDIR)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
endif
	-rm -f $(MANDIR)/man1/$(PACKAGE).1
	-rm -rf $(DOCDIR)

clean:
	rm -f *.o
	rm -f $(BIN)
	rm -f *.bak *~
	rm -f */*.bak */*~

maintainer-clean: clean
	rm -f $(DOCFILES)
	rm -f $(MOFILES)

realclean: maintainer-clean


ZIPOBJ	= bin/$(BIN) \
	  share/man/man1/$(PACKAGE).1 \
	  share/doc/$(PACKAGE)-$(UNIX2DOS_VERSION) \
	  $(ZIPOBJ_EXTRA)

ifdef ENABLE_NLS
ZIPOBJ += share/locale/*/LC_MESSAGES/$(PACKAGE).mo
endif

VERSIONSUFFIX	= -bin
ZIPFILE = $(PACKAGE)-$(UNIX2DOS_VERSION)$(VERSIONSUFFIX).zip
TGZFILE = $(PACKAGE)-$(UNIX2DOS_VERSION)$(VERSIONSUFFIX).tar.gz

dist-zip:
	rm -f $(ZIPFILE)
	cd $(prefix) ; unix2dos share/doc/$(PACKAGE)-$(UNIX2DOS_VERSION)/*.txt share/man/man1/$(PACKAGE).1
	cd $(prefix) ; zip -r $(ZIPFILE) $(ZIPOBJ)
	mv $(prefix)/$(ZIPFILE) ../..

dist-tgz:
	cd $(prefix) ; dos2unix share/doc/$(PACKAGE)-$(UNIX2DOS_VERSION)/*.txt share/man/man1/$(PACKAGE).1
	cd $(prefix) ; tar cvzf $(TGZFILE) $(ZIPOBJ)
	mv $(prefix)/$(TGZFILE) ../..

dist: dist-tgz

