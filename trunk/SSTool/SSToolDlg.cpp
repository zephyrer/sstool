
// SSToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolDlg.h"
#include "afxdialogex.h"
#include "ConnPort.h"
#include "stdio.h"
#include "io.h"
#include "Dbt.h"
#include <strsafe.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "setupapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int	  g_iInPos=0;
static int	  g_iPos=0;
TCHAR strCmdBuf[MAX_CMD_BUFFER_SIZE][128]={0};

TCHAR *strConn[]=
{
	L"COM1",
	L"COM2",
	L"COM3",
	L"COM4",
	L"COM5",
	L"COM6",
	L"COM7",
	L"COM8",
	L"COM9",
};
TCHAR *strConnStore[]=
{
	L"COM1",
	L"COM2",
	L"COM3",
	L"COM4",
	L"COM5",
	L"COM6",
	L"COM7",
	L"COM8",
	L"COM9",
};

TCHAR *strBaudRate[]=
{
	L"110",
	L"300",
	L"600",
	L"1200",
	L"2400",
	L"4800",
	L"9600",
	L"14400",
	L"19200",
	L"38400",
	L"57600",
	L"115200",
	L"128000",
	L"256000",
};
TCHAR *strStopBits[]=
{
	L"1",
	L"1.5",
	L"2",
};

TCHAR *strDataBits[]=
{
	L"6",
	L"7",
	L"8",
};
TCHAR *strParity[]=
{
	L"NONE",
	L"ODD",
	L"EVEN",
	L"MARK",
	L"SPACE",
};
ConnPort  m_conn;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



CSSToolDlg::CSSToolDlg(const CString& str,CWnd* pParent /*=NULL*/)
	: CDialogEx(CSSToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	m_strCaption = str;
	m_iCurConn=0;
	m_iComCounts=0;
	m_bExtEnable=FALSE;
	m_TimeSend=FALSE;
	m_bSendBR=FALSE;
	m_bTimeShow=FALSE;
}
void CSSToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMLIST, m_ctrComList);
	DDX_Control(pDX, IDC_EDIT_MSG_OUT,	m_ctlMsgOut);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_cbBaudrate);
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_cbStopBits);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_cbDataBits);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_cbParity);
	DDX_Control(pDX, IDC_EDIT_SND, m_mSend);
	DDX_Control(pDX, IDC_BUTTON_CON, m_connBtn);
	DDX_Control(pDX, IDC_EDIT_STIME, m_sndTimer);
	DDX_Control(pDX, IDC_CHECK_HEXSEND, m_hexSnd);
	DDX_Control(pDX, IDC_CHECK_SC_SEND, m_scSnd);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_SendBtn);
	DDX_Control(pDX, IDC_STATE_ON, m_StateTip);
}

BEGIN_MESSAGE_MAP(CSSToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CON, &CSSToolDlg::OnBnClickedButtonCon)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CSSToolDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSSToolDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CSSToolDlg::OnBnClickedButtonSave)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_HEXSEND, &CSSToolDlg::OnBnClickedCheckHexsend)
	ON_BN_CLICKED(IDC_CHECK_SC_SEND, &CSSToolDlg::OnBnClickedCheckScSend)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_HEX_SHOW, &CSSToolDlg::OnBnClickedCheckHexShow)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_CBN_CLOSEUP(IDC_COMBO_COMLIST, &CSSToolDlg::OnCbnCloseupComboComlist)
	ON_BN_CLICKED(IDC_BUTTON_EXT, &CSSToolDlg::OnBnClickedButtonExt)
	ON_BN_CLICKED(IDC_BUTTON_CAP, &CSSToolDlg::OnBnClickedButtonCap)
	ON_BN_CLICKED(IDC_BTN_COPY, &CSSToolDlg::OnBnClickedBtnCopy)
	ON_CBN_SELCHANGE(IDC_COMBO_COMLIST, &CSSToolDlg::OnCbnSelchangeComboComlist)
	ON_BN_CLICKED(IDC_BTN_TIME, &CSSToolDlg::OnBnClickedBtnTime)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_BR, &CSSToolDlg::OnBnClickedCheckBr)
	ON_EN_CHANGE(IDC_EDIT_STIME, &CSSToolDlg::OnEnChangeEditStime)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_BW, &CSSToolDlg::OnBnClickedRadioBw)
	ON_BN_CLICKED(IDC_RADIO_BG, &CSSToolDlg::OnBnClickedRadioBg)
	ON_BN_CLICKED(IDC_RADIO_WB, &CSSToolDlg::OnBnClickedRadioWb)
