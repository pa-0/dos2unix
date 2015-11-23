/*
 *   Copyright (C) 2009-2015 Erwin Waterlander
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
#include "querycp.h"

#include <stdarg.h>

#if defined(D2U_UNIFILE) || (defined(D2U_UNICODE) && defined(_WIN32))
#include <windows.h>
#endif

#if defined(D2U_UNICODE) && !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
# include <langinfo.h>
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

/* global variable */
#ifdef D2U_UNIFILE
int d2u_display_encoding = D2U_DISPLAY_ANSI ;
#endif

/*
 * Print last system error on Windows.
 *
 */
#if (defined(_WIN32) && !defined(__CYGWIN__))
void d2u_PrintLastError(const char *progname)
{
    /* Retrieve the system error message for the last-error code */

    LPVOID lpMsgBuf;
    DWORD dw;

    dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    /* Display the error message */

    /* MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK); */
    fprintf(stderr, "%s: ",progname);
#ifdef D2U_UNIFILE
    fwprintf(stderr, L"%ls\n",(LPCTSTR)lpMsgBuf);
#else
    fprintf(stderr, "%s\n",(LPCTSTR)lpMsgBuf);
#endif

    LocalFree(lpMsgBuf);
}


int d2u_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
  int i;

  if ( (i = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar)) == 0)
      d2u_PrintLastError("dos2unix");

  return i;
}

int d2u_MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
  int i;

  if ( (i = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar)) == 0)
      d2u_PrintLastError("dos2unix");
  return i;
}

#endif

/*
 * d2u_fprintf()  : printf wrapper, print in Windows Command Prompt in Unicode
 * mode, to have consistent output. Regardless of active code page.
 *
 * On Windows the file system uses always Unicode UTF-16 encoding, regardless
 * of the system default code page. This means that files and directories can
 * have names that can't be encoded in the default system Windows ANSI code
 * page.
 *
 * Dos2unix for Windows with Unicode file name support translates all directory
 * names to UTF-8, to be able to  work with char type strings.  This is also
 * done to keep the code portable.
 *
 * Dos2unix's messages are encoded in the default Windows ANSI code page, which
 * can be translated with gettext. Gettext/libintl recodes messages (format) to
 * the system default ANSI code page.
 *
 * d2u_fprintf() on Windows assumes that:
 * - The format string is encoded in the system default ANSI code page.
 * - The arguments are encoded in UTF-8.
 *
 * There are several methods for printing Unicode in the Windows Console, but
 * none of them is perfect. There are so many issues that I decided to go back
 * to ANSI by default.
 */

void d2u_fprintf( FILE *stream, const char* format, ... ) {
   va_list args;
#ifdef D2U_UNIFILE
   char buf[D2U_MAX_PATH];
   char formatmbs[D2U_MAX_PATH];
   wchar_t formatwcs[D2U_MAX_PATH];
   UINT outputCP;
   wchar_t wstr[D2U_MAX_PATH];
   int prevmode;
/*   HANDLE out_handle;

   if (stream == stderr)
     out_handle =GetStdHandle(STD_ERROR_HANDLE);
   else
     out_handle =GetStdHandle(STD_OUTPUT_HANDLE);
*/
   va_start(args, format);

   /* The format string is encoded in the system default
    * Windows ANSI code page. May have been translated
    * by gettext. Convert it to wide characters. */
   d2u_MultiByteToWideChar(CP_ACP,0, format, -1, formatwcs, D2U_MAX_PATH);
   /* then convert the format string to UTF-8 */
   d2u_WideCharToMultiByte(CP_UTF8, 0, formatwcs, -1, formatmbs, D2U_MAX_PATH, NULL, NULL);

   /* The arguments (file names) are in UTF-8 encoding, because
    * in dos2unix for Windows all file names are in UTF-8 format.
    * Print to buffer (UTF-8) */
   vsnprintf(buf, sizeof(buf), formatmbs, args);

   if (d2u_display_encoding == D2U_DISPLAY_UTF8) {

   /* Using UTF-8 has my preference. The following method works fine when NLS is
      disabled. But when I enable NLS (ENABLE_NLS=1) all non-ASCII characters are
      printed as a square with a question mark in it. This will make the screen
      output of dos2unix for most languages unreadable.
      When I redirect the output to a file, the output is correct UTF-8. I don't
      know why NLS causes wrong printed text in the console. I just turn NLS off.
      A disadvantage of this method is that all non-ASCII characters are printed
      wrongly when the console uses raster font (which is the default).
      I tried on a Chinese Windows 7 (code page 936) and then all non-ASCII
      is printed wrongly, using raster and TrueType font. Only in ConEmu I
      get correct output. I'm afraid that most people use the default Command Prompt
      and PowerShell consolse, so for many people the text will be unreadable.
      On a Chinese Windows there was a lot of flickering during the printing of the
      lines of text. This is not acceptable, but I'm not sure it this was because the
      Windows Command Prompt was broken. It sometimes crashes.
 */
#ifdef ENABLE_NLS
      /* temporarely disable NLS */
      setlocale (LC_ALL, "C");
#endif
       /* print UTF-8 buffer to console in UTF-8 mode */
      outputCP = GetConsoleOutputCP();
      SetConsoleOutputCP(CP_UTF8);
      fwprintf(stream,L"%S",buf);
      SetConsoleOutputCP(outputCP);
#ifdef ENABLE_NLS
      /* re-enable NLS */
      setlocale (LC_ALL, "");
#endif

   /* The following UTF-8 method does not give correct output. I don't know why. */
   //prevmode = _setmode(_fileno(stream), _O_U8TEXT);
   //fwprintf(stream,L"%S",buf);
   //_setmode(_fileno(stream), prevmode);

   } else if (d2u_display_encoding == D2U_DISPLAY_UNICODE) {

   /* Another method for printing Unicode is using WriteConsoleW().
      WriteConsoleW always prints output correct in the console. Even when
      using raster font WriteConsoleW prints correctly when possible.
      WriteConsoleW has one big disadvantage: The output of WriteConsoleW
      can't be redirected. The output can't be piped to a log file. */
       /* Convert UTF-8 buffer to wide characters. */
       //d2u_MultiByteToWideChar(CP_UTF8,0, buf, -1, wstr, D2U_MAX_PATH);
       //WriteConsoleW(out_handle, wstr, wcslen(wstr), NULL, NULL);

   /* Printing UTF-16 works correctly like WriteConsoleW, with and without NLS enabled.
      Works also good with raster fonts. In a Chinese CP936 locale it works correctly
      in the Windows Command Prompt. The downside is that it is UTF-16. When this is
      redirected to a file it gives a big mess. It is not compatible with ASCII. So
      even a simple ASCII grep on the screen output will not work.
      When the output is redirected in a Windows Command Prompt to a file all line breaks end up as
      0d0a 00 (instead of 0d00 0a00), which makes it a corrupt UTF-16 file.
      In PowerShell you get correct line breaks 0d00 0a00 when you redirect to a file, but there are
      null characters (0000) inserted, as if it is UTF-32 with UTF-16 BOM and UTF-16 line breaks.
      See also test/testu16.c.    */
      d2u_MultiByteToWideChar(CP_UTF8,0, buf, -1, wstr, D2U_MAX_PATH);
      prevmode = _setmode(_fileno(stream), _O_U16TEXT);
      fwprintf(stream,L"%ls",wstr);
      _setmode(_fileno(stream), prevmode);
   } else {  /* ANSI */
      d2u_MultiByteToWideChar(CP_UTF8,0, buf, -1, wstr, D2U_MAX_PATH);
      /* Convert the whole message to ANSI, some Unicode characters may fail to translate to ANSI.
         They will be displayed as a question mark. */
      d2u_WideCharToMultiByte(CP_ACP, 0, wstr, -1, buf, D2U_MAX_PATH, NULL, NULL);
      fprintf(stream,"%s",buf);
   }

#else
   va_start(args, format);
   vfprintf(stream, format, args);
#endif
   va_end( args );
}

/* d2u_ansi_fprintf()
   fprintf wrapper for Windows console.

   Format and arguments are in ANSI format.
   Redirect the printing to d2u_fprintf such that the output
   format is consistent. To prevent a mix of ANSI/UTF-8/UTF-16
   encodings in the print output.
 */

void d2u_ansi_fprintf( FILE *stream, const char* format, ... ) {
   va_list args;
#ifdef D2U_UNIFILE
   char buf[D2U_MAX_PATH];        /* ANSI encoded string */
   char bufmbs[D2U_MAX_PATH];     /* UTF-8 encoded string */
   wchar_t bufwcs[D2U_MAX_PATH];  /* Wide encoded string */

   va_start(args, format);

   vsnprintf(buf, sizeof(buf), format, args);
   /* The format string and arguments are encoded in the system default
    * Windows ANSI code page. May have been translated
    * by gettext. Convert it to wide characters. */
   d2u_MultiByteToWideChar(CP_ACP,0, buf, -1, bufwcs, D2U_MAX_PATH);
   /* then convert the format string to UTF-8 */
   d2u_WideCharToMultiByte(CP_UTF8, 0, bufwcs, -1, bufmbs, D2U_MAX_PATH, NULL, NULL);

   d2u_fprintf(stream, "%s",bufmbs);

#else
   va_start(args, format);
   vfprintf(stream, format, args);
#endif
   va_end( args );
}

/*   d2u_rename
 *   wrapper for rename().
 *   On Windows file names are encoded in UTF-8.
 */
