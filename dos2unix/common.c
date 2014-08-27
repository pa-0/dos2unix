/*
 *   Copyright (C) 2009-2014 Erwin Waterlander
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
#include "dos2unix.h"

#if defined(D2U_UNICODE)
#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif
#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
# include <langinfo.h>
#endif
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

#if (defined(_WIN32) && !defined(__CYGWIN__))

int symbolic_link(const char *path)
{
   DWORD attrs;

   attrs = GetFileAttributes(path);

   if (attrs == INVALID_FILE_ATTRIBUTES)
      return(0);

   return ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
}

#else
int symbolic_link(const char *path)
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
#endif

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
#ifdef S_ISBLK
      if (S_ISBLK(buf.st_mode))
         fprintf(stderr, " (block device)");
#endif
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
     if (ipFlag->verbose)
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
     if (ipFlag->verbose)
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
  printf("%s", _("\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions\n\
are met:\n\
1. Redistributions of source code must retain the above copyright\n\
   notice, this list of conditions and the following disclaimer.\n\
2. Redistributions in binary form must reproduce the above copyright\n\
   notice in the documentation and/or other materials provided with\n\
   the distribution.\n\n\
"));
  printf("%s", _("\
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

int is_dos2unix(const char *progname)
{
  if ((strncmp(progname, "dos2unix", sizeof("dos2unix")) == 0) || (strncmp(progname, "mac2unix", sizeof("mac2unix")) == 0))
    return 1;
  else
    return 0;
}

void PrintUsage(const char *progname)
{
  printf(_("Usage: %s [options] [file ...] [-n infile outfile ...]\n"), progname);
  printf(_(" -ascii                convert only line breaks (default)\n"));
  printf(_(" -iso                  conversion between DOS and ISO-8859-1 character set\n"));
  printf(_("   -1252               use Windows code page 1252 (Western European)\n"));
  printf(_("   -437                use DOS code page 437 (US) (default)\n"));
  printf(_("   -850                use DOS code page 850 (Western European)\n"));
  printf(_("   -860                use DOS code page 860 (Portuguese)\n"));
  printf(_("   -863                use DOS code page 863 (French Canadian)\n"));
  printf(_("   -865                use DOS code page 865 (Nordic)\n"));
  printf(_(" -7                    convert 8 bit characters to 7 bit space\n"));
  if (is_dos2unix(progname))
    printf(_(" -b, --keep-bom        keep Byte Order Mark\n"));
  else
    printf(_(" -b, --keep-bom        keep Byte Order Mark (default)\n"));
  printf(_(" -c, --convmode        conversion mode\n\
   convmode            ascii, 7bit, iso, mac, default to ascii\n"));
  printf(_(" -f, --force           force conversion of binary files\n"));
  printf(_(" -h, --help            display this help text\n"));
  printf(_(" -k, --keepdate        keep output file date\n"));
  printf(_(" -L, --license         display software license\n"));
  printf(_(" -l, --newline         add additional newline\n"));
  printf(_(" -m, --add-bom         add Byte Order Mark (default UTF-8)\n"));
  printf(_(" -n, --newfile         write to new file\n\
   infile              original file in new-file mode\n\
   outfile             output file in new-file mode\n"));
  printf(_(" -o, --oldfile         write to old file (default)\n\
   file ...            files to convert in old-file mode\n"));
  printf(_(" -q, --quiet           quiet mode, suppress all warnings\n"));
  if (is_dos2unix(progname))
    printf(_(" -r, --remove-bom      remove Byte Order Mark (default)\n"));
  else
    printf(_(" -r, --remove-bom      remove Byte Order Mark\n"));
  printf(_(" -s, --safe            skip binary files (default)\n"));
#ifdef D2U_UNICODE
  printf(_(" -u,  --keep-utf16     keep UTF-16 encoding\n"));
  printf(_(" -ul, --assume-utf16le assume that the input format is UTF-16LE\n"));
  printf(_(" -ub, --assume-utf16be assume that the input format is UTF-16BE\n"));
#endif
  printf(_(" -v,  --verbose        verbose operation\n"));
#ifdef S_ISLNK
  printf(_(" -F, --follow-symlink  follow symbolic links and convert the targets\n"));
#endif
#if defined(S_ISLNK) || (defined(_WIN32) && !defined(__CYGWIN__))
  printf(_(" -R, --replace-symlink replace symbolic links with converted files\n\
                         (original target files remain unchanged)\n"));
  printf(_(" -S, --skip-symlink    keep symbolic links and targets unchanged (default)\n"));
#endif
  printf(_(" -V, --version         display version number\n"));
}

#define MINGW32_W64 1

void PrintVersion(char *progname)
{
  printf("%s %s (%s)\n", progname, VER_REVISION, VER_DATE);
#if DEBUG
  printf("VER_AUTHOR: %s\n", VER_AUTHOR);
#endif
#if defined(__WATCOMC__) && defined(__I86__)
  printf("%s", _("DOS 16 bit version (WATCOMC).\n"));
#elif defined(__TURBOC__) && defined(__MSDOS__)
  printf("%s", _("DOS 16 bit version (TURBOC).\n"));
#elif defined(__WATCOMC__) && defined(__DOS__)
  printf("%s", _("DOS 32 bit version (WATCOMC).\n"));
#elif defined(__DJGPP__)
  printf("%s", _("DOS 32 bit version (DJGPP).\n"));
#elif defined(__MSYS__)
  printf("%s", _("MSYS version.\n"));
#elif defined(__CYGWIN__)
  printf("%s", _("Cygwin version.\n"));
#elif defined(__WIN64__) && defined(__MINGW64__)
  printf("%s", _("Windows 64 bit version (MinGW-w64).\n"));
#elif defined(__WATCOMC__) && defined(__NT__)
  printf("%s", _("Windows 32 bit version (WATCOMC).\n"));
#elif defined(_WIN32) && defined(__MINGW32__) && (D2U_COMPILER == MINGW32_W64)
  printf("%s", _("Windows 32 bit version (MinGW-w64).\n"));
#elif defined(_WIN32) && defined(__MINGW32__)
  printf("%s", _("Windows 32 bit version (MinGW).\n"));
#elif defined(_WIN64) && defined(_MSC_VER)
  printf(_("Windows 64 bit version (MSVC %d).\n"),_MSC_VER);
#elif defined(_WIN32) && defined(_MSC_VER)
  printf(_("Windows 32 bit version (MSVC %d).\n"),_MSC_VER);
#elif defined (__OS2__) && defined(__WATCOMC__) /* OS/2 Warp */
  printf("%s", _("OS/2 version (WATCOMC).\n"));
