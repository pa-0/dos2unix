/*
 *   Copyright (C) 2009-2012 Erwin Waterlander
 *   All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice in the documentation and/or other materials provided with
 *      the distribution.
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 *   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 *   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *   OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common.h"
#if defined(D2U_UNICODE) && defined(WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#endif

#if defined(__GLIBC__)
/* on glibc, canonicalize_file_name() broken prior to 2.4 (06-Mar-2006) */
# if __GNUC_PREREQ (2,4)
#  define USE_CANONICALIZE_FILE_NAME 1
# endif
#elif defined(__CYGWIN__)
/* on cygwin, canonicalize_file_name() available since api 0/213 */
/* (1.7.0beta61, 25-Sep-09) */
# include <cygwin/version.h>
# if (CYGWIN_VERSION_DLL_COMBINED >= 213) && (CYGWIN_VERSION_DLL_MAJOR >= 1007)
#  define USE_CANONICALIZE_FILE_NAME 1
# endif
#endif


/******************************************************************
 *
 * int symbolic_link(char *path)
 *
 * test if *path points to a file that exists and is a symbolic link
 *
 * returns 1 on success, 0 when it fails.
 *
 ******************************************************************/
int symbolic_link(char *path)
{
#ifdef S_ISLNK
   struct stat buf;

   if (STAT(path, &buf) == 0)
   {
      if (S_ISLNK(buf.st_mode))
         return(1);
   }
#endif
   return(0);
}

/******************************************************************
 *
 * int regfile(char *path, int allowSymlinks)
 *
 * test if *path points to a regular file (or is a symbolic link,
 * if allowSymlinks != 0).
 *
 * returns 0 on success, -1 when it fails.
 *
 ******************************************************************/
int regfile(char *path, int allowSymlinks, CFlag *ipFlag, char *progname)
{
   struct stat buf;
   char *errstr;

   if (STAT(path, &buf) == 0)
   {
#if DEBUG
      fprintf(stderr, "%s: %s MODE 0%o ", progname, path, buf.st_mode);
#ifdef S_ISSOCK
      if (S_ISSOCK(buf.st_mode))
         fprintf(stderr, " (socket)");
#endif
#ifdef S_ISLNK
      if (S_ISLNK(buf.st_mode))
         fprintf(stderr, " (symbolic link)");
#endif
      if (S_ISREG(buf.st_mode))
         fprintf(stderr, " (regular file)");
      if (S_ISBLK(buf.st_mode))
         fprintf(stderr, " (block device)");
      if (S_ISDIR(buf.st_mode))
         fprintf(stderr, " (directory)");
      if (S_ISCHR(buf.st_mode))
         fprintf(stderr, " (character device)");
      if (S_ISFIFO(buf.st_mode))
         fprintf(stderr, " (FIFO)");
      fprintf(stderr, "\n");
#endif
      if ((S_ISREG(buf.st_mode))
#ifdef S_ISLNK
          || (S_ISLNK(buf.st_mode) && allowSymlinks)
#endif
         )
         return(0);
      else
         return(-1);
   }
   else
   {
     if (!ipFlag->Quiet)
     {
       ipFlag->error = errno;
       errstr = strerror(errno);
       fprintf(stderr, "%s: %s: %s\n", progname, path, errstr);
     }
     return(-1);
   }
}

/******************************************************************
 *
 * int regfile_target(char *path)
 *
 * test if *path points to a regular file (follow symbolic link)
 *
 * returns 0 on success, -1 when it fails.
 *
 ******************************************************************/
int regfile_target(char *path, CFlag *ipFlag, char *progname)
{
   struct stat buf;
   char *errstr;

   if (stat(path, &buf) == 0)
   {
      if (S_ISREG(buf.st_mode))
         return(0);
      else
         return(-1);
   }
   else
   {
     if (!ipFlag->Quiet)
     {
       ipFlag->error = errno;
       errstr = strerror(errno);
       fprintf(stderr, "%s: %s: %s\n", progname, path, errstr);
     }
     return(-1);
   }
}