int d2u_rename(const char *oldname, const char *newname)
{
#ifdef D2U_UNIFILE
   wchar_t oldnamew[D2U_MAX_PATH];
   wchar_t newnamew[D2U_MAX_PATH];
   d2u_MultiByteToWideChar(CP_UTF8, 0, oldname, -1, oldnamew, D2U_MAX_PATH);
   d2u_MultiByteToWideChar(CP_UTF8, 0, newname, -1, newnamew, D2U_MAX_PATH);
   return _wrename(oldnamew, newnamew);
#else
   return rename(oldname, newname);
#endif
}

/*   d2u_unlink
 *   wrapper for unlink().
 *   On Windows file names are encoded in UTF-8.
 */
int d2u_unlink(const char *filename)
{
#ifdef D2U_UNIFILE
   wchar_t filenamew[D2U_MAX_PATH];
   d2u_MultiByteToWideChar(CP_UTF8, 0, filename, -1, filenamew, D2U_MAX_PATH);
   return _wunlink(filenamew);
#else
   return unlink(filename);
#endif
}

/******************************************************************
 *
 * int symbolic_link(char *path)
 *
 * test if *path points to a file that exists and is a symbolic link
 *
 * returns 1 on success, 0 when it fails.
 *
 ******************************************************************/

#ifdef D2U_UNIFILE

int symbolic_link(const char *path)
{
   DWORD attrs;
   wchar_t pathw[D2U_MAX_PATH];

   d2u_MultiByteToWideChar(CP_UTF8, 0, path, -1, pathw, D2U_MAX_PATH);
   attrs = GetFileAttributesW(pathw);

   if (attrs == INVALID_FILE_ATTRIBUTES)
      return(0);

   return ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
}