#elif defined (__OS2__) && defined(__EMX__) /* OS/2 Warp */
  printf("%s", _("OS/2 version (EMX).\n"));
#endif
#ifdef D2U_UNICODE
  printf("%s", _("With Unicode UTF-16 support.\n"));
#else
  printf("%s", _("Without Unicode UTF-16 support.\n"));
#endif
#ifdef ENABLE_NLS
  printf("%s", _("With native language support.\n"));
#else
  printf("%s", "Without native language support.\n");
#endif
}

#ifdef ENABLE_NLS
void PrintLocaledir(char *localedir)
{
  printf("LOCALEDIR: %s\n", localedir);
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

#if defined(__TURBOC__) || defined(__MSYS__) || defined(_MSC_VER)
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
    if (ipFlag->verbose)
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
      if (ipFlag->verbose)
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
      if (ipFlag->verbose)
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
        if (ipFlag->verbose)
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

FILE *write_bom (FILE *f, CFlag *ipFlag, const char *progname)
{
  if (ipFlag->keep_utf16)
  {
    switch (ipFlag->bomtype)
    {
      case FILE_UTF16LE:   /* UTF-16 Little Endian */
        fprintf(f, "%s", "\xFF\xFE");
        if (ipFlag->verbose > 1)
        {
          fprintf(stderr, "%s: ", progname);
          fprintf(stderr, _("Writing %s BOM.\n"), "UTF-16LE");
        }
        break;
      case FILE_UTF16BE:   /* UTF-16 Big Endian */
        fprintf(f, "%s", "\xFE\xFF");
        if (ipFlag->verbose > 1)
        {
          fprintf(stderr, "%s: ", progname);
          fprintf(stderr, _("Writing %s BOM.\n"), "UTF-16BE");
        }
        break;
      case FILE_UTF8:      /* UTF-8 */
        fprintf(f, "%s", "\xEF\xBB\xBF");
        if (ipFlag->verbose > 1)
        {
          fprintf(stderr, "%s: ", progname);
          fprintf(stderr, _("Writing %s BOM.\n"), "UTF-8");
        }
        break;
      default:
      ;
    }
  } else {
    fprintf(f, "%s", "\xEF\xBB\xBF");
    if (ipFlag->verbose > 1)
    {
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Writing %s BOM.\n"), "UTF-8");
    }
  }
  return(f);
}

void print_bom (const int bomtype, const char *filename, const char *progname)
{
    switch (bomtype)
    {
    case FILE_UTF16LE:   /* UTF-16 Little Endian */
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Input file %s has %s BOM.\n"), filename, "UTF-16LE");
      break;
    case FILE_UTF16BE:   /* UTF-16 Big Endian */
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Input file %s has %s BOM.\n"), filename, "UTF-16BE");
      break;
    case FILE_UTF8:      /* UTF-8 */
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Input file %s has %s BOM.\n"), filename, "UTF-8");
      break;
    default:
    ;
  }
}

int check_unicode(FILE *InF, FILE *TempF,  CFlag *ipFlag, const char *ipInFN, const char *progname)
{
  int RetVal = 0;

#ifdef D2U_UNICODE
  if (ipFlag->verbose > 1) {
    if (ipFlag->ConvMode == CONVMODE_UTF16LE) {
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Assuming UTF-16LE encoding.\n") );
    }
    if (ipFlag->ConvMode == CONVMODE_UTF16BE) {
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Assuming UTF-16BE encoding.\n") );
    }
  }
#endif
  InF = read_bom(InF, &ipFlag->bomtype);
  if (ipFlag->verbose > 1)
    print_bom(ipFlag->bomtype, ipInFN, progname);
#ifdef D2U_UNICODE
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16LE))
    ipFlag->bomtype = FILE_UTF16LE;
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16BE))
    ipFlag->bomtype = FILE_UTF16BE;
