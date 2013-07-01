
// SSToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolDlg.h"
#include "afxdialogex.h"
#include "ConnPort.h"


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

CSSToolDlg::CSSToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSSToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iCurConn=0;
}
void CSSToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMLIST, m_ctrComList);
	DDX_Control(pDX, IDC_EDIT_MSG_OUT, m_ctlMsgOut);
	DDX_Control(pDX, IDC_TAB1, m_TabItem);
}

BEGIN_MESSAGE_MAP(CSSToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CON, &CSSToolDlg::OnBnClickedButtonCon)
	ON_BN_CLICKED(IDC_BUTTON_DISCONN, &CSSToolDlg::OnBnClickedButtonDisconn)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CSSToolDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CSSToolDlg message handlers

void CSSToolDlg::InitCommList()
{
	int iLen=sizeof(strConn)/sizeof(strConn[0]);

	for(int i=0;i<iLen;i++)
	{
		m_ctrComList.AddString(strConn[i]);
	}
	m_ctrComList.SetCurSel(COM1);

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
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	InitCommList();
	/*Init Tab control item parameter*/
	m_TabItem.InsertItem(0,L"Picture");
	m_TabItem.InsertItem(1,L"Sound");
	m_TabItem.InsertItem(2,L"System");
	/*Init Tab control item parameter*/

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
	if(m_ctlMsgOut.GetLineCount()>400)
	{
		m_RecieveData.Empty();
		UpdateData(FALSE);
	}
	iLen=m_ctlMsgOut.GetWindowTextLengthW();
	m_ctlMsgOut.SetSel(iLen,iLen);
	m_ctlMsgOut.ReplaceSel((LPCTSTR)strMsg);
	iLen+=strMsg.GetLength();

	m_RecieveData+=strMsg;
}
void CSSToolDlg::OnBnClickedButtonCon()
{
	m_iCurConn=m_ctrComList.GetCurSel();
	if(m_conn.OpenPort(strConn[m_iCurConn]))
		MessageBox(L"Connect Success!");
	else
		MessageBox(L"Connect Failed!");
}
void CSSToolDlg::OnBnClickedButtonDisconn()
{
	if(m_conn.ClosePort())
		MessageBox(L"Done!");
	else
		MessageBox(L"Error to disconnect!");
}


void CSSToolDlg::OnBnClickedButtonClear()
{
	m_RecieveData.Empty();
	m_ctlMsgOut.SetSel(0,-1);
	m_ctlMsgOut.ReplaceSel(L" ");
	UpdateData(FALSE);
}
