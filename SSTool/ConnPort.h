#pragma once

class ConnPort
{
public:
	ConnPort(void);
	~ConnPort(void);
	BOOL ClosePort();
	BOOL OpenPort(TCHAR *szPort);

protected:
	TCHAR m_WrriteBuffer[512];

	HANDLE m_hPort;
	HANDLE m_hThreadRead;
	HANDLE m_hThreadWrite;
	HANDLE m_hDataParse;
	CRITICAL_SECTION m_csRead;
	CRITICAL_SECTION m_csWrite;

	BOOL   ConfigPort();
	BOOL   CommTimeouts();
	void   SendComData(char *szRevData);
	static DWORD  ReadThreadProc(LPVOID lpParameter);
	static DWORD  WriteThreadProc(LPVOID lpParameter);
	static DWORD  PareDataProc(LPVOID lpParameter);
};