#endif

#ifdef D2U_UNICODE
#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
  if (!ipFlag->keep_utf16 && ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))) {
    if (strcmp(nl_langinfo(CODESET), "UTF-8") != 0) {
      /* Don't convert UTF-16 files when the locale encoding is not UTF-8
       * to prevent loss of characters. */
      ipFlag->status |= LOCALE_NOT_UTF8 ;
      if (!ipFlag->error) ipFlag->error = 1;
      RetVal = -1;
    }
  }
#endif
#if !defined(_WIN32) && !defined(__CYGWIN__) /* Not Windows or Cygwin */
  if (!ipFlag->keep_utf16 && ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))) {
    if (sizeof(wchar_t) < 4) {
      /* A decoded UTF-16 surrogate pair must fit in a wchar_t */
      ipFlag->status |= WCHAR_T_TOO_SMALL ;
      if (!ipFlag->error) ipFlag->error = 1;
      RetVal = -1;
    }
  }
#endif
#endif

  if ((ipFlag->add_bom) || ((ipFlag->keep_bom) && (ipFlag->bomtype > 0)))
    write_bom(TempF, ipFlag, progname);

  /* Turn off ISO and 7-bit conversion for Unicode text files */
  /* When we assume UTF16, don't change the conversion mode. We need to remember it. */
  if ((ipFlag->bomtype > 0) && (ipFlag->ConvMode != CONVMODE_UTF16LE) && (ipFlag->ConvMode != CONVMODE_UTF16BE))
    ipFlag->ConvMode = CONVMODE_ASCII;

  return RetVal;
}

