# how to works

### 1 - When injector is executed

1. Injector (injector/main.c):
  - Reads the DLL file (compiled from dll/main.c) into memory
  - Locates the ReflectiveLoader function offset within the DLL
  - Allocates executable memory in the target process
  - Writes the DLL image to that memory
  - Calculates the address of ReflectiveLoader in the target process
  - Creates a remote thread starting at ReflectiveLoader


### 2 - When ReflectiveLoader starts executing dll/loader.c:

2. Loader (dll/loader.c):
  - Executes in the target process context via the remote thread
  - Performs reflective loading of the DLL image, in an location in memory the ReflectiveLoader will first calculate its own image's current location in memory so as to be able to parse its own headers for use later on.
  - Transfers execution to the DLL's DllMain entry point


Step 0: Self-Location

  - Uses caller() (via _ReturnAddress()) to get its own return address
  - Searches backward in memory to find the DLL's MZ/PE header
  - Determines the current base address of the loaded DLL image

Step 1: API Resolution

  - Parses the PEB to locate kernel32.dll and ntdll.dll in memory
  - Resolves required functions by comparing hashed export names:
    - LoadLibraryA, GetProcAddress, VirtualAlloc from kernel32.dll
    - NtFlushInstructionCache from ntdll.dll

Step 2: Memory Allocation & Header Copy

  - Allocates new memory space for the DLL using VirtualAlloc
  - Copies the DOS headers, NT headers, and section tables to the new location

Step 3: Section Mapping

  - Iterates through each section in the DLL
  - Copies raw section data from the original image to the new memory location at the correct virtual addresses

Step 4: Import Resolution

  - Processes the DLL's import table
  - For each imported DLL:
    - Loads it using the resolved LoadLibraryA
    - Resolves each imported function using the resolved GetProcAddress
    - Writes the function addresses into the Import Address Table (IAT)

Step 5: Relocation Processing

  - Calculates the delta between preferred and actual base addresses
  - Processes the base relocation table to adjust absolute addresses
  - Handles different relocation types (DIR64, HIGHLOW, HIGH, LOW)

Step 6: Entry Point Execution (dll/main.c)

  - Flushes the instruction cache to ensure coherency after relocations
  - Calculates the address of the DLL's DllMain function
  - Calls DllMain with:
    - hinstDLL: Base address of the newly loaded DLL
    - dwReason: DLL_PROCESS_ATTACH
    - lpParameter: Parameter passed to ReflectiveLoader (NULL in this case)