END_MESSAGE_MAP()

void CSSToolDlg::InitCommList()
{
	HANDLE  hComm;
	int		nConm=0;
	int		nCount=0;
	BOOL	bFlag=FALSE;
	int iLen=sizeof(strConn)/sizeof(strConn[0]);
	memset(strConnStore,0,sizeof(strConnStore)/sizeof(strConnStore[0]));
	for(int i=0;i<iLen;i++)
	{
		hComm = CreateFile(strConn[i], 0, 0, 0, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
         if(INVALID_HANDLE_VALUE != hComm )
         {
			CloseHandle(hComm);
			m_ctrComList.AddString(strConn[i]);
			strConnStore[nCount++]=strConn[i];
			m_iComCounts++;
			if(!bFlag)
			{
				nConm=i;
				bFlag=TRUE;
			}
		 }
	}
	m_ctrComList.SetCurSel(0);

	iLen=sizeof(strBaudRate)/sizeof(strBaudRate[0]);
	for(int i=0;i<iLen;i++)
	{
		m_cbBaudrate.AddString(strBaudRate[i]);
	}
	m_cbBaudrate.SetCurSel(BAUD115200);

	iLen=sizeof(strStopBits)/sizeof(strStopBits[0]);
	for(int i=0;i<iLen;i++)
	{
		m_cbStopBits.AddString(strStopBits[i]);
	}
	m_cbStopBits.SetCurSel(ONESTOPBIT);

	iLen=sizeof(strDataBits)/sizeof(strDataBits[0]);
	for(int i=0;i<iLen;i++)
	{
		m_cbDataBits.AddString(strDataBits[i]);
	}
	m_cbDataBits.SetCurSel(DATABIT8);
	
	iLen=sizeof(strParity)/sizeof(strParity[0]);
	for(int i=0;i<iLen;i++)
	{
		m_cbParity.AddString(strParity[i]);
	}
	m_cbParity.SetCurSel(NOPARITY);	
}
BOOL CSSToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	this->SetWindowText(m_strCaption);
	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	InitCommList();
	m_ctlMsgOut.SetLimitText(MAX_BYTES_NUM);

	m_showFont.CreateFont(16,9,0,0,100,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Courier New");
	m_ctlMsgOut.SetFont(&m_showFont,FALSE);

	m_sndTimer.SetWindowTextW(L"1000");
	ShowExtItems(FALSE);
	ReSizeMainWindow();
	return TRUE;
}

void CSSToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
void CSSToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); 
		CRect   rect; 
		GetDlgItem(IDC_STATE_ON)->GetWindowRect(&rect);  
		ScreenToClient(&rect);
		CDC   dcMem;   
		dcMem.CreateCompatibleDC(&dc);   
		CBitmap   bmpBackground; 
		if(m_conn.IsConnect())
		bmpBackground.LoadBitmap(IDB_BITMAP_ON);
		else
		bmpBackground.LoadBitmap(IDB_BITMAP_OFF);
		if(1)
		{                
			BITMAP  bitmap;   
			bmpBackground.GetBitmap(&bitmap);
			CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
			dc.StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&dcMem,0,0,   
			bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);  
		}
		//CDialogEx::OnPaint();
	}
}

