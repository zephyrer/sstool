#include "StdAfx.h"
#include "ConnPort.h"
#include "SSToolDlg.h"
#include "string.h"

#define     MAX_BUFFER_SIZE   512

volatile	int			g_iExitFlag=0;
static		int			g_wExitFlag=0;
static		int			g_rExitFlag=0;
static		int			g_pExitFlag=0;

static		BYTE		g_DataBuf[MAX_BUFFER_SIZE];
static      int			g_iInPos=0;
static		int			g_iOutPos=0;

ConnPort::ConnPort(void)
{
}

ConnPort::~ConnPort(void)
{

}
BOOL ConnPort::ConfigPort()
{
    DCB portDCB;
    portDCB.DCBlength=sizeof(DCB);
    GetCommState(m_hPort,&portDCB);
    //设置DCB结构
    portDCB.BaudRate=115200;
    portDCB.ByteSize=8;
    portDCB.fBinary=TRUE;
    portDCB.fDsrSensitivity=FALSE;
    portDCB.fAbortOnError=FALSE;
    portDCB.fDtrControl=DTR_CONTROL_DISABLE;
    portDCB.fErrorChar=FALSE;
    portDCB.ErrorChar=FALSE;
    portDCB.fInX=FALSE;
    portDCB.fNull=FALSE;
    portDCB.fOutX=FALSE;
    portDCB.fOutxCtsFlow=FALSE;
    portDCB.fOutxDsrFlow=FALSE;
    portDCB.fParity=FALSE;
    portDCB.fRtsControl=RTS_CONTROL_DISABLE;
    portDCB.fTXContinueOnXoff=FALSE;
    portDCB.Parity=NOPARITY;//无奇偶校验
    portDCB.StopBits=ONESTOPBIT;
    
    if(!SetCommState(m_hPort,&portDCB))
    {
        return FALSE;
    }
    else 
    {
        return TRUE;
    }
}

BOOL ConnPort::CommTimeouts()
{ 
    COMMTIMEOUTS commTimeOut;
    
    GetCommTimeouts(m_hPort,&commTimeOut);
    
    commTimeOut.ReadIntervalTimeout=MAXWORD;
    commTimeOut.ReadTotalTimeoutConstant=0;
    commTimeOut.ReadTotalTimeoutMultiplier=0;
    commTimeOut.WriteTotalTimeoutConstant=2000;
    commTimeOut.WriteTotalTimeoutMultiplier=50;

    if(!SetCommTimeouts(m_hPort,&commTimeOut)) 
    {
        return FALSE;
    }
    return TRUE;
    
}
BOOL ConnPort::ClosePort()
{
	g_iExitFlag=1;
	
	while(1)
	{
		static int iTimeoutCount=0;
		Sleep(30);	
		if(50==iTimeoutCount)
		{
			TerminateThread(m_hThreadRead,0);
			TerminateThread(m_hThreadWrite,0);
			TerminateThread(m_hDataParse,0);
			break;
		}
		else if(1==g_rExitFlag && 1==g_wExitFlag && 1==g_pExitFlag)
			break;
		else
			iTimeoutCount++;	
	}
    EscapeCommFunction(m_hPort,CLRRTS);
    EscapeCommFunction(m_hPort,CLRDTR);
    DeleteCriticalSection(&m_csRead);
    DeleteCriticalSection(&m_csWrite);
    SetCommMask(m_hPort,0);
	
	if(NULL==m_hPort)return FALSE;
	if(NULL==m_hThreadRead)return FALSE;
	if(NULL==m_hThreadWrite)return FALSE;
	if(NULL==m_hDataParse)return FALSE;

    if(!CloseHandle(m_hPort))return FALSE;
    if(!CloseHandle(m_hThreadRead))return FALSE;
    if(!CloseHandle(m_hThreadWrite))return FALSE;
	if(!CloseHandle(m_hDataParse))return FALSE;

    return TRUE;
}

