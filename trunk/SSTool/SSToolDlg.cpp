
// SSToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolDlg.h"
#include "afxdialogex.h"
#include "ConnPort.h"
#include "stdio.h"
#include "io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CSSToolDlg dialog

CSSToolDlg::CSSToolDlg(const CString& str,CWnd* pParent /*=NULL*/)
	: CDialogEx(CSSToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strCaption = str;
	m_iCurConn=0;
}
void CSSToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMLIST, m_ctrComList);
	DDX_Control(pDX, IDC_EDIT_MSG_OUT, m_ctlMsgOut);
	DDX_Control(pDX, IDC_TAB1, m_TabItem);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_cbBaudrate);
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_cbStopBits);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_cbDataBits);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_cbParity);
	DDX_Control(pDX, IDC_EDIT_SND, m_mSend);
	DDX_Control(pDX, IDC_BUTTON_CON, m_connBtn);
	DDX_Control(pDX, IDC_BUTTON_HEX, m_hexBtn);
	DDX_Control(pDX, IDC_EDIT_STIME, m_sndTimer);
	DDX_Control(pDX, IDC_CHECK_HEXSEND, m_hexSnd);
	DDX_Control(pDX, IDC_CHECK_SC_SEND, m_scSnd);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_SendBtn);
}

BEGIN_MESSAGE_MAP(CSSToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CON, &CSSToolDlg::OnBnClickedButtonCon)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CSSToolDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSSToolDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_HEX, &CSSToolDlg::OnBnClickedButtonHex)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CSSToolDlg::OnBnClickedButtonSave)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_HEXSEND, &CSSToolDlg::OnBnClickedCheckHexsend)
	ON_BN_CLICKED(IDC_CHECK_SC_SEND, &CSSToolDlg::OnBnClickedCheckScSend)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSSToolDlg message handlers

void CSSToolDlg::InitCommParams()
{
	HANDLE  hComm;
	int		nConm=0;
	int		nCount=0;
	BOOL	bFlag=FALSE;
	int iLen=sizeof(strConn)/sizeof(strConn[0]);

	for(int i=0;i<iLen;i++)
	{
		hComm = CreateFile(strConn[i], 0, 0, 0, 
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
         if(INVALID_HANDLE_VALUE != hComm )
         {
			CloseHandle(hComm);
			m_ctrComList.AddString(strConn[i]);
			memset(strConnStore,0,sizeof(strConnStore)/sizeof(strConnStore[0]));
			strConnStore[nCount++]=strConn[i];
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

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitCommParams();
	/*Init Tab control item parameter*/
	m_TabItem.InsertItem(0,L"Picture");
	m_TabItem.InsertItem(1,L"Sound");
	m_TabItem.InsertItem(2,L"System");
	/*Init Tab control item parameter*/

	m_sndTimer.SetWindowTextW(L"1000");
	return TRUE;  // return TRUE  unless you set the focus to a control
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSSToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSSToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void  CSSToolDlg::OutMsg(CString strMsg)
{
	int iLen=0;
	if(m_ctlMsgOut.GetLineCount()>600)
	{
		m_strStoreText.Empty();
		m_strStoreText=m_RecieveData;
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
			m_connBtn.SetWindowTextW(L"DISCONNECT");
		}
		else
		{
			MessageBox(L"Connect Failed!");
		}
	}
	else
	{
		if(m_conn.ClosePort())
		{
			m_connBtn.SetWindowTextW(L"CONNECT");
		}
	}
}
void CSSToolDlg::OnBnClickedButtonClear()
{
	m_RecieveData.Empty();
	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.ReplaceSel(L" ");
	UpdateData(FALSE);
}

void CSSToolDlg::OnBnClickedButtonSend()
{
	TCHAR *szSend=NULL;
	CString strWrite;
	m_mSend.GetWindowTextW(strWrite);
	szSend=strWrite.GetBuffer(strWrite.GetLength());
	m_conn.WriteString(szSend,strWrite.GetLength());

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
void CSSToolDlg::OnBnClickedButtonHex()
{
	if(!m_conn.IsConnect())
	{
		MessageBox(L"Serial port is not connected£¡");
		return;
	}
	if(!m_conn.GetHexShowEnable())
	{
		m_conn.SetHexShow(TRUE);
		m_hexBtn.SetWindowTextW(L"DEC");
	}
	else
	{
		m_conn.SetHexShow(FALSE);
		m_hexBtn.SetWindowTextW(L"HEX");
	}
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
	char szbuf[20];
	CTime tt;
	CString m_strTime;
	CString m_strTextFile;


	m_StrCurPath=CommonGetCurPath();
	nLength=m_StrCurPath.GetLength();

	for(int nLoop=0;nLoop<nLength;nLoop++)
	{
		if(m_StrCurPath.GetAt(nLoop)=='\\')
		{
			CreateDirectory(m_StrCurPath.Left(nLoop+1),NULL);
		}
	}
	CreateDirectory(m_StrCurPath,NULL);
	SetCurrentDirectory(m_StrCurPath);

	for(int nLoop=1;nLoop<80;nLoop++)
	{
		sprintf(szbuf,"SaveDebugText%02d.txt",nLoop);
		if((access(szbuf,0)==-1))
			break;
	}
	m_strTextFile=szbuf;
	if(!m_CFile.Open(m_strTextFile,CFile::modeCreate| CFile::modeWrite))
	{
		AfxMessageBox(L"Create debug text fail!");
		return;
	}
	#if 0
	if((access(szbuf,0))==-1)
	{
		AfxMessageBox(L"Create debug text fail!");
		return;
	}
	#endif
	tt=CTime::GetCurrentTime();
	m_strTime=tt.Format(L"===================\r\n%Y-%m-%d %H:%M:%S\r\n===================\r\n");
	m_CFile.Write((LPCTSTR)m_strTime,m_strTime.GetLength()*sizeof(TCHAR));
	m_CFile.Write((LPCTSTR)m_strStoreText,m_strStoreText.GetLength()*sizeof(TCHAR));
	m_CFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));
	m_CFile.Flush();
	m_CFile.Close();

	MessageBox(_T("Done!"),NULL, MB_OK);
}


void CSSToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnTimer(nIDEvent);
}


void CSSToolDlg::OnBnClickedCheckHexsend()
{
	// TODO: Add your control notification handler code here
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
		SetTimer(1,nTime,NULL);
	}
	else
	{
		KillTimer(1);
	}
}


void CSSToolDlg::OnSize(UINT nType, int cx, int cy)
{
	
	CRect rcConn,rcText,rcDlg,rcSendBox,rcSendBtn;
	CWnd *pWnd;

	CDialogEx::OnSize(nType, cx, cy);

	pWnd = GetDlgItem(IDC_EDIT_MSG_OUT);
	if(pWnd)
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
		rcText.top=0;
		rcText.bottom=rcSendBtn.top-2;

		::MoveWindow(m_SendBtn.m_hWnd,rcSendBtn.left,rcSendBtn.top,rcSendBtn.right-rcSendBtn.left,rcSendBtn.bottom-rcSendBtn.top,0);
		::MoveWindow(m_mSend.m_hWnd,rcSendBox.left,rcSendBox.top,rcSendBox.right-rcSendBox.left,rcSendBox.bottom-rcSendBox.top,0);
		::MoveWindow(m_ctlMsgOut.m_hWnd,rcText.left,rcText.top,rcText.right-rcText.left,rcText.bottom-rcText.top,0);
		InvalidateRect(FALSE);
		UpdateWindow();
	}
	
}
