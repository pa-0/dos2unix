/*
 *  Name: unix2dos
 *  Documentation:
 *    Convert lf ('\x0a') characters in a file to cr lf ('\x0d' '\x0a')
 *    combinations.
 *
 *  The dos2unix package is distributed under FreeBSD style license.
 *  See also http://www.freebsd.org/copyright/freebsd-license.html
 *  --------
 *
 *  Copyright (C) 2009-2014 Erwin Waterlander
 *  Copyright (C) 1994-1995 Benjamin Lin.
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
 *     Rewritten from scratch.
 *  == 2.2 == 1995.03.30 == Benjamin Lin (blin@socs.uts.edu.au)
 *     Conversion from SunOS charset implemented.
 *
 *  See ChangeLog.txt for complete version history.
 *
 */


/* #define DEBUG 1 */
#define __UNIX2DOS_C

#include "common.h"
#include "unix2dos.h"
#include "querycp.h"
#ifdef D2U_UNICODE
#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
# include <langinfo.h>
#endif
#endif

void PrintLicense(void)
{
  printf("%s", _("\
Copyright (C) 2009-2014 Erwin Waterlander\n\
Copyright (C) 1994-1995 Benjamin Lin\n\
All rights reserved.\n\n"));
  PrintBSDLicense();
}

#ifdef D2U_UNICODE
void AddDOSNewLineW(FILE* ipOutF, CFlag *ipFlag, wint_t CurChar, wint_t PrevChar)
{
  if (ipFlag->NewLine) {  /* add additional CR-LF? */
    /* Don't add line ending if it is a DOS line ending. Only in case of Unix line ending. */
    if ((CurChar == 0x0a) && (PrevChar != 0x0d)) {
      d2u_putwc(0x0d, ipOutF, ipFlag);
      d2u_putwc(0x0a, ipOutF, ipFlag);
    }
  }
}
#endif

void AddDOSNewLine(FILE* ipOutF, CFlag *ipFlag, int CurChar, int PrevChar)
{
  if (ipFlag->NewLine) {  /* add additional CR-LF? */
    /* Don't add line ending if it is a DOS line ending. Only in case of Unix line ending. */
    if ((CurChar == '\x0a') && (PrevChar != '\x0d')) {
      fputc('\x0d', ipOutF);
      fputc('\x0a', ipOutF);
    }
  }
}

/* converts stream ipInF to DOS format text and write to stream ipOutF
 * RetVal: 0  if success
 *         -1  otherwise
 */
