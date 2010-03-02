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


d2u_EXE_dependencies =  \
 dos2unix.obj

u2d_EXE_dependencies =  \
 unix2dos.obj

all: dos2unix.exe unix2dos.exe mac2unix.exe unix2mac.exe

#		*Explicit Rules*
dos2unix.exe: dos2unix.cfg $(d2u_EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
dos2unix.obj
dos2unix
		# no map file
emu.lib+
mathl.lib+
cl.lib
|

unix2dos.exe: dos2unix.cfg $(u2d_EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
unix2dos.obj
unix2dos
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


mac2unix.exe: dos2unix.exe
	copy /v dos2unix.exe mac2unix.exe

unix2mac.exe: unix2dos.exe
	copy /v unix2dos.exe unix2mac.exe

#		*Individual File Dependencies*
dos2unix.obj: dos2unix.cfg dos2unix.c 
	$(CC) -c dos2unix.c

unix2dos.obj: dos2unix.cfg unix2dos.c 
	$(CC) -c unix2dos.c

strip:
	tdstrip dos2unix.exe
	tdstrip mac2unix.exe
	tdstrip unix2dos.exe
	tdstrip unix2mac.exe

clean:
	del *.obj
	del *.exe

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
-DVER_REVISION="5.0.1-beta1"
-DVER_DATE="2010-03-02"
| dos2unix.cfg


