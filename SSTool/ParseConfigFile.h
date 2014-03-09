#pragma once

#define SSToolConfigFile  L"SSToolConfig.ini"

class ParseConfigFile
{
public:
	ParseConfigFile(void);
	~ParseConfigFile(void);
	BOOL GetSSToolSetting(CString strParseType,int *nResult);
};