HCURSOR CSSToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSSToolDlg::UpdateItem()
{
		CRect   rect; 
		CString strUpdate;
		int iReadBytes=0;
		int iWriteBytes=0;
		static BOOL bComStateLast=0;

		iReadBytes=m_conn.GetConnReadBytes();
		iWriteBytes=m_conn.GetConnWriteBytes();

		if(bComStateLast!=m_conn.IsConnect())
		GetDlgItem(IDC_STATE_ON)->GetWindowRect(&rect);  
		ScreenToClient(&rect);
		InvalidateRect(rect,FALSE);

		GetDlgItem(IDC_STATIC_CONN_STATE)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		strUpdate.Empty();
		strUpdate=strConnStore[m_iCurConn];
		if(m_conn.IsConnect())
		{
			strUpdate+=L"已打开";
			GetDlgItem(IDC_STATIC_CONN_STATE)->SetWindowTextW(strUpdate);
			bComStateLast=TRUE;
		}
		else
		{
			GetDlgItem(IDC_STATIC_CONN_STATE)->SetWindowTextW(L"串口未打开");
			bComStateLast=FALSE;
		}

		GetDlgItem(IDC_STATIC_READ_COUNT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		strUpdate.Empty();
		strUpdate.Format(L"%d",iReadBytes);
		GetDlgItem(IDC_STATIC_READ_COUNT)->SetWindowTextW(strUpdate);
		InvalidateRect(rect,FALSE);


		GetDlgItem(IDC_STATIC_WRITE_COUNT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		strUpdate.Empty();
		strUpdate.Format(L"%d",iWriteBytes);
		GetDlgItem(IDC_STATIC_WRITE_COUNT)->SetWindowTextW(strUpdate);
		InvalidateRect(rect,FALSE);
}
void CSSToolDlg::EnterWorkPath()
{
	int		nLength=0;
	CString strPath;
	strPath.Empty();
	strPath=CommonGetCurPath();
	nLength=strPath.GetLength();

	for(int nLoop=0;nLoop<nLength;nLoop++)
	{
		if(strPath.GetAt(nLoop)=='\\')
		{
			CreateDirectory(strPath.Left(nLoop+1),NULL);
		}
	}
	CreateDirectory(strPath,NULL);
	SetCurrentDirectory(strPath);
}
void  CSSToolDlg::OutMsg(CString strMsg)
{
	int iLen=0;
	char szbuf[MAX_PATH];
	CString strPath;
	if(m_ctlMsgOut.GetLineCount()>((m_conn.GetHexShowEnable()==TRUE)?MAX_HEX_LINE:MAX_LINE_SHOW))
	{
		CFile m_CCacheFile;
		EnterWorkPath();
		strPath.Empty();
		strPath=CommonGetCurPath();
		strPath+=CACHE_FILE_NAME;
		sprintf(szbuf,"%s",strPath);
		if((access(szbuf,0)==-1))
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeCreate| CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"缓存失败！");
				return;
			}
			DWORD FileAttr = GetFileAttributes(strPath);
			SetFileAttributes(strPath,FileAttr | FILE_ATTRIBUTE_HIDDEN);
		}
		else
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"缓存失败！");
				return;
			}
		}
		m_CCacheFile.SeekToEnd();
		m_CCacheFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

		m_CCacheFile.Flush();
		m_CCacheFile.Close();

		m_RecieveData.Empty();
		m_ctlMsgOut.SetSel(0,-1);
		m_ctlMsgOut.ReplaceSel(L" ");
	}
	iLen=m_ctlMsgOut.GetWindowTextLengthW();
	m_ctlMsgOut.SetSel(iLen,iLen);
	m_ctlMsgOut.ReplaceSel((LPCTSTR)strMsg);
	iLen+=strMsg.GetLength();

	m_RecieveData+=strMsg;
}
void CSSToolDlg::OnBnClickedButtonCon()
{
	if(!m_conn.IsConnect())
	{
		m_iCurConn=m_ctrComList.GetCurSel();
		m_iCurBaudrate=m_cbBaudrate.GetCurSel();
		m_iCurDataBits=m_cbDataBits.GetCurSel();
		m_iCurStopBits=m_cbStopBits.GetCurSel();
		m_iCurParity=m_cbParity.GetCurSel();

		if(m_conn.OpenPort(strConnStore[m_iCurConn],m_iCurBaudrate,m_iCurParity,m_iCurDataBits,m_iCurStopBits))
		{
			m_connBtn.SetWindowTextW(L"关闭串口");
			m_cbBaudrate.EnableWindow(FALSE);
			m_cbStopBits.EnableWindow(FALSE);
			m_cbDataBits.EnableWindow(FALSE);
			m_cbParity.EnableWindow(FALSE);
			UpdateItem();
			SetTimer(2,100,NULL);
		}
		else
		{
			if(0==m_iComCounts)
			{
				MessageBox(L"找不到串口!");
			}
			else
			{
				MessageBox(L"串口被占用!");
				m_cbBaudrate.EnableWindow(TRUE);
				m_cbStopBits.EnableWindow(TRUE);
				m_cbDataBits.EnableWindow(TRUE);
				m_cbParity.EnableWindow(TRUE);
				m_connBtn.SetWindowTextW(L"打开串口");
				UpdateItem();
			}
		}
	}
	else
	{
		if(m_conn.ClosePort())
		{
			m_connBtn.SetWindowTextW(L"打开串口");
			m_cbBaudrate.EnableWindow(TRUE);
			m_cbStopBits.EnableWindow(TRUE);
			m_cbDataBits.EnableWindow(TRUE);
			m_cbParity.EnableWindow(TRUE);
			UpdateItem();
		}
	}
}
void CSSToolDlg::OnBnClickedButtonClear()
{
	CString strPath;
	strPath.Empty();
	m_strCache.Empty();
	m_RecieveData.Empty();

	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.ReplaceSel(L" ");
	m_conn.EmptyBytesCount();
	strPath=CommonGetCurPath();
	strPath+=CACHE_FILE_NAME;
	DeleteFile(strPath);
	UpdateData(FALSE);
	UpdateItem();
}