#elif(defined(_WIN32) && !defined(__CYGWIN__))

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

   if (STAT(path, &buf) == 0) {
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
int regfile(char *path, int allowSymlinks, CFlag *ipFlag, const char *progname)
{
#ifdef D2U_UNIFILE
   struct _stat buf;
   wchar_t pathw[D2U_MAX_PATH];
#else
   struct stat buf;
#endif
   char *errstr;

#ifdef D2U_UNIFILE
   d2u_MultiByteToWideChar(CP_UTF8, 0, path, -1, pathw, D2U_MAX_PATH);
   if (_wstat(pathw, &buf) == 0) {
#else
   if (STAT(path, &buf) == 0) {
#endif
#if DEBUG
      d2u_fprintf(stderr, "%s: %s", progname, path);
      d2u_fprintf(stderr, " MODE 0%o ", buf.st_mode);
#ifdef S_ISSOCK
      if (S_ISSOCK(buf.st_mode))
         d2u_fprintf(stderr, " (socket)");
#endif
#ifdef S_ISLNK
      if (S_ISLNK(buf.st_mode))
         d2u_fprintf(stderr, " (symbolic link)");
#endif
      if (S_ISREG(buf.st_mode))
         d2u_fprintf(stderr, " (regular file)");
#ifdef S_ISBLK
      if (S_ISBLK(buf.st_mode))
         d2u_fprintf(stderr, " (block device)");
#endif
      if (S_ISDIR(buf.st_mode))
         d2u_fprintf(stderr, " (directory)");
      if (S_ISCHR(buf.st_mode))
         d2u_fprintf(stderr, " (character device)");
      if (S_ISFIFO(buf.st_mode))
         d2u_fprintf(stderr, " (FIFO)");
      d2u_fprintf(stderr, "\n");
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
   else {
     if (ipFlag->verbose) {
       ipFlag->error = errno;
       errstr = strerror(errno);
       d2u_fprintf(stderr, "%s: %s:", progname, path);
       d2u_ansi_fprintf(stderr, " %s\n", errstr);
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
int regfile_target(char *path, CFlag *ipFlag, const char *progname)
{
#ifdef D2U_UNIFILE
   struct _stat buf;
   wchar_t pathw[D2U_MAX_PATH];
#else
   struct stat buf;
#endif
   char *errstr;

#ifdef D2U_UNIFILE
   d2u_MultiByteToWideChar(CP_UTF8, 0, path, -1, pathw, D2U_MAX_PATH);
   if (_wstat(pathw, &buf) == 0) {
#else
   if (stat(path, &buf) == 0) {
#endif
      if (S_ISREG(buf.st_mode))
         return(0);
      else
         return(-1);
   }
   else {
     if (ipFlag->verbose) {
       ipFlag->error = errno;
       errstr = strerror(errno);
       d2u_fprintf(stderr, "%s: %s:", progname, path);
       d2u_ansi_fprintf(stderr, " %s\n", errstr);
     }
     return(-1);
   }
}

/*
 *   glob_warg() expands the wide command line arguments.
 *   Input  : wide Unicode arguments.
 *   Output : argv : expanded arguments in UTF-8 format.
 *   Returns: new argc value.
 *            -1 when an error occurred.
 *
 */

#ifdef D2U_UNIFILE
int glob_warg(int argc, wchar_t *wargv[], char ***argv, CFlag *ipFlag, const char *progname)
{
  int i;
  int argc_glob = 0;
  wchar_t *warg;
  wchar_t *path;
  wchar_t *path_and_filename;
  wchar_t *ptr;
  char  *arg;
  char  **argv_new;
  char *errstr;
  size_t len;
  int found, add_path;
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  argv_new = (char **)malloc(sizeof(char**));
  if (argv_new == NULL) goto glob_failed;

  len = (size_t)d2u_WideCharToMultiByte(CP_UTF8, 0, wargv[0], -1, NULL, 0, NULL, NULL);
  arg = (char *)malloc(len);
  if (argv == NULL) goto glob_failed;
  d2u_WideCharToMultiByte(CP_UTF8, 0, wargv[argc_glob], -1, arg, (int)len, NULL, NULL);
  argv_new[argc_glob] = arg;

  for (i=1; i<argc; ++i)
  {
    warg = wargv[i];
    found = 0;
    add_path = 0;
    /* FindFileData.cFileName has the path stripped off. We need to add it again. */
    path = _wcsdup(warg);
    /* replace all back slashes with slashes */
    while ( (ptr = wcschr(path,L'\\')) != NULL) {
      *ptr = L'/';
    }
    if ( (ptr = wcsrchr(path,L'/')) != NULL) {
      ptr++;
      *ptr = L'\0';
      add_path = 1;
    }

    hFind = FindFirstFileW(warg, &FindFileData);
    while (hFind != INVALID_HANDLE_VALUE)
    {
      len = wcslen(path) + wcslen(FindFileData.cFileName) + 2;
      path_and_filename = (wchar_t *)malloc(len*sizeof(wchar_t));
      if (path_and_filename == NULL) goto glob_failed;
      if (add_path) {
        wcsncpy(path_and_filename, path, wcslen(path)+1);
        wcsncat(path_and_filename, FindFileData.cFileName, wcslen(FindFileData.cFileName)+1);
      } else {
        wcsncpy(path_and_filename, FindFileData.cFileName, wcslen(FindFileData.cFileName)+1);
      }

      found = 1;
      ++argc_glob;
      len =(size_t) d2u_WideCharToMultiByte(CP_UTF8, 0, path_and_filename, -1, NULL, 0, NULL, NULL);
      arg = (char *)malloc((size_t)len);
      if (argv == NULL) goto glob_failed;
      d2u_WideCharToMultiByte(CP_UTF8, 0, path_and_filename, -1, arg, (int)len, NULL, NULL);
      free(path_and_filename);
      argv_new = (char **)realloc(argv_new, (size_t)(argc_glob+1)*sizeof(char**));
      if (argv_new == NULL) goto glob_failed;
      argv_new[argc_glob] = arg;

      if (!FindNextFileW(hFind, &FindFileData)) {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
      }
    }
    free(path);
    if (found == 0) {
    /* Not a file. Just copy the argument */
      ++argc_glob;
      len =(size_t) d2u_WideCharToMultiByte(CP_UTF8, 0, warg, -1, NULL, 0, NULL, NULL);
      arg = (char *)malloc((size_t)len);
      if (argv == NULL) goto glob_failed;
      d2u_WideCharToMultiByte(CP_UTF8, 0, warg, -1, arg, (int)len, NULL, NULL);
      argv_new = (char **)realloc(argv_new, (size_t)(argc_glob+1)*sizeof(char**));
      if (argv_new == NULL) goto glob_failed;
      argv_new[argc_glob] = arg;
    }
  }
  *argv = argv_new;
  return ++argc_glob;

  glob_failed:
  ipFlag->error = errno;
  errstr = strerror(errno);
  d2u_fprintf(stderr, "%s:", progname);
  d2u_ansi_fprintf(stderr, " %s\n", errstr);
  return -1;
}
#endif

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
#ifdef D2U_UNIFILE
  printf(_(" -D, --display-enc     set encoding of displayed text messages\n\
   encoding            ansi, unicode, utf8, default to ansi\n"));
#endif
  printf(_(" -f, --force           force conversion of binary files\n"));
#ifdef D2U_UNICODE
#if (defined(_WIN32) && !defined(__CYGWIN__))
  printf(_(" -gb, --gb18030        convert UTF-16 to GB18030\n"));
#endif
#endif
  printf(_(" -h, --help            display this help text\n"));
  printf(_(" -i, --info[=FLAGS]    display file information\n\
   file ...            files to analyze\n"));
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

void PrintVersion(const char *progname, const char *localedir)
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
#elif defined(__OS)
  printf(_("%s version.\n"), __OS);
#endif
#if defined(_WIN32) && defined(WINVER)
  printf("WINVER 0x%X\n",WINVER);
#endif
#ifdef D2U_UNICODE
  printf("%s", _("With Unicode UTF-16 support.\n"));
#else
  printf("%s", _("Without Unicode UTF-16 support.\n"));
#endif
#ifdef _WIN32
#ifdef D2U_UNIFILE
  printf("%s", _("With Unicode file name support.\n"));
#else
  printf("%s", _("Without Unicode file name support.\n"));
#endif
#endif
#ifdef ENABLE_NLS
  printf("%s", _("With native language support.\n"));
#else
  printf("%s", "Without native language support.\n");
#endif
#ifdef ENABLE_NLS
  printf("LOCALEDIR: %s\n", localedir);
#endif
  printf("http://waterlan.home.xs4all.nl/dos2unix.html\n");
}


/* opens file of name ipFN in read only mode
 * RetVal: NULL if failure
 *         file stream otherwise
 */
FILE* OpenInFile(char *ipFN)
{
#ifdef D2U_UNIFILE
  wchar_t pathw[D2U_MAX_PATH];

  d2u_MultiByteToWideChar(CP_UTF8, 0, ipFN, -1, pathw, D2U_MAX_PATH);
  return _wfopen(pathw, R_CNTRLW);
#else
  return (fopen(ipFN, R_CNTRL));
#endif
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
  else {
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
#ifdef D2U_UNIFILE
  wchar_t fname_strw[D2U_MAX_PATH];
  wchar_t *namew;
#endif

  *fname_ret = NULL;

  if (!cpy)
    goto make_failed;

  dir = dirname(cpy);

#ifdef D2U_UNIFILE
  fname_len = D2U_MAX_PATH;
#else
  fname_len = strlen(dir) + strlen("/d2utmpXXXXXX") + sizeof (char);
#endif
  if (!(fname_str = (char *)malloc(fname_len)))
    goto make_failed;
  sprintf(fname_str, "%s%s", dir, "/d2utmpXXXXXX");
  *fname_ret = fname_str;

  free(cpy);

#ifdef NO_MKSTEMP
#ifdef D2U_UNIFILE
  d2u_MultiByteToWideChar(CP_UTF8, 0, fname_str, -1, fname_strw, D2U_MAX_PATH);
  namew = _wmktemp(fname_strw);
  d2u_WideCharToMultiByte(CP_UTF8, 0, namew, -1, fname_str, (int)fname_len, NULL, NULL);
  *fname_ret = fname_str;
  if ((fd = _wfopen(fname_strw, W_CNTRLW)) == NULL)
    goto make_failed;
#else
  name = mktemp(fname_str);
  *fname_ret = name;
  if ((fd = fopen(fname_str, W_CNTRL)) == NULL)
    goto make_failed;
#endif
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
int ResolveSymbolicLink(char *lFN, char **rFN, CFlag *ipFlag, const char *progname)
{
  int RetVal = 0;
#ifdef S_ISLNK
  struct stat StatBuf;
  char *errstr;
  char *targetFN = NULL;

  if (STAT(lFN, &StatBuf)) {
    if (ipFlag->verbose) {
      ipFlag->error = errno;
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: %s:", progname, lFN);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
    }
    RetVal = -1;
  }
  else if (S_ISLNK(StatBuf.st_mode)) {
#if USE_CANONICALIZE_FILE_NAME
    targetFN = canonicalize_file_name(lFN);
    if (!targetFN) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: %s:", progname, lFN);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
      }
      RetVal = -1;
    }
    else {
      *rFN = targetFN;
      RetVal = 1;
    }
#else
    /* Sigh. Use realpath, but realize that it has a fatal
     * flaw: PATH_MAX isn't necessarily the maximum path
     * length -- so realpath() might fail. */
    targetFN = (char *) malloc(PATH_MAX * sizeof(char));
    if (!targetFN) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: %s:", progname, lFN);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
      }
      RetVal = -1;
    }
    else {
      /* is there any platform with S_ISLNK that does not have realpath? */
      char *rVal = realpath(lFN, targetFN);
      if (!rVal) {
        if (ipFlag->verbose) {
          ipFlag->error = errno;
          errstr = strerror(errno);
          d2u_fprintf(stderr, "%s: %s:", progname, lFN);
          d2u_ansi_fprintf(stderr, " %s\n", errstr);
        }
        free(targetFN);
        RetVal = -1;
      }
      else {
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

/* Read the Byte Order Mark.
   Returns file pointer or NULL in case of a read error */

FILE *read_bom (FILE *f, int *bomtype)
{
  int bom[4];
  /* BOMs
   * UTF16-LE  ff fe
   * UTF16-BE  fe ff
   * UTF-8     ef bb bf
   * GB18030   84 31 95 33
   */

  *bomtype = FILE_MBS;

   /* Check for BOM */
   if  (f != NULL) {
      if ((bom[0] = fgetc(f)) == EOF) {
         if (ferror(f)) {
           return NULL;
         }
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] != 0xff) && (bom[0] != 0xfe) && (bom[0] != 0xef) && (bom[0] != 0x84)) {
         if (ungetc(bom[0], f) == EOF) return NULL;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[1] = fgetc(f)) == EOF) {
         if (ferror(f)) {
           return NULL;
         }
         if (ungetc(bom[1], f) == EOF) return NULL;
         if (ungetc(bom[0], f) == EOF) return NULL;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xff) && (bom[1] == 0xfe)) { /* UTF16-LE */
         *bomtype = FILE_UTF16LE;
         return(f);
      }
      if ((bom[0] == 0xfe) && (bom[1] == 0xff)) { /* UTF16-BE */
         *bomtype = FILE_UTF16BE;
         return(f);
      }
      if ((bom[2] = fgetc(f)) == EOF) {
         if (ferror(f)) {
           return NULL;
         }
         if (ungetc(bom[2], f) == EOF) return NULL;
         if (ungetc(bom[1], f) == EOF) return NULL;
         if (ungetc(bom[0], f) == EOF) return NULL;
         *bomtype = FILE_MBS;
         return(f);
      }
      if ((bom[0] == 0xef) && (bom[1] == 0xbb) && (bom[2]== 0xbf)) { /* UTF-8 */
         *bomtype = FILE_UTF8;
         return(f);
      }
      if ((bom[0] == 0x84) && (bom[1] == 0x31) && (bom[2]== 0x95)) {
         bom[3] = fgetc(f);
           if (ferror(f)) {
             return NULL;
          }
         if (bom[3]== 0x33) { /* GB18030 */
           *bomtype = FILE_GB18030;
           return(f);
         }
         if (ungetc(bom[3], f) == EOF) return NULL;
      }
      if (ungetc(bom[2], f) == EOF) return NULL;
      if (ungetc(bom[1], f) == EOF) return NULL;
      if (ungetc(bom[0], f) == EOF) return NULL;
      *bomtype = FILE_MBS;
      return(f);
   }
  return(f);
}

FILE *write_bom (FILE *f, CFlag *ipFlag, const char *progname)
{
  int bomtype = ipFlag->bomtype;

  if ((bomtype == FILE_MBS)&&(ipFlag->locale_target == TARGET_GB18030))
    bomtype = FILE_GB18030;

  if (ipFlag->keep_utf16)
  {
    switch (bomtype) {
      case FILE_UTF16LE:   /* UTF-16 Little Endian */
        if (fprintf(f, "%s", "\xFF\xFE") < 0) return NULL;
        if (ipFlag->verbose > 1) {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("UTF-16LE"));
        }
        break;
      case FILE_UTF16BE:   /* UTF-16 Big Endian */
        if (fprintf(f, "%s", "\xFE\xFF") < 0) return NULL;
        if (ipFlag->verbose > 1) {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("UTF-16BE"));
        }
        break;
      case FILE_GB18030:  /* GB18030 */
        if (fprintf(f, "%s", "\x84\x31\x95\x33") < 0) return NULL;
        if (ipFlag->verbose > 1) {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("GB18030"));
        }
        break;
      default:      /* UTF-8 */
        if (fprintf(f, "%s", "\xEF\xBB\xBF") < 0) return NULL;
        if (ipFlag->verbose > 1) {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("UTF-8"));
        }
      ;
    }
  } else {
    if ((bomtype == FILE_GB18030) ||
        (((bomtype == FILE_UTF16LE)||(bomtype == FILE_UTF16BE))&&(ipFlag->locale_target == TARGET_GB18030))
       ) {
        if (fprintf(f, "%s", "\x84\x31\x95\x33") < 0) return NULL; /* GB18030 */
        if (ipFlag->verbose > 1)
        {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("GB18030"));
        }
     } else {
        if (fprintf(f, "%s", "\xEF\xBB\xBF") < 0) return NULL; /* UTF-8 */
        if (ipFlag->verbose > 1)
        {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_ansi_fprintf(stderr, _("Writing %s BOM.\n"), _("UTF-8"));
        }
     }
  }
  return(f);
}

void print_bom (const int bomtype, const char *filename, const char *progname)
{
  char informat[64];
# ifdef D2U_UNIFILE
  wchar_t informatw[64];
#endif

    switch (bomtype) {
    case FILE_UTF16LE:   /* UTF-16 Little Endian */
      strncpy(informat,_("UTF-16LE"),sizeof(informat));
      break;
    case FILE_UTF16BE:   /* UTF-16 Big Endian */
      strncpy(informat,_("UTF-16BE"),sizeof(informat));
      break;
    case FILE_UTF8:      /* UTF-8 */
      strncpy(informat,_("UTF-8"),sizeof(informat));
      break;
    case FILE_GB18030:      /* GB18030 */
      strncpy(informat,_("GB18030"),sizeof(informat));
      break;
    default:
    ;
  }

  if (bomtype > 0) {
    informat[sizeof(informat)-1] = '\0';

/* Change informat to UTF-8 for d2u_fprintf. */
# ifdef D2U_UNIFILE
    /* The format string is encoded in the system default
     * Windows ANSI code page. May have been translated
     * by gettext. Convert it to wide characters. */
    d2u_MultiByteToWideChar(CP_ACP,0, informat, -1, informatw, sizeof(informat));
    /* then convert the format string to UTF-8 */
    d2u_WideCharToMultiByte(CP_UTF8, 0, informatw, -1, informat, sizeof(informat), NULL, NULL);
#endif

    d2u_fprintf(stderr, "%s: ", progname);
    d2u_fprintf(stderr, _("Input file %s has %s BOM.\n"), filename, informat);
  }

}

