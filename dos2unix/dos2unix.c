#include <fnmatch.h>
  int ConvMode;                         /* 0: ascii, 1: 7bit, 2: iso, 3: mac */  
   convmode       ascii, 7bit, iso, mac, default to ascii\n\
 -l --newline     add additional newline\n\
      case 0: /* ascii */
      case 1: /* 7bit */
      case 2: /* iso */
            /* TempChar is a CR */
            if (putc('\x0a', ipOutF) == EOF) /* MAC line end (CR). Put LF */
            if (ipFlag->NewLine) {  /* add additional LF? */
              putc('\x0a', ipOutF);
            }
  if (fnmatch("*mac2unix", argv[0], FNM_CASEFOLD) == 0)
    pFlag->ConvMode = 3;
          if (strcmpi(argv[ArgIdx],"ascii") == 0)
          else if (strcmpi(argv[ArgIdx], "7bit") == 0)
          else if (strcmpi(argv[ArgIdx], "iso") == 0)
          else if (strcmpi(argv[ArgIdx], "mac") == 0)