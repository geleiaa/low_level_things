#include <Windows.h>
#include <stdio.h>

int main() {
    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFOEXA StartupInfoEx;
	ZeroMemory(&StartupInfoEx, sizeof(StartupInfoEx));
	StartupInfoEx.StartupInfo.cb = sizeof(STARTUPINFOEXA);
	PPROC_THREAD_ATTRIBUTE_LIST pAttributeList = NULL;
	DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
    DWORD dwFlags = EXTENDED_STARTUPINFO_PRESENT;

    SIZE_T cbAttributeListSize = 0;
    DWORD attributeCount = 1;
    InitializeProcThreadAttributeList(NULL, attributeCount, 0, &cbAttributeListSize);

    pAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, cbAttributeListSize);
    if (NULL == pAttributeList) {
        printf("GetLastError: %lu\n", GetLastError());
        return 0;
    }
    if (! InitializeProcThreadAttributeList(pAttributeList, attributeCount, 0, &cbAttributeListSize)) {
        printf("GetLastError: %lu\n", GetLastError());
        return 0;
    }

    if (! UpdateProcThreadAttribute(pAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &policy, sizeof(DWORD64), NULL, NULL)) {
        printf("GetLastError: %lu\n", GetLastError());
        return 0;
    }

    StartupInfoEx.lpAttributeList = pAttributeList;
    CreateProcessA(NULL, "notepad.exe", 0, 0, TRUE, dwFlags, 0, 0, &StartupInfoEx.StartupInfo, &ProcessInformation);

	return 0;
}