/* convert file ipInFN and write to file ipOutFN
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertNewFile(char *ipInFN, char *ipOutFN, CFlag *ipFlag, char *progname,
                   int (*Convert)(FILE*, FILE*, CFlag *, char *)
#ifdef D2U_UNICODE
                 , int (*ConvertW)(FILE*, FILE*, CFlag *, char *)
#endif
                  )
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  char *errstr;
  struct stat StatBuf;
  struct utimbuf UTimeBuf;
#ifndef NO_CHMOD
  mode_t mask;
#endif
#ifdef NO_MKSTEMP
  FILE* fd;
#else
  int fd;
#endif
  char *TargetFN = NULL;
  int ResolveSymlinkResult = 0;

  ipFlag->status = 0 ;

  /* Test if output file is a symbolic link */
  if (symbolic_link(ipOutFN) && !ipFlag->Follow)
  {
    ipFlag->status |= OUTPUTFILE_SYMLINK ;
    /* Not a failure, skipping input file according spec. (keep symbolic link unchanged) */
    return -1;
  }

  /* Test if input file is a regular file or symbolic link */
  if (regfile(ipInFN, 1, ipFlag, progname))
  {
    ipFlag->status |= NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }

  /* Test if input file target is a regular file */
  if (symbolic_link(ipInFN) && regfile_target(ipInFN, ipFlag,progname))
  {
    ipFlag->status |= INPUT_TARGET_NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }

  /* Test if output file target is a regular file */
  if (symbolic_link(ipOutFN) && (ipFlag->Follow == SYMLINK_FOLLOW) && regfile_target(ipOutFN, ipFlag,progname))
  {
    ipFlag->status |= OUTPUT_TARGET_NO_REGFILE ;
    /* Failure, input is regular, cannot produce output. */
    if (!ipFlag->error) ipFlag->error = 1;
    return -1;
  }

  /* retrieve ipInFN file date stamp */
  if (stat(ipInFN, &StatBuf))
  {
    if (ipFlag->verbose)
    {
      ipFlag->error = errno;
      errstr = strerror(errno);
      fprintf(stderr, "%s: %s: %s\n", progname, ipInFN, errstr);
    }
    RetVal = -1;
  }

#ifdef NO_MKSTEMP
  if((fd = MakeTempFileFrom(ipOutFN, &TempPath))==NULL) {
#else
  if((fd = MakeTempFileFrom (ipOutFN, &TempPath)) < 0) {
#endif
    if (ipFlag->verbose)
    {
      ipFlag->error = errno;
      errstr = strerror(errno);
      fprintf(stderr, "%s: ", progname);
      fprintf(stderr, _("Failed to open temporary output file: %s\n"), errstr);
    }
    RetVal = -1;
  }

#if DEBUG
  fprintf(stderr, "%s: ", progname);
  fprintf(stderr, _("using %s as temporary file\n"), TempPath);
#endif

  /* can open in file? */
  if (!RetVal)
  {
    InF=OpenInFile(ipInFN);
    if (InF == NULL)
    {
      ipFlag->error = errno;
      errstr = strerror(errno);
      fprintf(stderr, "%s: %s: %s\n", progname, ipInFN, errstr);
      RetVal = -1;
    }
  }

  /* can open output file? */
  if ((!RetVal) && (InF))
  {
#ifdef NO_MKSTEMP
    if ((TempF=fd) == NULL)
    {
#else
    if ((TempF=OpenOutFile(fd)) == NULL)
    {
      ipFlag->error = errno;
      errstr = strerror(errno);
      fprintf(stderr, "%s: %s\n", progname, errstr);
#endif
      fclose (InF);
      InF = NULL;
      RetVal = -1;
    }
  }

  if (check_unicode(InF, TempF, ipFlag, ipInFN, progname))
    RetVal = -1;

  /* conversion sucessful? */
#ifdef D2U_UNICODE
  if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))
  {
    if ((!RetVal) && (ConvertW(InF, TempF, ipFlag, progname)))
      RetVal = -1;
    if (ipFlag->status & UNICODE_CONVERSION_ERROR)
    {
      if (!ipFlag->error) ipFlag->error = 1;
      RetVal = -1;
    }
  } else {
    if ((!RetVal) && (Convert(InF, TempF, ipFlag, progname)))
      RetVal = -1;
  }