void CSSToolDlg::OnBnClickedButtonSend()
{
	TCHAR *szSend=NULL;
	CString strWrite;
	strWrite.Empty();
	m_mSend.GetWindowTextW(strWrite);
	szSend=strWrite.GetBuffer(strWrite.GetLength());
	if(strWrite.IsEmpty())
	{
		m_conn.WriteString(L"\n",1);
	}
	m_conn.WriteString(szSend,strWrite.GetLength());
	if(TRUE==m_bSendBR)
		m_conn.WriteString(L"\n",1);
	if(m_conn.IsConnect())
	{
		this->OutMsg(strWrite);
		this->OutMsg(L"\r\n");
	}
	if(!m_TimeSend)
	m_mSend.SetWindowTextW(L"");
	
	if(g_iInPos>=MAX_CMD_BUFFER_SIZE)
		g_iInPos=0;
	
	memset(strCmdBuf[g_iInPos],0,128);
	memcpy(strCmdBuf[g_iInPos++],szSend,strWrite.GetLength()*sizeof(TCHAR));
	g_iPos=0;
}

BOOL CSSToolDlg::Char2Hex(TCHAR *szBuffer,TCHAR *szOut,int iLen)
{
	unsigned int uRet;
	TCHAR szHexChar[8]={0};
    if(NULL==szBuffer)return 0;
	if(NULL==szOut)return 0;

	for(int i=0;i<iLen;i++)
	{
		uRet=(unsigned int)szBuffer[i];
		wsprintf(szHexChar,L"%02X ",uRet);
		wcscat(szOut,szHexChar);
	}
    return TRUE;
}
CString CSSToolDlg::CommonGetCurPath()  
{   
   CString    sPath;   
   GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
   sPath.ReleaseBuffer();   
   int    nPos;   
   nPos=sPath.ReverseFind('\\');   
   sPath=sPath.Left(nPos);   
   return    sPath;   
}
void CSSToolDlg::OnBnClickedButtonSave()
{
	int nLength=0;
	CFile m_CFile;
	char szbuf[100];
	CTime tt;
	CString m_strTextFile;
	CString m_strTmp;
	CString strSubSrcPath;
	CString strSubDestPath;

	if(m_RecieveData.IsEmpty())
	{
		AfxMessageBox(L"没有数据，保存失败！");
		return;
	}
	EnterWorkPath();
	tt=CTime::GetCurrentTime();

	sprintf(szbuf,"SSToolText_%d-%d-%d-%d-%d-%d.txt",
		tt.GetYear(),tt.GetMonth(),tt.GetDay(),tt.GetHour(),tt.GetMinute(),tt.GetSecond());

	m_strTextFile=szbuf;
	strSubSrcPath=CommonGetCurPath()+CACHE_FILE_NAME;
	strSubDestPath=CommonGetCurPath()+L"\\"+m_strTextFile;

	if(GetFileAttributes(strSubSrcPath)!=0xFFFFFFFF)
	{
		CopyFile(strSubSrcPath,strSubDestPath,FALSE);
		DWORD FileAttr = GetFileAttributes(strSubDestPath);
		SetFileAttributes(strSubDestPath,FileAttr | FILE_ATTRIBUTE_NORMAL);

		if(!m_CFile.Open(m_strTextFile,CFile::modeWrite))
		{
			AfxMessageBox(L"创建文件失败！");
			return;
		}
	}
	else
	{
		if(!m_CFile.Open(m_strTextFile,CFile::modeCreate|CFile::modeWrite))
		{
			AfxMessageBox(L"创建文件失败！");
			return;
		}
	}
	m_CFile.SeekToEnd();
	m_CFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

	m_CFile.Flush();
	m_CFile.Close();

	MessageBox(_T("已保存!"),NULL, MB_OK);
}
void CSSToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(m_TimeHandle==nIDEvent)
	{
		OnBnClickedButtonSend();
	}
	else
	{
		UpdateItem();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CSSToolDlg::OnBnClickedCheckHexsend()
{
	if(((CButton *)GetDlgItem(IDC_CHECK_HEXSEND))-> GetCheck())
	{
		m_conn.SetHexSend(TRUE);
	}
	else
	{
		m_conn.SetHexSend(FALSE);
	}
}

void CSSToolDlg::OnBnClickedCheckScSend()
{
	CString strTimeGet;
	if(((CButton *)GetDlgItem(IDC_CHECK_SC_SEND))-> GetCheck())
	{
		GetDlgItem(IDC_EDIT_STIME)->GetWindowTextW(strTimeGet);
		int nTime=_wtoi(strTimeGet.GetBuffer()) ;
		m_TimeHandle=SetTimer(1,nTime,NULL);
		m_TimeSend=TRUE;
	}
	else
	{
		KillTimer(1);
		m_TimeSend=FALSE;
	}
}
void CSSToolDlg::ReSizeExtItems()
{
	CRect rcText,rcStGroup,rcCap,rcTime;
	CRect rcColorGroup,rcColorBW,rcColorBG,rcColorWB;
	if(m_bExtEnable)
	{
		m_ctlMsgOut.GetWindowRect(rcText);
		ScreenToClient(&rcText);

		GetDlgItem(IDC_STATIC_EXT_GROUP)->GetWindowRect(&rcStGroup);
		ScreenToClient(&rcStGroup);
		rcStGroup.top=rcText.top+3;
		rcStGroup.left=rcText.right+5;
		rcStGroup.right=rcText.right+EXT_MENU_WIDTH-1;

		GetDlgItem(IDC_BUTTON_CAP)->GetWindowRect(&rcCap);
		ScreenToClient(&rcCap);
		rcCap.top=rcStGroup.top+30;
		rcCap.left=rcStGroup.left+1;
		rcCap.bottom=rcCap.top+EXT_BTN_HEIGHT;
		rcCap.right=rcCap.left+EXT_MENU_WIDTH-10;


		GetDlgItem(IDC_BTN_TIME)->GetWindowRect(&rcTime);
		ScreenToClient(&rcTime);
		rcTime.left=rcCap.left+1;
		rcTime.top=rcCap.bottom+1;
		rcTime.bottom=rcTime.top+EXT_BTN_HEIGHT;
		rcTime.right=rcTime.left+EXT_MENU_WIDTH-10;

		GetDlgItem(IDC_STATIC_COLOR_MODE)->GetWindowRect(&rcColorGroup);
		ScreenToClient(&rcColorGroup);
		rcColorGroup.top=rcStGroup.top+120;
		rcColorGroup.left=rcStGroup.left;
		rcColorGroup.right=rcStGroup.right;
		rcColorGroup.bottom=rcColorGroup.top+130;

		GetDlgItem(IDC_RADIO_BW)->GetWindowRect(&rcColorBW);
		ScreenToClient(&rcColorBW);
		rcColorBW.top=rcColorGroup.top+25;
		rcColorBW.left=rcColorGroup.left+5;
		rcColorBW.right=rcColorGroup.right;
		rcColorBW.bottom=rcColorBW.top+20;
		
		GetDlgItem(IDC_RADIO_BG)->GetWindowRect(&rcColorBG);
		ScreenToClient(&rcColorBG);
		rcColorBG.top=rcColorBW.bottom+2;
		rcColorBG.left=rcColorGroup.left+5;
		rcColorBG.right=rcColorGroup.right;
		rcColorBG.bottom=rcColorBG.top+20;

		GetDlgItem(IDC_RADIO_WB)->GetWindowRect(&rcColorWB);
		ScreenToClient(&rcColorWB);
		rcColorWB.top=rcColorBG.bottom+2;
		rcColorWB.left=rcColorGroup.left+5;
		rcColorWB.right=rcColorGroup.right;
		rcColorWB.bottom=rcColorWB.top+20;

		GetDlgItem(IDC_STATIC_EXT_GROUP)->MoveWindow(rcStGroup);
		GetDlgItem(IDC_BUTTON_CAP)->MoveWindow(rcCap);
		GetDlgItem(IDC_BTN_TIME)->MoveWindow(rcTime);

		GetDlgItem(IDC_STATIC_COLOR_MODE)->MoveWindow(rcColorGroup);
		GetDlgItem(IDC_RADIO_WB)->MoveWindow(rcColorWB);
		GetDlgItem(IDC_RADIO_BW)->MoveWindow(rcColorBW);
		GetDlgItem(IDC_RADIO_BG)->MoveWindow(rcColorBG);

		InvalidateRect(FALSE);
		UpdateWindow();
	}
}
void CSSToolDlg::ReSizeMainWindow()
{
	CRect rcConn,rcText,rcDlg,rcSendBox,rcSendBtn;
	CWnd *pWnd;

	pWnd = GetDlgItem(IDC_EDIT_MSG_OUT);
	if(pWnd!=NULL)
	{
		m_connBtn.GetWindowRect(&rcConn);
		GetClientRect(&rcDlg);
		m_ctlMsgOut.GetWindowRect(rcText);
		m_mSend.GetWindowRect(rcSendBox);
		m_SendBtn.GetWindowRect(rcSendBtn);
		ScreenToClient(&rcConn);
		ScreenToClient(&rcText);
		ScreenToClient(&rcSendBtn);
		ScreenToClient(&rcSendBox);

		rcSendBtn.left=rcDlg.right-rcSendBtn.Width()-1;
		rcSendBtn.right=rcDlg.right-1;
		rcSendBtn.top=rcDlg.bottom-2-rcSendBtn.Height();
		rcSendBtn.bottom=rcDlg.bottom-2;

		rcSendBox.left=rcConn.right+3;
		rcSendBox.right=rcSendBtn.left-1;
		rcSendBox.top=rcSendBtn.top;
		rcSendBox.bottom=rcDlg.bottom-2;

		rcText.left=rcConn.right+3;
		rcText.right=rcDlg.right-2;
		if(m_bExtEnable)
		{
			rcText.right=rcText.right-EXT_MENU_WIDTH;
		}
		rcText.top=0;
		rcText.bottom=rcSendBtn.top-2;

		::MoveWindow(m_SendBtn.m_hWnd,rcSendBtn.left,rcSendBtn.top,rcSendBtn.right-rcSendBtn.left,rcSendBtn.bottom-rcSendBtn.top,0);
		::MoveWindow(m_mSend.m_hWnd,rcSendBox.left,rcSendBox.top,rcSendBox.right-rcSendBox.left,rcSendBox.bottom-rcSendBox.top,0);
		::MoveWindow(m_ctlMsgOut.m_hWnd,rcText.left,rcText.top,rcText.right-rcText.left,rcText.bottom-rcText.top,0);
		
		if(!m_bExtEnable)
		{
			InvalidateRect(FALSE);
			UpdateWindow();
		}
	}
}
void CSSToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	ReSizeMainWindow();
	ReSizeExtItems();
}
BOOL CSSToolDlg::PreTranslateMessage(MSG* pMsg)
{
	
	CString strCmd;

	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_RETURN)
		{
			if(this->GetFocus()->m_hWnd==m_mSend.m_hWnd)
			{
				OnBnClickedButtonSend();
				return 1;
			}
			else if(this->GetFocus()->m_hWnd == m_ctlMsgOut.m_hWnd)
			{
				m_conn.WriteByte(L'\n');
				this->OutMsg(L"\r\n");
			}
		}
		else if((pMsg->wParam==VK_SPACE) && (this->GetFocus()->m_hWnd == m_ctlMsgOut.m_hWnd))
		{
			m_conn.WriteByte(0x20);
		}
		else if(pMsg->wParam==VK_UP)
		{
			strCmd=strCmdBuf[g_iInPos-g_iPos-1];
			if(g_iPos<g_iInPos-1)
			g_iPos++;
			m_mSend.SetWindowTextW(strCmd);
			m_mSend.SetSel(-1);
			return 1;
		}
		else if(pMsg->wParam==VK_DOWN)
		{
			strCmd=strCmdBuf[g_iInPos-g_iPos-1];
			if(g_iPos>0)
			g_iPos--;
			m_mSend.SetWindowTextW(strCmd);
			m_mSend.SetSel(-1);
			return 1;
		}
		else if(pMsg->wParam == 'Z' && GetKeyState(VK_CONTROL)&& GetKeyState(VK_SHIFT))
		{
			OnBnClickedButtonCap();
		}
		else if((this->GetFocus()->m_hWnd == m_ctlMsgOut.m_hWnd)&&(pMsg->wParam>=65 && pMsg->wParam<=90 && (GetKeyState(VK_SHIFT) || GetKeyState(VK_CAPITAL)&0x0001)))
		{
			m_conn.WriteByte(pMsg->wParam);
			return 1;
		}
		else if((this->GetFocus()->m_hWnd == m_ctlMsgOut.m_hWnd)&&(pMsg->wParam>=65 && pMsg->wParam<=90))
		{
			m_conn.WriteByte(pMsg->wParam+32);
			return 1;
		}
		else if(this->GetFocus()->m_hWnd == m_ctlMsgOut.m_hWnd)
		{
			if(pMsg->wParam==190)
			m_conn.WriteByte(0x2E);
			else
			m_conn.WriteByte(pMsg->wParam);
			return 1;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);

}
void CSSToolDlg::OnBnClickedCheckHexShow()
{
	if(!m_conn.IsConnect())
	{
		((CButton *)GetDlgItem(IDC_CHECK_HEX_SHOW))->SetCheck(FALSE);
		MessageBox(L"串口未打开！");
		return;
	}
	if(((CButton *)GetDlgItem(IDC_CHECK_HEX_SHOW))-> GetCheck())
	{
		m_conn.SetHexShow(TRUE);
	}
	else
	{
		m_conn.SetHexShow(FALSE);
	}
}


