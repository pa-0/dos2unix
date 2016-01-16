#include <stdio.h>
#include <fcntl.h>
#include <windows.h>
#include <locale.h>

/*

 This program proves that when you set the locale to "", the Chinese ANSI CP936
 encoded text is printed wrongly.

To test this program you first need to change the Windows locale setting to
simplified Chinese. There is no problem doing that, because the "display
language" will stay the original language, and you can still use your Windows.

Control Panel > Region and Language > Administrative > Change system locale

Select simplified Chinese and reboot your PC.

 */

int main() {

  char str[5];
  wchar_t wstr[10];
  int prevmode;

  setlocale (LC_ALL, "");
  /* uncomment the line below and the ANSI text will be printed correctly. */
  /* setlocale (LC_ALL, "C"); */


/* Create ANSI CP936 string (meaning: Western-European). */
  str[0] = 0xce;
  str[1] = 0xf7;
  str[2] = 0xc5;
  str[3] = 0xb7;
  str[4] = '\0';

/* When the locale is set to "" the following line will produce wrong output */
  printf ("CP936 %s\n",str);

/* Convert CP936 to UTF-16. This will produce correct output in all cases. */
  MultiByteToWideChar(936, 0, str, -1, wstr, sizeof(wstr));
  prevmode = _setmode(_fileno(stdout), _O_U16TEXT);
  wprintf(L"UTF-16 %ls\n",wstr);
  _setmode(_fileno(stdout), prevmode);

  return  0;
}
