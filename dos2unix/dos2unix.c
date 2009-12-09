/*
 * Added extra newline if ^M occurs
 * Christian Wurll, wurll@ira.uka.de
 * Thu Nov 19 1998 
 * 
 * Added Mac text file translation, i.e. \r to \n conversion
 * Bernd Johannes Wuebben, wuebben@kde.org
 * Wed Feb  4 19:12:58 EST 1998      
 *
 *  Name: dos2unix
 *  Documentation:
 *    Remove cr ('\x0d') characters from a file.
 *  Version: $$Id: dos2unix.c 2.2 1995/03/31 01:40:24 blin Exp blin $$
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
 *  == 1.2 == 1995.03.16 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Modified to more conform to UNIX style.
 *  == 2.0 == 1995.03.19 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Rewritten from scratch.
 *  == 2.1 == 1995.03.29 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Conversion to SunOS charset implemented.
 *  == 2.2 == 1995.03.30 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Fixed a bug in 2.1 where in new file mode, if outfile already exists
 *     conversion can not be completed properly.
 *
 *  == BUG ==
 *     stdio process under DOS not working
 */


#define RCS_AUTHOR   "$$Author:  wurll $$"
#define RCS_DATE     "$$Date: Thu Nov 19 1998 $$"
#define RCS_REVISION "$$Revision: 3.1 $$"
#define VER_AUTHOR   "Erwin Waterlander"

#define MACMODE  1
static int macmode = 0;

/* #define DEBUG */

#ifdef __MSDOS__
#  include <dir.h>
#endif
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif
#include "dos2unix.h"


#ifdef __MSDOS__
  #define R_CNTRL   "rb"
  #define W_CNTRL   "wb"
#else
  #define R_CNTRL   "r"
  #define W_CNTRL   "w"
#endif


typedef struct
{
  int NewFile;                          /* is in new file mode? */
  int Quiet;                            /* is in quiet mode? */
  int KeepDate;                         /* should keep date stamp? */
  int ConvMode;                         /* 0 - ASCII, 1 - 7 bit, 2 - ISO, 3- Mac*/  
  int NewLine;                          /* if TRUE, then additional newline */
} CFlag;



