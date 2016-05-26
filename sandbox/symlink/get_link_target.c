// get target of symbolic link on Windows.


// https://msdn.microsoft.com/en-us/library/6sehtctf.aspx
// GetFinalPathNameByHandle requires minimal Windows Vista (0x600).
#define WINVER 0x600
#define _WIN32_WINNT 0x600

#include <windows.h>
#include <stdio.h>

// https://msdn.microsoft.com/en-us/library/aa364962%28VS.85%29.aspx

#define BUFSIZE 128

int main(int argc, char *argv[])
{
    char Path[BUFSIZE];
    HANDLE hFile;
    DWORD dwRet;

    printf("\n");
    printf("WINVER 0x%X\n",WINVER);
    printf("_WIN32_WINNT 0x%X\n",_WIN32_WINNT);

    if( argc != 2 )
    {
        printf("ERROR:\tIncorrect number of arguments\n\n");
        printf("%s <file_name>\n", argv[0]);
        return 1;
    }

    hFile = CreateFile(argv[1],               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no attr. template

    if( hFile == INVALID_HANDLE_VALUE)
    {
        // printf("Could not open file (error %d\n)", GetLastError());
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    dwRet = GetFinalPathNameByHandle( hFile, Path, BUFSIZE, VOLUME_NAME_NT );
    if(dwRet < BUFSIZE)
    {
        printf("\nThe final path is: %s\n", Path);
    }
    else printf("\nThe required buffer size is %d.\n", (int)dwRet);

    CloseHandle(hFile);
    return 0;
}
