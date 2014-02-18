#include "StdAfx.h"
#include "ConnPort.h"
#include "SSToolDlg.h"
#include "string.h"

#define     MAX_BUFFER_SIZE   512
#define		MAX_SEND_SIZE	  256
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
	m_bHexSend=FALSE;
	m_wCount	=0;
	m_rCount	=0;
	m_bTimeShow =FALSE;
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
    
    commTimeOut.ReadIntervalTimeout=1000;
    commTimeOut.ReadTotalTimeoutConstant=0;
    commTimeOut.ReadTotalTimeoutMultiplier=0;
    commTimeOut.WriteTotalTimeoutConstant=500;
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
int ConnPort::OpenPort(TCHAR *szPort,int iBaudrate,int iParity,int iDataBits,int iStopBits)
{
	 DWORD dwThreadID=0;
	 g_iExitFlag=0;
	 m_hPort=NULL;
	
	 m_hPort=CreateFile(szPort,
						GENERIC_READ|GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED,
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
   SetupComm(m_hPort,1024,1024);
   //刷新缓冲区信息->输入、输出缓冲区
   PurgeComm(m_hPort,PURGE_TXCLEAR|PURGE_RXCLEAR);

   if(!ConfigPort(iBaudrate,iParity,iDataBits,iStopBits)) return FALSE;
   if(!CommTimeouts())return FALSE;

  //创建读写线程
   m_hThreadRead	=	CreateThread(0,0,(LPTHREAD_START_ROUTINE)ReadThreadProc,	(void*)this,0,&dwThreadID);
   m_hThreadWrite	=	CreateThread(0,0,(LPTHREAD_START_ROUTINE)WriteThreadProc,	(void*)this,0,&dwThreadID);
   m_hDataParse		=   CreateThread(0,0,(LPTHREAD_START_ROUTINE)PareDataProc,		(void*)this,0,&dwThreadID);

   m_bIsConnect=TRUE;
   return TRUE;
}
DWORD ConnPort::ReadThreadProc(LPVOID p)
{
    COMSTAT		comStat;
    DWORD		dwErrFlag;
    DWORD		dwLength;
	DWORD		dwRead;
	DWORD		dwCommStatus;
    ConnPort	*pThis=(ConnPort *)p;
	BYTE		RXBuffer;
    OVERLAPPED  osRead;
    if(INVALID_HANDLE_VALUE==pThis->m_hPort)
    {
        AfxMessageBox(L"打开串口失败!");
    }
	memset(g_ReadDataBuf,0,MAX_BUFFER_SIZE);
	memset(&comStat,0,sizeof(COMSTAT));
	 
	memset(&osRead,0,sizeof(osRead)); 
	osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);     

    while(1)
    {
		if(1==g_iExitFlag)
		{
			break;
		}
		if(GetOverlappedResult(pThis->m_hPort,&osRead,&dwRead,TRUE) == FALSE)
		{
		DWORD dwErrors;
		if(GetLastError() != ERROR_IO_PENDING)
        {
             //...
        }
        memset(&comStat,0,sizeof(comStat));
        ClearCommError(pThis->m_hPort,&dwErrors,&comStat);
		}
		Sleep(2);
		WaitCommEvent(pThis->m_hPort,&dwCommStatus,&osRead);
		if(dwCommStatus&EV_RXCHAR)
		{
				ClearCommError(pThis->m_hPort,&dwErrFlag,&comStat);
				dwLength=comStat.cbInQue;
				while(dwLength>0)
				{
					if(g_iRInPos==g_iROutPos) Sleep(20);
					if(ReadFile(pThis->m_hPort,&RXBuffer,1,&dwRead,&osRead)==FALSE)
					{
						if(GetLastError() != ERROR_IO_PENDING)
						{
							//...
						}
						if(GetOverlappedResult(pThis->m_hPort,&osRead,&dwRead,TRUE) == FALSE)
						{
							//...
						}
						if(dwRead == 0)
						{
							continue;
						}
					}
					g_ReadDataBuf[g_iRInPos]=RXBuffer;
					g_iRInPos=((g_iRInPos++)%(MAX_BUFFER_SIZE-1));
					pThis->m_rCount++;
					dwLength--;
				}
		}
	}
	g_rExitFlag=1;
    return 0;
}

