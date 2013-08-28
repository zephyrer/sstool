#include "StdAfx.h"
#include "ConnPort.h"
#include "SSToolDlg.h"
#include "string.h"

#define     MAX_BUFFER_SIZE   512

volatile int			g_iExitFlag=0;
static	 int			g_wExitFlag=0;
static	 int			g_rExitFlag=0;
static	 int			g_pExitFlag=0;

static	BYTE		g_ReadDataBuf[MAX_BUFFER_SIZE];
static	TCHAR		g_WriteDataBuf[MAX_BUFFER_SIZE];
static  int			g_iRInPos=0;
static	int			g_iROutPos=0;
static  int			g_iWInPos=0;
static	int			g_iWOutPos=0;

const int g_iBaudrate[]=
{
	CBR_110,
	CBR_300,
	CBR_600,
	CBR_1200,
	CBR_2400,
	CBR_4800,
	CBR_9600,
	CBR_14400,
	CBR_19200,
	CBR_38400,
	CBR_57600,
	CBR_115200,
	CBR_128000,
	CBR_256000,
};

ConnPort::ConnPort(void)
{
	m_bIsConnect=FALSE;
}

ConnPort::~ConnPort(void)
{

}
BOOL ConnPort::ConfigPort(int iBaudrate,int iParity,int iDataBits,int iStopBits)
{
    DCB portDCB;
    portDCB.DCBlength=sizeof(DCB);
    GetCommState(m_hPort,&portDCB);
    //设置DCB结构
    portDCB.BaudRate=g_iBaudrate[iBaudrate];
    portDCB.ByteSize=iDataBits+6;
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
    portDCB.Parity=iParity;//无奇偶校验
    portDCB.StopBits=iStopBits;
  
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
	m_bIsConnect=FALSE;
    return TRUE;
}
BOOL ConnPort::IsConnect()
{
	return m_bIsConnect;
}
BOOL ConnPort::OpenPort(TCHAR *szPort,int iBaudrate,int iParity,int iDataBits,int iStopBits)
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

   if(!ConfigPort(iBaudrate,iParity,iDataBits,iStopBits)) return FALSE;
   if(!CommTimeouts())return FALSE;

   InitializeCriticalSection(&m_csRead);
   InitializeCriticalSection(&m_csWrite);
  //创建读写线程
   m_hThreadRead  = CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadThreadProc, (void*)this,0,&dwThreadID);
   m_hThreadWrite = CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteThreadProc,(void*)this,0,&dwThreadID);
   m_hDataParse = CreateThread(0,0,(LPTHREAD_START_ROUTINE)PareDataProc,(void*)this,0,&dwThreadID);
   m_bIsConnect=TRUE;
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
	memset(g_ReadDataBuf,0,MAX_BUFFER_SIZE);
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
					if(g_iRInPos==g_iROutPos) Sleep(20);
					BOOL bRet=ReadFile(pThis->m_hPort,&RXBuffer,1,&dwRead,NULL);
					if(!bRet||0==dwLength) 
					{
						continue;
					}
					g_ReadDataBuf[g_iRInPos]=RXBuffer;
					g_iRInPos=((g_iRInPos++)%MAX_BUFFER_SIZE);
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
		if(g_iROutPos==g_iRInPos) Sleep(10);
		sprintf(&szTmp,"%c",g_ReadDataBuf[g_iROutPos]);
		if(szTmp=='\n')
		{
			szRev[iReadCount++]='\n';
			pThis->SendComData(szRev,iReadCount);
			iReadCount=0;
			g_ReadDataBuf[g_iROutPos]=0;
			g_iROutPos=((g_iROutPos++)%MAX_BUFFER_SIZE);
			memset(szRev,0,MAX_BUFFER_SIZE);
		}
		else if(szTmp!=0)
		{
			szRev[iReadCount++]=szTmp;
			g_ReadDataBuf[g_iROutPos]=0;
			g_iROutPos=((g_iROutPos++)%MAX_BUFFER_SIZE);
		}	
	}
	g_pExitFlag=1;
	return TRUE;
}
BOOL ConnPort::WriteByte(BYTE byWrite)
{
	g_WriteDataBuf[(g_iWInPos++)%MAX_BUFFER_SIZE]=byWrite;
	return  TRUE;
}
BOOL ConnPort::WriteString(TCHAR *szWriteData,int iLen)
{
	if(NULL==szWriteData|| 0==iLen)
		return FALSE;
	for(int iLoop=0;iLoop<iLen;iLoop++)
	{
		g_WriteDataBuf[(g_iWInPos++)%MAX_BUFFER_SIZE]=szWriteData[iLoop];
	}
	return  TRUE;
}
DWORD ConnPort::WriteThreadProc( LPVOID p )
{

    DWORD dwWritten;
	TCHAR  *szTmp;
    ConnPort *pThis=(ConnPort *)p;
	memset(g_WriteDataBuf,0,MAX_BUFFER_SIZE);
    while(TRUE)
    {
		if(1==g_iExitFlag)
		{
			break;
		}
		if(g_iWInPos==g_iWOutPos) continue;
		EnterCriticalSection(&pThis->m_csWrite);
		if(INVALID_HANDLE_VALUE!=pThis->m_hPort)
		{
         SetCommMask(pThis->m_hPort,EV_RXCHAR);
         PurgeComm(pThis->m_hPort,PURGE_TXCLEAR|PURGE_RXCLEAR);
		 pThis->m_WrriteBuffer=g_WriteDataBuf[(g_iWOutPos++)%MAX_BUFFER_SIZE];
         WriteFile(pThis->m_hPort,&(pThis->m_WrriteBuffer),1,&dwWritten,NULL);
		}
		LeaveCriticalSection(&pThis->m_csWrite);
    }
	g_wExitFlag=1;
	return 0;
}
Uint ConnPort::Char2Hex(char *Buffer,char *szOut,int iLen)
{
    Uint uRet=0;
	char szHexChar[4]={0};
    if(NULL==Buffer)return 0;
	if(NULL==szOut)return 0;
    static const char szHex[]="!\"#$%&`()*+,-.\/0123456789:;<=>?@ABCDEFGHIJLLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqstuvwxyz{|}~"
		"€‚ƒ„…†‡ˆ‰Š‹ŒŽ‘’“”•–—˜™š›œžŸ";
    static char  szHexCompare[256]={0};
    for(int i=0;i<sizeof(szHex)-1;i++)
    {
        szHexCompare[szHex[i]]=i+33;
    }
	for(int i=0;i<iLen;i++)
	{
		uRet=szHexCompare[Buffer[i]];
		sprintf(szHexChar,"%2X",uRet);
		strcat(szOut,szHexChar);
		strcat(szOut,"  ");
	}
    return TRUE;
}
void ConnPort::SetHexShow(BOOL bHex)
{
	m_bHexShow=bHex;
}
BOOL ConnPort::GetHexShowEnable()
{
	return m_bHexShow;
}
void ConnPort::SendComData(char *szRevData,int iLen)
{
		CString     strMsg;
		TCHAR		m_revData[MAX_BUFFER_SIZE];
		char		szTrans[MAX_BUFFER_SIZE];

        CSSToolDlg		*dlg=(CSSToolDlg *)AfxGetApp()->GetMainWnd();
        memset(m_revData,0,MAX_BUFFER_SIZE);
		memset(szTrans,0,MAX_BUFFER_SIZE);
		if(m_bHexShow)
		{
			Char2Hex(szRevData,szTrans,iLen);
			MultiByteToWideChar(CP_ACP,0,szTrans,-1,m_revData,MAX_BUFFER_SIZE);
		}
		else
		{
			MultiByteToWideChar(CP_ACP,0,szRevData,-1,m_revData,MAX_BUFFER_SIZE);
		}
		strMsg=m_revData;
        if(!strMsg.IsEmpty())
        {
            dlg->OutMsg(strMsg);
        }
}
