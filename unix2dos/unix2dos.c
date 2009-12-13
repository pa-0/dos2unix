/*
 *  Name: unix2dos
 *  Documentation:
 *    Convert lf ('\x0a') characters in a file to cr lf ('\x0d' '\x0a')
 *    combinations.
 *  Version: $$Id: unix2dos.c 2.2 1995/03/31 01:41:00 blin Exp blin $$
 *
 *  Copyright (c) 1994, 1995 Benjamin Lin.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice in the documentation and/or other materials provided with
 *     the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  == 1.0 == 1989.10.04 == John Birchfield (jb@koko.csustan.edu)
 *  == 1.1 == 1994.12.20 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Cleaned up for Borland C/C++ 4.02
 *  == 1.2 == 1995.03.09 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Fixed minor typo error
 *  == 1.3 == 1995.03.16 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Modified to more conform to UNIX style.
 *  == 2.0 == 1995.03.19 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Rewritten from scratch
 *  == 2.2 == 1995.03.30 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Conversion from SunOS charset implemented.
 *
 *  == BUG ==
 *     stdio process under DOS not working
 */


#define RCS_AUTHOR   "$$Author: blin $$"
#define RCS_DATE     "$$Date: 1995/03/31 01:41:00 $$"
#define RCS_REVISION "$$Revision: 2.2 $$"
#define VER_AUTHOR   "Benjamin Lin"

/* #define DEBUG */

#ifdef DJGPP
#  include <dir.h>
#  include <unistd.h>
#else
#  include <libgen.h>
#  include <sys/unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif
#include "unix2dos.h"

#if defined(WIN32)
#define MSDOS
#endif

#ifdef MSDOS /* DJGPP and MINGW32 */
#include <fcntl.h>
#include <io.h>
#endif


#ifdef MSDOS
#  define R_CNTRL   "rb"
#  define W_CNTRL   "wb"
#else
#  define R_CNTRL   "r"
#  define W_CNTRL   "w"
#endif


typedef struct
{
  int NewFile;                          /* is in new file mode? */
  int Quiet;                            /* is in quiet mode? */
  int KeepDate;                         /* should keep date stamp? */
  int ConvMode;                         /* 0 - ASCII, 1 - 7 bit, 2 - ISO */  
} CFlag;


void PrintUsage(void)
{
  fprintf(stderr, _("\
unix2dos %s (%s)\n\
Usage: unix2dos [-hkqLV] [-o file ...] [-c convmode] [-n infile outfile ...]\n\
 -h --help        give this help\n\
 -k --keepdate    keep output file date\n\
 -q --quiet       quiet mode, suppress all warnings\n\
                  always on in stdin->stdout mode\n\
 -L --license     print software license\n\
 -V --version     display version number\n\
 -c --convmode    conversion mode\n\
 convmode         ASCII, 7bit, ISO, default to ASCII\n\
 -o --oldfile     write to old file\n\
 file ...         files to convert in old file mode\n\
 -n --newfile     write to new file\n\
 infile           original file in new file mode\n\
 outfile          output file in new file mode\n"), VER_REVISION, VER_DATE);
}

void PrintLicense(void)
{
  fprintf(stderr, _("\
Copyright (c) 1994-1995 Benjamin Lin\n\
Copyright (c) 2009      Erwin Waterlander\n\
All rights reserved.\n\n\
\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions\n\
are met:\n\
1. Redistributions of source code must retain the above copyright\n\
   notice, this list of conditions and the following disclaimer.\n\
2. Redistributions in binary form must reproduce the above copyright\n\
   notice in the documentation and/or other materials provided with\n\
   the distribution.\n\n\
\
THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY\n\
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n\
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE\n\
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n\
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT\n\
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR\n\
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n\
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE\n\
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN\n\
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\
"));
}

