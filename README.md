# Win32-WinRT Interop: ShareContractSource
A pure Win32 implementation of a WinRT share contract source helper object.

Should work with most versions of Visual C++ with any SDK. Does not use WRL or any other helper libraries.

```Batchfile
cl App.cpp WRTInterop.cpp WRTInterop_ShareContractSource.cpp /Fe"Win32ShareTest" /O1 /W4 /GS- /GR- /Zl /link /subsystem:WINDOWS /NODEFAULTLIB:libcpmt.lib kernel32.lib gdi32.lib user32.lib uuid.lib ole32.lib
```

```Batchfile
g++ App.cpp WRTInterop.cpp WRTInterop_ShareContractSource.cpp -o Win32ShareTest.exe -mwindows -Os -Wall -static-libgcc -static-libstdc++ -s -Wl,--file-alignment,512 -lkernel32 -luser32 -lgdi32 -luuid -lversion
```