DWORD ConnPort::PareDataProc(LPVOID p)
{
	int		iReadCount=0;
	static char	szTmp;
	static BOOL bBRLast;
	static BOOL bNewLine;
	char	szRev[MAX_BUFFER_SIZE];
	ConnPort *pThis=(ConnPort *)p;
	memset(szRev,0,MAX_BUFFER_SIZE);
	bBRLast=TRUE;
	bNewLine=FALSE;
	while(1)
	{
		Sleep(0);
		if(1==g_iExitFlag||!pThis->m_bIsConnect) break;
		if(g_iROutPos == g_iRInPos)
		{
			if(iReadCount==0)
			{
				Sleep(2);
				continue;
			}
			if(bBRLast==TRUE)
			{
				pThis->SendComData(szRev,iReadCount,TRUE);
				bBRLast=FALSE;
				bNewLine=FALSE;
			}
			else
				pThis->SendComData(szRev,iReadCount,FALSE);

			iReadCount=0;
			memset(szRev,0,MAX_BUFFER_SIZE);
		}
		sprintf(&szTmp,"%c",g_ReadDataBuf[g_iROutPos]);

		if(szTmp=='\n'||szTmp=='\r')
		{
			szRev[iReadCount++]=szTmp;
			szRev[iReadCount++]='\n';
			if(TRUE==bBRLast || bNewLine==TRUE)
				pThis->SendComData(szRev,iReadCount,TRUE);
			else
				pThis->SendComData(szRev,iReadCount,FALSE);
			bBRLast=TRUE;
			bNewLine=FALSE;
			iReadCount=0;
			g_ReadDataBuf[g_iROutPos]=0;
			g_iROutPos=((g_iROutPos++)%(MAX_BUFFER_SIZE-1));
			memset(szRev,0,MAX_BUFFER_SIZE);
		
		}
		else if(szTmp!=0)
		{
			szRev[iReadCount++]=szTmp;
			g_ReadDataBuf[g_iROutPos]=0;
			g_iROutPos=((g_iROutPos++)%(MAX_BUFFER_SIZE-1));
		}
	}
	g_pExitFlag=1;
	return TRUE;
}

BOOL ConnPort::WriteByte(TCHAR szChar)
{
	g_WriteDataBuf[(g_iWInPos++)%(MAX_BUFFER_SIZE-1)]=szChar;
	m_wCount++;
	return  TRUE;
}

BOOL ConnPort::WriteString(TCHAR *szWriteData,int iLen)
{
	TCHAR szCharHex[MAX_SEND_SIZE]={0};
	if(NULL==szWriteData|| 0==iLen)
		return FALSE;
	if(!IsConnect())
		return FALSE;

	if(m_bHexSend)
	{
		if(iLen>MAX_SEND_SIZE) 
			return FALSE;
		Hex2wChar(szWriteData,szCharHex,&iLen);
	}
	for(int iLoop=0;iLoop<iLen;iLoop++)
	{
		if(m_bHexSend)
		{
			g_WriteDataBuf[(g_iWInPos++)%(MAX_BUFFER_SIZE-1)]=szCharHex[iLoop];
		}
		else
		{
			g_WriteDataBuf[(g_iWInPos++)%(MAX_BUFFER_SIZE-1)]=szWriteData[iLoop];
		}
		m_wCount++;
	}
	return  TRUE;
}