#else
  if ((!RetVal) && (Convert(InF, TempF, ipFlag, progname)))
    RetVal = -1;
#endif

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  /* can close output file? */
  if (TempF)
  {
    if (fclose(TempF) == EOF)
    {
       if (ipFlag->verbose)
       {
         ipFlag->error = errno;
         errstr = strerror(errno);
         fprintf(stderr, "%s: ", progname);
         fprintf(stderr, _("Failed to write to temporary output file %s: %s\n"), TempPath, errstr);
       }
      RetVal = -1;
    }
  }

#ifdef NO_MKSTEMP
  if(fd!=NULL)
    fclose(fd);
#else
  if(fd>=0)
    close(fd);
#endif

#ifndef NO_CHMOD
  if (!RetVal)
  {
    if (ipFlag->NewFile == 0) /* old-file mode */
    {
       RetVal = chmod (TempPath, StatBuf.st_mode); /* set original permissions */
    }
    else
    {
       mask = umask(0); /* get process's umask */
       umask(mask); /* set umask back to original */
       RetVal = chmod(TempPath, StatBuf.st_mode & ~mask); /* set original permissions, minus umask */
    }

    if (RetVal)
    {
       if (ipFlag->verbose)
       {
         ipFlag->error = errno;
         errstr = strerror(errno);
         fprintf(stderr, "%s: ", progname);
         fprintf(stderr, _("Failed to change the permissions of temporary output file %s: %s\n"), TempPath, errstr);
       }
    }
  }
#endif

#ifndef NO_CHOWN
  if (!RetVal && (ipFlag->NewFile == 0))  /* old-file mode */
  {
     /* Change owner and group of the temporary output file to the original file's uid and gid. */
     /* Required when a different user (e.g. root) has write permission on the original file. */
     /* Make sure that the original owner can still access the file. */
     if (chown(TempPath, StatBuf.st_uid, StatBuf.st_gid))
     {
        if (ipFlag->verbose)
        {
          ipFlag->error = errno;
          errstr = strerror(errno);
          fprintf(stderr, "%s: ", progname);
          fprintf(stderr, _("Failed to change the owner and group of temporary output file %s: %s\n"), TempPath, errstr);
        }
        RetVal = -1;
     }
  }
