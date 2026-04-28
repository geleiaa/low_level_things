x86_64-w64-mingw32-gcc -m64 info_winapi.c -o info_winapi.exe


CC_64=x86_64-w64-mingw32-gcc
CFLAGS_64=-Wall -m64 -s -Os -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libgcc -fno-keep-inline-dllexport -Wl,--gc-sections -Wno-missing-braces
CFLAGS_DLL=-DBUILD_DLL -shared

boxreflect:
	$(CC_64) boxreflect.c loader.c -o boxreflect.dll $(CFLAGS_64) $(CFLAGS_LINKER) $(CFLAGS_DLL)

peparser:
	$(CC_64) peParser.c -o peParser.exe $(CFLAGS_LINKER)



x86_64-w64-mingw32-gcc cmdshell.c -o cmdshell.exe -lws2_32

https://github.com/itm4n/PPLdump

https://github.com/stephenfewer/ReflectiveDLLInjection/tree/master

https://github.com/RedSiege/EXCELntDonut