void PrintVersion(void)
{
  fprintf(stderr, "unix2dos %s (%s)\n", VER_REVISION, VER_DATE);
#ifdef ENABLE_NLS
  fprintf(stderr, _("With native language support.\n"));
#else
  fprintf(stderr, "Without native language support.\n");
#endif
#ifdef DEBUG
  fprintf(stderr, "RCS_AUTHOR: %s\n", RCS_AUTHOR);
  fprintf(stderr, "RCS_DATE: %s\n", RCS_DATE);
  fprintf(stderr, "RCS_REVISION: %s\n", RCS_REVISION);
  fprintf(stderr, "VER_AUTHOR: %s\n", VER_AUTHOR);
  fprintf(stderr, "VER_DATE: %s\n", VER_DATE);
  fprintf(stderr, "VER_REVISION: %s\n", VER_REVISION);
#endif
}


/* opens file of name ipFN in read only mode
 * RetVal: NULL if failure
 *         file stream otherwise
 */
FILE* OpenInFile(char *ipFN)
{
  return (fopen(ipFN, R_CNTRL));
}


/* opens file of name ipFN in write only mode
 * RetVal: NULL if failure
 *         file stream otherwise
 */
FILE* OpenOutFile(int fd)
{
  return (fdopen(fd, W_CNTRL));
}


/* converts stream ipInF to DOS format text and write to stream ipOutF
 * RetVal: 0  if success
 *         -1  otherwise
 */
int ConvertUnixToDos(FILE* ipInF, FILE* ipOutF, CFlag *ipFlag)
{
    int RetVal = 0;
    int TempChar;

    /* LF    -> CR-LF */
    /* CR-LF -> CR-LF, in case the input file is a DOS text file */
    /* \x0a = Newline/Line Feed (LF) */
    /* \x0d = Carriage Return (CR) */

    switch (ipFlag->ConvMode)
    {
      case 0: /* ASCII */
        while ((TempChar = getc(ipInF)) != EOF)  /* get character */
          if (
              /* read a Unix line end */
              ((TempChar == '\x0a') && (putc('\x0d', ipOutF) == EOF)) ||  /* got LF, put CR */
              /* read a DOS line end */
              ((TempChar == '\x0d') && (((TempChar = getc(ipInF)) == EOF) || (putc('\x0d', ipOutF) == EOF))) || /* got CR, put CR and get next char (LF) */
              (putc(U2DAsciiTable[TempChar], ipOutF) == EOF)) /* put char (LF or other char) */
          {
              RetVal = -1;
              if (!ipFlag->Quiet)
                fprintf(stderr, _("unix2dos: can not write to output file\n"));
              break;
          }
        break;
      case 1: /* 7Bit */
        while ((TempChar = getc(ipInF)) != EOF)
          if (((TempChar == '\x0a') && (putc('\x0d', ipOutF) == EOF)) ||
              ((TempChar == '\x0d') && (((TempChar = getc(ipInF)) == EOF) || (putc('\x0d', ipOutF) == EOF))) ||
              (putc(U2D7BitTable[TempChar], ipOutF) == EOF))
          {
              RetVal = -1;
              if (!ipFlag->Quiet)
                fprintf(stderr, _("unix2dos: can not write to output file\n"));
              break;
          }
        break;
      case 2: /* ISO */
        while ((TempChar = getc(ipInF)) != EOF)
          if (((TempChar == '\x0a') && (putc('\x0d', ipOutF) == EOF)) ||
              ((TempChar == '\x0d') && (((TempChar = getc(ipInF)) == EOF) || (putc('\x0d', ipOutF) == EOF))) ||
              (putc(U2DIsoTable[TempChar], ipOutF) == EOF))
          {
              RetVal = -1;
              if (!ipFlag->Quiet)
                fprintf(stderr, _("unix2dos: can not write to output file\n"));
              break;
          }
        break;
      default: /*unknown convmode */
      ;
#ifdef DEBUG
      fprintf(stderr, _("unix2dos: program error, invalid conversion mode %d\n"),ipFlag->ConvMode);
      exit(1);
#endif
  }
  return RetVal;
}

