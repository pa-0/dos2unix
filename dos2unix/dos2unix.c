 -L --license     display software license\n\
  char *ptr;
  if ( ((ptr=strrchr(argv[0],'/')) == NULL) && ((ptr=strrchr(argv[0],'\\')) == NULL) )
    ptr = argv[0];
  else
    ptr++;

  if ((strcmpi("mac2unix", ptr) == 0) || (strcmpi("mac2unix.exe", ptr) == 0))