#endif

  if ((!RetVal) && (ipFlag->KeepDate))
  {
    UTimeBuf.actime = StatBuf.st_atime;
    UTimeBuf.modtime = StatBuf.st_mtime;
    /* can change output file time to in file time? */
    if (utime(TempPath, &UTimeBuf) == -1)
    {
      if (ipFlag->verbose)
      {
        ipFlag->error = errno;
        errstr = strerror(errno);
        fprintf(stderr, "%s: %s: %s\n", progname, TempPath, errstr);
      }
      RetVal = -1;
    }
  }

  /* any error? cleanup the temp file */
  if (RetVal && (TempPath != NULL))
  {
    if (unlink(TempPath) && (errno != ENOENT))
    {
      if (ipFlag->verbose)
      {
        ipFlag->error = errno;
        errstr = strerror(errno);
        fprintf(stderr, "%s: %s: %s\n", progname, TempPath, errstr);
      }
      RetVal = -1;
    }
  }

  /* If output file is a symbolic link, optional resolve the link and modify  */
  /* the target, instead of removing the link and creating a new regular file */
  TargetFN = ipOutFN;
  if (symbolic_link(ipOutFN) && !RetVal)
  {
    ResolveSymlinkResult = 0; /* indicates that TargetFN need not be freed */
    if (ipFlag->Follow == SYMLINK_FOLLOW)
    {
      ResolveSymlinkResult = ResolveSymbolicLink(ipOutFN, &TargetFN, ipFlag, progname);
      if (ResolveSymlinkResult < 0)
      {
        if (ipFlag->verbose)
        {
          fprintf(stderr, "%s: ", progname);
          fprintf(stderr, _("problems resolving symbolic link '%s'\n"), ipOutFN);
          fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
        }
        RetVal = -1;
      }
    }
  }

  /* can rename temporary file to output file? */
  if (!RetVal)
  {
#ifdef NEED_REMOVE
    if (unlink(TargetFN) && (errno != ENOENT))
    {
      if (ipFlag->verbose)
      {
        ipFlag->error = errno;
        errstr = strerror(errno);
        fprintf(stderr, "%s: %s: %s\n", progname, TargetFN, errstr);
      }
      RetVal = -1;
    }
#endif
    if (rename(TempPath, TargetFN) == -1)
    {
      if (ipFlag->verbose)
      {
        ipFlag->error = errno;
        errstr = strerror(errno);
        fprintf(stderr, "%s: ", progname);
        fprintf(stderr, _("problems renaming '%s' to '%s': %s\n"), TempPath, TargetFN, errstr);
#ifdef S_ISLNK
        if (ResolveSymlinkResult > 0)
          fprintf(stderr, _("          which is the target of symbolic link '%s'\n"), ipOutFN);
#endif
        fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
      }
      RetVal = -1;
    }

    if (ResolveSymlinkResult > 0)
      free(TargetFN);
  }
  free(TempPath);
  return RetVal;
}

/* convert stdin and write to stdout
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertStdio(CFlag *ipFlag, char *progname,
                   int (*Convert)(FILE*, FILE*, CFlag *, char *)
#ifdef D2U_UNICODE
                 , int (*ConvertW)(FILE*, FILE*, CFlag *, char *)
#endif
                  )
{
    ipFlag->NewFile = 1;
    ipFlag->KeepDate = 0;

#if defined(_WIN32) && !defined(__CYGWIN__)

    /* stdin and stdout are by default text streams. We need
     * to set them to binary mode. Otherwise an LF will
     * automatically be converted to CR-LF on DOS/Windows.
     * Erwin */

    /* POSIX 'setmode' was deprecated by MicroSoft since
     * Visual C++ 2005. Use ISO C++ conformant '_setmode' instead. */

    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stdin), _O_BINARY);
#elif defined(__MSDOS__) || defined(__CYGWIN__) || defined(__OS2__)
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);
#endif

    if (check_unicode(stdin, stdout, ipFlag, "stdin", progname))
        return 1;

#ifdef D2U_UNICODE
    if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE)) {
        return ConvertW(stdin, stdout, ipFlag, progname);
    } else {
        return Convert(stdin, stdout, ipFlag, progname);
    }
#else
    return Convert(stdin, stdout, ipFlag, progname);
#endif
}