#ifdef D2U_UNICODE
int ConvertUnixToDosW(FILE* ipInF, FILE* ipOutF, CFlag *ipFlag, char *progname)
{
    int RetVal = 0;
    wint_t TempChar;
    wint_t PreviousChar = 0;
    unsigned int line_nr = 1;
    unsigned int converted = 0;
    char *errstr;

    ipFlag->status = 0;

    /* LF    -> CR-LF */
    /* CR-LF -> CR-LF, in case the input file is a DOS text file */
    /* \x0a = Newline/Line Feed (LF) */
    /* \x0d = Carriage Return (CR) */

    switch (ipFlag->FromToMode)
    {
      case FROMTO_UNIX2DOS: /* unix2dos */
        while ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {  /* get character */
          if ((ipFlag->Force == 0) &&
              (TempChar < 32) &&
              (TempChar != 0x0a) &&  /* Not an LF */
              (TempChar != 0x0d) &&  /* Not a CR */
              (TempChar != 0x09) &&  /* Not a TAB */
              (TempChar != 0x0c)) {  /* Not a form feed */
            RetVal = -1;
            ipFlag->status |= BINARY_FILE ;
            if (ipFlag->verbose)
            {
              fprintf(stderr, "%s: ", progname);
              fprintf(stderr, _("Binary symbol 0x00%02X found at line %d\n"),TempChar, line_nr);
            }
            break;
          }
          if (TempChar == 0x0a)
          {
            d2u_putwc(0x0d, ipOutF, ipFlag); /* got LF, put extra CR */
            converted++;
          } else {
             if (TempChar == 0x0d) /* got CR */
             {
               if ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) == WEOF) /* get next char (possibly LF) */
                 TempChar = 0x0d;  /* Read error, or end of file. */
               else
               {
                 d2u_putwc(0x0d, ipOutF, ipFlag); /* put CR */
                 PreviousChar = 0x0d;
               }
             }
          }
          if (TempChar == 0x0a) /* Count all DOS and Unix line breaks */
            ++line_nr;
          if (d2u_putwc(TempChar, ipOutF, ipFlag) == WEOF)
          {
              RetVal = -1;
              if (ipFlag->verbose)
              {
                if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
                {
                  errstr = strerror(errno);
                  fprintf(stderr, "%s: ", progname);
                  fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                }
              }
              break;
          } else {
            AddDOSNewLineW( ipOutF, ipFlag, TempChar, PreviousChar);
          }
          PreviousChar = TempChar;
        }
        break;
      case FROMTO_UNIX2MAC: /* unix2mac */
        while ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {
          if ((ipFlag->Force == 0) &&
              (TempChar < 32) &&
              (TempChar != 0x0a) &&  /* Not an LF */
              (TempChar != 0x0d) &&  /* Not a CR */
              (TempChar != 0x09) &&  /* Not a TAB */
              (TempChar != 0x0c)) {  /* Not a form feed */
            RetVal = -1;
            ipFlag->status |= BINARY_FILE ;
            if (ipFlag->verbose)
            {
              fprintf(stderr, "%s: ", progname);
              fprintf(stderr, _("Binary symbol 0x00%02X found at line %d\n"),TempChar, line_nr);
            }
            break;
          }
          if (TempChar != 0x0a) /* Not an LF */
          {
            if(d2u_putwc(TempChar, ipOutF, ipFlag) == WEOF){
              RetVal = -1;
              if (ipFlag->verbose)
              {
                if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
                {
                  errstr = strerror(errno);
                  fprintf(stderr, "%s: ", progname);
                  fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                }
              }
              break;
            }
            PreviousChar = TempChar;
            if (TempChar == 0x0d) /* CR */
              ++line_nr;
          }
          else{
            /* TempChar is an LF */
            if (PreviousChar != 0x0d) /* CR already counted */
              ++line_nr;
            /* Don't touch this delimiter if it's a CR,LF pair. */
            if ( PreviousChar == 0x0d ) {
              if (d2u_putwc(0x0a, ipOutF, ipFlag) == WEOF)  /* CR,LF pair. Put LF */
                {
                  RetVal = -1;
                  if (ipFlag->verbose)
                  {
                    if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
                    {
                      errstr = strerror(errno);
                      fprintf(stderr, "%s: ", progname);
                      fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                    }
                  }
                  break;
                }
              PreviousChar = TempChar;
              continue;
            }
            PreviousChar = TempChar;
            if (d2u_putwc(0x0d, ipOutF, ipFlag) == WEOF) /* Unix line end (LF). Put CR */
              {
                RetVal = -1;
                if (ipFlag->verbose)
                {
                  if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
                  {
                    errstr = strerror(errno);
                    fprintf(stderr, "%s: ", progname);
                    fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                  }
                }
                break;
              }
            converted++;
            if (ipFlag->NewLine) {  /* add additional CR? */
              d2u_putwc(0x0d, ipOutF, ipFlag);
            }
          }
        }
        break;
      default: /* unknown FromToMode */
      ;
#if DEBUG
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("program error, invalid conversion mode %d\n"),ipFlag->FromToMode);
      exit(1);
#endif
    }
    if (ipFlag->verbose > 1)
    {
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Converted %d out of %d line breaks.\n"),converted, line_nr -1);
    }
    return RetVal;
}
#endif

/* converts stream ipInF to DOS format text and write to stream ipOutF
 * RetVal: 0  if success
 *         -1  otherwise
 */