BOOL CSSToolDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialogEx::OnEraseBkgnd(pDC);
	//return TRUE;
}


void CSSToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSSToolDlg::RefreshComPort()
{
	HANDLE  hComm;
	int		nCount=0;
	int iLen=sizeof(strConn)/sizeof(strConn[0]);
	m_iComCounts=0;
	memset(strConnStore,0,sizeof(strConnStore)/sizeof(strConnStore[0]));
	m_ctrComList.ResetContent();

	for(int i=0;i<iLen;i++)
	{
		hComm = CreateFile(strConn[i], 0, 0, 0, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
         if(INVALID_HANDLE_VALUE != hComm)
         {
			CloseHandle(hComm);
			m_ctrComList.AddString(strConn[i]);
			strConnStore[nCount++]=strConn[i];
			m_iComCounts++;
		 }
	}
	m_ctrComList.UpdateData();
	m_ctrComList.SetCurSel(m_ctrComList.GetCurSel());
}

void CSSToolDlg::OnCbnCloseupComboComlist()
{
}

LRESULT CSSToolDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if( WM_DEVICECHANGE==message)
  {  
	CString   str;  
	DEV_BROADCAST_HDR*   dhr   =   (DEV_BROADCAST_HDR *)lParam;  
	switch(wParam)  
	{ 
		case   DBT_DEVICEARRIVAL:  
		if(dhr->dbch_devicetype   ==   DBT_DEVTYP_PORT)  
		{  
			 OutMsg(L"正在检测串口，请稍后....\n");
			 RefreshComPort();
			 OutMsg(L"OK!!\n");	 
		}  
		break;  
		case   DBT_DEVICEREMOVECOMPLETE:  
		if(dhr->dbch_devicetype   ==   DBT_DEVTYP_PORT)  
		{  
			if(m_conn.IsConnect())
			{
				str.Format(L"串口连接已断开！\n");  
				AfxMessageBox(str);
				OutMsg(str);
				m_conn.ResetComStatues();	
				RefreshComPort();
				m_connBtn.SetWindowTextW(L"打开串口");
				m_cbBaudrate.EnableWindow(TRUE);
				m_cbStopBits.EnableWindow(TRUE);
				m_cbDataBits.EnableWindow(TRUE);
				m_cbParity.EnableWindow(TRUE);
				UpdateItem();
			}
			else
			{
				OutMsg(L"串口连接已断开！！！\n");
				RefreshComPort();
				UpdateItem();
			}
		}  
		break;  
		default:  
		break;  
	}  
  }  

	return CDialogEx::WindowProc(message, wParam, lParam);
}

