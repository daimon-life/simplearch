CFactoryTemplate g_Templates[] =
{
	{ 
		L"Create NLEFontLibrary", 
		&CLSID_NLEFontLibrary, 
		CreateFoneEngineInstance, 
	},
};

int g_cTemplates
= sizeof(g_Templates) / sizeof(CFactoryTemplate);

