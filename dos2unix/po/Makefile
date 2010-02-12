
# Author: Erwin Waterlander
# Copyright (C) 2009-2010 Erwin Waterlander
# This file is distributed under the same license as the dos2unix package.

include version.mk

CC=gcc
PACKAGE=dos2unix
MAC2UNIX=mac2unix

ENABLE_NLS = 1

EXE=
ifneq (, $(wildcard /cygdrive))
	EXE = .exe
endif
BIN=$(PACKAGE)$(EXE)
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
	NLSDEFS = -DENABLE_NLS -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
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
		  $(CFLAGS_OS) \
		  $(NLSDEFS)

ifdef STATIC
	EXTRA_CFLAGS += -static
endif

LDFLAGS		= $(LDFLAGS_EXTRA)

DEFS		= $(EXTRA_DEFS)

# .......................................................... targets ...


all: $(BIN) $(MAC2UNIX_BIN) $(MAC2UNIX).1 $(DOCFILES) $(MOFILES)

%.o : %.c
	$(CC) $(DEFS) $(EXTRA_CFLAGS) $(CFLAGS) -c $< -o $@

$(BIN): dos2unix.o
	$(CC) $< $(LDFLAGS) -o $@

$(MAC2UNIX_BIN) : $(BIN)
	$(LINK) $< $@

$(MAC2UNIX).1 : $(PACKAGE).1
	$(LINK) $< $@


mofiles: $(MOFILES)

docfiles: $(DOCFILES)

tags: $(POT)

merge: $(POFILES)

%.po : $(POT)
	msgmerge -U $@ $(POT) --backup=numbered

%.mo : %.po
	msgfmt -c $< -o $@

$(POT) : dos2unix.c
	xgettext -C --keyword=_ $+ -o $(POT)

$(PACKAGE).txt : dos2unix.1
	LC_ALL=C nroff -man -c $< | col -bx > $@

$(PACKAGE).ps : dos2unix.1
	groff -man $< -T ps > $@

%.pdf: %.ps
	ps2pdf $< $@

install: all
	$(MKDIR) -p -m 755 $(DESTDIR)$(bindir)
	$(INSTALL)  -m 755 $(BIN) $(DESTDIR)$(bindir)
	$(INSTALL)  -m 755 $(MAC2UNIX_BIN) $(DESTDIR)$(bindir)
	$(MKDIR) -p -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL)  -m 644 $(PACKAGE).1 $(DESTDIR)$(man1dir)
	$(INSTALL)  -m 644 $(MAC2UNIX).1 $(DESTDIR)$(man1dir)
ifdef ENABLE_NLS
	@echo "-- install-mo"
	$(foreach mofile, $(MOFILES), $(MKDIR) -p -m 755 $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES ;)
	$(foreach mofile, $(MOFILES), $(INSTALL) -m 644 $(mofile) $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
endif
	@echo "-- install-doc"
	$(MKDIR) -p -m 755 $(DESTDIR)$(docdir)
	$(INSTALL) -m 644 $(INSTALL_OBJS_DOC) $(DESTDIR)$(docdir)

uninstall:
	@echo "-- target: uninstall"
	-rm -f $(DESTDIR)$(bindir)/$(BIN)
	-rm -f $(DESTDIR)$(bindir)/$(MAC2UNIX_BIN)
ifdef ENABLE_NLS
	$(foreach mofile, $(MOFILES), rm -f $(DESTDIR)$(localedir)/$(basename $(notdir $(mofile)))/LC_MESSAGES/$(PACKAGE).mo ;)
endif
	-rm -f $(DESTDIR)$(mandir)/man1/$(PACKAGE).1
	-rm -f $(DESTDIR)$(mandir)/man1/$(MAC2UNIX).1
	-rm -rf $(DESTDIR)$(docdir)

clean:
	rm -f *.o
	rm -f $(BIN) $(MAC2UNIX_BIN) $(MAC2UNIX).1
	rm -f *.bak *~
	rm -f */*.bak */*~

maintainer-clean: clean
	rm -f $(DOCFILES)
	rm -f $(MOFILES)

realclean: maintainer-clean


ZIPOBJ	= bin/$(BIN) \
	  bin/$(MAC2UNIX_BIN) \
	  share/man/man1/$(PACKAGE).1 \
	  share/man/man1/$(MAC2UNIX).1 \
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
	cd $(prefix) ; unix2dos share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1
	cd $(prefix) ; zip -r $(ZIPFILE) $(ZIPOBJ)
	mv -f $(prefix)/$(ZIPFILE) ..

dist-tgz:
	cd $(prefix) ; dos2unix share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1
	cd $(prefix) ; tar cvzf $(TGZFILE) $(ZIPOBJ)
	mv $(prefix)/$(TGZFILE) ..

dist-tbz:
	cd $(prefix) ; dos2unix share/doc/$(PACKAGE)-$(DOS2UNIX_VERSION)/*.txt share/man/man1/$(PACKAGE).1 share/man/man1/$(MAC2UNIX).1
	cd $(prefix) ; tar cvjf $(TBZFILE) $(ZIPOBJ)
	mv $(prefix)/$(TBZFILE) ..

dist: dist-tgz

strip:
	strip $(BIN)
ifeq ($(LINK),cp -f)
	strip $(MAC2UNIX_BIN)
endif