void print_bom_info (const int bomtype)
{
/* The BOM info must not be translated to other languages, otherwise scripts
   that process the output may not work in other than English locales. */
    switch (bomtype) {
    case FILE_UTF16LE:   /* UTF-16 Little Endian */
      d2u_fprintf(stdout, "  UTF-16LE");
      break;
    case FILE_UTF16BE:   /* UTF-16 Big Endian */
      d2u_fprintf(stdout, "  UTF-16BE");
      break;
    case FILE_UTF8:      /* UTF-8 */
      d2u_fprintf(stdout, "  UTF-8   ");
      break;
    case FILE_GB18030:   /* GB18030 */
      d2u_fprintf(stdout, "  GB18030 ");
      break;
    default:
      d2u_fprintf(stdout, "  no_bom  ");
    ;
  }
}

int check_unicode_info(FILE *InF, CFlag *ipFlag, const char *progname, int *bomtype_orig)
{
  int RetVal = 0;

#ifdef D2U_UNICODE
  if (ipFlag->verbose > 1) {
    if (ipFlag->ConvMode == CONVMODE_UTF16LE) {
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("Assuming UTF-16LE encoding.\n") );
    }
    if (ipFlag->ConvMode == CONVMODE_UTF16BE) {
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("Assuming UTF-16BE encoding.\n") );
    }
  }
#endif
  if ((InF = read_bom(InF, &ipFlag->bomtype)) == NULL) {
    d2u_getc_error(ipFlag,progname);
    return -1;
  }
  *bomtype_orig = ipFlag->bomtype;
#ifdef D2U_UNICODE
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16LE))
    ipFlag->bomtype = FILE_UTF16LE;
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16BE))
    ipFlag->bomtype = FILE_UTF16BE;


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

  return RetVal;
}

int check_unicode(FILE *InF, FILE *TempF,  CFlag *ipFlag, const char *ipInFN, const char *progname)
{
  int RetVal = 0;

#ifdef D2U_UNICODE
  if (ipFlag->verbose > 1) {
    if (ipFlag->ConvMode == CONVMODE_UTF16LE) {
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("Assuming UTF-16LE encoding.\n") );
    }
    if (ipFlag->ConvMode == CONVMODE_UTF16BE) {
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("Assuming UTF-16BE encoding.\n") );
    }
  }
#endif
  if ((InF = read_bom(InF, &ipFlag->bomtype)) == NULL) {
    d2u_getc_error(ipFlag,progname);
    return -1;
  }
  if (ipFlag->verbose > 1)
    print_bom(ipFlag->bomtype, ipInFN, progname);
#ifndef D2U_UNICODE
  /* It is possible that an UTF-16 has no 8-bit binary symbols. We must stop
   * processing an UTF-16 file when UTF-16 is not supported. Don't trust on
   * finding a binary symbol.
   */
  if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE)) {
    ipFlag->status |= UNICODE_NOT_SUPPORTED ;
    return -1;
  }
#endif
#ifdef D2U_UNICODE
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16LE))
    ipFlag->bomtype = FILE_UTF16LE;
  if ((ipFlag->bomtype == FILE_MBS) && (ipFlag->ConvMode == CONVMODE_UTF16BE))
    ipFlag->bomtype = FILE_UTF16BE;


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

#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
  if (strcmp(nl_langinfo(CODESET), "GB18030") == 0)
    ipFlag->locale_target = TARGET_GB18030;
#endif
#endif

  if ((!RetVal) && ((ipFlag->add_bom) || ((ipFlag->keep_bom) && (ipFlag->bomtype > 0))))
    if (write_bom(TempF, ipFlag, progname) == NULL) return -1;

  return RetVal;
}

/* convert file ipInFN and write to file ipOutFN
 * RetVal: 0 if success
 *         -1 otherwise
 */
int ConvertNewFile(char *ipInFN, char *ipOutFN, CFlag *ipFlag, const char *progname,
                   int (*Convert)(FILE*, FILE*, CFlag *, const char *)
#ifdef D2U_UNICODE
                 , int (*ConvertW)(FILE*, FILE*, CFlag *, const char *)
#endif
                  )
{
  int RetVal = 0;
  FILE *InF = NULL;
  FILE *TempF = NULL;
  char *TempPath;
  char *errstr;
#ifdef D2U_UNIFILE
   struct _stat StatBuf;
   wchar_t pathw[D2U_MAX_PATH];
#else
  struct stat StatBuf;
#endif
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
  if (symbolic_link(ipOutFN) && !ipFlag->Follow) {
    ipFlag->status |= OUTPUTFILE_SYMLINK ;
    /* Not a failure, skipping input file according spec. (keep symbolic link unchanged) */
    return -1;
  }

  /* Test if input file is a regular file or symbolic link */
  if (regfile(ipInFN, 1, ipFlag, progname)) {
    ipFlag->status |= NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }

  /* Test if input file target is a regular file */
  if (symbolic_link(ipInFN) && regfile_target(ipInFN, ipFlag,progname)) {
    ipFlag->status |= INPUT_TARGET_NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }

  /* Test if output file target is a regular file */
  if (symbolic_link(ipOutFN) && (ipFlag->Follow == SYMLINK_FOLLOW) && regfile_target(ipOutFN, ipFlag,progname)) {
    ipFlag->status |= OUTPUT_TARGET_NO_REGFILE ;
    /* Failure, input is regular, cannot produce output. */
    if (!ipFlag->error) ipFlag->error = 1;
    return -1;
  }

  /* retrieve ipInFN file date stamp */
#ifdef D2U_UNIFILE
   d2u_MultiByteToWideChar(CP_UTF8, 0, ipInFN, -1, pathw, D2U_MAX_PATH);
   if (_wstat(pathw, &StatBuf)) {
#else
  if (stat(ipInFN, &StatBuf)) {
#endif
    if (ipFlag->verbose) {
      ipFlag->error = errno;
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: %s:", progname, ipInFN);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
    }
    RetVal = -1;
  }

#ifdef NO_MKSTEMP
  if((fd = MakeTempFileFrom(ipOutFN, &TempPath))==NULL) {
#else
  if((fd = MakeTempFileFrom (ipOutFN, &TempPath)) < 0) {
#endif
    if (ipFlag->verbose) {
      ipFlag->error = errno;
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_ansi_fprintf(stderr, _("Failed to open temporary output file: %s\n"), errstr);
    }
    RetVal = -1;
  }

#if DEBUG
  d2u_fprintf(stderr, "%s: ", progname);
  d2u_fprintf(stderr, _("using %s as temporary file\n"), TempPath);
#endif

  /* can open in file? */
  if (!RetVal) {
    InF=OpenInFile(ipInFN);
    if (InF == NULL) {
      ipFlag->error = errno;
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: %s:", progname, ipInFN);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
      RetVal = -1;
    }
  }

  /* can open output file? */
  if ((!RetVal) && (InF)) {
#ifdef NO_MKSTEMP
    if ((TempF=fd) == NULL) {
#else
    if ((TempF=OpenOutFile(fd)) == NULL) {
      ipFlag->error = errno;
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s:", progname);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
#endif
      fclose (InF);
      InF = NULL;
      RetVal = -1;
    }
  }

  if (!RetVal)
    if (check_unicode(InF, TempF, ipFlag, ipInFN, progname))
      RetVal = -1;

  /* conversion sucessful? */
#ifdef D2U_UNICODE
  if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE)) {
    if ((!RetVal) && (ConvertW(InF, TempF, ipFlag, progname)))
      RetVal = -1;
    if (ipFlag->status & UNICODE_CONVERSION_ERROR) {
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
  if (TempF) {
    if (fclose(TempF) == EOF) {
       if (ipFlag->verbose) {
         ipFlag->error = errno;
         errstr = strerror(errno);
         d2u_fprintf(stderr, "%s: ", progname);
         d2u_fprintf(stderr, _("Failed to write to temporary output file %s:"), TempPath);
         d2u_ansi_fprintf(stderr, " %s\n", errstr);
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
    if (ipFlag->NewFile == 0) { /* old-file mode */
       RetVal = chmod (TempPath, StatBuf.st_mode); /* set original permissions */
    } else {
       mask = umask(0); /* get process's umask */
       umask(mask); /* set umask back to original */
       RetVal = chmod(TempPath, StatBuf.st_mode & ~mask); /* set original permissions, minus umask */
    }

    if (RetVal) {
       if (ipFlag->verbose) {
         ipFlag->error = errno;
         errstr = strerror(errno);
         d2u_fprintf(stderr, "%s: ", progname);
         d2u_fprintf(stderr, _("Failed to change the permissions of temporary output file %s:"), TempPath);
         d2u_ansi_fprintf(stderr, " %s\n", errstr);
       }
    }
  }
#endif

#ifndef NO_CHOWN
  if (!RetVal && (ipFlag->NewFile == 0)) { /* old-file mode */
     /* Change owner and group of the temporary output file to the original file's uid and gid. */
     /* Required when a different user (e.g. root) has write permission on the original file. */
     /* Make sure that the original owner can still access the file. */
     if (chown(TempPath, StatBuf.st_uid, StatBuf.st_gid)) {
        if (ipFlag->verbose) {
          ipFlag->error = errno;
          errstr = strerror(errno);
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_fprintf(stderr, _("Failed to change the owner and group of temporary output file %s:"), TempPath);
          d2u_ansi_fprintf(stderr, " %s\n", errstr);
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
    if (utime(TempPath, &UTimeBuf) == -1) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: %s:", progname, TempPath);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
      }
      RetVal = -1;
    }
  }

  /* any error? cleanup the temp file */
  if (RetVal && (TempPath != NULL)) {
    if (d2u_unlink(TempPath) && (errno != ENOENT)) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: %s:", progname, TempPath);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
      }
      RetVal = -1;
    }
  }

  /* If output file is a symbolic link, optional resolve the link and modify  */
  /* the target, instead of removing the link and creating a new regular file */
  TargetFN = ipOutFN;
  if (symbolic_link(ipOutFN) && !RetVal) {
    ResolveSymlinkResult = 0; /* indicates that TargetFN need not be freed */
    if (ipFlag->Follow == SYMLINK_FOLLOW) {
      ResolveSymlinkResult = ResolveSymbolicLink(ipOutFN, &TargetFN, ipFlag, progname);
      if (ResolveSymlinkResult < 0) {
        if (ipFlag->verbose) {
          d2u_fprintf(stderr, "%s: ", progname);
          d2u_fprintf(stderr, _("problems resolving symbolic link '%s'\n"), ipOutFN);
          d2u_fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
        }
        RetVal = -1;
      }
    }
  }

  /* can rename temporary file to output file? */
  if (!RetVal) {
#ifdef NEED_REMOVE
    if (d2u_unlink(TargetFN) && (errno != ENOENT)) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: %s:", progname, TargetFN);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
      }
      RetVal = -1;
    }
#endif

    if (d2u_rename(TempPath, TargetFN) != 0) {
      if (ipFlag->verbose) {
        ipFlag->error = errno;
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: ", progname);
        d2u_fprintf(stderr, _("problems renaming '%s' to '%s':"), TempPath, TargetFN);
        d2u_ansi_fprintf(stderr, " %s\n", errstr);
#ifdef S_ISLNK
        if (ResolveSymlinkResult > 0)
          d2u_fprintf(stderr, _("          which is the target of symbolic link '%s'\n"), ipOutFN);
#endif
        d2u_fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
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
int ConvertStdio(CFlag *ipFlag, const char *progname,
                   int (*Convert)(FILE*, FILE*, CFlag *, const char *)
#ifdef D2U_UNICODE
                 , int (*ConvertW)(FILE*, FILE*, CFlag *, const char *)
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
        return -1;

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

void print_messages_stdio(const CFlag *pFlag, const char *progname)
{
    if (pFlag->status & BINARY_FILE) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping binary file %s\n"), "stdin");
    } else if (pFlag->status & WRONG_CODEPAGE) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
#ifdef D2U_UNICODE
    } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), "stdin", (int)sizeof(wchar_t));
    } else if (pFlag->status & UNICODE_CONVERSION_ERROR) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred on line %u.\n"), "stdin", pFlag->line_nr);
#else
    } else if (pFlag->status & UNICODE_NOT_SUPPORTED) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping UTF-16 file %s, UTF-16 conversion is not supported in this version of %s.\n"), "stdin", progname);
#endif
    }
}