void PrintUsage(void)
{
  fprintf(stderr, _("\
dos2unix %s (%s)\n\
Usage: dos2unix [-hkqLV] [-c convmode] [-o file ...] [-n infile outfile ...]\n\
 -h --help        give this help\n\
 -k --keepdate    keep output file date\n\
 -q --quiet       quiet mode, suppress all warnings\n\
                  always on in stdin->stdout mode\n\
 -L --license     print software license\n\
 -V --version     display version number\n\
 -c --convmode    conversion mode\n\
 convmode         ASCII, 7bit, ISO, Mac, default to ASCII\n\
 -l --newline     add additional newline in all but Mac convmode\n\
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
Copyright (c) 1998      Bernd Johannes Wuebben (Version 3.0)\n\
Copyright (c) 1998      Christian Wurll (Version 3.1)\n\
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
  fprintf(stderr, "dos2unix %s (%s)\n", VER_REVISION, VER_DATE);
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


void StripDelimiter(FILE* ipInF, FILE* ipOutF, CFlag *ipFlag, int CurChar)
{
  int TempNextChar;
  /* Don't modify Mac files when in dos2unix mode. */
  if ( (TempNextChar = getc(ipInF)) != EOF) {
    ungetc( TempNextChar, ipInF );  /* put back peek char */
    if ( TempNextChar != '\x0a' ) {
      putc( CurChar, ipOutF );  /* Mac line, put back CR */
    }
  }
  else if ( CurChar == '\x0d' ) {  /* EOF: last Mac line delimiter (CR)? */
    putc( CurChar, ipOutF );
  }
  if (ipFlag->NewLine) {  /* add additional LF? */
    putc('\n', ipOutF);
  }
}

/* converts stream ipInF to UNIX format text and write to stream ipOutF
 * RetVal: 0  if success
 *         -1  otherwise
 */
int ConvertDosToUnix(FILE* ipInF, FILE* ipOutF, CFlag *ipFlag)
{
    int RetVal = 0;
    int TempChar;
    int TempNextChar;
    
    if ( macmode )
      ipFlag->ConvMode = 3;

    switch (ipFlag->ConvMode)
    {
        case 0: /* ASCII */
	  while ((TempChar = getc(ipInF)) != EOF) {
	    if (TempChar != '\x0d') {
	      if (putc(D2UAsciiTable[TempChar], ipOutF) == EOF) {
		RetVal = -1;
		if (!ipFlag->Quiet)
		  fprintf(stderr, _("dos2unix: can not write to out file\n"));
		break;
	      } 
	    } else {
	      StripDelimiter( ipInF, ipOutF, ipFlag, TempChar );
	    }
	  }
	  break;
        case 1: /* 7Bit */
	  while ((TempChar = getc(ipInF)) != EOF) {
	    if (TempChar != '\x0d') {
	      if (putc(D2U7BitTable[TempChar], ipOutF) == EOF) {
		RetVal = -1;
		if (!ipFlag->Quiet)
		  fprintf(stderr, _("dos2unix: can not write to out file\n"));
		break;
	      }
	    } else {
	      StripDelimiter( ipInF, ipOutF, ipFlag, TempChar );
	    }
	  }
	  break;
        case 2: /* ISO */
	  while ((TempChar = getc(ipInF)) != EOF) {
	    if (TempChar != '\x0d') {
	      if (putc(D2UIsoTable[TempChar], ipOutF) == EOF) {
		RetVal = -1;
		if (!ipFlag->Quiet)
		  fprintf(stderr, _("dos2unix: can not write to out file\n"));
		break;
	      }
	    } else {
	      StripDelimiter( ipInF, ipOutF, ipFlag, TempChar );
	    }
	  }
	  break;
    case 3: /* Mac */
	  while ((TempChar = getc(ipInF)) != EOF)
	    if ((TempChar != '\x0d'))
	      {
		if(putc(D2UAsciiTable[TempChar], ipOutF) == EOF){
		  RetVal = -1;
		  if (!ipFlag->Quiet)
		    fprintf(stderr, _("dos2unix: can not write to out file\n"));
		  break;
		}
	      }
	    else{
	      if ( (TempNextChar = getc(ipInF)) != EOF) {
		ungetc( TempNextChar, ipInF );  /* put back peek char */
		/* Don't touch this delimiter if it's a CR,LF pair. */
		if ( TempNextChar == '\x0a' ) {
		  continue;
		}
	      }
	      if (putc('\x0a', ipOutF) == EOF)
		{
		  RetVal = -1;
		  if (!ipFlag->Quiet)
		    fprintf(stderr, _("dos2unix: can not write to out file\n"));
		  break;
		}
	    }
	  break;
    default: /* unknown convmode */
      ;
#ifdef DEBUG
      fprintf(stderr, _("dos2unix: program error, invalid conversion mode %d\n"),ipFlag->ConvMode);
      exit(1);
#endif
    }
    
    return RetVal;
}

static int MakeTempFileFrom(const char *OutFN, char **fname_ret)
{
  char *cpy = strdup(OutFN);
  char *dir = NULL;
  size_t fname_len = 0;
  char  *fname_str = NULL;
  int fd = -1;
  
  *fname_ret = NULL;
  
  if (!cpy)
    goto make_failed;
  
  dir = dirname(cpy);
  
  fname_len = strlen(dir) + strlen("/d2utmpXXXXXX") + sizeof (char);
  if (!(fname_str = malloc(fname_len)))
    goto make_failed;
  sprintf(fname_str, "%s%s", dir, "/d2utmpXXXXXX");
  *fname_ret = fname_str;
  
  free(cpy);
  
  if ((fd = mkstemp(fname_str)) == -1)
    goto make_failed;
  
  return (fd);
  
 make_failed:
  free(*fname_ret);
  *fname_ret = NULL;
  return (-1);
}

/* convert file ipInFN to UNIX format text and write to file ipOutFN
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertDosToUnixNewFile(char *ipInFN, char *ipOutFN, CFlag *ipFlag)
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  struct stat StatBuf;
  struct utimbuf UTimeBuf;
  int fd;

  /* retrieve ipInFN file date stamp */
  if ((ipFlag->KeepDate) && stat(ipInFN, &StatBuf))
    RetVal = -1;

  if((fd = MakeTempFileFrom(ipOutFN, &TempPath))<0) {
	  perror("Failed to open output temp file");
	  RetVal = -1;
  }

#ifdef DEBUG
  fprintf(stderr, _("dos2unix: using %s as temp file\n"), TempPath);
#endif

  /* can open in file? */
  if ((!RetVal) && ((InF=OpenInFile(ipInFN)) == NULL))
    RetVal = -1;

  /* can open out file? */
  if ((!RetVal) && (InF) && ((TempF=OpenOutFile(fd)) == NULL))
  {
    fclose (InF);
    InF = NULL;
    RetVal = -1;
  }

  /* conversion sucessful? */
  if ((!RetVal) && (ConvertDosToUnix(InF, TempF, ipFlag)))
    RetVal = -1;

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  /* can close out file? */
  if ((TempF) && (fclose(TempF) == EOF))
    RetVal = -1;
  if(fd>=0)
	  close(fd);

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
    if ((rename(TempPath, ipOutFN) == -1) && (!ipFlag->Quiet))
    {
      fprintf(stderr, _("dos2unix: problems renaming '%s' to '%s'\n"), TempPath, ipOutFN);
      fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
      RetVal = -1;
    }
  }
  free(TempPath);
  return RetVal;
}




/* convert file ipInFN to UNIX format text
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertDosToUnixOldFile(char* ipInFN, CFlag *ipFlag)
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  struct stat StatBuf;
  struct utimbuf UTimeBuf;
  mode_t mode = S_IRUSR | S_IWUSR;
  int fd;

  /* retrieve ipInFN file date stamp */
  if (stat(ipInFN, &StatBuf))
    RetVal = -1;
  else
    mode = StatBuf.st_mode;

  if((fd = MakeTempFileFrom(ipInFN, &TempPath))<0) {
	  perror("Failed to open output temp file");
	  RetVal = -1;
  }

  if (!RetVal && fchmod (fd, mode) && fchmod (fd, S_IRUSR | S_IWUSR))
    RetVal = -1;

#ifdef DEBUG
  fprintf(stderr, _("dos2unix: using %s as temp file\n"), TempPath);
#endif

  /* can open in file? */
  if ((!RetVal) && ((InF=OpenInFile(ipInFN)) == NULL))
    RetVal = -1;

  /* can open out file? */
  if ((!RetVal) && (InF) && ((TempF=OpenOutFile(fd)) == NULL))
  {
    fclose (InF);
    InF = NULL;
    RetVal = -1;
  }

  /* conversion sucessful? */
  if ((!RetVal) && (ConvertDosToUnix(InF, TempF, ipFlag)))
    RetVal = -1;

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  /* can close out file? */
  if ((TempF) && (fclose(TempF) == EOF))
    RetVal = -1;

  if(fd>=0)
	  close(fd);

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

  /* can rename out file to in file? */
  if ((!RetVal) && (rename(TempPath, ipInFN) == -1))
  {
    if (!ipFlag->Quiet)
    {
      fprintf(stderr, _("dos2unix: problems renaming '%s' to '%s'\n"), TempPath, ipInFN);
      fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
    }
    RetVal = -1;
  }
  free(TempPath);
  return RetVal;
}