void PrintBSDLicense(void)
{
  fprintf(stderr, "%s", _("\
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

void PrintUsage(char *progname)
{
  fprintf(stderr, _("\
%s %s (%s)\n\
Usage: %s [options] [file ...] [-n infile outfile ...]\n\
 -ascii                convert only line breaks (default)\n\
 -iso                  conversion between DOS and ISO-8859-1 character set\n\
   -1252               Use Windows code page 1252 (Western European)\n\
   -437                Use DOS code page 437 (US) (default)\n\
   -850                Use DOS code page 850 (Western European)\n\
   -860                Use DOS code page 860 (Portuguese)\n\
   -863                Use DOS code page 863 (French Canadian)\n\
   -865                Use DOS code page 865 (Nordic)\n\
 -7                    Convert 8 bit characters to 7 bit space\n\
 -c, --convmode        conversion mode\n\
   convmode            ascii, 7bit, iso, mac, default to ascii\n\
 -f, --force           force conversion of binary files\n\
 -h, --help            give this help\n\
 -k, --keepdate        keep output file date\n\
 -L, --license         display software license\n\
 -l, --newline         add additional newline\n\
 -m, --add-bom         add UTF-8 Byte Order Mark\n\
 -n, --newfile         write to new file\n\
   infile              original file in new file mode\n\
   outfile             output file in new file mode\n\
 -o, --oldfile         write to old file\n\
   file ...            files to convert in old file mode\n\
 -q, --quiet           quiet mode, suppress all warnings\n\
                       always on in stdio mode\n\
 -s, --safe            skip binary files (default)\n"),
 progname, VER_REVISION, VER_DATE, progname);
#ifdef S_ISLNK
  fprintf(stderr, _("\
 -F, --follow-symlink  follow symbolic links and convert the targets\n\
 -R, --replace-symlink replace symbolic links with converted files\n\
                       (original target files remain unchanged)\n\
 -S, --skip-symlink    keep symbolic links and targets unchanged (default)\n"));
#endif
  fprintf(stderr, _("\
 -V, --version         display version number\n"));
}


void PrintVersion(char *progname)
{
  fprintf(stderr, "%s %s (%s)\n", progname, VER_REVISION, VER_DATE);
#if DEBUG
  fprintf(stderr, "VER_AUTHOR: %s\n", VER_AUTHOR);
#endif
#if defined(__WATCOMC__) && defined(__I86__)
  fprintf(stderr, "%s", _("DOS 16 bit version (WATCOMC).\n"));
#elif defined(__TURBOC__)
  fprintf(stderr, "%s", _("DOS 16 bit version (TURBOC).\n"));
#elif defined(__WATCOMC__) && defined(__DOS__)
  fprintf(stderr, "%s", _("DOS 32 bit version (WATCOMC).\n"));
#elif defined(DJGPP)
  fprintf(stderr, "%s", _("DOS 32 bit version (DJGPP).\n"));
#elif defined(__MSYS__)
  fprintf(stderr, "%s", _("MSYS version.\n"));
#elif defined(__CYGWIN__)
  fprintf(stderr, "%s", _("Cygwin version.\n"));
#elif defined(__WIN64__)
  fprintf(stderr, "%s", _("Windows 64 bit version (MinGW-w64).\n"));
#elif defined(__WATCOMC__) && defined(__NT__)
  fprintf(stderr, "%s", _("Windows 32 bit version (WATCOMC).\n"));
#elif defined(__WIN32__)
  fprintf(stderr, "%s", _("Windows 32 bit version (MinGW).\n"));
#elif defined (__OS2__) /* OS/2 Warp */
  fprintf(stderr, "%s", _("OS/2 version.\n"));
#endif
#ifdef D2U_UNICODE
  fprintf(stderr, "%s", _("With Unicode UTF-16 support.\n"));
#else
  fprintf(stderr, "%s", _("Without Unicode UTF-16 support.\n"));
#endif
#ifdef ENABLE_NLS
  fprintf(stderr, "%s", _("With native language support.\n"));
#else
  fprintf(stderr, "%s", "Without native language support.\n");
#endif
}

#ifdef ENABLE_NLS
void PrintLocaledir(char *localedir)
{
  fprintf(stderr, "LOCALEDIR: %s\n", localedir);
}
#endif

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

#if defined(__TURBOC__) || defined(__MSYS__)
char *dirname(char *path)
{
  char *ptr;

  if (( path == NULL) || (((ptr=strrchr(path,'/')) == NULL) && ((ptr=strrchr(path,'\\')) == NULL)) )
    return ".";
  else
  {
    *ptr = '\0';
    return(path);
  }
}
#endif

#ifdef NO_MKSTEMP
FILE* MakeTempFileFrom(const char *OutFN, char **fname_ret)
#else
int MakeTempFileFrom(const char *OutFN, char **fname_ret)
#endif
{
  char *cpy = strdup(OutFN);
  char *dir = NULL;
  size_t fname_len = 0;
  char  *fname_str = NULL;
#ifdef NO_MKSTEMP
  char *name;
  FILE *fd = NULL;
#else
  int fd = -1;
#endif
  
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

#ifdef NO_MKSTEMP
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
#ifdef NO_MKSTEMP
  return (NULL);
#else
  return (-1);
#endif
}

/* Test if *lFN is the name of a symbolic link.  If not, set *rFN equal
 * to lFN, and return 0.  If so, then use canonicalize_file_name or
 * realpath to determine the pointed-to file; the resulting name is
 * stored in newly allocated memory, *rFN is set to point to that value,
 * and 1 is returned. On error, -1 is returned and errno is set as
 * appropriate.
 *
 * Note that if symbolic links are not supported, then 0 is always returned
 * and *rFN = lFN.
 *
 * RetVal: 0 if success, and *lFN is not a symlink
 *         1 if success, and *lFN is a symlink
 *         -1 otherwise
 */
int ResolveSymbolicLink(char *lFN, char **rFN, CFlag *ipFlag, char *progname)
{
  int RetVal = 0;
#ifdef S_ISLNK
  struct stat StatBuf;
  char *errstr;
  char *targetFN = NULL;

  if (STAT(lFN, &StatBuf))
  {
    if (!ipFlag->Quiet)
    {
      ipFlag->error = errno;
      errstr = strerror(errno);
      fprintf(stderr, "%s: %s: %s\n", progname, lFN, errstr);
    }
    RetVal = -1;
  }
  else if (S_ISLNK(StatBuf.st_mode))
  {
#if USE_CANONICALIZE_FILE_NAME
    targetFN = canonicalize_file_name(lFN);
    if (!targetFN)
    {
      if (!ipFlag->Quiet)
      {
        errstr = strerror(errno);
        fprintf(stderr, "%s: %s: %s\n", progname, lFN, errstr);
        ipFlag->error = 1;
      }
      RetVal = -1;
    }
    else
    {
      *rFN = targetFN;
      RetVal = 1;
    }
#else
    /* Sigh. Use realpath, but realize that it has a fatal
     * flaw: PATH_MAX isn't necessarily the maximum path
     * length -- so realpath() might fail. */
    targetFN = (char *) malloc(PATH_MAX * sizeof(char));
    if (!targetFN)
    {
      if (!ipFlag->Quiet)
      {
        errstr = strerror(errno);
        fprintf(stderr, "%s: %s: %s\n", progname, lFN, errstr);
        ipFlag->error = 1;
      }
      RetVal = -1;
    }
    else
    {
      /* is there any platform with S_ISLNK that does not have realpath? */
      char *rVal = realpath(lFN, targetFN);
      if (!rVal)
      {
        if (!ipFlag->Quiet)
        {
          errstr = strerror(errno);
          fprintf(stderr, "%s: %s: %s\n", progname, lFN, errstr);
          ipFlag->error = 1;
        }
        free(targetFN);
        RetVal = -1;
      }
      else
      {
        *rFN = rVal;
        RetVal = 1;
      }
    }
#endif /* !USE_CANONICALIZE_FILE_NAME */
  }
  else
    *rFN = lFN;
#else  /* !S_ISLNK */
  *rFN = lFN;
#endif /* !S_ISLNK */
  return RetVal;
}

FILE *read_bom (FILE *f, int *bomtype)
{
  int bom[3];
  /* BOMs
   * UTF16-LE  ff fe
   * UTF16-BE  fe ff
   * UTF-8     ef bb bf
   */

  *bomtype = FILE_MBS;

   /* Check for BOM */
   if  (f != NULL)
   {
      if ((bom[0] = fgetc(f)) == EOF)
      {
         ungetc(bom[0], f);
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] != 0xff) && (bom[0] != 0xfe) && (bom[0] != 0xef))
      {
         ungetc(bom[0], f);
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[1] = fgetc(f)) == EOF)
      {
         ungetc(bom[1], f);
         ungetc(bom[0], f);
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xff) && (bom[1] == 0xfe)) /* UTF16-LE */
      {
         *bomtype = FILE_UTF16LE;
         return(f);
      }
      if ((bom[0] == 0xfe) && (bom[1] == 0xff)) /* UTF16-BE */
      {
         *bomtype = FILE_UTF16BE;
         return(f);
      }
      if ((bom[2] = fgetc(f)) == EOF)
      {
         ungetc(bom[2], f);
         ungetc(bom[1], f);
         ungetc(bom[0], f);
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xef) && (bom[1] == 0xbb) && (bom[2]== 0xbf)) /* UTF-8 */
      {
         *bomtype = FILE_UTF8;
         return(f);
      }
      ungetc(bom[2], f);
      ungetc(bom[1], f);
      ungetc(bom[0], f);
      *bomtype = FILE_MBS;
      return(f);
   }
  return(f);
}


