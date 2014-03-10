#include "StdAfx.h"
#include "ParseConfigFile.h"


ParseConfigFile::ParseConfigFile(void)
{
	int    nPos=0;  
	m_strConfigFile.Empty();

	GetModuleFileName(NULL,m_strConfigFile.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	m_strConfigFile.ReleaseBuffer();   
   
	nPos=m_strConfigFile.ReverseFind('\\');   
	m_strConfigFile=m_strConfigFile.Left(nPos); 
	m_strConfigFile=m_strConfigFile+L"\\"+SSToolConfigFile;
}


ParseConfigFile::~ParseConfigFile(void)
{
}

BOOL ParseConfigFile::GetSSToolSetting(CString strKeyWord,int *nResult)
{
	
   *nResult=::GetPrivateProfileInt(L"SSTool_Setting",strKeyWord,0,m_strConfigFile);

   return TRUE;
}
void ParseConfigFile::WriteSSToolSetting(CString strKeyWord,int nValue)
{
	CString strValue;
	strValue.Format(L"%d",nValue);
	::WritePrivateProfileString(L"SSTool_Setting",strKeyWord,strValue,m_strConfigFile); 
}
void ParseConfigFile::WriteSSToolSettingByString(CString strKeyWord,CString strValue)
{
	::WritePrivateProfileString(L"SSTool_Setting",strKeyWord,strValue,m_strConfigFile); 
}