void print_errors_stdio(const CFlag *pFlag, const char *progname)
{
    if (pFlag->status & WRONG_CODEPAGE) {
      if (pFlag->verbose) {
        fprintf(stderr,"%s: ",progname);
        fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
      }
    } else if (pFlag->status & LOCALE_NOT_UTF8) {
      if (pFlag->verbose) {
        fprintf(stderr,"%s: ",progname);
        fprintf(stderr, _("Skipping UTF-16 file %s, the current locale character encoding is not UTF-8.\n"), "stdin");
      }
    } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
      if (pFlag->verbose) {
        fprintf(stderr,"%s: ",progname);
        fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), "stdin", (int)sizeof(wchar_t));
      }
    } else if (pFlag->status & UNICODE_CONVERSION_ERROR) {
      if (pFlag->verbose) {
        fprintf(stderr,"%s: ",progname);
        fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred.\n"), "stdin");
      }
    }
}

void print_errors_newfile(const CFlag *pFlag, const char *infile, const char *outfile, const char *progname, const int RetVal)
{
  if (pFlag->status & NO_REGFILE) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping %s, not a regular file.\n"), infile);
    }
  } else if (pFlag->status & OUTPUTFILE_SYMLINK) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping %s, output file %s is a symbolic link.\n"), infile, outfile);
    }
  } else if (pFlag->status & INPUT_TARGET_NO_REGFILE) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), infile);
    }
  } else if (pFlag->status & OUTPUT_TARGET_NO_REGFILE) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping %s, target of symbolic link %s is not a regular file.\n"), infile, outfile);
    }
  } else if (pFlag->status & BINARY_FILE) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping binary file %s\n"), infile);
    }
  } else if (pFlag->status & WRONG_CODEPAGE) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
    }
  } else if (pFlag->status & LOCALE_NOT_UTF8) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping UTF-16 file %s, the current locale character encoding is not UTF-8.\n"), infile);
    }
  } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), infile, (int)sizeof(wchar_t));
    }
  } else if (pFlag->status & UNICODE_CONVERSION_ERROR) {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred.\n"), infile);
    }
  } else {
    if (pFlag->verbose) {
      fprintf(stderr,"%s: ",progname);
      if (is_dos2unix(progname))
        fprintf(stderr, _("converting file %s to file %s in Unix format...\n"), infile, outfile);
      else {
        if (pFlag->FromToMode == FROMTO_UNIX2MAC)
          fprintf(stderr, _("converting file %s to file %s in Mac format...\n"), infile, outfile);
        else
          fprintf(stderr, _("converting file %s to file %s in DOS format...\n"), infile, outfile);
      }
      if (RetVal) {
        if (pFlag->verbose) {
          fprintf(stderr,"%s: ",progname);
          fprintf(stderr, _("problems converting file %s to file %s\n"), infile, outfile);
        }
      }
    }
  }
}

#ifdef D2U_UNICODE
wint_t d2u_getwc(FILE *f, int bomtype)
{
   int c_trail, c_lead;
   wint_t wc;

   if (((c_lead=fgetc(f)) == EOF)  || ((c_trail=fgetc(f)) == EOF))
      return(WEOF);

   if (bomtype == FILE_UTF16LE)  /* UTF16 little endian */
   {
      c_trail <<=8;
      wc = (wint_t)(c_trail + c_lead) ;
   } else {                      /* UTF16 big endian */
      c_lead <<=8;
      wc = (wint_t)(c_trail + c_lead) ;
   }
   return(wc);
}

wint_t d2u_ungetwc(wint_t wc, FILE *f, int bomtype)
{
   int c_trail, c_lead;

   if (bomtype == FILE_UTF16LE)  /* UTF16 little endian */
   {
      c_trail = (int)(wc & 0xff00);
      c_trail >>=8;
      c_lead  = (int)(wc & 0xff);
   } else {                      /* UTF16 big endian */
      c_lead = (int)(wc & 0xff00);
      c_lead >>=8;
      c_trail  = (int)(wc & 0xff);
   }

   /* push back in reverse order */
   if ((ungetc(c_trail,f) == EOF)  || (ungetc(c_lead,f) == EOF))
      return(WEOF);
   return(wc);
}