#ifdef MSDOS
FILE* MakeTempFileFrom(const char *OutFN, char **fname_ret)
#else
static int MakeTempFileFrom(const char *OutFN, char **fname_ret)
#endif
{
  char *cpy = strdup(OutFN);
  char *dir = NULL;
  size_t fname_len = 0;
  char  *fname_str = NULL;
#ifdef MSDOS
  char *name;
  FILE *fd = NULL;
#else
  int fd = -1;
#endif
  
  *fname_ret = NULL;
 
  if (!cpy)
    goto make_failed;
  
  dir = dirname(cpy);
  
  fname_len = strlen(dir) + strlen("/u2dtmpXXXXXX") + sizeof (char);
  if (!(fname_str = malloc(fname_len)))
    goto make_failed;
  sprintf(fname_str, "%s%s", dir, "/u2dtmpXXXXXX");
  *fname_ret = fname_str;

  free(cpy);

#ifdef MSDOS
  name = mktemp(fname_str);
  *fname_ret = name;
  if ((fd = fopen(fname_str, W_CNTRL)) == NULL)
    goto make_failed;
#else
  if ((fd = mkstemp(fname_str)) == -1)
    goto make_failed;
#endif
  
  return (fd);
  
 make_failed:
  free(*fname_ret);
  *fname_ret = NULL;
#ifdef MSDOS
  return (NULL);
#else
  return (-1);
#endif
}

/* convert file ipInFN to DOS format text and write to file ipOutFN
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertUnixToDosNewFile(char *ipInFN, char *ipOutFN, CFlag *ipFlag)
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  struct stat StatBuf;
  struct utimbuf UTimeBuf;
#ifdef MSDOS
  FILE* fd;
#else
  int fd;
#endif

  /* retrieve ipInFN file date stamp */
  if ((ipFlag->KeepDate) && stat(ipInFN, &StatBuf))
    RetVal = -1;

#ifdef MSDOS
  if((fd = MakeTempFileFrom(ipOutFN, &TempPath))==NULL) {
#else
  if((fd = MakeTempFileFrom (ipOutFN, &TempPath)) < 0) {
#endif
    perror("Can't open output temp file");
    RetVal = -1;
  }

#ifdef DEBUG
  fprintf(stderr, _("unix2dos: using %s as temp file\n"), TempPath);
#endif

  /* can open in file? */
  if ((!RetVal) && ((InF=OpenInFile(ipInFN)) == NULL))
    RetVal = -1;

  /* can open out file? */
#ifdef MSDOS
  if ((!RetVal) && (InF) && ((TempF=fd) == NULL))
#else
  if ((!RetVal) && (InF) && ((TempF=OpenOutFile(fd)) == NULL))
#endif
  {
    fclose (InF);
    InF = NULL;
    RetVal = -1;
  }

  /* conversion sucessful? */
  if ((!RetVal) && (ConvertUnixToDos(InF, TempF, ipFlag)))
    RetVal = -1;

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  /* can close out file? */
  if ((TempF) && (fclose(TempF) == EOF))
    RetVal = -1;

#ifdef MSDOS
  if(fd!=NULL)
    fclose(fd);
#else
  if(fd>=0)
    close(fd);
#endif

  if ((!RetVal) && (ipFlag->KeepDate))
  {
    UTimeBuf.actime = StatBuf.st_atime;
    UTimeBuf.modtime = StatBuf.st_mtime;
    /* can change out file time to in file time? */
    if (utime(TempPath, &UTimeBuf) == -1)
      RetVal = -1;
  }

  /* any error? */
  if ((RetVal) && (unlink(TempPath)))
    RetVal = -1;

  /* can rename temp file to out file? */
  if (!RetVal)
  {
#ifdef MSDOS
    remove(ipOutFN);
#endif
    if ((rename(TempPath, ipOutFN) == -1) && (!ipFlag->Quiet))
    {
      fprintf(stderr, _("unix2dos: problems renaming '%s' to '%s'\n"), TempPath, ipOutFN);
      fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
      RetVal = -1;
    }
  }
  free(TempPath);
  return RetVal;
}


