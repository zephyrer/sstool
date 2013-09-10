
// SSToolDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include"resource.h"
#include "afxcmn.h"
typedef enum
{
	COM1=0,
	COM2,
	COM3,
	COM4,
	COM5,
	COM6,
	COM7,
	COM8,
	COM9,
}COMM_LIST;

typedef enum
{
	BAUD110=0,
	BAUD300,
	BAUD600,
	BAUD1200,
	BAUD2400,
	BAUD4800,
	BAUD9600,
	BAUD14400,
	BAUD19200,
	BAUD38400,
	BAUD57600,
	BAUD115200,
	BAUD128000,
	BAUD256000,

}COM_Baudrate_t;

typedef enum
{
	DATABIT6=0,
	DATABIT7=1,
	DATABIT8=2,
}COM_DataBits_t;

// CSSToolDlg dialog
class CSSToolDlg : public CDialogEx
{
// Construction
public:
	CSSToolDlg(const CString& str,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SSTOOL_DIALOG};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;
	int	  m_iCurConn;
	int	  m_iCurBaudrate;
	int	  m_iCurDataBits;
	int	  m_iCurStopBits;
	int	  m_iCurParity;
	BOOL  m_HexSend;
	CString	m_strCaption;
	CString m_RecieveData;
	CString	m_StrCurPath;
	void InitCommParams();
	CString CommonGetCurPath(); 
	BOOL Char2Hex(TCHAR *Buffer,TCHAR *szOut,int iLen);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void  OutMsg(CString strMsg);
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonHex();
	afx_msg void OnBnClickedButtonCon();
	CComboBox	m_ctrComList;
	CTabCtrl	m_TabItem;
	CComboBox	m_cbBaudrate;
	CComboBox	m_cbStopBits;
	CComboBox	m_cbDataBits;
	CComboBox	m_cbParity;
	CEdit		m_ctlMsgOut;
	CEdit		m_mSend;
	CButton		m_connBtn;
	CButton		m_hexBtn;
	CEdit		m_sndTimer;
	CButton		m_hexSnd;
	CButton		m_scSnd;
	CButton		m_SendBtn;
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckHexsend();
	afx_msg void OnBnClickedCheckScSend();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
};