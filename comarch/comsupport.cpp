typedef struct COMInfo
{
	CLSID clsid; 
	HMODULE hDllModule; 
	std::wstring dllName;
}COMInfo;

static std::vector<COMInfo> g_comInfoVec;

static std::wstring g_dllProfileName(L"comsupport.xml");

static HRESULT ParseComProfile(std::vector<COMInfo> &comInfoVec, const WCHAR *pXMLFilePath)
{	
	HRESULT hr = S_OK;
	CMarkup xmlParser;
#ifdef WIN32
    bool bSuccess = xmlParser.Load(pXMLFilePath);
#else
    char szFileName[MAX_PATH];
    NLECommon::WChar2Char(pXMLFilePath, szFileName, MAX_PATH);
    
	bool bSuccess = xmlParser.Load(szFileName);
#endif
	if (!bSuccess)
	{
		goto end_position;		
	}

	bSuccess = xmlParser.FindElem(TAG_ROOT);
	if (!bSuccess)
	{
		goto end_position;
	}
	bSuccess = xmlParser.IntoElem();
	while (1)
	{
		bSuccess = xmlParser.FindElem();
		if (!bSuccess)
		{
			//arrive the last node
			break;
		}

		COMInfo comNode;
		comNode.hDllModule = NULL;
		std::wstring CLSIDStr = xmlParser.GetAttrib(L"id");
		comNode.dllName = xmlParser.GetAttrib(L"name");
		hr = CLSIDFromString(CLSIDStr.c_str(), &comNode.clsid); CHECK(hr);
		if (FAILED(hr))
		{
			goto end_position;
		}	
		g_comInfoVec.push_back(comNode);
	}
	bSuccess = xmlParser.OutOfElem();

end_position:
	if (!bSuccess || FAILED(hr))
	{
		hr = E_FAIL;
		LOGGER_ERROR_(L"can't open nlecomsupport_config.xml file or the file is not complete");
	}

	return hr;
}


NLECOMMON_API HMODULE NLECommon::NLELoadDllByName(const wchar_t *dllName)
{
	wchar_t dllPath[1024] = { 0 };
	GetDllPath(dllPath, dllName);

#ifdef WIN32
	HMODULE hModule = LoadLibraryW(dllPath);
#else
	HMODULE hModule = dlopen(pName, RTLD_NOW);
#endif

	if (NULL == hModule)
	{
		LOGGER_ERROR_(L"load library failed[%ls]\n", dllPath);
	}

	return hModule;
}