/* convert file ipInFN to DOS format text
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertUnixToDosOldFile(char* ipInFN, CFlag *ipFlag)
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  struct stat StatBuf;
  struct utimbuf UTimeBuf;
  mode_t mode = S_IRUSR | S_IWUSR;
#ifdef MSDOS
  FILE* fd;
#else
  int fd;
#endif

  /* retrieve ipInFN file date stamp */
  if (stat(ipInFN, &StatBuf))
    RetVal = -1;
  else
    mode = StatBuf.st_mode;

#ifdef MSDOS
  if((fd = MakeTempFileFrom(ipInFN, &TempPath))==NULL) {
#else
  if((fd = MakeTempFileFrom(ipInFN, &TempPath)) < 0) {
#endif
      perror("Can't open output temp file");
      RetVal = -1;
  }

#ifndef MSDOS
  if (!RetVal && fchmod (fd, mode) && fchmod (fd, S_IRUSR | S_IWUSR))
    RetVal = -1;
#endif

#ifdef DEBUG
  fprintf(stderr, _("unix2dos: using %s as temp file\n"), TempPath);
#endif

  /* can open in file? */
  if ((!RetVal) && ((InF=OpenInFile(ipInFN)) == NULL))
    RetVal = -1;

  /* can open out file? */
#ifdef MSDOS
  if ((!RetVal) && (InF) && ((TempF=fd) == NULL))
#else
  if ((!RetVal) && (InF) && ((TempF=OpenOutFile(fd)) == NULL))
#endif
  {
    fclose (InF);
    InF = NULL;
    RetVal = -1;
  }

  /* conversion sucessful? */
  if ((!RetVal) && (ConvertUnixToDos(InF, TempF, ipFlag)))
    RetVal = -1;

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  /* can close out file? */
  if ((TempF) && (fclose(TempF) == EOF))
    RetVal = -1;

#ifdef MSDOS
  if(fd!=NULL)
    fclose(fd);
#else
  if(fd>=0)
    close(fd);
#endif

  if ((!RetVal) && (ipFlag->KeepDate))
  {
    UTimeBuf.actime = StatBuf.st_atime;
    UTimeBuf.modtime = StatBuf.st_mtime;
    /* can change out file time to in file time? */
    if (utime(TempPath, &UTimeBuf) == -1)
      RetVal = -1;
  }

  /* any error? */
  if ((RetVal) && (unlink(TempPath)))
    RetVal = -1;

#ifdef MSDOS
  if (!RetVal)
    remove(ipInFN);
#endif
  /* can rename out file to in file? */
  if ((!RetVal) && (rename(TempPath, ipInFN) == -1))
  {
    if (!ipFlag->Quiet)
    {
      fprintf(stderr, _("unix2dos: problems renaming '%s' to '%s'\n"), TempPath, ipInFN);
      fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
    }
    RetVal = -1;
  }
  free(TempPath);
  return RetVal;
}


/* convert stdin to DOS format text and write to stdout
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertUnixToDosStdio(CFlag *ipFlag)
{
    ipFlag->NewFile = 1;
    ipFlag->Quiet = 1;
    ipFlag->KeepDate = 0;

    /* stdin and stdout are by default text streams. We need
     * to reopen them in binary mode. Otherwise a LF will
     * automatically be converted to CR-LF on DOS/Windows.
     * Erwin */

#ifdef WIN32
    /* 'setmode' was deprecated by MicroSoft, starting
     * since Visual C++ 2005. Use '_setmode' instead. */
    _setmode(fileno(stdout), O_BINARY);
    _setmode(fileno(stdin), O_BINARY);
    return (ConvertUnixToDos(stdin, stdout, ipFlag));
#elif defined(MSDOS)
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);
    return (ConvertUnixToDos(stdin, stdout, ipFlag));
