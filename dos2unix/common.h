 *   Copyright (C) 2009-2012 Erwin Waterlander
#  ifndef __MSYS__
#    include <libgen.h>
#  endif
#include <wchar.h>
#if (defined(__WATCOMC__) && defined(__NT__))  /* Watcom */
#  define WIN32
#endif

#if defined(WIN32) && !defined(__CYGWIN__) /* Windows */
#define FILE_MBS     0  /* Multi-byte string or 8-bit char */
#define FILE_UTF16LE 1  /* UTF-16 Little Endian */
#define FILE_UTF16BE 2  /* UTF-16 Big Endian */
#define FILE_UTF8    3  /* UTF-8 */

  int bomtype;                          /* byte order mark */
  int add_bom;                          /* 1: write BOM */
#if defined(__TURBOC__) || defined(__MSYS__)
FILE *read_bom (FILE *f, int *bomtype);
#ifdef D2U_UNICODE
wint_t d2u_getwc(FILE *f, int bomtype);
wint_t d2u_ungetwc(wint_t wc, FILE *f, int bomtype);
wint_t d2u_putwc(wint_t wc, FILE *f);
#endif