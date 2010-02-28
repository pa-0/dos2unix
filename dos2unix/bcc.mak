# Makefile for Borland C++

.AUTODEPEND

#		*Translator Definitions*
CC = bcc +DOS2UNIX.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BC4\LIB;
INCLUDEPATH = .;C:\BC4\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 dos2unix.obj

#		*Explicit Rules*
dos2unix.exe: dos2unix.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
dos2unix.obj
dos2unix
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
dos2unix.obj: dos2unix.cfg dos2unix.c 
	$(CC) -c dos2unix.c

unix2dos.obj: dos2unix.cfg unix2dos.c 
	$(CC) -c unix2dos.c


#		*Compiler Configuration File*
dos2unix.cfg: bcc.mak
  copy &&|
-ml
-v
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS
-DVER_REVISION="5.0"
-DVER_DATE="2010-02-16"
| dos2unix.cfg


