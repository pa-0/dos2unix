#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

int main() {

  wchar_t wstr[2];
  char str[4];
  char *errstr;
  size_t i;

  setlocale(LC_ALL, "");

  wstr[0] = 0x8428; /* Unicode CJK U+8428 */
  wstr[1] = 0x0;

  for (i=0;i<sizeof(str);i++)
    str[i]='\0';

  if (wcstombs(str, wstr, sizeof(str)) != (size_t)-1) {
    fprintf(stderr,"%02X %02X %02X %02X\n",(unsigned char)str[0], (unsigned char)str[1], (unsigned char)str[2], (unsigned char)str[3]);
    /* fprintf(stderr,"%s\n",str); */
    fprintf(stdout,"PASS\n");
    return 0;
  } else {
    errstr = strerror(errno);
    fprintf(stderr,"%s\n",errstr);
    fprintf(stdout,"FAIL\n");
    return 1;
  }

}
