
# Author: Erwin Waterlander
# Copyright (C) 2009 Erwin Waterlander
# This file is distributed under the same license as the unix2dos package.

prefix=c:/djgpp
ENABLE_NLS=
ifdef $(ENABLE_NLS)
LDFLAGS_EXTRA = -lintl -liconv
endif
ZIPOBJ_EXTRA = bin/cwsdpmi.exe

all:
	$(MAKE) all EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"

install:
	$(MAKE) install EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LDFLAGS_EXTRA="$(LDFLAGS_EXTRA)" prefix=$(prefix) LINK="cp -f"

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix)

clean:
	$(MAKE) clean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

dist:
	$(MAKE) dist-zip EXE=.exe prefix=$(prefix) VERSIONSUFFIX="-dos32" ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" ENABLE_NLS=$(ENABLE_NLS)