int ConvertUnixToDos(FILE* ipInF, FILE* ipOutF, CFlag *ipFlag, char *progname)
{
    int RetVal = 0;
    int TempChar;
    int PreviousChar = 0;
    int *ConvTable;
    unsigned int line_nr = 1;
    unsigned int converted = 0;
    char *errstr;

    ipFlag->status = 0;

    switch (ipFlag->ConvMode)
    {
      case CONVMODE_ASCII: /* ascii */
      case CONVMODE_UTF16LE: /* Assume UTF-16LE, bomtype = FILE_UTF8 */
      case CONVMODE_UTF16BE: /* Assume UTF-16BE, bomtype = FILE_UTF8 */
        ConvTable = U2DAsciiTable;
        break;
      case CONVMODE_7BIT: /* 7bit */
        ConvTable = U2D7BitTable;
        break;
      case CONVMODE_437: /* iso */
        ConvTable = U2DIso437Table;
        break;
      case CONVMODE_850: /* iso */
        ConvTable = U2DIso850Table;
        break;
      case CONVMODE_860: /* iso */
        ConvTable = U2DIso860Table;
        break;
      case CONVMODE_863: /* iso */
        ConvTable = U2DIso863Table;
        break;
      case CONVMODE_865: /* iso */
        ConvTable = U2DIso865Table;
        break;
      case CONVMODE_1252: /* iso */
        ConvTable = U2DIso1252Table;
        break;
      default: /* unknown convmode */
        ipFlag->status |= WRONG_CODEPAGE ;
        return(-1);
    }
    if ((ipFlag->ConvMode > CONVMODE_7BIT) && (ipFlag->verbose)) /* not ascii or 7bit */
    {
       fprintf(stderr, "%s: ", progname);
       fprintf(stderr, _("using code page %d.\n"), ipFlag->ConvMode);
    }

    /* LF    -> CR-LF */
    /* CR-LF -> CR-LF, in case the input file is a DOS text file */
    /* \x0a = Newline/Line Feed (LF) */
    /* \x0d = Carriage Return (CR) */

    switch (ipFlag->FromToMode)
    {
      case FROMTO_UNIX2DOS: /* unix2dos */
        while ((TempChar = fgetc(ipInF)) != EOF) {  /* get character */
          if ((ipFlag->Force == 0) &&
              (TempChar < 32) &&
              (TempChar != '\x0a') &&  /* Not an LF */
              (TempChar != '\x0d') &&  /* Not a CR */
              (TempChar != '\x09') &&  /* Not a TAB */
              (TempChar != '\x0c')) {  /* Not a form feed */
            RetVal = -1;
            ipFlag->status |= BINARY_FILE ;
            if (ipFlag->verbose)
            {
              fprintf(stderr, "%s: ", progname);
              fprintf(stderr, _("Binary symbol 0x%02X found at line %d\n"),TempChar, line_nr);
            }
            break;
          }
          if (TempChar == '\x0a')
          {
            fputc('\x0d', ipOutF); /* got LF, put extra CR */
            converted++;
          } else {
             if (TempChar == '\x0d') /* got CR */
             {
               if ((TempChar = fgetc(ipInF)) == EOF) /* get next char (possibly LF) */
                 TempChar = '\x0d';  /* Read error, or end of file. */
               else
               {
                 fputc('\x0d', ipOutF); /* put CR */
                 PreviousChar = '\x0d';
               }
             }
          }
          if (TempChar == '\x0a') /* Count all DOS and Unix line breaks */
            ++line_nr;
          if (fputc(ConvTable[TempChar], ipOutF) == EOF) /* put LF or other char */
          {
              RetVal = -1;
              if (ipFlag->verbose)
              {
                errstr = strerror(errno);
                fprintf(stderr, "%s: ", progname);
                fprintf(stderr, _("can not write to output file: %s\n"), errstr);
              }
              break;
          } else {
            AddDOSNewLine( ipOutF, ipFlag, TempChar, PreviousChar);
          }
          PreviousChar = TempChar;
        }
        break;
      case FROMTO_UNIX2MAC: /* unix2mac */
        while ((TempChar = fgetc(ipInF)) != EOF) {
          if ((ipFlag->Force == 0) &&
              (TempChar < 32) &&
              (TempChar != '\x0a') &&  /* Not an LF */
              (TempChar != '\x0d') &&  /* Not a CR */
              (TempChar != '\x09') &&  /* Not a TAB */
              (TempChar != '\x0c')) {  /* Not a form feed */
            RetVal = -1;
            ipFlag->status |= BINARY_FILE ;
            if (ipFlag->verbose)
            {
              fprintf(stderr, "%s: ", progname);
              fprintf(stderr, _("Binary symbol 0x%02X found at line %d\n"),TempChar, line_nr);
            }
            break;
          }
          if (TempChar != '\x0a') /* Not an LF */
          {
            if(fputc(ConvTable[TempChar], ipOutF) == EOF){
              RetVal = -1;
              if (ipFlag->verbose)
              {
                errstr = strerror(errno);
                fprintf(stderr, "%s: ", progname);
                fprintf(stderr, _("can not write to output file: %s\n"), errstr);
              }
              break;
            }
            PreviousChar = TempChar;
            if (TempChar == '\x0d') /* CR */
              ++line_nr;
          }
          else {
            /* TempChar is an LF */
            if (PreviousChar != '\x0d') /* CR already counted */
              ++line_nr;
            /* Don't touch this delimiter if it's a CR,LF pair. */
            if ( PreviousChar == '\x0d' ) {
              if (fputc('\x0a', ipOutF) == EOF)  /* CR,LF pair. Put LF */
                {
                  RetVal = -1;
                  if (ipFlag->verbose)
                  {
                    errstr = strerror(errno);
                    fprintf(stderr, "%s: ", progname);
                    fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                  }
                  break;
                }
              PreviousChar = TempChar;
              continue;
            }
            PreviousChar = TempChar;
            if (fputc('\x0d', ipOutF) == EOF) /* Unix line end (LF). Put CR */
              {
                RetVal = -1;
                if (ipFlag->verbose)
                {
                  errstr = strerror(errno);
                  fprintf(stderr, "%s: ", progname);
                  fprintf(stderr, _("can not write to output file: %s\n"), errstr);
                }
                break;
              }
            converted++;
            if (ipFlag->NewLine) {  /* add additional CR? */
              fputc('\x0d', ipOutF);
            }
          }
        }
        break;
      default: /* unknown FromToMode */
      ;
#if DEBUG
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("program error, invalid conversion mode %d\n"),ipFlag->FromToMode);
      exit(1);
#endif
    }
    if (ipFlag->verbose > 1)
    {
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Converted %d out of %d line breaks.\n"),converted, line_nr -1);
    }
    return RetVal;
}