/* convert stdin to UNIX format text and write to stdout
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertDosToUnixStdio(CFlag *ipFlag)
{
    ipFlag->NewFile = 1;
    ipFlag->Quiet = 1;
    ipFlag->KeepDate = 0;
    return (ConvertDosToUnix(stdin, stdout, ipFlag));
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
  pFlag->NewLine = 0;

  if( strcmp(argv[0],"mac2unix") == 0 )
    macmode = MACMODE;

  /* no option, use stdin and stdout */
  if (argc == 1)
  {
    exit(ConvertDosToUnixStdio(pFlag));
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
      if ((strcmp(argv[ArgIdx],"-l") == 0) || (strcmp(argv[ArgIdx],"--newline") == 0))
        pFlag->NewLine = 1;
      if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0))
        PrintVersion();
      if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0))
        PrintLicense();
      
      if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0))
      {
        if (++ArgIdx < argc)
        {
          if (strcmpi(argv[ArgIdx],"ASCII") == 0)
            pFlag->ConvMode = 0;
          else if (strcmpi(argv[ArgIdx], "7Bit") == 0)
            pFlag->ConvMode = 1;
          else if (strcmpi(argv[ArgIdx], "ISO") == 0)
            pFlag->ConvMode = 2;
          else if (strcmpi(argv[ArgIdx], "Mac") == 0)
            pFlag->ConvMode = 3;
          else
          {
            if (!pFlag->Quiet)
              fprintf(stderr, _("dos2unix: invalid %s conversion mode specified\n"),argv[ArgIdx]);
            ShouldExit = 1;
          }
        }
        else
        {
          ArgIdx--;
          if (!pFlag->Quiet)
            fprintf(stderr,_("dos2unix: option `%s' requires an argument\n"),argv[ArgIdx]);
          ShouldExit = 1;
        }
      }

      if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0))
      {
        /* last convert not paired */
        if (!CanSwitchFileMode)
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("dos2unix: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
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
            fprintf(stderr, _("dos2unix: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
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
            fprintf(stderr, _("dos2unix: converting file %s to file %s in UNIX format ...\n"), argv[ArgIdx-1], argv[ArgIdx]);
          if (ConvertDosToUnixNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag))
          {
            if (!pFlag->Quiet)
              fprintf(stderr, _("dos2unix: problems converting file %s to file %s\n"), argv[ArgIdx-1], argv[ArgIdx]);
            ShouldExit = 1;
          }
          CanSwitchFileMode = 1;
        }
      }
      else
      {
        if (!pFlag->Quiet)
          fprintf(stderr, _("dos2unix: converting file %s to UNIX format ...\n"), argv[ArgIdx]);
        if (ConvertDosToUnixOldFile(argv[ArgIdx], pFlag))
        {
          if (!pFlag->Quiet)
            fprintf(stderr, _("dos2unix: problems converting file %s\n"), argv[ArgIdx]);
          ShouldExit = 1;
        }
      }
    }
  }
  
  if ((!pFlag->Quiet) && (!CanSwitchFileMode))
  {
    fprintf(stderr, _("dos2unix: target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
    ShouldExit = 1;
  }
  free(pFlag);
  return (ShouldExit);
}
