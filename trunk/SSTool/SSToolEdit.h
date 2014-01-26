#pragma once


// CSSToolEdit

class CSSToolEdit : public CEdit
{
	DECLARE_DYNAMIC(CSSToolEdit)

public:
	CSSToolEdit();
	virtual ~CSSToolEdit();

protected:
	CBrush   m_bkBrush;
    COLORREF m_bkColor;
    COLORREF m_txtColor;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


