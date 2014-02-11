#pragma once

#define BLACK_BACK_WHITE_FONT	1
#define BLACK_BACK_GREEN_FONT	2
#define WHITE_BACK_BLACK_FONT	3

class CSSToolEdit : public CEdit
{
	DECLARE_DYNAMIC(CSSToolEdit)

public:
	CSSToolEdit();
	virtual ~CSSToolEdit();
	void SetColorMode(int nMode);

protected:
	CBrush   m_bkBrush;
    COLORREF m_bkColor;
    COLORREF m_txtColor;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


