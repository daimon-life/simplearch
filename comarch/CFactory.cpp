///////////////////////////////////////////////////////////
//
// CFactory
//   - Base class for reusing a single class factory for
//     all components in a DLL
//
// #include "stdafx.h"

#include "objbase.hpp"
#include "scombase.h"
#include "CFactory.h"




#ifdef WIN32
#include "COMRegistry.cpp"
#endif 
///////////////////////////////////////////////////////////
//
// Static variables
//
LONG CFactory::s_cServerLocks = 0 ;    // Count of locks

HANDLE CFactory::s_hModule = NULL ;   // DLL module handle

///////////////////////////////////////////////////////////
//
// CFactory implementation
//
CFactory::CFactory(const CFactoryTemplate* pFactoryTemplate)
: m_cRef(1)
{
	m_pFactoryTemplate = pFactoryTemplate;
}

//
// IUnknown implementation
//
HRESULT __stdcall CFactory::QueryInterface(REFIID iid, void** ppv)
{ 	
	IUnknown* pI ;
	if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
	{
		pI= this ;
	}
	else
	{
		*ppv = NULL ;
		return E_NOINTERFACE ;
	}
	pI->AddRef() ;
	*ppv = pI ;
	return S_OK ;
}

ULONG __stdcall CFactory::AddRef()
{
	return ::InterlockedIncrement(&m_cRef) ;
}

ULONG __stdcall CFactory::Release()
{
	if (::InterlockedDecrement(&m_cRef) == 0)
	{
		delete this ;
		return 0 ;
	}
	return m_cRef ;
}

//
// IClassFactory implementation
//
HRESULT __stdcall CFactory::CreateInstance(IUnknown* pUnknownOuter,
                                           const IID& iid,
                                           void** ppv)
{
	// Aggregate only if the requested IID is IID_IUnknown.
	if ((pUnknownOuter != NULL) && (iid != IID_IUnknown))
	{
		return CLASS_E_NOAGGREGATION ;
	}

	// Create the component.
	
	HRESULT hr;
	scombase::CUnknown* pNewComponent = m_pFactoryTemplate->CreateInstance(pUnknownOuter, &hr) ;

	if (FAILED(hr))
	{
		return hr ;
	}
/*
	// Initialize the component.
	hr = pNewComponent->Init() ;
	if (FAILED(hr))
	{
		// Initialization failed.  Release the component.
		pNewComponent->NondelegatingRelease() ;
		return hr ;
	}
*/
	// Get the requested interface.
	hr = pNewComponent->NonDelegatingQueryInterface(iid, ppv) ;

	// Release the reference held by the class factory.
	pNewComponent->NonDelegatingRelease() ;
	return hr ;
}

// LockServer
HRESULT __stdcall CFactory::LockServer( int bLock)
{
	if (bLock) 
	{
		::InterlockedIncrement(&s_cServerLocks) ;
	}
	else
	{
		::InterlockedDecrement(&s_cServerLocks) ;
	}
	return S_OK ;
}

///////////////////////////////////////////////////////////
//
// GetClassObject
//   - Create a class factory based on a CLSID.
//
HRESULT CFactory::GetClassObject(const CLSID& clsid,
                                 const IID& iid,
                                 void** ppv)
{
	if ((iid != IID_IUnknown) && (iid != IID_IClassFactory))
	{
		return E_NOINTERFACE ;
	}

	// Traverse the array of data looking for this class ID.
	for (int i = 0; i < g_cTemplates; i++)
	{
		const CFactoryTemplate* pFactoryTemplate = &g_Templates[i] ;
		if (pFactoryTemplate->IsClassID(clsid))
		{
			// Found the ClassID in the array of components we can
			// create.  So create a class factory for this component.
			// Pass the CFactoryData structure to the class factory
			// so that it knows what kind of components to create.
			*ppv = (IUnknown*) new CFactory(pFactoryTemplate) ;
			if (*ppv == NULL)
			{
				return E_OUTOFMEMORY ;
			}
			return NOERROR ;
		}
	}
	return CLASS_E_CLASSNOTAVAILABLE ;
}
//#include <atlcom.h>
//
// Register all components.
//
HRESULT CFactory::RegisterAll()
{
	
#ifdef WIN32
	if (NULL == s_hModule)
	{
		return E_FAIL;
	}
	WCHAR szModule[1024];
	DWORD dwResult = ::GetModuleFileName((HMODULE)s_hModule, szModule, 1024);
	
	
	if (dwResult == 0)
	{
		return E_FAIL;
	}

	for(int i = 0 ; i < g_cTemplates ; i++)
	{
		COMRegisterServer(
			*g_Templates[i].m_ClsID,
			L"NLECommon Component",
			szModule);
	}
	
#endif
	return S_OK ;
}

HRESULT CFactory::UnregisterAll()
{
#ifdef WIN32
	for (int i = 0; i < g_cTemplates; i++)
	{
		return COMUnregisterServer(*g_Templates[i].m_ClsID);
	}
#endif

	return S_OK ;
}

//
// Determine if the component can be unloaded.
//
HRESULT CFactory::CanUnloadNow()
{
/*
	if (CUnknown::ActiveComponents() || IsLocked())
	{
		return S_FALSE ;
	}
	else
	{
		return S_OK ;
	}
*/
	return S_OK;
}

//////////////////////////////////////////////////////////
//
// Exported functions
//

STDAPI DllCanUnloadNow()
{

	return CFactory::CanUnloadNow() ;
}

//
// Get class factory
//
STDAPI DllGetClassObject(const CLSID& clsid,
                         const IID& iid,
                         void** ppv)
{
	return CFactory::GetClassObject(clsid, iid, ppv) ;
}

//
// Server registration
//
STDAPI DllRegisterServer()
{
	printf("222222222222222");
	return CFactory::RegisterAll() ;
}


STDAPI DllUnregisterServer()
{
	return CFactory::UnregisterAll() ;
}