#ifdef D2U_UNICODE
wint_t d2u_getwc(FILE *f, int bomtype)
{
   int c_high, c_low;
   wint_t wc;

   if (((c_low=fgetc(f)) == EOF)  || ((c_high=fgetc(f)) == EOF))
      return(WEOF);

   if (bomtype == FILE_UTF16LE)  /* UTF16 little endian */
   {
      c_high <<=8;
      wc = (wint_t)(c_high + c_low) ;
   } else {                              /* UTF16 big endian */
      c_low <<=8;
      wc = (wint_t)(c_high + c_low) ;
   }
   return(wc);
}

wint_t d2u_ungetwc(wint_t wc, FILE *f, int bomtype)
{
   int c_high, c_low;

   if (bomtype == FILE_UTF16LE)  /* UTF16 little endian */
   {
      c_high = (int)(wc & 0xff00);
      c_high >>=8;
      c_low  = (int)(wc & 0xff);
   } else {                              /* UTF16 big endian */
      c_low = (int)(wc & 0xff00);
      c_low >>=8;
      c_high  = (int)(wc & 0xff);
   }

   /* push back in reverse order */
   if ((ungetc(c_high,f) == EOF)  || (ungetc(c_low,f) == EOF))
      return(WEOF);
   return(wc);
}

/* Put wide character */
wint_t d2u_putwc(wint_t wc, FILE *f)
{
   static char mbs[8];
   int i,len;

#if defined(WIN32) && !defined(__CYGWIN__)
   /* On Windows we convert UTF-16 always to UTF-8 */
   wchar_t wstr[2];
   
   wstr[0] = (wchar_t)wc;
   wstr[1] = L'\0';

   len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, mbs, sizeof(mbs), NULL, NULL) -1;
#else
   /* On Unix we convert UTF-16 to the locale encoding */
   len = wctomb(mbs, (wchar_t)wc);
#endif

   if ( len < 0 )
   {  /* Character cannot be represented in current locale. Put a dot `.' */
      fputc(0x2e, f);
   } else {
      for (i=0; i<len; i++)
      {
         if (fputc(mbs[i], f) == EOF)
            return(WEOF);
      }
   }
   return(wc);
}
#endif
