#pragma once
#include "WRTInterop.h"

/*

IWRTIShareContract is a helper object that abstracts the details of starting a WinRT share operation.
When a share operation is started the callback function must use the IDataRequestedEventArgs parameter to add the data and properties of the thing you are trying to share.
The ShowShareUI method is asynchronous so the object should not be released until after the callback has been called.

*/

typedef HRESULT(CALLBACK*SHARECONTRACTSOURCE_PROVIDEDATACALLBACK)(void*CallerData, void*Sender, Interop::WRT::IDataRequestedEventArgs*Event);

struct DECLSPEC_NOVTABLE IWRTIShareContractSource : public IUnknown {
	STDMETHOD(ShowShareUI)(THIS_ HWND hwnd, SHARECONTRACTSOURCE_PROVIDEDATACALLBACK Callback, void*CallerData)PURE; // Only STA is allowed if COM is initialized on the calling thread
};

EXTERN_C HRESULT WINAPI ShareContractSource_CreateInstance(IWRTIShareContractSource*&pSCS);
