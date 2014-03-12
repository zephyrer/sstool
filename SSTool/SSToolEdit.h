#pragma once
#include "io.h"

#define BLACK_BACK_WHITE_FONT	0
#define BLACK_BACK_GREEN_FONT	1
#define WHITE_BACK_BLACK_FONT	2

#define MAX_LINE_SHOW		3000
#define MAX_HEX_LINE		8000
#define CACHE_FILE_NAME		L"\\sstool_cache.txt"

class CSSToolEdit : public CEdit
{
	DECLARE_DYNAMIC(CSSToolEdit)

public:
	CSSToolEdit();
	virtual ~CSSToolEdit();
	void SetColorMode(int nMode);
	void OutMsg(CString strMsg,BOOL bEnableHex=FALSE);
	void WriteMessage(CString strMsg,BOOL bEnableHex=FALSE);
	void ClearData();
	void EnableMonospacedFont(BOOL bEnable);
	BOOL SaveDataToFile();
protected:
	CFont	 m_showFont;
	CBrush   m_bkBrush;
    COLORREF m_bkColor;
    COLORREF m_txtColor;
	CString  m_RecieveData;
	CString  AppGetCurPath();  
	void EnterWorkPath();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


