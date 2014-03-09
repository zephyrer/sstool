#include "StdAfx.h"
#include "ParseConfigFile.h"


ParseConfigFile::ParseConfigFile(void)
{
}


ParseConfigFile::~ParseConfigFile(void)
{
}

BOOL ParseConfigFile::GetSSToolSetting(CString strParseType,int *nResult)
{
	CString strConfigFile;
	strConfigFile.Empty();

   GetModuleFileName(NULL,strConfigFile.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
   strConfigFile.ReleaseBuffer();   
   int    nPos;   
   nPos=strConfigFile.ReverseFind('\\');   
   strConfigFile=strConfigFile.Left(nPos); 
   strConfigFile=strConfigFile+L"\\"+SSToolConfigFile;

   *nResult=::GetPrivateProfileInt(L"SSTool_Setting",strParseType,0,strConfigFile);

   return TRUE;
}