int main (int argc, char *argv[])
{
  /* variable declarations */
  char progname[9];
  int ArgIdx;
  int CanSwitchFileMode;
  int ShouldExit;
  int RetVal = 0;
  int process_options = 1;
  CFlag *pFlag;
  char *ptr;
#ifdef ENABLE_NLS
  char localedir[1024];
#endif
# ifdef __MINGW64__
  int _dowildcard = -1; /* enable wildcard expansion for Win64 */
# endif

  progname[8] = '\0';
  strcpy(progname,"unix2dos");

#ifdef ENABLE_NLS
   ptr = getenv("DOS2UNIX_LOCALEDIR");
   if (ptr == NULL)
      strcpy(localedir,LOCALEDIR);
   else
   {
      if (strlen(ptr) < sizeof(localedir))
         strcpy(localedir,ptr);
      else
      {
         fprintf(stderr,"%s: ",progname);
         fprintf(stderr, "%s", _("error: Value of environment variable DOS2UNIX_LOCALEDIR is too long.\n"));
         strcpy(localedir,LOCALEDIR);
      }
   }
#endif

#if defined(ENABLE_NLS) || (defined(D2U_UNICODE) && !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__))
/* setlocale() is also needed for nl_langinfo() */
   setlocale (LC_ALL, "");
#endif

#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, localedir);
   textdomain (PACKAGE);
#endif


  /* variable initialisations */
  ArgIdx = 0;
  CanSwitchFileMode = 1;
  ShouldExit = 0;
  pFlag = (CFlag*)malloc(sizeof(CFlag));
  pFlag->NewFile = 0;
  pFlag->verbose = 1;
  pFlag->KeepDate = 0;
  pFlag->ConvMode = CONVMODE_ASCII;  /* default ascii */
  pFlag->FromToMode = FROMTO_UNIX2DOS;  /* default unix2dos */
  pFlag->NewLine = 0;
  pFlag->Force = 0;
  pFlag->Follow = SYMLINK_SKIP;
  pFlag->status = 0;
  pFlag->stdio_mode = 1;
  pFlag->error = 0;
  pFlag->bomtype = FILE_MBS;
  pFlag->add_bom = 0;
  pFlag->keep_bom = 1;
  pFlag->keep_utf16 = 0;

  if ( ((ptr=strrchr(argv[0],'/')) == NULL) && ((ptr=strrchr(argv[0],'\\')) == NULL) )
    ptr = argv[0];
  else
    ptr++;

  if ((strcmpi("unix2mac", ptr) == 0) || (strcmpi("unix2mac.exe", ptr) == 0))
  {
    pFlag->FromToMode = FROMTO_UNIX2MAC;
    strcpy(progname,"unix2mac");
  }

  while ((++ArgIdx < argc) && (!ShouldExit))
  {
    /* is it an option? */
    if ((argv[ArgIdx][0] == '-') && process_options)
    {
      /* an option */
      if (strcmp(argv[ArgIdx],"--") == 0)
        process_options = 0;
      else if ((strcmp(argv[ArgIdx],"-h") == 0) || (strcmp(argv[ArgIdx],"--help") == 0))
      {
        PrintUsage(progname);
        return(pFlag->error);
      }
      else if ((strcmp(argv[ArgIdx],"-b") == 0) || (strcmp(argv[ArgIdx],"--keep-bom") == 0))
        pFlag->keep_bom = 1;
      else if ((strcmp(argv[ArgIdx],"-k") == 0) || (strcmp(argv[ArgIdx],"--keepdate") == 0))
        pFlag->KeepDate = 1;
      else if ((strcmp(argv[ArgIdx],"-f") == 0) || (strcmp(argv[ArgIdx],"--force") == 0))
        pFlag->Force = 1;
      else if ((strcmp(argv[ArgIdx],"-s") == 0) || (strcmp(argv[ArgIdx],"--safe") == 0))
        pFlag->Force = 0;
      else if ((strcmp(argv[ArgIdx],"-q") == 0) || (strcmp(argv[ArgIdx],"--quiet") == 0))
        pFlag->verbose = 0;
      else if ((strcmp(argv[ArgIdx],"-v") == 0) || (strcmp(argv[ArgIdx],"--verbose") == 0))
        pFlag->verbose = 2;
      else if ((strcmp(argv[ArgIdx],"-l") == 0) || (strcmp(argv[ArgIdx],"--newline") == 0))
        pFlag->NewLine = 1;
      else if ((strcmp(argv[ArgIdx],"-m") == 0) || (strcmp(argv[ArgIdx],"--add-bom") == 0))
        pFlag->add_bom = 1;
      else if ((strcmp(argv[ArgIdx],"-r") == 0) || (strcmp(argv[ArgIdx],"--remove-bom") == 0))
      {
        pFlag->keep_bom = 0;
        pFlag->add_bom = 0;
      }
      else if ((strcmp(argv[ArgIdx],"-S") == 0) || (strcmp(argv[ArgIdx],"--skip-symlink") == 0))
        pFlag->Follow = SYMLINK_SKIP;
      else if ((strcmp(argv[ArgIdx],"-F") == 0) || (strcmp(argv[ArgIdx],"--follow-symlink") == 0))
        pFlag->Follow = SYMLINK_FOLLOW;
      else if ((strcmp(argv[ArgIdx],"-R") == 0) || (strcmp(argv[ArgIdx],"--replace-symlink") == 0))
        pFlag->Follow = SYMLINK_REPLACE;
      else if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0))
      {
        PrintVersion(progname);
#ifdef ENABLE_NLS
        PrintLocaledir(localedir);
#endif
        return(pFlag->error);
      }
      else if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0))
      {
        PrintLicense();
        return(pFlag->error);
      }
      else if (strcmp(argv[ArgIdx],"-ascii") == 0)  /* SunOS compatible options */
      {
        pFlag->ConvMode = CONVMODE_ASCII;
        pFlag->keep_utf16 = 0;
      }
      else if (strcmp(argv[ArgIdx],"-7") == 0)
        pFlag->ConvMode = CONVMODE_7BIT;
      else if (strcmp(argv[ArgIdx],"-iso") == 0)
      {
        pFlag->ConvMode = (int)query_con_codepage();
        if (pFlag->verbose)
        {
           fprintf(stderr,"%s: ",progname);
           fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
        }
        if (pFlag->ConvMode < 2)
           pFlag->ConvMode = CONVMODE_437;
      }
      else if (strcmp(argv[ArgIdx],"-437") == 0)
        pFlag->ConvMode = CONVMODE_437;
      else if (strcmp(argv[ArgIdx],"-850") == 0)
        pFlag->ConvMode = CONVMODE_850;
      else if (strcmp(argv[ArgIdx],"-860") == 0)
        pFlag->ConvMode = CONVMODE_860;
      else if (strcmp(argv[ArgIdx],"-863") == 0)
        pFlag->ConvMode = CONVMODE_863;
      else if (strcmp(argv[ArgIdx],"-865") == 0)
        pFlag->ConvMode = CONVMODE_865;
      else if (strcmp(argv[ArgIdx],"-1252") == 0)
        pFlag->ConvMode = CONVMODE_1252;
