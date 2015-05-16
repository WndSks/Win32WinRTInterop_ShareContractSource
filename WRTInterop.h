#pragma once
#include <Windows.h>
#include <Objbase.h>


// We want to support older/3rd-party SDKs and compilers:
#ifndef DECLSPEC_NOVTABLE
#	define DECLSPEC_NOVTABLE
#endif
#ifndef ARRAYSIZE
#	define ARRAYSIZE(___A) ( sizeof(___A)/sizeof((___A)[0]) )
#endif
#ifndef IID_PPV_ARG
#	define IID_PPV_ARG(IT,ppT) (IID_##IT),reinterpret_cast<void**>(static_cast<IT**>(ppT))
#endif
#ifndef IFACEMETHODIMP
#	define IFACEMETHODIMP_ STDMETHODIMP_
#	define IFACEMETHODIMP STDMETHODIMP
#endif


namespace Interop { namespace WRT {

typedef unsigned char boolean; // RpcNdr.h

DECLARE_HANDLE(HSTRING);
EXTERN_C HRESULT WINAPI WindowsCreateString(PCWSTR s, UINT len, HSTRING*p);
EXTERN_C HRESULT WINAPI WindowsDeleteString(HSTRING hs);

typedef enum RO_INIT_TYPE { RO_INIT_SINGLETHREADED = 0, RO_INIT_MULTITHREADED = 1 } RO_INIT_TYPE;
EXTERN_C HRESULT WINAPI RoInitialize(RO_INIT_TYPE mode);
EXTERN_C void WINAPI RoUninitialize();
EXTERN_C HRESULT WINAPI RoGetActivationFactory(HSTRING a, REFIID b, void**c);

typedef enum TrustLevel { BaseTrust, PartialTrust, FullTrust } TrustLevel;
struct DECLSPEC_NOVTABLE IInspectable : public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE GetIids(ULONG *iidCount, IID **iids) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING *className) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel *trustLevel) = 0;
};

typedef UINT64 EventRegistrationToken;

typedef struct DateTime { INT64 UniversalTime; } DateTime; // ABI::Windows::Foundation
typedef IUnknown IRandomAccessStreamReference; // Dummy
typedef IUnknown IDataRequestDeferral; // Dummy
typedef IUnknown IDataPackageView; // Dummy
typedef IUnknown IDataProviderHandler; // Dummy
typedef IUnknown IUriRuntimeClass; // Dummy

struct DECLSPEC_NOVTABLE IDataPackagePropertySet : public IInspectable {
	virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Title(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(HSTRING *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Thumbnail(IRandomAccessStreamReference **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Thumbnail(IRandomAccessStreamReference *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_FileTypes(LPVOID **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationName(HSTRING *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationName(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationListingUri(IUriRuntimeClass **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationListingUri(IUriRuntimeClass *value) = 0;
};

typedef enum { DataPackageOperation_None = 0, DataPackageOperation_Copy = 1, DataPackageOperation_Move = 2, DataPackageOperation_Link = 4 } DataPackageOperation;

struct DECLSPEC_NOVTABLE IDataPackage : public IInspectable {
	virtual HRESULT STDMETHODCALLTYPE GetView(IDataPackageView **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Properties(IDataPackagePropertySet **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RequestedOperation(DataPackageOperation *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_RequestedOperation(DataPackageOperation value) = 0;
	virtual HRESULT STDMETHODCALLTYPE add_OperationCompleted(LPVOID*handler, EventRegistrationToken *eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE remove_OperationCompleted(EventRegistrationToken eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE add_Destroyed(LPVOID *handler,EventRegistrationToken *eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE remove_Destroyed(EventRegistrationToken eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetData(HSTRING formatId, IInspectable *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetDataProvider(HSTRING formatId, IDataProviderHandler *delayRenderer) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetText(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetUri(IUriRuntimeClass *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetHtmlFormat(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResourceMap(LPVOID **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetRtf(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetBitmap(IRandomAccessStreamReference *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetStorageItemsReadOnly(LPVOID *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetStorageItems(LPVOID *value, boolean readOnly) = 0;
};
struct DECLSPEC_NOVTABLE IDataRequest : public IInspectable {
	virtual HRESULT STDMETHODCALLTYPE get_Data(IDataPackage **value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Data(IDataPackage *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Deadline(DateTime *value) = 0;
	virtual HRESULT STDMETHODCALLTYPE FailWithDisplayText(HSTRING value) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDeferral(IDataRequestDeferral **value) = 0;
};
struct DECLSPEC_NOVTABLE IDataRequestedEventArgs : public IInspectable {
	virtual HRESULT STDMETHODCALLTYPE get_Request(IDataRequest **value) = 0;
};

namespace Util {
	EXTERN_C HRESULT WINAPI CreateString(PCWSTR Str, HSTRING*pHS);
}

} }
