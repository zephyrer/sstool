// SSToolEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolEdit.h"

// CSSToolEdit

IMPLEMENT_DYNAMIC(CSSToolEdit, CEdit)

CSSToolEdit::CSSToolEdit()
{
	m_txtColor =	RGB(255,255,255);
    m_bkColor  =	RGB(31,31,31);
    m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
}

CSSToolEdit::~CSSToolEdit()
{
}

BEGIN_MESSAGE_MAP(CSSToolEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


HBRUSH CSSToolEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor( m_bkColor);
	pDC->SetTextColor(m_txtColor);	
	return (HBRUSH)m_bkBrush;
}
void CSSToolEdit::SetColorMode(int nMode)
{
	switch(nMode)
	{
	case BLACK_BACK_WHITE_FONT:
		m_txtColor =	RGB(255,255,255);
		m_bkColor  =	RGB(31,31,31);
		m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
		break;
	case BLACK_BACK_GREEN_FONT:
		m_txtColor =	RGB(80,240,90);
		m_bkColor  =	RGB(31,31,31);
		m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
		break;
	case WHITE_BACK_BLACK_FONT:
		m_txtColor =	RGB(0,0,0);
		m_bkColor  =	RGB(255,255,255);
		m_bkBrush.CreateSolidBrush( RGB(255,255,255) );
		break;
	default:
		break;
	}
	this->InvalidateRect(FALSE);
	this->UpdateWindow();
}