BOOL ConnPort::OpenPort(TCHAR *szPort)
{
	 DWORD dwThreadID=0;
	 g_iExitFlag=0;
	 m_hPort=NULL;
	
	 m_hPort=CreateFile(szPort,
						GENERIC_READ|GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						0/*FILE_FLAG_OVERLAPPED*/,
						NULL);
    if(!m_hPort) 
    {
        m_hPort=NULL;
        wprintf(TEXT("open port failed!!!\r\n"));
        return FALSE;
    }
   //指定端口检测的事件集
   SetCommMask(m_hPort,EV_RXCHAR);
   //设置缓冲区，内部输入、输出缓冲区大小
   SetupComm(m_hPort,512,512);
   //刷新缓冲区信息->输入、输出缓冲区
   PurgeComm(m_hPort,PURGE_TXCLEAR|PURGE_RXCLEAR);

   if(!ConfigPort()) return FALSE;
   if(!CommTimeouts())return FALSE;

   InitializeCriticalSection(&m_csRead);
   InitializeCriticalSection(&m_csWrite);
  //创建读写线程
   m_hThreadRead  = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadThreadProc, (void*)this,0,&dwThreadID);
   m_hThreadWrite = CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteThreadProc,(void*)this,0,&dwThreadID);
   m_hDataParse = CreateThread(0,0,(LPTHREAD_START_ROUTINE)PareDataProc,(void*)this,0,&dwThreadID);

   return TRUE;

}
DWORD ConnPort::ReadThreadProc(LPVOID p)
{
    COMSTAT		comStat;
    DWORD		dwErrFlag;
    DWORD		dwLength;
	DWORD		dwRead;
    DWORD		dwModemStat;
    ConnPort	*pThis=(ConnPort *)p;
	BYTE    RXBuffer;
  
    if(INVALID_HANDLE_VALUE==pThis->m_hPort)
    {
        AfxMessageBox(L"the port is not opened!");
    }
	memset(g_DataBuf,0,MAX_BUFFER_SIZE);
	memset(&comStat,0,sizeof(COMSTAT));
    while(1)
    {
		if(1==g_iExitFlag)
		{
			break;
		}
		if(1)
		{
				ClearCommError(pThis->m_hPort,&dwErrFlag,&comStat);
				dwLength=comStat.cbInQue;
				EnterCriticalSection(&pThis->m_csRead);
				while(dwLength>0)
				{
					//读数据
					if(g_iInPos==g_iOutPos) Sleep(20);
					BOOL bRet=ReadFile(pThis->m_hPort,&RXBuffer,1,&dwRead,NULL);
					if(!bRet||0==dwLength) 
					{
						continue;
					}
					g_DataBuf[g_iInPos]=RXBuffer;
					g_iInPos=((g_iInPos++)%MAX_BUFFER_SIZE);
					dwLength--;
				}
				LeaveCriticalSection(&pThis->m_csRead);
		}
	}
	g_rExitFlag=1;
    return 0;
}

DWORD ConnPort::PareDataProc(LPVOID p)
{
	int		iReadCount=0;
	static char	szTmp;
	char	szRev[MAX_BUFFER_SIZE];
	ConnPort *pThis=(ConnPort *)p;
	memset(szRev,0,MAX_BUFFER_SIZE);
	while(1)
	{
		if(1==g_iExitFlag) break;
		if(g_iOutPos==g_iInPos) Sleep(10);
		sprintf(&szTmp,"%c",g_DataBuf[g_iOutPos]);
		if(szTmp=='\n')
		{
			szRev[iReadCount++]='\n';
			pThis->SendComData(szRev);
			iReadCount=0;
			g_DataBuf[g_iOutPos]=0;
			g_iOutPos=((g_iOutPos++)%MAX_BUFFER_SIZE);
			memset(szRev,0,MAX_BUFFER_SIZE);
		}
		else if(szTmp!=0)
		{
			szRev[iReadCount++]=szTmp;
			g_DataBuf[g_iOutPos]=0;
			g_iOutPos=((g_iOutPos++)%MAX_BUFFER_SIZE);
		}	
	}
	g_pExitFlag=1;
	return 1;
}
DWORD ConnPort::WriteThreadProc( LPVOID p )
{

    DWORD dwWritten;
    ConnPort *pThis=(ConnPort *)p;

    while(TRUE)
    {
	 if(1==g_iExitFlag) break;
	 #if 0
     //WaitForSingleObject(g_hEvent[hWrite],INFINITE);
     EnterCriticalSection(&pThis->m_csWrite);
     if(INVALID_HANDLE_VALUE!=pThis->m_hPort)
     {
         SetCommMask(pThis->m_hPort,EV_RXCHAR);
         PurgeComm(pThis->m_hPort,PURGE_TXCLEAR|PURGE_RXCLEAR);
         WriteFile(pThis->m_hPort,pThis->m_WrriteBuffer,(strlen(pThis->m_WrriteBuffer)+1)*sizeof(TCHAR),&dwWritten,NULL);
     }
     LeaveCriticalSection(&pThis->m_csWrite);
	#endif
    }
  g_wExitFlag=1;
  return 0;
}
void ConnPort::SendComData(char *szRevData)
{
		CString         strMsg;
		TCHAR			m_revData[MAX_BUFFER_SIZE];
        CSSToolDlg		*dlg=(CSSToolDlg *)AfxGetApp()->GetMainWnd();
        memset(m_revData,0,MAX_BUFFER_SIZE);
		MultiByteToWideChar(CP_ACP,0,szRevData,-1,m_revData,MAX_BUFFER_SIZE);
		strMsg=m_revData;
        if(!strMsg.IsEmpty())
        {
            dlg->OutMsg(strMsg);
        }
}