void print_format(const CFlag *pFlag, char *informat, char *outformat, size_t lin, size_t lout)
{
  informat[0]='\0';
  outformat[0]='\0';

  if (pFlag->bomtype == FILE_UTF16LE)
    strncpy(informat,_("UTF-16LE"),lin);
  if (pFlag->bomtype == FILE_UTF16BE)
    strncpy(informat,_("UTF-16BE"),lin);
  informat[lin-1]='\0';

#ifdef D2U_UNICODE
  if ((pFlag->bomtype == FILE_UTF16LE)||(pFlag->bomtype == FILE_UTF16BE)) {
#if !defined(__MSDOS__) && !defined(_WIN32) && !defined(__OS2__)  /* Unix, Cygwin */
    strncpy(outformat,nl_langinfo(CODESET),lout);
#endif

#if defined(_WIN32) && !defined(__CYGWIN__) /* Windows, not Cygwin */
    if (pFlag->locale_target == TARGET_GB18030)
      strncpy(outformat, _("GB18030"),lout);
    else
      strncpy(outformat, _("UTF-8"),lout);
#endif

    if (pFlag->keep_utf16)
    {
      if (pFlag->bomtype == FILE_UTF16LE)
        strncpy(outformat,_("UTF-16LE"),lout);
      if (pFlag->bomtype == FILE_UTF16BE)
        strncpy(outformat,_("UTF-16BE"),lout);
    }
    outformat[lout-1]='\0';
  }
#endif
}

void print_messages_newfile(const CFlag *pFlag, const char *infile, const char *outfile, const char *progname, const int RetVal)
{
  char informat[32];
  char outformat[64];
# ifdef D2U_UNIFILE
  wchar_t informatw[32];
  wchar_t outformatw[64];
#endif

  print_format(pFlag, informat, outformat, sizeof(informat), sizeof(outformat));

/* Change informat and outformat to UTF-8 for d2u_fprintf. */
# ifdef D2U_UNIFILE
   /* The format string is encoded in the system default
    * Windows ANSI code page. May have been translated
    * by gettext. Convert it to wide characters. */
   d2u_MultiByteToWideChar(CP_ACP,0, informat, -1, informatw, sizeof(informat));
   d2u_MultiByteToWideChar(CP_ACP,0, outformat, -1, outformatw, sizeof(outformat));
   /* then convert the format string to UTF-8 */
   d2u_WideCharToMultiByte(CP_UTF8, 0, informatw, -1, informat, sizeof(informat), NULL, NULL);
   d2u_WideCharToMultiByte(CP_UTF8, 0, outformatw, -1, outformat, sizeof(outformat), NULL, NULL);
#endif

  if (pFlag->status & NO_REGFILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping %s, not a regular file.\n"), infile);
  } else if (pFlag->status & OUTPUTFILE_SYMLINK) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping %s, output file %s is a symbolic link.\n"), infile, outfile);
  } else if (pFlag->status & INPUT_TARGET_NO_REGFILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), infile);
  } else if (pFlag->status & OUTPUT_TARGET_NO_REGFILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping %s, target of symbolic link %s is not a regular file.\n"), infile, outfile);
  } else if (pFlag->status & BINARY_FILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping binary file %s\n"), infile);
  } else if (pFlag->status & WRONG_CODEPAGE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
#ifdef D2U_UNICODE
  } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), infile, (int)sizeof(wchar_t));
  } else if (pFlag->status & UNICODE_CONVERSION_ERROR) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred on line %u.\n"), infile, pFlag->line_nr);
#else
  } else if (pFlag->status & UNICODE_NOT_SUPPORTED) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, UTF-16 conversion is not supported in this version of %s.\n"), infile, progname);
#endif
  } else {
    d2u_fprintf(stderr,"%s: ",progname);
    if (informat[0] == '\0') {
      if (is_dos2unix(progname)) {
        if (!RetVal) d2u_fprintf(stderr, _("converting file %s to file %s in Unix format...\n"), infile, outfile);
      } else {
        if (pFlag->FromToMode == FROMTO_UNIX2MAC) {
          if (!RetVal) d2u_fprintf(stderr, _("converting file %s to file %s in Mac format...\n"), infile, outfile);
        } else {
          if (!RetVal) d2u_fprintf(stderr, _("converting file %s to file %s in DOS format...\n"), infile, outfile);
        }
      }
    } else {
      if (is_dos2unix(progname)) {
    /* TRANSLATORS:
1st %s is encoding of input file.
2nd %s is name of input file.
3rd %s is encoding of output file.
4th %s is name of output file.
E.g.: converting UTF-16LE file in.txt to UTF-8 file out.txt in Unix format... */
        if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s file %s in Unix format...\n"), informat, infile, outformat, outfile);
      } else {
        if (pFlag->FromToMode == FROMTO_UNIX2MAC) {
          if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s file %s in Mac format...\n"), informat, infile, outformat, outfile);
        } else {
          if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s file %s in DOS format...\n"), informat, infile, outformat, outfile);
        }
      }
    }
    if (RetVal) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("problems converting file %s to file %s\n"), infile, outfile);
    }
  }
}

void print_messages_oldfile(const CFlag *pFlag, const char *infile, const char *progname, const int RetVal)
{
  char informat[10];
  char outformat[32];
# ifdef D2U_UNIFILE
  wchar_t informatw[32];
  wchar_t outformatw[64];
#endif

  print_format(pFlag, informat, outformat, sizeof(informat), sizeof(outformat));

/* Change informat and outformat to UTF-8 for d2u_fprintf. */
# ifdef D2U_UNIFILE
   /* The format string is encoded in the system default
    * Windows ANSI code page. May have been translated
    * by gettext. Convert it to wide characters. */
   d2u_MultiByteToWideChar(CP_ACP,0, informat, -1, informatw, sizeof(informat));
   d2u_MultiByteToWideChar(CP_ACP,0, outformat, -1, outformatw, sizeof(outformat));
   /* then convert the format string to UTF-8 */
   d2u_WideCharToMultiByte(CP_UTF8, 0, informatw, -1, informat, sizeof(informat), NULL, NULL);
   d2u_WideCharToMultiByte(CP_UTF8, 0, outformatw, -1, outformat, sizeof(outformat), NULL, NULL);
#endif

  if (pFlag->status & NO_REGFILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping %s, not a regular file.\n"), infile);
  } else if (pFlag->status & OUTPUTFILE_SYMLINK) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping symbolic link %s.\n"), infile);
  } else if (pFlag->status & INPUT_TARGET_NO_REGFILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), infile);
  } else if (pFlag->status & BINARY_FILE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping binary file %s\n"), infile);
  } else if (pFlag->status & WRONG_CODEPAGE) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
