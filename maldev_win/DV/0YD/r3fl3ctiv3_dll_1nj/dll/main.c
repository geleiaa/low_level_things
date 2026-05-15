#include "loader.h"
#include "stdio.h"
#include <inttypes.h>

//#pragma comment(lib, "advapi32.lib")

typedef int (WINAPI* myMessageBoxA)(
	HWND   hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT   uType
	);
//
//UINT64 GetKernel32() {
//	ULONG_PTR kernel32dll, val1, val2, val3;
//	USHORT usCounter;
//
//	// kernel32.dll is at 0x60 offset and __readgsqword is compiler intrinsic,
//	// so we don't need to extract it's symbol
//	kernel32dll = __readgsqword(0x60);
//
//	kernel32dll = (ULONG_PTR)((_PPEB)kernel32dll)->pLdr;
//	val1 = (ULONG_PTR)((PPEB_LDR_DATA)kernel32dll)->InMemoryOrderModuleList.Flink;
//	while (val1) {
//		val2 = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)val1)->BaseDllName.pBuffer;
//		usCounter = ((PLDR_DATA_TABLE_ENTRY)val1)->BaseDllName.Length;
//		val3 = 0;
//
//		//calculate the hash of kernel32.dll
//		do {
//			val3 = ror13((DWORD)val3);
//			if (*((BYTE*)val2) >= 'a')
//				val3 += *((BYTE*)val2) - 0x20;
//			else
//				val3 += *((BYTE*)val2);
//			val2++;
//		} while (--usCounter);
//
//		// compare the hash kernel32.dll
//		if ((DWORD)val3 == KERNEL32DLL_HASH) {
//			//return kernel32.dll if found
//			kernel32dll = (ULONG_PTR)((PLDR_DATA_TABLE_ENTRY)val1)->DllBase;
//			return kernel32dll;
//		}
//		val1 = DEREF(val1);
//	}
//	return 0;
//}
//
//UINT64 GetSymbolAddress(HANDLE hModule, LPCSTR lpProcName) {
//	UINT64 dllAddress = (UINT64)hModule,
//		symbolAddress = 0,
//		exportedAddressTable = 0,
//		namePointerTable = 0,
//		ordinalTable = 0;
//
//	if (hModule == NULL) {
//		return 0;
//	}
//
//	PIMAGE_NT_HEADERS ntHeaders = NULL;
//	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
//	PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;
//
//	ntHeaders = (PIMAGE_NT_HEADERS)(dllAddress + ((PIMAGE_DOS_HEADER)dllAddress)->e_lfanew);
//	dataDirectory = (PIMAGE_DATA_DIRECTORY)&ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
//	exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dllAddress + dataDirectory->VirtualAddress);
//
//	exportedAddressTable = (dllAddress + exportDirectory->AddressOfFunctions);
//	namePointerTable = (dllAddress + exportDirectory->AddressOfNames);
//	ordinalTable = (dllAddress + exportDirectory->AddressOfNameOrdinals);
//
//	if (((UINT64)lpProcName & 0xFFFF0000) == 0x00000000) {
//		exportedAddressTable += ((IMAGE_ORDINAL((UINT64)lpProcName) - exportDirectory->Base) * sizeof(DWORD));
//		symbolAddress = (UINT64)(dllAddress + DEREF_32(exportedAddressTable));
//	}
//	else {
//		DWORD dwCounter = exportDirectory->NumberOfNames;
//		while (dwCounter--) {
//			char* cpExportedFunctionName = (char*)(dllAddress + DEREF_32(namePointerTable));
//			if (my_strcmp(cpExportedFunctionName, lpProcName) == 0) {
//				exportedAddressTable += (DEREF_16(ordinalTable) * sizeof(DWORD));
//				symbolAddress = (UINT64)(dllAddress + DEREF_32(exportedAddressTable));
//				break;
//			}
//			namePointerTable += sizeof(DWORD);
//			ordinalTable += sizeof(WORD);
//		}
//	}
//
//	return symbolAddress;
//}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	BOOL bReturnValue = TRUE;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: {
		//UINT64 hk32 = GetKernel32();
		HMODULE huser32 = LoadLibraryA("user32.dll");
		//myMessageBoxA msgboxFunc = (myMessageBoxA)GetSymbolAddress(huser32, "MessageBoxA");
		myMessageBoxA msgboxFunc = (myMessageBoxA)GetProcAddress(huser32, "MessageBoxA");
		msgboxFunc(NULL, "Function Called", "User32 Test", MB_OK);

		//MessageBoxA(NULL, "Test Message", "Message Box Reflected", MB_OK);
		//printf("Returning this output\n");
		fflush(stdout);
		break;
	}
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}