#else
    return (ConvertUnixToDos(stdin, stdout, ipFlag));
#endif
}


int main (int argc, char *argv[])
{
  /* variable declarations */
  int ArgIdx;
  int CanSwitchFileMode;
  int ShouldExit;
  CFlag *pFlag;

#ifdef ENABLE_NLS
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif


  /* variable initialisations */
  ArgIdx = 0;
  CanSwitchFileMode = 1;
  ShouldExit = 0;
  pFlag = (CFlag*)malloc(sizeof(CFlag));  
  pFlag->NewFile = 0;
  pFlag->Quiet = 0;
  pFlag->KeepDate = 0;
  pFlag->ConvMode = 0;
  
  /* no option, use stdin and stdout */
  if (argc == 1)
  {
    exit(ConvertUnixToDosStdio(pFlag));
  }

  while ((++ArgIdx < argc) && (!ShouldExit))
  {
    /* is it an option? */
    if (argv[ArgIdx][0] == '-')
    {
      /* an option */
      if ((strcmp(argv[ArgIdx],"-h") == 0) || (strcmp(argv[ArgIdx],"--help") == 0))
        PrintUsage();
      if ((strcmp(argv[ArgIdx],"-k") == 0) || (strcmp(argv[ArgIdx],"--keepdate") == 0))
        pFlag->KeepDate = 1;
      if ((strcmp(argv[ArgIdx],"-q") == 0) || (strcmp(argv[ArgIdx],"--quiet") == 0))
        pFlag->Quiet = 1;
      if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0))
        PrintVersion();
      if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0))
        PrintLicense();

      if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0))
      {
        ArgIdx++;
        if (strcmpi(argv[ArgIdx],"ASCII") == 0)
          pFlag->ConvMode = 0;
        else if (strcmpi(argv[ArgIdx], "7Bit") == 0)
          pFlag->ConvMode = 1;
        else if (strcmpi(argv[ArgIdx], "ISO") == 0)
          pFlag->ConvMode = 2;
        else
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("unix2dos: invalid %s conversion mode specified\n"),argv[ArgIdx]);
          ShouldExit = 1;
        }
      }

      if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0))
      {
        /* last convert not paired */
        if (!CanSwitchFileMode)
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("unix2dos: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
          ShouldExit = 1;
        }
        pFlag->NewFile = 0;
      }
      if ((strcmp(argv[ArgIdx],"-n") == 0) || (strcmp(argv[ArgIdx],"--newfile") == 0))
      {
        /* last convert not paired */
        if (!CanSwitchFileMode)
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("unix2dos: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
          ShouldExit = 1;
        }
        pFlag->NewFile = 1;
      }
    }
    else
    {
      /* not an option */
      if (pFlag->NewFile)
      {
        if (CanSwitchFileMode)
          CanSwitchFileMode = 0;
        else
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("unix2dos: converting file %s to file %s in DOS format ...\n"), argv[ArgIdx-1], argv[ArgIdx]);
          if (ConvertUnixToDosNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag))
          {
            if (!pFlag->Quiet)
              fprintf(stderr, _("unix2dos: problems converting file %s to file %s\n"), argv[ArgIdx-1], argv[ArgIdx]);
            ShouldExit = 1;
          }
          CanSwitchFileMode = 1;
        }
      }
      else
      {
        if (!pFlag->Quiet)
          fprintf(stderr, _("unix2dos: converting file %s to DOS format ...\n"), argv[ArgIdx]);
        if (ConvertUnixToDosOldFile(argv[ArgIdx], pFlag))
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("unix2dos: problems converting file %s\n"), argv[ArgIdx]);
          ShouldExit = 1;
        }
      }
    }
  }

  if ((!pFlag->Quiet) && (!CanSwitchFileMode))
  {
    fprintf(stderr, _("unix2dos: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
    ShouldExit = 1;
  }
  free(pFlag);
  return (ShouldExit);
}