#ifdef D2U_UNICODE
      else if ((strcmp(argv[ArgIdx],"-u") == 0) || (strcmp(argv[ArgIdx],"--keep-utf16") == 0))
        pFlag->keep_utf16 = 1;
      else if ((strcmp(argv[ArgIdx],"-ul") == 0) || (strcmp(argv[ArgIdx],"--assume-utf16le") == 0))
        pFlag->ConvMode = CONVMODE_UTF16LE;
      else if ((strcmp(argv[ArgIdx],"-ub") == 0) || (strcmp(argv[ArgIdx],"--assume-utf16be") == 0))
        pFlag->ConvMode = CONVMODE_UTF16BE;
#endif
      else if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0))
      {
        if (++ArgIdx < argc)
        {
          if (strcmpi(argv[ArgIdx],"ascii") == 0)  /* Benjamin Lin's legacy options */
          {
            pFlag->ConvMode = CONVMODE_ASCII;
            pFlag->keep_utf16 = 0;
          }
          else if (strcmpi(argv[ArgIdx], "7bit") == 0)
            pFlag->ConvMode = CONVMODE_7BIT;
          else if (strcmpi(argv[ArgIdx], "iso") == 0)
          {
            pFlag->ConvMode = (int)query_con_codepage();
            if (pFlag->verbose)
            {
               fprintf(stderr,"%s: ",progname);
               fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
            }
            if (pFlag->ConvMode < 2)
               pFlag->ConvMode = CONVMODE_437;
          }
          else if (strcmpi(argv[ArgIdx], "mac") == 0)
            pFlag->FromToMode = FROMTO_UNIX2MAC;
          else
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("invalid %s conversion mode specified\n"),argv[ArgIdx]);
            pFlag->error = 1;
            ShouldExit = 1;
            pFlag->stdio_mode = 0;
          }
        }
        else
        {
          ArgIdx--;
          fprintf(stderr,"%s: ",progname);
          fprintf(stderr,_("option '%s' requires an argument\n"),argv[ArgIdx]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
      }

      else if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0))
      {
        /* last convert not paired */
        if (!CanSwitchFileMode)
        {
          fprintf(stderr,"%s: ",progname);
          fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
        pFlag->NewFile = 0;
      }

      else if ((strcmp(argv[ArgIdx],"-n") == 0) || (strcmp(argv[ArgIdx],"--newfile") == 0))
      {
        /* last convert not paired */
        if (!CanSwitchFileMode)
        {
          fprintf(stderr,"%s: ",progname);
          fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
        pFlag->NewFile = 1;
      }
      else { /* wrong option */
        PrintUsage(progname);
        ShouldExit = 1;
        pFlag->error = 1;
        pFlag->stdio_mode = 0;
      }
    }
    else
    {
      pFlag->stdio_mode = 0;
      /* not an option */
      if (pFlag->NewFile)
      {
        if (CanSwitchFileMode)
          CanSwitchFileMode = 0;
        else
        {
#ifdef D2U_UNICODE
          RetVal = ConvertNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag, progname, ConvertUnixToDos, ConvertUnixToDosW);
#else
          RetVal = ConvertNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag, progname, ConvertUnixToDos);
#endif
          print_errors_newfile(pFlag, argv[ArgIdx-1], argv[ArgIdx], progname, RetVal);
          CanSwitchFileMode = 1;
        }
      }
      else
      {
#ifdef D2U_UNICODE
        RetVal = ConvertNewFile(argv[ArgIdx], argv[ArgIdx], pFlag, progname, ConvertUnixToDos, ConvertUnixToDosW);
#else
        RetVal = ConvertNewFile(argv[ArgIdx], argv[ArgIdx], pFlag, progname, ConvertUnixToDos);
#endif
        if (pFlag->status & NO_REGFILE)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping %s, not a regular file.\n"), argv[ArgIdx]);
          }
        } else if (pFlag->status & OUTPUTFILE_SYMLINK)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping symbolic link %s.\n"), argv[ArgIdx]);
          }
        } else if (pFlag->status & INPUT_TARGET_NO_REGFILE)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), argv[ArgIdx]);
          }
        } else if (pFlag->status & BINARY_FILE)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping binary file %s\n"), argv[ArgIdx]);
          }
        } else if (pFlag->status & WRONG_CODEPAGE)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
          }
        } else if (pFlag->status & LOCALE_NOT_UTF8)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping UTF-16 file %s, the current locale character encoding is not UTF-8.\n"), argv[ArgIdx]);
          }
        } else if (pFlag->status & WCHAR_T_TOO_SMALL)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), argv[ArgIdx], (int)sizeof(wchar_t));
          }
        } else if (pFlag->status & UNICODE_CONVERSION_ERROR)
        {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred.\n"), argv[ArgIdx]);
          }
        } else {
          if (pFlag->verbose)
          {
            fprintf(stderr,"%s: ",progname);
            if (pFlag->FromToMode == FROMTO_UNIX2MAC)
              fprintf(stderr, _("converting file %s to Mac format...\n"), argv[ArgIdx]);
            else
              fprintf(stderr, _("converting file %s to DOS format...\n"), argv[ArgIdx]);
          }
          if (RetVal)
          {
            if (pFlag->verbose)
            {
              fprintf(stderr,"%s: ",progname);
              fprintf(stderr, _("problems converting file %s\n"), argv[ArgIdx]);
            }
          }
        }
      }
    }
  }

  /* no file argument, use stdin and stdout */
  if (pFlag->stdio_mode)
  {
#ifdef D2U_UNICODE
    ConvertStdio(pFlag, progname, ConvertUnixToDos, ConvertUnixToDosW);
#else
    ConvertStdio(pFlag, progname, ConvertUnixToDos);
#endif
    print_errors_stdio(pFlag, progname);
    return pFlag->error;
  }


  if (!CanSwitchFileMode)
  {
    fprintf(stderr,"%s: ",progname);
    fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
    pFlag->error = 1;
  }
  return (pFlag->error);
}

