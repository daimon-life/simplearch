#ifndef __CFactory_h__
#define __CFactory_h__

#include "scombase.h"
///////////////////////////////////////////////////////////

// Forward reference
class CFactoryTemplate ;

// Global data used by CFactory
extern CFactoryTemplate g_Templates[] ;
extern int g_cTemplates ;

//////////////////////////////////////////////////////////
//
//  Component creation function
//
#ifdef WIN32
class scombase::CUnknown ;
#endif
/* A function that can create a new COM object */

typedef scombase::CUnknown *(CALLBACK *FPCREATEINSTANCE)( LPUNKNOWN pUnkOuter, HRESULT *phr);

/*  A function (can be NULL) which is called from the DLL entrypoint
routine for each factory template:

bLoading - TRUE on DLL load, FALSE on DLL unload
rclsid   - the m_ClsID of the entry
*/
typedef void (CALLBACK *LPFNInitRoutine)(BOOL bLoading, const CLSID *rclsid);

/* Create one of these per object class in an array so that
the default class factory code can create new instances */

///////////////////////////////////////////////////////////
//
// CFactoryData
//   - Information CFactory needs to create a component
//     supported by the DLL
//
class CFactoryTemplate {

public:

	const WCHAR *              m_Name;
	const CLSID *              m_ClsID;
	FPCREATEINSTANCE           m_CreateInstance;

	BOOL IsClassID(REFCLSID rclsid) const {
		return (IsEqualCLSID(*m_ClsID,rclsid));
	};

	scombase::CUnknown *CreateInstance( LPUNKNOWN pUnk, HRESULT *phr) const {
		CheckPointer(phr,NULL);
		return m_CreateInstance(pUnk, phr);
	};
};


///////////////////////////////////////////////////////////
//
// Class Factory
//
class CFactory : public IClassFactory
{
public:
	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) ;
	virtual ULONG   __stdcall AddRef() ;
	virtual ULONG   __stdcall Release() ;
	
	// IClassFactory
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter,
	                                         const IID& iid,
	                                         void** ppv) ;
	virtual HRESULT __stdcall LockServer( int bLock) ; 

	// Constructor - Pass pointer to data of component to create.
	CFactory(const CFactoryTemplate* pFactoryTemplate) ;

	// Destructor
	~CFactory() {}

	//
	// Static FactoryData support functions
	//

	// DllGetClassObject support
	static HRESULT GetClassObject(const CLSID& clsid,
	                              const IID& iid, 
	                              void** ppv) ;

	// Helper function for DllCanUnloadNow 
	static BOOL IsLocked()
		{ return (s_cServerLocks > 0) ;}

	// Functions to [un]register all components
	static HRESULT RegisterAll() ;
	static HRESULT UnregisterAll() ;

	// Function to determine if component can be unloaded
	static HRESULT CanUnloadNow() ;

public:
	// Reference Count
	LONG m_cRef ;

	// Pointer to information about class this factory creates
	const CFactoryTemplate* m_pFactoryTemplate;
      
	// Count of locks
	static LONG s_cServerLocks ;   

	// Module handle
	static HANDLE s_hModule ;
} ;

#endif
