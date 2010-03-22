 *
 *  The dos2unix package is distributed under FreeBSD style license.
 *  See also http://www.freebsd.org/copyright/freebsd-license.html
 *  --------
 *  Copyright (C) 2009-2010 Erwin Waterlander
 *  Copyright (C) 1998 Christian Wurll
 *  Copyright (C) 1998 Bernd Johannes Wuebben
 *  Copyright (C) 1994-1995 Benjamin Lin.
#if defined(DJGPP) || defined(__TURBOC__) /* DJGPP */
#ifndef __TURBOC__
#endif
#ifdef __TURBOC__
#define __FLAT__
#endif
#if  defined(__TURBOC__) || defined(DJGPP) || defined(__MINGW32__)
 * BORLANDC, DJGPP, MINGW32 */
#endif

#ifdef MSDOS
 * BORLANDC, DJGPP, MINGW32, OPENWATCOM */
  int stdio_mode;                       /* if TRUE, stdio mode */
Copyright (C) 2009-2010 Erwin Waterlander\n\
Copyright (C) 1998      Christian Wurll (Version 3.1)\n\
Copyright (C) 1998      Bernd Johannes Wuebben (Version 3.0)\n\
Copyright (C) 1994-1995 Benjamin Lin\n\
                putc('\x0d', ipOutF); /* put CR, part of DOS CR-LF */
#ifdef __TURBOC__
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

#ifndef NO_FCHMOD
#endif
#ifndef NO_FCHMOD
#endif
  pFlag->stdio_mode = 1;
      {
        return(0);
      }
      else if ((strcmp(argv[ArgIdx],"-k") == 0) || (strcmp(argv[ArgIdx],"--keepdate") == 0))
      else if ((strcmp(argv[ArgIdx],"-f") == 0) || (strcmp(argv[ArgIdx],"--force") == 0))
      else if ((strcmp(argv[ArgIdx],"-q") == 0) || (strcmp(argv[ArgIdx],"--quiet") == 0))
      else if ((strcmp(argv[ArgIdx],"-l") == 0) || (strcmp(argv[ArgIdx],"--newline") == 0))
      else if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0))
        return(0);
      else if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0))
      {
        return(0);
      }
      else if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0))
      else if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0))
      else if ((strcmp(argv[ArgIdx],"-n") == 0) || (strcmp(argv[ArgIdx],"--newfile") == 0))
      else { /* wrong option */
        PrintUsage();
        return(0);
      }
      pFlag->stdio_mode = 0;
  /* no file argument, use stdin and stdout */
  if (pFlag->stdio_mode)
  {
    exit(ConvertDosToUnixStdio(pFlag));
  }