/* Put wide character */
wint_t d2u_putwc(wint_t wc, FILE *f, CFlag *ipFlag)
{
   static char mbs[8];
   static wchar_t lead, trail;
   static wchar_t wstr[3];
   size_t i,len;
   int c_trail, c_lead;

   if (ipFlag->keep_utf16)
   {
     if (ipFlag->bomtype == FILE_UTF16LE)  /* UTF16 little endian */
     {
        c_trail = (int)(wc & 0xff00);
        c_trail >>=8;
        c_lead  = (int)(wc & 0xff);
     } else {                      /* UTF16 big endian */
        c_lead = (int)(wc & 0xff00);
        c_lead >>=8;
        c_trail  = (int)(wc & 0xff);
     }
     if ((fputc(c_lead,f) == EOF)  || (fputc(c_trail,f) == EOF))
       return(WEOF);
     return wc;
   }

   if ((wc >= 0xd800) && (wc < 0xdc00))
   {
      /* fprintf(stderr, "UTF-16 lead %x\n",wc); */
      lead = (wchar_t)wc; /* lead (high) surrogate */
      return(wc);
   }
   if ((wc >= 0xdc00) && (wc < 0xe000))
   {
      /* fprintf(stderr, "UTF-16 trail %x\n",wc); */
      trail = (wchar_t)wc; /* trail (low) surrogate */
#if defined(_WIN32) || defined(__CYGWIN__)
      /* On Windows (including Cygwin) wchar_t is 16 bit */
      /* We cannot decode an UTF-16 surrogate pair, because it will
         not fit in a 16 bit wchar_t. */
      wstr[0] = lead;
      wstr[1] = trail;
      wstr[2] = L'\0';
#else
      /* On Unix wchar_t is 32 bit */
      /* When we don't decode the UTF-16 surrogate pair, wcstombs() does not
       * produce the same UTF-8 as WideCharToMultiByte().  The UTF-8 output
       * produced by wcstombs() is bigger, because it just translates the wide
       * characters in the range 0xD800..0xDBFF individually to UTF-8 sequences
       * (although these code points are reserved for use only as surrogate
       * pairs in UTF-16).
       *
       * Some smart viewers can still display this UTF-8 correctly (like Total
       * Commander lister), however the UTF-8 is not readable by Windows
       * Notepad (on Windows 7).  When we decode the UTF-16 surrogate pairs
       * ourselves the wcstombs() UTF-8 output is identical to what
       * WideCharToMultiByte() produces, and is readable by Notepad.
       *
       * Surrogate halves in UTF-8 are invalid. See also
       * http://en.wikipedia.org/wiki/UTF-8#Invalid_code_points
       * http://tools.ietf.org/html/rfc3629#page-5
       * It is a bug in (some implemenatations of) wcstombs().
       * On Cygwin 1.7 wcstombs() produces correct UTF-8 from UTF-16 surrogate pairs.
       */
      /* Decode UTF-16 surrogate pair */
      wstr[0] = 0x10000;
      wstr[0] += (lead & 0x03FF) << 10;
      wstr[0] += (trail & 0x03FF);
      wstr[1] = L'\0';
      /* fprintf(stderr, "UTF-32  %x\n",wstr[0]); */
#endif
   } else {
      wstr[0] = (wchar_t)wc;
      wstr[1] = L'\0';
   }

#if defined(_WIN32) || defined(__CYGWIN__)
   /* On Windows we convert UTF-16 always to UTF-8 */
   len = (size_t)(WideCharToMultiByte(CP_UTF8, 0, wstr, -1, mbs, sizeof(mbs), NULL, NULL) -1);
#else
   /* On Unix we convert UTF-16 to the locale encoding */
   len = wcstombs(mbs, wstr, sizeof(mbs));
   /* fprintf(stderr, "len  %d\n",len); */
#endif

   if ( len == (size_t)(-1) )
   {  /* Stop when there is a conversion error */
      ipFlag->status |= UNICODE_CONVERSION_ERROR ;
      return(WEOF);
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
