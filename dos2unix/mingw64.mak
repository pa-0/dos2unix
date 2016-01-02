# Author: Erwin Waterlander
# Copyright (C) 2012-2016 Erwin Waterlander
# This file is distributed under the same license as the dos2unix package.

# This makefile is for use with MSYS2 and MinGW-w64 target 64 bit (x86_64)
# http://sourceforge.net/projects/msys2/
.PHONY: test check

UNIFILE=1
CC = gcc
CRT_GLOB_OBJ = /mingw64/x86_64-w64-mingw32/lib/CRT_glob.o

prefix=c:/usr/local64
ENABLE_NLS=

ifdef ENABLE_NLS
LIBS_EXTRA = -lintl -liconv
ZIPOBJ_EXTRA = bin/libintl-8.dll bin/libiconv-2.dll
endif
LIBS_EXTRA += $(CRT_GLOB_OBJ)
CFLAGS_OS=-I/mingw64/include
VERSIONSUFFIX="-win64"

all:
	$(MAKE) all EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LIBS_EXTRA="$(LIBS_EXTRA)" prefix=$(prefix) LINK="cp -f" CC=$(CC) CFLAGS_OS=$(CFLAGS_OS) UNIFILE=$(UNIFILE)

test: all
	cd test; $(MAKE) test

check: test

install:
	$(MAKE) install EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LIBS_EXTRA="$(LIBS_EXTRA)" prefix=$(prefix) LINK="cp -f" CC=$(CC) CFLAGS_OS=$(CFLAGS_OS) UNIFILE=$(UNIFILE)

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix)

clean:
	$(MAKE) clean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

mostlyclean:
	$(MAKE) mostlyclean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

dist:
	$(MAKE) dist-zip EXE=.exe prefix=$(prefix) VERSIONSUFFIX=$(VERSIONSUFFIX) ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" ENABLE_NLS=$(ENABLE_NLS)

strip:
	$(MAKE) strip LINK="cp -f" EXE=.exe

