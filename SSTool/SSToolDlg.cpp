
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
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	m_strCaption = str;
	m_iCurConn=0;
	m_iComCounts=0;
	m_bExtEnable=FALSE;
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
	ON_BN_CLICKED(IDC_BTN_SEL_ALL, &CSSToolDlg::OnBnClickedBtnSelAll)
	ON_BN_CLICKED(IDC_BTN_COPY, &CSSToolDlg::OnBnClickedBtnCopy)
	ON_BN_CLICKED(IDC_BTN_CUT, &CSSToolDlg::OnBnClickedBtnCut)
END_MESSAGE_MAP()


// CSSToolDlg message handlers

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
	InitCommList();
	m_ctlMsgOut.SetLimitText(MAX_BYTES_NUM);
	//setup editbox font
	m_showFont.CreateFont(14,7,0,0,100,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Courier New");
	m_ctlMsgOut.SetFont(&m_showFont,FALSE);

	m_sndTimer.SetWindowTextW(L"1000");
	InitExtItems();
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
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
void  CSSToolDlg::OutMsg(CString strMsg)
{
	int iLen=0;
	if(m_ctlMsgOut.GetLineCount()>((m_conn.GetHexShowEnable()==TRUE)?MAX_HEX_LINE:MAX_LINE_SHOW))
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
			m_connBtn.SetWindowTextW(L"关闭串口");
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
			}
		}
	}
	else
	{
		if(m_conn.ClosePort())
		{
			m_connBtn.SetWindowTextW(L"打开串口");
			UpdateItem();
		}
	}
}
void CSSToolDlg::OnBnClickedButtonClear()
{
	m_RecieveData.Empty();
	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.ReplaceSel(L" ");
	m_conn.EmptyBytesCount();
	UpdateData(FALSE);
	UpdateItem();
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
	char szbuf[22];
	int nLoop=0;
	CTime tt;
	CString m_strTime;
	CString m_strTextFile;

	if(m_RecieveData.IsEmpty())
	{
		AfxMessageBox(L"没有数据，保存失败！");
		return;
	}
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

	for(nLoop=1;nLoop<MAX_SAVE_TEXT_NUM;nLoop++)
	{
		sprintf(szbuf,"SaveDebugText_%03d.txt",nLoop);
		if((access(szbuf,0)==-1))
			break;
	}
	if(MAX_SAVE_TEXT_NUM==nLoop)
	{
		AfxMessageBox(L"保存的文件数过多，请先清理！");
		return;
	}
	m_strTextFile=szbuf;
	if(!m_CFile.Open(m_strTextFile,CFile::modeCreate| CFile::modeWrite))
	{
		AfxMessageBox(L"创建文件失败！");
		return;
	}
	tt=CTime::GetCurrentTime();
	m_strTime=tt.Format(L"===================\r\n%Y-%m-%d %H:%M:%S\r\n===================\r\n");
	m_CFile.Write((LPCTSTR)m_strTime,m_strTime.GetLength()*sizeof(TCHAR));
	if(!m_strStoreText.IsEmpty())
	m_CFile.Write((LPCTSTR)m_strStoreText,m_strStoreText.GetLength()*sizeof(TCHAR));
	m_CFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

	m_CFile.Flush();
	m_CFile.Close();

	MessageBox(_T("已保存!"),NULL, MB_OK);
}


void CSSToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(1==nIDEvent)
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
		SetTimer(1,nTime,NULL);
	}
	else
	{
		KillTimer(1);
	}
}
void CSSToolDlg::ReSizeExtItems()
{
	CRect rcText,rcStGroup,rcCopy,rcCap,rcCut,rcSelAll;
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

		GetDlgItem(IDC_BTN_SEL_ALL)->GetWindowRect(&rcSelAll);
		ScreenToClient(&rcSelAll);
		rcSelAll.top=rcCap.bottom+2;
		rcSelAll.left=rcStGroup.left+1;
		rcSelAll.right=rcSelAll.left+EXT_MENU_WIDTH/2;
		rcSelAll.bottom=rcSelAll.top+EXT_BTN_HEIGHT;

		GetDlgItem(IDC_BTN_COPY)->GetWindowRect(&rcCopy);
		ScreenToClient(&rcCopy);
		rcCopy.left=rcSelAll.right;
		rcCopy.top=rcSelAll.top;
		rcCopy.bottom=rcCopy.top+EXT_BTN_HEIGHT;
		rcCopy.right=rcCopy.left+EXT_MENU_WIDTH/2-10;


		GetDlgItem(IDC_BTN_CUT)->GetWindowRect(&rcCut);
		ScreenToClient(&rcCut);
		rcCut.left=rcStGroup.left+1;
		rcCut.top=rcSelAll.bottom+1;
		rcCut.bottom=rcCut.top+EXT_BTN_HEIGHT;
		rcCut.right=rcCut.left+EXT_MENU_WIDTH/2;

		GetDlgItem(IDC_STATIC_EXT_GROUP)->MoveWindow(rcStGroup);
		GetDlgItem(IDC_BUTTON_CAP)->MoveWindow(rcCap);
		GetDlgItem(IDC_BTN_SEL_ALL)->MoveWindow(rcSelAll);
		GetDlgItem(IDC_BTN_COPY)->MoveWindow(rcCopy);
		GetDlgItem(IDC_BTN_CUT)->MoveWindow(rcCut);

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
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_RETURN)
		{
			if(m_mSend.GetFocus())
			{
				OnBnClickedButtonSend();
				return FALSE;
			}
		}
		else if(pMsg->wParam == 'Z' && GetKeyState(VK_CONTROL)&& GetKeyState(VK_SHIFT))
		{
			OnBnClickedButtonCap();
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
	//UpdateItem();
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
	m_ctrComList.SetCurSel(0);
}

void CSSToolDlg::OnCbnCloseupComboComlist()
{
	if(!m_conn.IsConnect())
		RefreshComPort();
}

LRESULT CSSToolDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if( WM_DEVICECHANGE==message)
  {  
	CString   str;  
	PDEV_BROADCAST_PORT pDevPort;
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

void CSSToolDlg::InitExtItems()
{
	GetDlgItem(IDC_STATIC_EXT_GROUP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_CAP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_SEL_ALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_COPY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_CUT)->ShowWindow(SW_HIDE);
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
			GetDlgItem(IDC_STATIC_EXT_GROUP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_CAP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_SEL_ALL)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_COPY)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_CUT)->ShowWindow(SW_SHOW);
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
			GetDlgItem(IDC_STATIC_EXT_GROUP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_CAP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_SEL_ALL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_COPY)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTN_CUT)->ShowWindow(SW_HIDE);	
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
	if(ReleaseExe(strPath,IDR_EXE_CAP,L"EXE"))
		ShellExecute(NULL,L"open",L"SCTool.exe",NULL,CommonGetCurPath(),SW_SHOWNORMAL);

	DeleteFile(strPath);

}

void CSSToolDlg::OnBnClickedBtnSelAll()
{
	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.SetFocus();
}


void CSSToolDlg::OnBnClickedBtnCopy()
{
	if(m_ctlMsgOut.GetFocus())
	m_ctlMsgOut.Copy();
}


void CSSToolDlg::OnBnClickedBtnCut()
{
	if(m_ctlMsgOut.GetFocus())
	{
		m_ctlMsgOut.Cut();
		m_strStoreText.Empty();
		m_RecieveData.Empty();
	}
}
