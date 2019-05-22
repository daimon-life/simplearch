scombase::CUnknown* CALLBACK CreateFoneEngineInstance(LPUNKNOWN pUnkOuter, HRESULT *phr);

scombase::CUnknown* CALLBACK CreateFoneEngineInstance(LPUNKNOWN pUnkOuter, HRESULT *phr)
{
	CNLEFontEngine* pFontEngine = CNLEFontEngine::GetInstance();
	NLE_ASSERT(phr);
	if (pFontEngine)
	{
		scombase::CUnknown *pNewObj = (scombase::CUnknown*)pFontEngine;
		pNewObj->NonDelegatingAddRef();
		*phr = S_OK;
		return pNewObj;
	}
	else
	{
		*phr = E_OUTOFMEMORY;
		return NULL;
	}
}
