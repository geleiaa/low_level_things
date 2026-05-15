#include <windows.h>
#include "shellcode.h"

extern "C" __declspec(dllexport) LONG Cplapplet(HWND hwndCpl, UINT msg, LPARAM lParam1, LPARAM lParam2) {
    MessageBoxA(NULL, "Message", "Caption", MB_OK);
    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            Cplapplet(NULL, NULL, NULL, NULL);
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
