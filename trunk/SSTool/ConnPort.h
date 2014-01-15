#pragma once
typedef unsigned int Uint;

class ConnPort
{
public:
	ConnPort(void);
	~ConnPort(void);
	BOOL ClosePort();
	int  OpenPort(TCHAR *szPort,int iBaudrate,int iParity,int iDataBits,int iStopBits);
	BOOL WriteByte(TCHAR szChar);
	BOOL WriteString(TCHAR *szWriteData,int iLen);
	void SetHexShow(BOOL bHex);
	void SetHexSend(BOOL bHex);
	BOOL GetHexShowEnable();
	BOOL GetHexSendEnable();
	BOOL IsConnect();
	int  GetConnReadBytes();
	int  GetConnWriteBytes();
	void EmptyBytesCount();
	void ResetComStatues();
	void ComEnableTimeShow(BOOL bEnable);
protected:
	TCHAR	m_WrriteBuffer;
	BOOL	m_bHexShow;
	BOOL	m_bTimeShow;
	HANDLE	m_hPort;
	HANDLE	m_hThreadRead;
	HANDLE	m_hThreadWrite;
	HANDLE	m_hDataParse;

	HANDLE	m_hEventParse;
	HANDLE	m_hRead;
	HANDLE	m_hRetOK;
	HANDLE	m_hThreadSignal;
	BOOL	m_bIsConnect;
	BOOL	m_bHexSend;
	int		m_rCount;
	int     m_wCount;

	Uint   Char2Hex(char *Buffer,char *szOut,int iLen);
	Uint   Hex2wChar(TCHAR *Buffer,TCHAR *szOut,int *iLen);
	BOOL   ConfigPort(int iBaudrate,int iParity,int iDataBits,int iStopBits);
	BOOL   CommTimeouts();
	void   SendComData(char *szRevData,int iLen);
	static DWORD  ReadThreadProc(LPVOID lpParameter);
	static DWORD  WriteThreadProc(LPVOID lpParameter);
	static DWORD  PareDataProc(LPVOID lpParameter);
	static DWORD  WaitSignalProc(LPVOID lpParameter);
};