#ifdef D2U_UNICODE
  } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), infile, (int)sizeof(wchar_t));
  } else if (pFlag->status & UNICODE_CONVERSION_ERROR) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred on line %u.\n"), infile, pFlag->line_nr);
#else
  } else if (pFlag->status & UNICODE_NOT_SUPPORTED) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("Skipping UTF-16 file %s, UTF-16 conversion is not supported in this version of %s.\n"), infile, progname);
#endif
  } else {
    d2u_fprintf(stderr,"%s: ",progname);
    if (informat[0] == '\0') {
      if (is_dos2unix(progname)) {
        if (!RetVal) d2u_fprintf(stderr, _("converting file %s to Unix format...\n"), infile);
      } else {
        if (pFlag->FromToMode == FROMTO_UNIX2MAC) {
          if (!RetVal) d2u_fprintf(stderr, _("converting file %s to Mac format...\n"), infile);
        } else {
          if (!RetVal) d2u_fprintf(stderr, _("converting file %s to DOS format...\n"), infile);
        }
      }
    } else {
      if (is_dos2unix(progname)) {
    /* TRANSLATORS:
1st %s is encoding of input file.
2nd %s is name of input file.
3rd %s is encoding of output (input file is overwritten).
E.g.: converting UTF-16LE file foo.txt to UTF-8 Unix format... */
        if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s Unix format...\n"), informat, infile, outformat);
      } else {
        if (pFlag->FromToMode == FROMTO_UNIX2MAC) {
          if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s Mac format...\n"), informat, infile, outformat);
        } else {
          if (!RetVal) d2u_fprintf(stderr, _("converting %s file %s to %s DOS format...\n"), informat, infile, outformat);
        }
      }
    }
    if (RetVal) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("problems converting file %s\n"), infile);
    }
  }
}

void print_messages_info(const CFlag *pFlag, const char *infile, const char *progname)
{
  if (pFlag->status & NO_REGFILE) {
    if (pFlag->verbose) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping %s, not a regular file.\n"), infile);
    }
  } else if (pFlag->status & INPUT_TARGET_NO_REGFILE) {
    if (pFlag->verbose) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), infile);
    }
#ifdef D2U_UNICODE
  } else if (pFlag->status & WCHAR_T_TOO_SMALL) {
    if (pFlag->verbose) {
      d2u_fprintf(stderr,"%s: ",progname);
      d2u_fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), infile, (int)sizeof(wchar_t));
    }
#endif
  }
}

#ifdef D2U_UNICODE
void FileInfoW(FILE* ipInF, CFlag *ipFlag, const char *filename, int bomtype, const char *progname)
{
  wint_t TempChar;
  wint_t PreviousChar = 0;
  unsigned int lb_dos = 0;
  unsigned int lb_unix = 0;
  unsigned int lb_mac = 0;
  char *errstr;

  ipFlag->status = 0;

  while ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {
    if ( (TempChar < 32) &&
        (TempChar != 0x0a) &&  /* Not an LF */
        (TempChar != 0x0d) &&  /* Not a CR */
        (TempChar != 0x09) &&  /* Not a TAB */
        (TempChar != 0x0c)) {  /* Not a form feed */
      ipFlag->status |= BINARY_FILE ;
    }
    if (TempChar != 0x0a) { /* Not an LF */
      PreviousChar = TempChar;
      if (TempChar == 0x0d) /* CR */
        lb_mac++;
    } else{
      /* TempChar is an LF */
      if ( PreviousChar == 0x0d ) { /* CR,LF pair. */
        lb_dos++;
        lb_mac--;
        PreviousChar = TempChar;
        continue;
      }
      PreviousChar = TempChar;
      lb_unix++; /* Unix line end (LF). */
    }
  }
  if ((TempChar == WEOF) && ferror(ipInF)) {
    ipFlag->error = errno;
    if (ipFlag->verbose) {
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("can not read from input file %s:"), filename);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
    }
    return;
  }

  if (ipFlag->file_info & INFO_CONVERT) {
    if ((ipFlag->FromToMode == FROMTO_DOS2UNIX) && (lb_dos == 0))
      return;
    if (((ipFlag->FromToMode == FROMTO_UNIX2DOS)||(ipFlag->FromToMode == FROMTO_UNIX2MAC)) && (lb_unix == 0))
      return;
    if ((ipFlag->FromToMode == FROMTO_MAC2UNIX) && (lb_mac == 0))
      return;
    if ((ipFlag->Force == 0) && (ipFlag->status & BINARY_FILE))
      return;
  }

  if (ipFlag->file_info & INFO_DOS)
    d2u_fprintf(stdout, "  %6u", lb_dos);
  if (ipFlag->file_info & INFO_UNIX)
    d2u_fprintf(stdout, "  %6u", lb_unix);
  if (ipFlag->file_info & INFO_MAC)
    d2u_fprintf(stdout, "  %6u", lb_mac);
  if (ipFlag->file_info & INFO_BOM)
    print_bom_info(bomtype);
  if (ipFlag->file_info & INFO_TEXT) {
    if (ipFlag->status & BINARY_FILE)
      d2u_fprintf(stdout, "  binary");
    else
      d2u_fprintf(stdout, "  text  ");
  }
  d2u_fprintf(stdout, "  %s",filename);
  d2u_fprintf(stdout, "\n");
}
#endif

void FileInfo(FILE* ipInF, CFlag *ipFlag, const char *filename, int bomtype, const char *progname)
{
  int TempChar;
  int PreviousChar = 0;
  unsigned int lb_dos = 0;
  unsigned int lb_unix = 0;
  unsigned int lb_mac = 0;
  char *errstr;


  ipFlag->status = 0;

  while ((TempChar = fgetc(ipInF)) != EOF) {
    if ( (TempChar < 32) &&
        (TempChar != '\x0a') &&  /* Not an LF */
        (TempChar != '\x0d') &&  /* Not a CR */
        (TempChar != '\x09') &&  /* Not a TAB */
        (TempChar != '\x0c')) {  /* Not a form feed */
      ipFlag->status |= BINARY_FILE ;
      }
    if (TempChar != '\x0a') { /* Not an LF */
      PreviousChar = TempChar;
      if (TempChar == '\x0d') /* CR */
        lb_mac++;
    } else {
      /* TempChar is an LF */
      if ( PreviousChar == '\x0d' ) { /* CR,LF pair. */
        lb_dos++;
        lb_mac--;
        PreviousChar = TempChar;
        continue;
      }
      PreviousChar = TempChar;
      lb_unix++; /* Unix line end (LF). */
    }
  }
  if ((TempChar == EOF) && ferror(ipInF)) {
    ipFlag->error = errno;
    if (ipFlag->verbose) {
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("can not read from input file %s:"), filename);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
    }
    return;
  }


  if (ipFlag->file_info & INFO_CONVERT) {
    if ((ipFlag->FromToMode == FROMTO_DOS2UNIX) && (lb_dos == 0))
      return;
    if (((ipFlag->FromToMode == FROMTO_UNIX2DOS)||(ipFlag->FromToMode == FROMTO_UNIX2MAC)) && (lb_unix == 0))
      return;
    if ((ipFlag->FromToMode == FROMTO_MAC2UNIX) && (lb_mac == 0))
      return;
    if ((ipFlag->Force == 0) && (ipFlag->status & BINARY_FILE))
      return;
  }

  if (ipFlag->file_info & INFO_DOS)
    d2u_fprintf(stdout, "  %6u", lb_dos);
  if (ipFlag->file_info & INFO_UNIX)
    d2u_fprintf(stdout, "  %6u", lb_unix);
  if (ipFlag->file_info & INFO_MAC)
    d2u_fprintf(stdout, "  %6u", lb_mac);
  if (ipFlag->file_info & INFO_BOM)
    print_bom_info(bomtype);
  if (ipFlag->file_info & INFO_TEXT) {
    if (ipFlag->status & BINARY_FILE)
      d2u_fprintf(stdout, "  binary");
    else
      d2u_fprintf(stdout, "  text  ");
  }
  d2u_fprintf(stdout, "  %s",filename);
  d2u_fprintf(stdout, "\n");
}

int GetFileInfo(char *ipInFN, CFlag *ipFlag, const char *progname)
{
  int RetVal = 0;
  FILE *InF = NULL;
  char *errstr;
  int bomtype_orig = FILE_MBS; /* messages must print the real bomtype, not the assumed bomtype */

  ipFlag->status = 0 ;

  /* Test if input file is a regular file or symbolic link */
  if (regfile(ipInFN, 1, ipFlag, progname)) {
    ipFlag->status |= NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }

  /* Test if input file target is a regular file */
  if (symbolic_link(ipInFN) && regfile_target(ipInFN, ipFlag,progname)) {
    ipFlag->status |= INPUT_TARGET_NO_REGFILE ;
    /* Not a failure, skipping non-regular input file according spec. */
    return -1;
  }


  /* can open in file? */
  InF=OpenInFile(ipInFN);
  if (InF == NULL) {
    ipFlag->error = errno;
    errstr = strerror(errno);
    d2u_fprintf(stderr, "%s: %s: ", progname, ipInFN);
    d2u_ansi_fprintf(stderr, "%s\n", errstr);
    RetVal = -1;
  }


  if (!RetVal)
    if (check_unicode_info(InF, ipFlag, progname, &bomtype_orig))
      RetVal = -1;

  /* info sucessful? */
#ifdef D2U_UNICODE
  if (!RetVal) {
    if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE)) {
      FileInfoW(InF, ipFlag, ipInFN, bomtype_orig, progname);
    } else {
      FileInfo(InF, ipFlag, ipInFN, bomtype_orig, progname);
    }
  }
