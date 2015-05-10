#include "WRTInterop.h"

namespace Interop { namespace WRT {

namespace Util {
	FARPROC GetCombaseProcAddress(LPCSTR Name)
	{
		return GetProcAddress(LoadLibraryA("COMBASE"), Name);
	}

	EXTERN_C HRESULT WINAPI CreateString(PCWSTR Str, HSTRING*pHS)
	{
		return WindowsCreateString(Str, lstrlenW(Str), pHS);
	}
}


EXTERN_C HRESULT WINAPI RoInitialize(RO_INIT_TYPE mode)
{
	FARPROC f = Util::GetCombaseProcAddress("RoInitialize");
	return f ? ((HRESULT(WINAPI*)(RO_INIT_TYPE))f)(mode) : E_NOTIMPL;
}
EXTERN_C void WINAPI RoUninitialize()
{
	FARPROC f = Util::GetCombaseProcAddress("RoUninitialize");
	if (f) ((void(WINAPI*)(void))f)();
}

EXTERN_C HRESULT WINAPI RoGetActivationFactory(HSTRING a, REFIID b, void**c)
{
	HRESULT(WINAPI*f)(HSTRING, REFIID, void**);
	((FARPROC&)f) = Util::GetCombaseProcAddress("RoGetActivationFactory");
	return f(a, b, c);
}

EXTERN_C HRESULT WINAPI WindowsCreateString(PCWSTR s, UINT len, HSTRING*p)
{
	static FARPROC f = 0;
	if (!f) f = Util::GetCombaseProcAddress("WindowsCreateString");
	return f ? ((HRESULT(WINAPI*)(PCWSTR,UINT,HSTRING*))f)(s, len, p) : E_NOTIMPL;
}
EXTERN_C HRESULT WINAPI WindowsDeleteString(HSTRING hs)
{
	static FARPROC f = 0;
	if (!f) f = Util::GetCombaseProcAddress("WindowsDeleteString");
	return f ? ((HRESULT(WINAPI*)(HSTRING))f)(hs) : E_NOTIMPL;
}


} }