char CSSToolDlg::FirstDriveFromMask (ULONG unitmask)
{
   char i;

   for (i = 0; i < 26; ++i)
   {
      if (unitmask & 0x1)
         break;
      unitmask = unitmask >> 1;
   }
   return (i + 'A');
}

void CSSToolDlg::ShowExtItems(BOOL bShow)
{
	if(!bShow)
	{
		GetDlgItem(IDC_STATIC_EXT_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CAP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_TIME)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_COLOR_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_BG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_WB)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_EXT_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CAP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_TIME)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_COLOR_MODE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_BW)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_BG)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_WB)->ShowWindow(SW_SHOW);
	}
	
}
void CSSToolDlg::OnBnClickedButtonExt()
{
	    CRect rcConn,rcText,rcDlg,rcSendBox,rcSendBtn;
		m_ctlMsgOut.GetWindowRect(&rcText);
		ScreenToClient(&rcText);

		if(!m_bExtEnable)
		{
			rcText.right=rcText.right-EXT_MENU_WIDTH;
			m_bExtEnable=TRUE;
			::MoveWindow(m_ctlMsgOut.m_hWnd,rcText.left,rcText.top,rcText.right-rcText.left,rcText.bottom-rcText.top,0);	
			GetDlgItem(IDC_BUTTON_EXT)->SetWindowText(L"隐藏");
			ShowExtItems(TRUE);
			ReSizeExtItems();
			Invalidate(FALSE);
			UpdateWindow();
		}
		else
		{
			rcText.right=rcText.right+EXT_MENU_WIDTH;
			m_bExtEnable=FALSE;
			::MoveWindow(m_ctlMsgOut.m_hWnd,rcText.left,rcText.top,rcText.right-rcText.left,rcText.bottom-rcText.top,0);	
			GetDlgItem(IDC_BUTTON_EXT)->SetWindowText(L"扩展");
			ShowExtItems(FALSE);
		}
}

