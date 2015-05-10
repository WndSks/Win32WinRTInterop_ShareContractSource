# Win32-WinRT Interop: ShareContractSource
A pure Win32 implementation of a WinRT share contract source helper object.

Should work with most versions of Visual C++ with any SDK. Does not use WRL or any other helper libraries.

```Batchfile
cl App.cpp WRTInterop.cpp WRTInterop_ShareContractSource.cpp /Fe:"Win32ShareTest" /W4 /GS- /GR- /Zl /link /subsystem:WINDOWS /NODEFAULTLIB:libcpmt.lib kernel32.lib gdi32.lib user32.lib
```
