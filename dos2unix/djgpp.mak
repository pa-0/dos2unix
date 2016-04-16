# Author: Erwin Waterlander
# Copyright (C) 2009-2016 Erwin Waterlander
# This file is distributed under the same license as the dos2unix package.

include version.mk

d2u_os=$(shell uname -s)

# DJGPP 2.03
LINK = ln -sf
# DJGPP 2.05
# In DJGPP 2.05 linking with ln works differently. The links created
# with DJGPP 2.05 do not work.
#LINK = cp -f

CROSS_COMP=0

ifeq ($(findstring CYGWIN,$(d2u_os)),CYGWIN)
	CROSS_COMP=1
endif

ifeq ($(CROSS_COMP),1)
	CROSS_COMPILE=i586-pc-msdosdjgpp-
	LINK = cp -f
endif

CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

prefix=c:/dos32
ENABLE_NLS=
VERSIONSUFFIX=-dos32

ifdef ENABLE_NLS
LIBS_EXTRA = -lintl -liconv
NLS_SUFFIX = -nls
endif
VERSIONSUFFIX = pm
ZIPFILE = d2u$(DOS2UNIX_VERSION_SHORT)$(VERSIONSUFFIX)$(NLS_SUFFIX).zip
ZIPOBJ_EXTRA = bin/cwsdpmi.exe
docsubdir = dos2unix

all:
	$(MAKE) all EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LIBS_EXTRA="$(LIBS_EXTRA)" prefix=$(prefix) LINK="$(LINK)" LINK_MAN="cp -f" docsubdir=$(docsubdir) UCS= CC=$(CC) D2U_OS=msdos

test: all
	cd test; $(MAKE) test UCS= SHELL=$(shell which sh) 

check: test

install:
	$(MAKE) install EXE=.exe ENABLE_NLS=$(ENABLE_NLS) LIBS_EXTRA="$(LIBS_EXTRA)" prefix=$(prefix) LINK="$(LINK)" LINK_MAN="cp -f" docsubdir=$(docsubdir) UCS= CC=$(CC) D2U_OS=msdos

uninstall:
	$(MAKE) uninstall EXE=.exe prefix=$(prefix) docsubdir=$(docsubdir)

clean:
	$(MAKE) clean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

mostlyclean:
	$(MAKE) mostlyclean EXE=.exe ENABLE_NLS=$(ENABLE_NLS) prefix=$(prefix)

dist:
	$(MAKE) dist-zip EXE=.exe prefix=$(prefix) VERSIONSUFFIX="$(VERSIONSUFFIX)" ZIPOBJ_EXTRA="${ZIPOBJ_EXTRA}" ENABLE_NLS=$(ENABLE_NLS) ZIPFILE=${ZIPFILE} docsubdir=$(docsubdir)

strip:
	$(MAKE) strip LINK="$(LINK)" LINK_MAN="cp -f" EXE=.exe STRIP=$(STRIP)
# Fix time stamps. Otherwise make install may rebuild mac2unix unix2mac.
# Now make install can be done in MSYS. In djgpp bash install is problematic.
	sleep 10
	touch mac2unix.exe
	touch unix2mac.exe