BOOL CSSToolDlg::ReleaseExe(CString strFileName,WORD wResID,CString strFileType)   
{     
    DWORD   dwWrite=0;         

    HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE,FILE_SHARE_WRITE,NULL,   
                                CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);   
    if ( hFile == INVALID_HANDLE_VALUE )   
    {   
        return FALSE;   
    }   

    HRSRC   hrsc = FindResource(NULL, MAKEINTRESOURCE(wResID), strFileType);   
    HGLOBAL hG = LoadResource(NULL, hrsc);   
    DWORD   dwSize = SizeofResource( NULL, hrsc);   

    // 写入文件   
    WriteFile(hFile,hG,dwSize,&dwWrite,NULL);      
    CloseHandle( hFile );   
    return TRUE;   
}
void CSSToolDlg::OnBnClickedButtonCap()
{
	CString strPath;
	strPath.Empty();
	strPath=CommonGetCurPath();
	strPath+=L"\\SCTool.exe";
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strPath;		
	ShExecInfo.lpParameters = L"";	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;

	if(ReleaseExe(strPath,IDR_EXE_CAP,L"EXE"))
	{
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	}

	DeleteFile(strPath);

}
void CSSToolDlg::OnBnClickedBtnCopy()
{
	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.SetFocus();
	m_ctlMsgOut.Copy();
}
void CSSToolDlg::OnCbnSelchangeComboComlist()
{
	m_ctrComList.SetCurSel(m_ctrComList.GetCurSel());
	if(m_iCurConn!=m_ctrComList.GetCurSel() && m_conn.IsConnect())
	{
		m_conn.ClosePort();
		OnBnClickedButtonCon();
	}	
}

