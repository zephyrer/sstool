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