#else
  if (!RetVal)
    FileInfo(InF, ipFlag, ipInFN, bomtype_orig, progname);
#endif

   /* can close in file? */
  if ((InF) && (fclose(InF) == EOF))
    RetVal = -1;

  return RetVal;
}

int GetFileInfoStdio(CFlag *ipFlag, const char *progname)
{
  int RetVal = 0;
  int bomtype_orig = FILE_MBS; /* messages must print the real bomtype, not the assumed bomtype */

  ipFlag->status = 0 ;

#if defined(_WIN32) && !defined(__CYGWIN__)

    /* stdin and stdout are by default text streams. We need
     * to set them to binary mode. Otherwise an LF will
     * automatically be converted to CR-LF on DOS/Windows.
     * Erwin */

    /* POSIX 'setmode' was deprecated by MicroSoft since
     * Visual C++ 2005. Use ISO C++ conformant '_setmode' instead. */

    _setmode(_fileno(stdin), _O_BINARY);
#elif defined(__MSDOS__) || defined(__CYGWIN__) || defined(__OS2__)
    setmode(fileno(stdin), O_BINARY);
#endif

  if (check_unicode_info(stdin, ipFlag, progname, &bomtype_orig))
    RetVal = -1;

  /* info sucessful? */
#ifdef D2U_UNICODE
  if (!RetVal) {
    if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE)) {
      FileInfoW(stdin, ipFlag, "", bomtype_orig, progname);
    } else {
      FileInfo(stdin, ipFlag, "", bomtype_orig, progname);
    }
  }
#else
  if (!RetVal)
    FileInfo(stdin, ipFlag, "", bomtype_orig, progname);
#endif

  return RetVal;
}

void get_info_options(char *option, CFlag *pFlag, const char *progname)
{
  char *ptr;

  ptr = option;

  if (*ptr == '\0') { /* no flags */
    pFlag->file_info |= INFO_DEFAULT;
    return;
  }

  while (*ptr != '\0') {
    switch (*ptr) {
      case 'd':   /* Print nr of DOS line breaks. */
        pFlag->file_info |= INFO_DOS;
        break;
      case 'u':   /* Print nr of Unix line breaks. */
        pFlag->file_info |= INFO_UNIX;
        break;
      case 'm':   /* Print nr of Mac line breaks. */
        pFlag->file_info |= INFO_MAC;
        break;
      case 'b':   /* Print BOM. */
        pFlag->file_info |= INFO_BOM;
        break;
      case 't':   /* Text or binary. */
        pFlag->file_info |= INFO_TEXT;
        break;
      case 'c':   /* Print only files that would be converted. */
        pFlag->file_info |= INFO_CONVERT;
        break;
      default:
       /* Terminate the program on a wrong option. If pFlag->file_info is
          zero and the program goes on, it may do unwanted conversions. */
        d2u_fprintf(stderr,"%s: ",progname);
        d2u_fprintf(stderr,_("wrong flag '%c' for option -i or --info\n"), *ptr);
        exit(1);
      ;
    }
    ptr++;
  }
}

int parse_options(int argc, char *argv[],
                  CFlag *pFlag, const char *localedir, const char *progname,
                  void (*PrintLicense)(void),
                  int (*Convert)(FILE*, FILE*, CFlag *, const char *)
#ifdef D2U_UNICODE
                , int (*ConvertW)(FILE*, FILE*, CFlag *, const char *)
#endif
                  )
{
  int ArgIdx = 0;
  int ShouldExit = 0;
  int CanSwitchFileMode = 1;
  int process_options = 1;
  int RetVal = 0;
#ifdef D2U_UNIFILE
  char *ptr;
#endif

  /* variable initialisations */
  pFlag->NewFile = 0;
  pFlag->verbose = 1;
  pFlag->KeepDate = 0;
  pFlag->ConvMode = CONVMODE_ASCII;  /* default ascii */
  pFlag->NewLine = 0;
  pFlag->Force = 0;
  pFlag->Follow = SYMLINK_SKIP;
  pFlag->status = 0;
  pFlag->stdio_mode = 1;
  pFlag->error = 0;
  pFlag->bomtype = FILE_MBS;
  pFlag->add_bom = 0;
  pFlag->keep_utf16 = 0;
  pFlag->file_info = 0;
  pFlag->locale_target = TARGET_UTF8;

#ifdef D2U_UNIFILE
   ptr = getenv("DOS2UNIX_DISPLAY_ENC");
   if (ptr != NULL) {
      if (strncmp(ptr, "ansi", sizeof("ansi")) == 0)
         d2u_display_encoding = D2U_DISPLAY_ANSI;
      else if (strncmp(ptr, "unicode", sizeof("unicode")) == 0)
         d2u_display_encoding = D2U_DISPLAY_UNICODE;
      else if (strncmp(ptr, "utf8", sizeof("utf8")) == 0)
         d2u_display_encoding = D2U_DISPLAY_UTF8;
   }
#endif

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
#ifdef D2U_UNICODE
#if (defined(_WIN32) && !defined(__CYGWIN__))
      else if ((strcmp(argv[ArgIdx],"-gb") == 0) || (strcmp(argv[ArgIdx],"--gb18030") == 0))
        pFlag->locale_target = TARGET_GB18030;
#endif
#endif
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
      else if ((strcmp(argv[ArgIdx],"-r") == 0) || (strcmp(argv[ArgIdx],"--remove-bom") == 0)) {
        pFlag->keep_bom = 0;
        pFlag->add_bom = 0;
      }
      else if ((strcmp(argv[ArgIdx],"-S") == 0) || (strcmp(argv[ArgIdx],"--skip-symlink") == 0))
        pFlag->Follow = SYMLINK_SKIP;
      else if ((strcmp(argv[ArgIdx],"-F") == 0) || (strcmp(argv[ArgIdx],"--follow-symlink") == 0))
        pFlag->Follow = SYMLINK_FOLLOW;
      else if ((strcmp(argv[ArgIdx],"-R") == 0) || (strcmp(argv[ArgIdx],"--replace-symlink") == 0))
        pFlag->Follow = SYMLINK_REPLACE;
      else if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0)) {
        PrintVersion(progname, localedir);
        return(pFlag->error);
      }
      else if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0)) {
        PrintLicense();
        return(pFlag->error);
      }
      else if (strcmp(argv[ArgIdx],"-ascii") == 0) { /* SunOS compatible options */
        pFlag->ConvMode = CONVMODE_ASCII;
        pFlag->keep_utf16 = 0;
        pFlag->locale_target = TARGET_UTF8;
      }
      else if (strcmp(argv[ArgIdx],"-7") == 0)
        pFlag->ConvMode = CONVMODE_7BIT;
      else if (strcmp(argv[ArgIdx],"-iso") == 0) {
        pFlag->ConvMode = (int)query_con_codepage();
        if (pFlag->verbose) {
           d2u_fprintf(stderr,"%s: ",progname);
           d2u_fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
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
      else if (strcmp(argv[ArgIdx],"--info") == 0)
        pFlag->file_info |= INFO_DEFAULT;
      else if (strncmp(argv[ArgIdx],"--info=", (size_t)7) == 0) {
        get_info_options(argv[ArgIdx]+7, pFlag, progname);
      } else if (strncmp(argv[ArgIdx],"-i", (size_t)2) == 0) {
        get_info_options(argv[ArgIdx]+2, pFlag, progname);
      } else if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0)) {
        if (++ArgIdx < argc) {
          if (strcmpi(argv[ArgIdx],"ascii") == 0) { /* Benjamin Lin's legacy options */
            pFlag->ConvMode = CONVMODE_ASCII;
            pFlag->keep_utf16 = 0;
          }
          else if (strcmpi(argv[ArgIdx], "7bit") == 0)
            pFlag->ConvMode = CONVMODE_7BIT;
          else if (strcmpi(argv[ArgIdx], "iso") == 0) {
            pFlag->ConvMode = (int)query_con_codepage();
            if (pFlag->verbose) {
               d2u_fprintf(stderr,"%s: ",progname);
               d2u_fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
            }
            if (pFlag->ConvMode < 2)
               pFlag->ConvMode = CONVMODE_437;
          }
          else if (strcmpi(argv[ArgIdx], "mac") == 0) {
            if (is_dos2unix(progname))
              pFlag->FromToMode = FROMTO_MAC2UNIX;
            else
              pFlag->FromToMode = FROMTO_UNIX2MAC;
          } else {
            d2u_fprintf(stderr,"%s: ",progname);
            d2u_fprintf(stderr, _("invalid %s conversion mode specified\n"),argv[ArgIdx]);
            pFlag->error = 1;
            ShouldExit = 1;
            pFlag->stdio_mode = 0;
          }
        } else {
          ArgIdx--;
          d2u_fprintf(stderr,"%s: ",progname);
          d2u_fprintf(stderr,_("option '%s' requires an argument\n"),argv[ArgIdx]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
      }

#ifdef D2U_UNIFILE
      else if ((strcmp(argv[ArgIdx],"-D") == 0) || (strcmp(argv[ArgIdx],"--display-enc") == 0)) {
        if (++ArgIdx < argc) {
          if (strcmpi(argv[ArgIdx],"ansi") == 0)
            d2u_display_encoding = D2U_DISPLAY_ANSI;
          else if (strcmpi(argv[ArgIdx], "unicode") == 0)
            d2u_display_encoding = D2U_DISPLAY_UNICODE;
          else if (strcmpi(argv[ArgIdx], "utf8") == 0) {
            d2u_display_encoding = D2U_DISPLAY_UTF8;
          } else {
            d2u_fprintf(stderr,"%s: ",progname);
            d2u_fprintf(stderr, _("invalid %s display encoding specified\n"),argv[ArgIdx]);
            pFlag->error = 1;
            ShouldExit = 1;
            pFlag->stdio_mode = 0;
          }
        } else {
          ArgIdx--;
          d2u_fprintf(stderr,"%s: ",progname);
          d2u_fprintf(stderr,_("option '%s' requires an argument\n"),argv[ArgIdx]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
      }
#endif

      else if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0)) {
        /* last convert not paired */
        if (!CanSwitchFileMode) {
          d2u_fprintf(stderr,"%s: ",progname);
          d2u_fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
        pFlag->NewFile = 0;
        pFlag->file_info = 0;
      }

      else if ((strcmp(argv[ArgIdx],"-n") == 0) || (strcmp(argv[ArgIdx],"--newfile") == 0)) {
        /* last convert not paired */
        if (!CanSwitchFileMode) {
          d2u_fprintf(stderr,"%s: ",progname);
          d2u_fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
          pFlag->error = 1;
          ShouldExit = 1;
          pFlag->stdio_mode = 0;
        }
        pFlag->NewFile = 1;
        pFlag->file_info = 0;
      }
      else { /* wrong option */
        PrintUsage(progname);
        ShouldExit = 1;
        pFlag->error = 1;
        pFlag->stdio_mode = 0;
      }
    } else {
      pFlag->stdio_mode = 0;
      /* not an option */
      if (pFlag->NewFile) {
        if (CanSwitchFileMode)
          CanSwitchFileMode = 0;
        else {
#ifdef D2U_UNICODE
          RetVal = ConvertNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag, progname, Convert, ConvertW);
#else
          RetVal = ConvertNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag, progname, Convert);
#endif
          if (pFlag->verbose)
            print_messages_newfile(pFlag, argv[ArgIdx-1], argv[ArgIdx], progname, RetVal);
          CanSwitchFileMode = 1;
        }
      } else {
        if (pFlag->file_info) {
          RetVal = GetFileInfo(argv[ArgIdx], pFlag, progname);
          print_messages_info(pFlag, argv[ArgIdx], progname);
        } else {
#ifdef D2U_UNICODE
          RetVal = ConvertNewFile(argv[ArgIdx], argv[ArgIdx], pFlag, progname, Convert, ConvertW);
#else
          RetVal = ConvertNewFile(argv[ArgIdx], argv[ArgIdx], pFlag, progname, Convert);
#endif
          if (pFlag->verbose)
            print_messages_oldfile(pFlag, argv[ArgIdx], progname, RetVal);
        }
      }
    }
  }

  /* no file argument, use stdin and stdout */
  if ( (argc > 0) && pFlag->stdio_mode) {
    if (pFlag->file_info) {
      RetVal = GetFileInfoStdio(pFlag, progname);
      print_messages_info(pFlag, "stdin", progname);
    } else {
#ifdef D2U_UNICODE
      ConvertStdio(pFlag, progname, Convert, ConvertW);
#else
      ConvertStdio(pFlag, progname, Convert);
#endif
      if (pFlag->verbose)
        print_messages_stdio(pFlag, progname);
    }
    return pFlag->error;
  }

  if (!CanSwitchFileMode) {
    d2u_fprintf(stderr,"%s: ",progname);
    d2u_fprintf(stderr, _("target of file %s not specified in new-file mode\n"), argv[ArgIdx-1]);
    pFlag->error = 1;
  }
  return pFlag->error;
}