void CSSToolDlg::OnBnClickedBtnTime()
{
	if(!m_conn.IsConnect()) return;
	if(!m_bTimeShow)
	{
		m_conn.ComEnableTimeShow(TRUE);
		m_bTimeShow=TRUE;
		GetDlgItem(IDC_BTN_TIME)->SetWindowText(L"隐藏时间");
	}
	else
	{
		m_conn.ComEnableTimeShow(FALSE);
		m_bTimeShow=FALSE;
		GetDlgItem(IDC_BTN_TIME)->SetWindowText(L"显示时间");
	}
}
void CSSToolDlg::OnClose()
{
	CString strPath;
	strPath.Empty();
	strPath=CommonGetCurPath();
	strPath+=CACHE_FILE_NAME;
	DeleteFile(strPath);
	m_bSendBR=FALSE;
	CDialogEx::OnClose();
}


void CSSToolDlg::OnBnClickedCheckBr()
{
	if(((CButton *)GetDlgItem(IDC_CHECK_BR))-> GetCheck())
	{
		m_bSendBR=TRUE;
	}
	else
	{
		m_bSendBR=FALSE;
	}
}

void CSSToolDlg::OnEnChangeEditStime()
{
	if(m_TimeHandle)
	KillTimer(m_TimeHandle);
	OnBnClickedCheckScSend();
}

HBRUSH CSSToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor); 
	return hbr;
}

void CSSToolDlg::OnBnClickedRadioBw()
{
	m_ctlMsgOut.SetColorMode(BLACK_BACK_WHITE_FONT);
	//m_mSend.SetColorMode(BLACK_BACK_WHITE_FONT);
}

void CSSToolDlg::OnBnClickedRadioBg()
{
	m_ctlMsgOut.SetColorMode(BLACK_BACK_GREEN_FONT);
	//m_mSend.SetColorMode(BLACK_BACK_GREEN_FONT);
}

void CSSToolDlg::OnBnClickedRadioWb()
{
	m_ctlMsgOut.SetColorMode(WHITE_BACK_BLACK_FONT);
	//m_mSend.SetColorMode(WHITE_BACK_BLACK_FONT);
}
