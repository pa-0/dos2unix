# Makefile for Microsoft Visual C++
#

!include version.mk

CC = cl.exe -nologo
LINK = link.exe -nologo
SRCDIR = .
UCS = 1

DEFINES = -DVER_REVISION=\"$(DOS2UNIX_VERSION)\" -DVER_DATE=\"$(DOS2UNIX_DATE)\"
CFLAGS = $(DEFINES) -DD2U_UNICODE

!ifdef DEBUG
LDFLAGS = -debug
!else
LDFLAGS =
!endif

PROGRAMS = dos2unix.exe unix2dos.exe mac2unix.exe unix2mac.exe

all: $(PROGRAMS)


dos2unix.exe: dos2unix.obj querycp.obj common.obj
	$(LINK) $(LDFLAGS) dos2unix.obj querycp.obj common.obj mpr.lib

unix2dos.exe: unix2dos.obj querycp.obj common.obj
	$(LINK) $(LDFLAGS) unix2dos.obj querycp.obj common.obj mpr.lib


dos2unix.obj :  $(SRCDIR)\dos2unix.c $(SRCDIR)\querycp.h $(SRCDIR)\common.h
	$(CC) -c $(CFLAGS) $(SRCDIR)\dos2unix.c

unix2dos.obj :  $(SRCDIR)\unix2dos.c $(SRCDIR)\querycp.h $(SRCDIR)\common.h
	$(CC) -c $(CFLAGS) $(SRCDIR)\unix2dos.c

querycp.obj :  $(SRCDIR)\querycp.c $(SRCDIR)\querycp.h
	$(CC) -c $(CFLAGS) $(SRCDIR)\querycp.c

common.obj :  $(SRCDIR)\common.c $(SRCDIR)\common.h
	$(CC) -c $(CFLAGS) $(SRCDIR)\common.c

mac2unix.exe : dos2unix.exe
	copy /v dos2unix.exe mac2unix.exe

unix2mac.exe : unix2dos.exe
	copy /v unix2dos.exe unix2mac.exe

clean:
	-del *.obj
	-del *.exe
