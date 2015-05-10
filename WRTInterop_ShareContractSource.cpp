#include "WRTInterop_ShareContractSource.h"
#include "WRTInterop.h"
#include <GuidDef.h> // For IsEqualIID
#include <new>

namespace {

MIDL_INTERFACE("A5CAEE9B-8708-49D1-8D36-67D25A8DA00C") IDataTransferManager : public Interop::WRT::IInspectable {
	virtual HRESULT STDMETHODCALLTYPE add_DataRequested(IUnknown *eventHandler,Interop::WRT::EventRegistrationToken *eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE remove_DataRequested(Interop::WRT::EventRegistrationToken eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE add_TargetApplicationChosen(void *eventHandler, Interop::WRT::EventRegistrationToken *eventCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE remove_TargetApplicationChosen(Interop::WRT::EventRegistrationToken eventCookie) = 0;
};
#define IID_IDataTransferManager __uuidof(IDataTransferManager) //extern const __declspec(selectany) IID & IID_IDataTransferManager = __uuidof(IDataTransferManager);

MIDL_INTERFACE("3A3DCD6C-3EAB-43DC-BCDE-45671CE800C8") IDataTransferManagerInterop : public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE GetForWindow(HWND appWindow,REFIID riid,void**dataTransferManager) = 0;
	virtual HRESULT STDMETHODCALLTYPE ShowShareUIForWindow(HWND appWindow) = 0;
};
#define IID_IDataTransferManagerInterop __uuidof(IDataTransferManagerInterop)

}


template<class P1, class P2>
struct DECLSPEC_NOVTABLE IWRTIEventDelegate : public IUnknown { // ITypedEventHandler:{EC6F9CC8-46D0-5E0E-B4D2-7D7773AE37A0} AKA __FITypedEventHandler_2_Windows__CApplicationModel__CDataTransfer__CDataTransferManager_Windows__CApplicationModel__CDataTransfer__CDataRequestedEventArgs
	STDMETHOD(Invoke)(THIS_ P1, P2)PURE;
};

class ShareContractSource : public IWRTIShareContractSource, public IWRTIEventDelegate<void*, Interop::WRT::IDataRequestedEventArgs*>
{
	void UnregisterDataproviderAndTransfermanager()
	{
		if (m_pDTM)
		{
			m_pDTM->remove_DataRequested(m_ERT);
			m_pDTM->Release();
			m_pDTM = 0;
		}
	}

public:
	IFACEMETHODIMP QueryInterface(REFIID riid, void**ppv)
	{
		if (!ppv) return E_POINTER; else *ppv = 0;
		if (IsEqualIID(riid, IID_IUnknown)) *ppv = this;
		return *ppv ? (((IUnknown*) *ppv)->AddRef(), S_OK) : E_NOINTERFACE;
	}
	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement((volatile LONG*) &m_RefCount);
	}
	IFACEMETHODIMP_(ULONG) Release()
	{ 
		ULONG c = InterlockedDecrement((volatile LONG*) &m_RefCount); 
		if (!c) delete this;
		return c; 
	}

	IFACEMETHODIMP ShowShareUI(HWND hwnd, SHARECONTRACTSOURCE_PROVIDEDATACALLBACK Callback, void*CallerData)
	{
		if (!Callback) return E_INVALIDARG;
		HRESULT hr = m_ROInit ? S_OK : Interop::WRT::RoInitialize(Interop::WRT::RO_INIT_SINGLETHREADED);
		m_ROInit = SUCCEEDED(hr);
		if (m_ROInit)
		{
			PCWSTR actclsid = L"Windows.ApplicationModel.DataTransfer.DataTransferManager";
			Interop::WRT::HSTRING hs;
			hr = Interop::WRT::Util::CreateString(actclsid, &hs);
			if (SUCCEEDED(hr))
			{
				IDataTransferManagerInterop*pDTMI;
				hr = Interop::WRT::RoGetActivationFactory(hs, IID_PPV_ARG(IDataTransferManagerInterop, &pDTMI));
				Interop::WRT::WindowsDeleteString(hs);
				if (!hr)
				{
					IDataTransferManager*pDTM;
					hr = pDTMI->GetForWindow(hwnd, IID_PPV_ARG(IDataTransferManager, &pDTM));
					if (SUCCEEDED(hr))
					{
						UnregisterDataproviderAndTransfermanager(); // Must remove previous registration before we overwrite our EventRegistrationToken

						m_Callback = Callback, m_CallerData = CallerData;
						IUnknown*pED = static_cast<IWRTIEventDelegate*>(this);
						hr = pDTM->add_DataRequested(pED, &m_ERT);
						if (SUCCEEDED(hr))
						{
							m_pDTM = pDTM, pDTM = 0;
							hr = pDTMI->ShowShareUIForWindow(hwnd); // ShowShareUIForWindow is asynchronous!
						}
						if (pDTM) pDTM->Release();
					}
					pDTMI->Release();
				}
			}
		}
		return hr;
	}

	IFACEMETHODIMP Invoke(void*Sender, Interop::WRT::IDataRequestedEventArgs*Event)
	{
		HRESULT hr = m_Callback(m_CallerData, Sender, Event);
		UnregisterDataproviderAndTransfermanager();
		return hr;
	}

	ShareContractSource() : m_pDTM(0), m_RefCount(1), m_ROInit(false) {}
	~ShareContractSource()
	{
		UnregisterDataproviderAndTransfermanager();
		if (m_ROInit) Interop::WRT::RoUninitialize();
	}

protected:
	SHARECONTRACTSOURCE_PROVIDEDATACALLBACK m_Callback;
	void*m_CallerData;
	IDataTransferManager*m_pDTM;
	Interop::WRT::EventRegistrationToken m_ERT;
	ULONG m_RefCount;
	bool m_ROInit;

public:
	static HRESULT CreateInstance(IWRTIShareContractSource*&pSCS)
	{
		pSCS = new (std::nothrow) ShareContractSource;
		return pSCS ? S_OK : E_OUTOFMEMORY;
	}
};

EXTERN_C HRESULT WINAPI ShareContractSource_CreateInstance(IWRTIShareContractSource*&pSCS)
{
	return ShareContractSource::CreateInstance(pSCS);
}