DWORD ConnPort::WriteThreadProc( LPVOID p )
{
    DWORD dwWritten;
	OVERLAPPED osWrite;
	DWORD dwBytes; 
    ConnPort *pThis=(ConnPort *)p;
	memset(g_WriteDataBuf,0,MAX_BUFFER_SIZE);

	memset(&osWrite,0,sizeof(osWrite));
	osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

    while(TRUE)
    {
		Sleep(2);
		if(1==g_iExitFlag)
		{
			break;
		}
		if(g_iWInPos==g_iWOutPos) 
		{
			Sleep(2);
			continue;
		}
		if(INVALID_HANDLE_VALUE!=pThis->m_hPort)
		{
			pThis->m_WrriteBuffer=g_WriteDataBuf[(g_iWOutPos++)%(MAX_BUFFER_SIZE-1)];
			if(WriteFile(pThis->m_hPort,&(pThis->m_WrriteBuffer),1,&dwWritten,&osWrite)==FALSE)
			{
				if(GetLastError() != ERROR_IO_PENDING)
				{

				}
				else
				{
					Sleep(2);
				}
			}
			if(GetOverlappedResult(pThis->m_hPort,&osWrite,&dwBytes,TRUE)== FALSE)
			{
				///.....
			}
		}
    }
	g_wExitFlag=1;
	return 1;
}
Uint ConnPort::Hex2wChar(TCHAR *Buffer,TCHAR *szOut,int *iLen)
{
	int iCount=0;
	int iDataLen=*iLen;
	TCHAR szCharHex[MAX_SEND_SIZE]={0};

	if(NULL==Buffer)return 0;

	static const TCHAR szHexUpper[]=L"0123456789ABCDEF";
	static const TCHAR szHexLow[]=L"abcdef";
	static TCHAR  szHexCompare[256]={0};
	
	for(int i=0;i<sizeof(szHexLow)-1;i++)
	{
		szHexCompare[szHexLow[i]]=i+10;
	}

	for(int i=0;i<sizeof(szHexUpper)-1;i++)
	{
		szHexCompare[szHexUpper[i]]=i;
	}

	for(int i=0;i<iDataLen;i=i+1)
	{
		if(Buffer[i]!=32)
		{
			szOut[iCount++]=((szHexCompare[Buffer[i]]<<4))|(szHexCompare[Buffer[i+1]]);
			i++;
		}
		else
		{
			
		}
	}

	*iLen=iCount;
    return TRUE;
}
Uint ConnPort::Char2Hex(char *Buffer,char *szOut,int iLen)
{
    Uint uRet=0;
	char szHexChar[8]={0};
    if(NULL==Buffer)return 0;
	if(NULL==szOut)return 0;

	for(int i=0;i<iLen;i++)
	{
		uRet=(Uint)Buffer[i];
		memset(szHexChar,0,sizeof(szHexChar));
		sprintf(szHexChar,"%02X ",uRet);
		strcat(szOut,szHexChar);
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
void ConnPort::SendComData(char *szRevData,int iLen,BOOL bNewLine)
{
		CString			strMsg;
		CTime			tt;
		CString			strTime;
		CString			strTmp;
		TCHAR			m_revData[MAX_BUFFER_SIZE];
		char			szTrans[MAX_BUFFER_SIZE];
	
        CSSToolDlg		*dlg=(CSSToolDlg *)AfxGetApp()->GetMainWnd();
        memset(m_revData,0,MAX_BUFFER_SIZE);
		memset(szTrans,	 0,MAX_BUFFER_SIZE);

		if(m_bHexShow)
		{
			Char2Hex(szRevData,szTrans,iLen);
			MultiByteToWideChar(CP_ACP,0,szTrans,-1,m_revData,MAX_BUFFER_SIZE);	
		}
		else
		{
			MultiByteToWideChar(CP_ACP,0,szRevData,-1,m_revData,MAX_BUFFER_SIZE);
		}
		strTmp=m_revData;
        if(!strTmp.IsEmpty())
        {
			
			tt=CTime::GetCurrentTime();
			strTime=tt.Format(L"[%m/%d %H:%M:%S] ");
			if(bNewLine&& !m_bHexShow &&m_bTimeShow)
				strMsg=strTime+strTmp;
			else
				strMsg=strTmp;
        }
        dlg->OutMsg(strMsg);
}
void ConnPort::SetHexSend(BOOL bHex)
{
	m_bHexSend=bHex;
}
BOOL ConnPort::GetHexSendEnable()
{
	return m_bHexSend;
}

int  ConnPort::GetConnReadBytes()
{
	return m_rCount;
}
int  ConnPort::GetConnWriteBytes()
{
	return m_wCount;
}
void ConnPort::EmptyBytesCount()
{
	m_rCount=0;
	m_wCount=0;
}
void ConnPort::ResetComStatues()
{
	m_rCount=0;
	m_wCount=0;
	m_bHexShow=FALSE;

	TerminateThread(m_hThreadRead,0);
	TerminateThread(m_hThreadWrite,0);
	TerminateThread(m_hDataParse,0);

	EscapeCommFunction(m_hPort,CLRRTS);
    EscapeCommFunction(m_hPort,CLRDTR);
    SetCommMask(m_hPort,0);
	
    CloseHandle(m_hPort);
    CloseHandle(m_hThreadRead);
    CloseHandle(m_hThreadWrite);
	CloseHandle(m_hDataParse);

	m_bIsConnect=FALSE;
}

void ConnPort::ComEnableTimeShow(BOOL bEnable)
{
	m_bTimeShow=bEnable;
}
