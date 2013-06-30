#include "StdAfx.h"
#include "ConnPort.h"
#include "SSToolDlg.h"
#include "string.h"

#define     MAX_BUFFER_SIZE   512

volatile	int			g_iExitFlag=0;
static		int			g_bWriteProcExit=0;
static		int			g_bReadProcExit=0;
static		HANDLE		g_hReadExit;
static		HANDLE		g_hWriteExit;

static		BYTE		g_DataBuf[MAX_BUFFER_SIZE];
static      int			g_iInPos=0;
static		int			g_iOutPos=0;

ConnPort::ConnPort(void)
{
	m_hPort=NULL;
	InitializeCriticalSection(&m_csRead);
	InitializeCriticalSection(&m_csWrite);
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
	SetEvent(g_hReadExit);
	SetEvent(g_hWriteExit);
	//wait thread to exit..
#if 0
	while(1)
	{
		static int iTimeoutCount=0;
		if(1==g_bWriteProcExit &&1==g_bReadProcExit)
			break;
		/*
		if(30==iTimeoutCount)
		{
			TerminateThread(m_hThreadRead,0);
			TerminateThread(m_hThreadWrite,0);
			break;
		}
		else
			iTimeoutCount++;
		 */
		Sleep(100);		
	}
#endif
	TerminateThread(m_hDataParse,0);
	TerminateThread(m_hThreadRead,0);
	TerminateThread(m_hThreadWrite,0);
    EscapeCommFunction(m_hPort,CLRRTS);
    EscapeCommFunction(m_hPort,CLRDTR);
    DeleteCriticalSection(&m_csRead);
    DeleteCriticalSection(&m_csWrite);
    SetCommMask(m_hPort,0);
	
	if(NULL==m_hPort)return FALSE;
	if(NULL==m_hThreadRead)return FALSE;
	if(NULL==m_hThreadWrite)return FALSE;

    if(!CloseHandle(m_hPort))return FALSE;
    if(!CloseHandle(m_hThreadRead))return FALSE;
    if(!CloseHandle(m_hThreadWrite))return FALSE;

    return TRUE;
}

BOOL ConnPort::OpenPort(TCHAR *szPort)
{
	 DWORD dwThreadID=0;
	 m_hPort=CreateFile(szPort,
						GENERIC_READ|GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						0,
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
 
  //创建读写线程
   m_hThreadRead  = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadThreadProc, (void*)this,0,&dwThreadID);
   printf("ReadThread ID=%x",dwThreadID);
   m_hThreadWrite = CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteThreadProc,(void*)this,0,&dwThreadID);
   printf("WriteThread ID=%x",dwThreadID);

   g_hWriteExit = CreateEvent( 
        NULL,               
        TRUE,              
        FALSE,              
        TEXT("WriteEvent")
        );
	g_hReadExit = CreateEvent( 
        NULL,              
        TRUE,               
        FALSE,             
        TEXT("ReadEvent")
        );
	 if (g_hReadExit == NULL||g_hWriteExit==NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return FALSE;
    }
   g_iExitFlag=0;
   if(INVALID_HANDLE_VALUE==m_hThreadRead || INVALID_HANDLE_VALUE==m_hThreadWrite) 
   {   
       wprintf(TEXT("Create read or write thread failed...\r\n"));
       return FALSE;
   }
   return TRUE;

}
DWORD ConnPort::ReadThreadProc(LPVOID p)
{
    COMSTAT comStat;
    DWORD   dwErrFlag;
    DWORD   dwLength;
    DWORD   dwModemStat;
    ConnPort *pThis=(ConnPort *)p;
	BYTE    szRev[512]={0};
  
    if(INVALID_HANDLE_VALUE==pThis->m_hPort)
    {
        AfxMessageBox(L"the port is not opened!");
    }
    while(1)
    {
		#if 0
		if(WAIT_OBJECT_0==WaitForSingleObject(g_hReadExit,1000))
		{
			ResetEvent(g_hReadExit);
			break;
		}
		#endif
        WaitCommEvent(pThis->m_hPort,&dwModemStat,NULL);
        EnterCriticalSection(&pThis->m_csRead);
        if(dwModemStat&EV_RXCHAR)
        {
			ClearCommError(pThis->m_hPort,&dwErrFlag,&comStat);
			dwLength=comStat.cbInQue;
			if(dwLength>0)
			{
				//读数据
				BOOL bRet=ReadFile(pThis->m_hPort,szRev,dwLength,&dwLength,NULL);
				if(!bRet) 
				{
					MessageBox(NULL,TEXT("Read Error"),TEXT("Error"),MB_OK);
				}
				else
				{
					pThis->ParseComData(szRev,dwLength);
				}
			}
        } 
        GetCommModemStatus(pThis->m_hPort,&dwModemStat);
        LeaveCriticalSection(&pThis->m_csRead);
    }
	g_bReadProcExit=1;
    return 0;
}
DWORD ConnPort::WriteThreadProc( LPVOID p )
{

    DWORD dwWritten;
    ConnPort *pThis=(ConnPort *)p;

    while(TRUE)
    {
	 if(WAIT_OBJECT_0==WaitForSingleObject(g_hWriteExit,1000))
			break;
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
  g_bWriteProcExit=1;
  return 0;
}
//解析串口数据
void ConnPort::ParseComData(BYTE *byteRev,int iLen)
{
		int				iCount=0;
		int				iLRCount=0;
        char            *szFind;
		char			szRev[512]={0};
		char			szTmp[512]={0};
		TCHAR			m_revData[512]={0};
        CString         strMsg;
        CSSToolDlg		*dlg=(CSSToolDlg *)AfxGetApp()->GetMainWnd();
        
		//sprintf(szRev,"%s",byteRev);
		sprintf(szTmp,"%s",byteRev);
#if 1
		while(iCount<iLen)
		{
			if(szTmp[iCount]=='\n')
			{
				szRev[iCount]='\r';
				szRev[iCount+1]='\n';
				iLRCount++;
			}
			else
			{
				szRev[iCount+iLRCount]=szTmp[iCount];
			}
			iCount++;
		}
#endif
		MultiByteToWideChar(CP_ACP,0,szRev,-1,m_revData,512);
		strMsg=m_revData;
        if(!strMsg.IsEmpty())
        {
            dlg->OutMsg(strMsg);
        }
}