void d2u_getc_error(CFlag *ipFlag, const char *progname)
{
    char *errstr;

    ipFlag->error = errno;
    if (ipFlag->verbose) {
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_ansi_fprintf(stderr, _("can not read from input file: %s\n"), errstr);
    }
}

void d2u_putc_error(CFlag *ipFlag, const char *progname)
{
    char *errstr;

    ipFlag->error = errno;
    if (ipFlag->verbose) {
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_ansi_fprintf(stderr, _("can not write to output file: %s\n"), errstr);
    }
}

#ifdef D2U_UNICODE
void d2u_putwc_error(CFlag *ipFlag, const char *progname)
{
    char *errstr;

    if (!(ipFlag->status & UNICODE_CONVERSION_ERROR)) {
      ipFlag->error = errno;
      if (ipFlag->verbose) {
        errstr = strerror(errno);
        d2u_fprintf(stderr, "%s: ", progname);
        d2u_ansi_fprintf(stderr, _("can not write to output file: %s\n"), errstr);
      }
    }
}

wint_t d2u_getwc(FILE *f, int bomtype)
{
   int c_trail, c_lead;
   wint_t wc;

   if (((c_lead=fgetc(f)) == EOF)  || ((c_trail=fgetc(f)) == EOF))
      return(WEOF);

   if (bomtype == FILE_UTF16LE) { /* UTF16 little endian */
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

   if (bomtype == FILE_UTF16LE) { /* UTF16 little endian */
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
wint_t d2u_putwc(wint_t wc, FILE *f, CFlag *ipFlag, const char *progname)
{
   static char mbs[8];
   static wchar_t lead=0x01, trail;  /* lead get's invalid value */
   static wchar_t wstr[3];
   size_t i,len;
   int c_trail, c_lead;
#if (defined(_WIN32) && !defined(__CYGWIN__))
   DWORD dwFlags;
#else
   char *errstr;
#endif

   if (ipFlag->keep_utf16) {
     if (ipFlag->bomtype == FILE_UTF16LE) { /* UTF16 little endian */
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

   /* Note: In the new Unicode standard lead is named "high", and trail is name "low". */

   /* check for lead without a trail */
   if ((lead >= 0xd800) && (lead < 0xdc00) && ((wc < 0xdc00) || (wc >= 0xe000))) {
      d2u_fprintf(stderr, "%s: ", progname);
      d2u_fprintf(stderr, _("error: Invalid surrogate pair. Missing low surrogate.\n"));
      ipFlag->status |= UNICODE_CONVERSION_ERROR ;
      return(WEOF);
   }

   if ((wc >= 0xd800) && (wc < 0xdc00)) {   /* Surrogate lead */
      /* fprintf(stderr, "UTF-16 lead %x\n",wc); */
      lead = (wchar_t)wc; /* lead (high) surrogate */
      return(wc);
   }
   if ((wc >= 0xdc00) && (wc < 0xe000)) {   /* Surrogate trail */

      /* check for trail without a lead */
      if ((lead < 0xd800) || (lead >= 0xdc00)) {
         d2u_fprintf(stderr, "%s: ", progname);
         d2u_fprintf(stderr, _("error: Invalid surrogate pair. Missing high surrogate.\n"));
         ipFlag->status |= UNICODE_CONVERSION_ERROR ;
         return(WEOF);
      }
      /* fprintf(stderr, "UTF-16 trail %x\n",wc); */
      trail = (wchar_t)wc; /* trail (low) surrogate */
#if defined(_WIN32) || defined(__CYGWIN__)
      /* On Windows (including Cygwin) wchar_t is 16 bit */
      /* We cannot decode an UTF-16 surrogate pair, because it will
         not fit in a 16 bit wchar_t. */
      wstr[0] = lead;
      wstr[1] = trail;
      wstr[2] = L'\0';
      lead = 0x01; /* make lead invalid */
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
      lead = 0x01; /* make lead invalid */
      /* fprintf(stderr, "UTF-32  %x\n",wstr[0]); */
#endif
   } else {
      wstr[0] = (wchar_t)wc;
      wstr[1] = L'\0';
   }

   if (wc == 0x0000) {
      if (fputc(0, f) == EOF)
         return(WEOF);
      return(wc);
   }

#if (defined(_WIN32) && !defined(__CYGWIN__))
/* The WC_ERR_INVALID_CHARS flag is available since Windows Vista (0x0600). It enables checking for
   invalid input characters. */
#if WINVER >= 0x0600
   dwFlags = WC_ERR_INVALID_CHARS;
#else
   dwFlags = 0;
#endif
   /* On Windows we convert UTF-16 always to UTF-8 or GB18030 */
   if (ipFlag->locale_target == TARGET_GB18030) {
     len = (size_t)(WideCharToMultiByte(54936, dwFlags, wstr, -1, mbs, sizeof(mbs), NULL, NULL) -1);
   } else {
     len = (size_t)(WideCharToMultiByte(CP_UTF8, dwFlags, wstr, -1, mbs, sizeof(mbs), NULL, NULL) -1);
   }
#else
   /* On Unix we convert UTF-16 to the locale encoding */
   len = wcstombs(mbs, wstr, sizeof(mbs));
   /* fprintf(stderr, "len  %d\n",len); */
#endif

   if ( len == (size_t)(-1) ) {
      /* Stop when there is a conversion error */
   /* On Windows we convert UTF-16 always to UTF-8 or GB18030 */
#if (defined(_WIN32) && !defined(__CYGWIN__))
      d2u_PrintLastError(progname);
#else
      errstr = strerror(errno);
      d2u_fprintf(stderr, "%s:", progname);
      d2u_ansi_fprintf(stderr, " %s\n", errstr);
#endif
      ipFlag->status |= UNICODE_CONVERSION_ERROR ;
      return(WEOF);
   } else {
      for (i=0; i<len; i++) {
         if (fputc(mbs[i], f) == EOF)
            return(WEOF);
      }
   }
   return(wc);
}
#endif
