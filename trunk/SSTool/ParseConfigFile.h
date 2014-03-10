#pragma once

#define SSToolConfigFile  L"SSToolConfig.ini"

class ParseConfigFile
{
public:
	ParseConfigFile(void);
	~ParseConfigFile(void);
	BOOL GetSSToolSetting(CString strKeyWord,int *nResult);
	void WriteSSToolSetting(CString strKeyWord,int nValue);
	void WriteSSToolSettingByString(CString strKeyWord,CString strValue);
private:
	CString m_strConfigFile;
};

