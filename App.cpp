#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include "WRTInterop_ShareContractSource.h"

struct WRTString // A really simple helper class for working with HSTRINGs
{
	Interop::WRT::HSTRING m_hs;
	WRTString() : m_hs(0) {}
	WRTString(PCWSTR Str) : m_hs(0) { Interop::WRT::Util::CreateString(Str, &m_hs); }
	~WRTString() { Interop::WRT::WindowsDeleteString(m_hs); }
	Interop::WRT::HSTRING Get() const { return m_hs; }
};

#ifdef _MSC_VER
#pragma comment(lib, "version.lib")
#endif
static bool WinVer_AtleastWin10()
{
	// This example application does not have a manifest with SupportedOS elements
	// so we cannot use GetVersion[Ex] nor VerifyVersionInfo to detect Windows 10.
	BYTE verdata[200];
	UINT cb;
	VS_FIXEDFILEINFO*pFFI;
	if (GetFileVersionInfo(TEXT("KERNEL32"), 0, sizeof(verdata), verdata))
		if (VerQueryValue(verdata, TEXT("\\"), (void**) &pFFI, &cb))
			return cb >= sizeof(*pFFI) && HIWORD(pFFI->dwProductVersionMS) >= 10;
	return false;
}

enum { IDC_TEXTDATA = 500, IDC_QUIT, IDC_SHOWSHAREUI };

static IWRTIShareContractSource*g_pShare = 0;

static HRESULT SetBasicShareSourceProperties(Interop::WRT::IDataPackage&DP, LPCWSTR Title, LPCWSTR Description)
{
	Interop::WRT::IDataPackagePropertySet*pDPPS;
	HRESULT hr = DP.get_Properties(&pDPPS);
	if (SUCCEEDED(hr))
	{
		hr = pDPPS->put_Title(WRTString(Title).Get());
		if (Description) pDPPS->put_Description(WRTString(Description).Get());
		pDPPS->Release();
	}
	return hr;
}

static HRESULT CALLBACK OnProvideShareData(void*CallerData, void*Sender, Interop::WRT::IDataRequestedEventArgs*Event)
{
	UNREFERENCED_PARAMETER(Sender);
	using namespace Interop::WRT;

	IDataRequest*pDR;
	HRESULT hr = Event->get_Request(&pDR);
	if (SUCCEEDED(hr))
	{
		IDataPackage*pDP;
		hr = pDR->get_Data(&pDP);
		if (SUCCEEDED(hr))
		{
			WCHAR titbuf[50];
			static UINT itemid = 0;
			wsprintfW(titbuf, L"The Title of item #%u", ++itemid);
			hr = SetBasicShareSourceProperties(*pDP, titbuf, L"This is the description of our data");

			if (SUCCEEDED(hr))
			{
				HWND hEdit = (HWND) CallerData;
				WCHAR buf[300];
				GetWindowTextW(hEdit, buf, ARRAYSIZE(buf));
				hr = pDP->SetText(WRTString(buf).Get());
			}
			pDP->Release();
		}
		pDR->Release();
	}
	return hr;
}

static int OnCreate(HWND hwnd)
{
	LPCTSTR initialText = TEXT("This is the text data that we are sharing");
	HWND hCtl = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, initialText, WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, (HMENU) IDC_TEXTDATA, 0, 0);
	if (!hCtl) return -1;

	// IDataTransferManagerInterop exists on Windows 8 but it can only 
	// be (successfully) used by the default Metro/WinRT web browser.
	// The ShowShareUIForWindow method will return S_OK but fails to perform the 
	// share operation in normal desktop applications!
	const bool canShare = WinVer_AtleastWin10();

	HMENU hMenu = CreateMenu();
	if (!hMenu) return -1;
	AppendMenu(hMenu, canShare ? 0 : MF_GRAYED, IDC_SHOWSHAREUI, TEXT("Show Share UI"));
	AppendMenu(hMenu, MF_HELP, IDC_QUIT, TEXT("Quit"));
	SetMenu(hwnd, hMenu), DrawMenuBar(hwnd);

	if (!canShare)
	{
		MessageBox(0, TEXT("Sharing is only supported on Windows 10 and later..."), 0, MB_APPLMODAL|MB_ICONERROR);
		return -1;
	}
	return 0;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	const UINT ctlpad = 8; // Note: A real application should use DPI scaled system metrics
	switch (msg)
	{
	case WM_SIZE:
		SetWindowPos(GetDlgItem(hwnd, IDC_TEXTDATA), 0, ctlpad, ctlpad, LOWORD(lp) - ctlpad*2, HIWORD(lp) - ctlpad*2, SWP_NOZORDER|SWP_NOACTIVATE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case IDC_QUIT:
			return SNDMSG(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		case IDC_SHOWSHAREUI:
			{
				IWRTIShareContractSource*pShare;
				HRESULT hr = ShareContractSource_CreateInstance(pShare);
				if (SUCCEEDED(hr))
				{
					if (g_pShare) g_pShare->Release();
					g_pShare = pShare;
					hr = g_pShare->ShowShareUI(hwnd, OnProvideShareData, (void*) GetDlgItem(hwnd, IDC_TEXTDATA));
				}
			}
			break;
		}
		break;
	case WM_DESTROY:
		if (g_pShare) g_pShare->Release(), g_pShare = 0;
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		return OnCreate(hwnd);
	case WM_CLOSE:
		return DestroyWindow(hwnd);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

static int App(HINSTANCE hInst)
{
	WNDCLASS wc;
	wc.style = wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.hIcon = 0, wc.lpszMenuName = 0;
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszClassName = TEXT("ShareSourceTest");
	wc.hInstance = hInst;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) return GetLastError();

	HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Share Source Test"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 300, 150, NULL, NULL, hInst, NULL);
	if (!hwnd) return GetLastError();
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

#ifndef _VC_NODEFAULTLIB // Defined in VS2005+ when compiling with /Zl
int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstOld, LPSTR CommandLine, int ShowMode)
{
	UNREFERENCED_PARAMETER(hInstOld);
	UNREFERENCED_PARAMETER(CommandLine);
	UNREFERENCED_PARAMETER(ShowMode);
	return App(hInst);
}
#else
EXTERN_C __declspec(noreturn) void APIENTRY WinMainCRTStartup()
{
	ExitProcess(App(GetModuleHandle(0)));
}
EXTERN_C int __cdecl memcmp(const void*a, const void*b, size_t cb);
#pragma function(memcmp)
EXTERN_C int __cdecl memcmp(const void*a, const void*b, size_t cb) { int cmp = 0; for (size_t i = 0; i < cb; ++i) if (0 != (cmp = ((INT8*)a)[i] - ((INT8*)b)[i])) break; return cmp; }
#ifdef __cplusplus
#include <new>
struct std::nothrow_t const std::nothrow;
void*__cdecl operator new(std::size_t cb, struct std::nothrow_t const &) { return LocalAlloc(LPTR, cb); }
void __cdecl operator delete(void*p) { LocalFree((HLOCAL) p); }
#if defined(_MSC_VER) && _MSC_VER <= 1400
EXTERN_C int __cdecl _purecall(void) { return 0; }
#endif
#endif
#endif
