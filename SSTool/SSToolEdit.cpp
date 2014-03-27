// SSToolEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolEdit.h"

IMPLEMENT_DYNAMIC(CSSToolEdit, CEdit)

CSSToolEdit::CSSToolEdit()
{
	m_txtColor =	RGB(255,255,255);
    m_bkColor  =	RGB(31,31,31);
    m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
	m_RecieveData = L"";
	m_showFont.CreateFont(15,9,0,0,100,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Courier New");
}
CSSToolEdit::~CSSToolEdit()
{

}
BEGIN_MESSAGE_MAP(CSSToolEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


HBRUSH CSSToolEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor( m_bkColor);
	pDC->SetTextColor(m_txtColor);
	return (HBRUSH)m_bkBrush;
}

void CSSToolEdit::EnableMonospacedFont(BOOL bEnable)
{
	if(bEnable)
	{
		this->SetFont(&m_showFont,FALSE);
	}
}
void CSSToolEdit::SetColorMode(int nMode)
{
	switch(nMode)
	{
	case BLACK_BACK_WHITE_FONT:
		{
			m_txtColor =	RGB(255,255,255);
			m_bkColor  =	RGB(31,31,31);
			m_bkBrush.DeleteObject();
			m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
		}
		break;
	case BLACK_BACK_GREEN_FONT:
		{
			m_txtColor =	RGB(80,240,90);
			m_bkColor  =	RGB(31,31,31);
			m_bkBrush.DeleteObject();
			m_bkBrush.CreateSolidBrush( RGB(31,31,31) );
		}
		break;
	case WHITE_BACK_BLACK_FONT:
		{
			m_txtColor =	RGB(0,0,0);
			m_bkColor  =	RGB(255,255,255);
			m_bkBrush.DeleteObject();
			m_bkBrush.CreateSolidBrush( RGB(255,255,255) );
		}
		break;
	default:
		break;
	}
	InvalidateRect(FALSE);
	UpdateWindow();
}
CString CSSToolEdit::AppGetCurPath()  
{   
   CString    sPath;   
   GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
   sPath.ReleaseBuffer();   
   int    nPos;   
   nPos=sPath.ReverseFind('\\');   
   sPath=sPath.Left(nPos);   
   return    sPath;   
}
void CSSToolEdit::EnterWorkPath()
{
	int		nLength=0;
	CString strPath;
	strPath.Empty();
	strPath=AppGetCurPath();
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
void  CSSToolEdit::OutMsg(CString strMsg,BOOL bEnableHex)
{
	int iLen=0;
	char szbuf[MAX_PATH];
	CString strPath;
	if(this->GetLineCount()>((bEnableHex==TRUE)?MAX_HEX_LINE:MAX_LINE_SHOW))
	{
		CFile m_CCacheFile;
		EnterWorkPath();
		strPath.Empty();
		strPath=AppGetCurPath();
		strPath+=CACHE_FILE_NAME;
		sprintf(szbuf,"%s",strPath);
		if((access(szbuf,0)==-1))
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeCreate| CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"»º´æÊ§°Ü£¡");
				return;
			}
			DWORD FileAttr = GetFileAttributes(strPath);
			SetFileAttributes(strPath,FileAttr | FILE_ATTRIBUTE_HIDDEN);
		}
		else
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"»º´æÊ§°Ü£¡");
				return;
			}
		}
		m_CCacheFile.SeekToEnd();
		m_CCacheFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

		m_CCacheFile.Flush();
		m_CCacheFile.Close();

		m_RecieveData.Empty();
		this->SetSel(0,-1);
		this->ReplaceSel(L" ");
	}
	iLen=this->GetWindowTextLengthW();
	this->SetSel(iLen,iLen);
	this->ReplaceSel((LPCTSTR)strMsg);
	iLen+=strMsg.GetLength();

	m_RecieveData+=strMsg;
}

void  CSSToolEdit::WriteMessage(CString strMsg,BOOL bEnableHex)
{
	int iLen=0;
	char szbuf[MAX_PATH];
	CString strPath;
	if(this->GetLineCount()>((bEnableHex==TRUE)?MAX_HEX_LINE:MAX_LINE_SHOW))
	{
		CFile m_CCacheFile;
		EnterWorkPath();
		strPath.Empty();
		strPath=AppGetCurPath();
		strPath+=CACHE_FILE_NAME;
		sprintf(szbuf,"%s",strPath);
		if((access(szbuf,0)==-1))
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeCreate| CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"»º´æÊ§°Ü£¡");
				return;
			}
			DWORD FileAttr = GetFileAttributes(strPath);
			SetFileAttributes(strPath,FileAttr | FILE_ATTRIBUTE_HIDDEN);
		}
		else
		{
			if(!m_CCacheFile.Open(strPath,CFile::modeWrite|CFile::modeNoTruncate))
			{
				AfxMessageBox(L"»º´æÊ§°Ü£¡");
				return;
			}
		}
		m_CCacheFile.SeekToEnd();
		m_CCacheFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

		m_CCacheFile.Flush();
		m_CCacheFile.Close();

		m_RecieveData.Empty();
		this->SetSel(0,-1);
		this->ReplaceSel(L" ");
	}
	iLen=this->GetWindowTextLengthW();
	this->SetSel(iLen,iLen);
	this->ReplaceSel((LPCTSTR)strMsg);
	iLen+=strMsg.GetLength();

	m_RecieveData+=strMsg;
}

void CSSToolEdit::ClearData()
{
	CString strPath;
	strPath.Empty();
	m_RecieveData.Empty();
	this->SetSel(0,-1);
	this->ReplaceSel(L"");

	strPath=AppGetCurPath();
	strPath+=CACHE_FILE_NAME;
	DeleteFile(strPath);
	UpdateData(FALSE);
}
BOOL CSSToolEdit::SaveDataToFile()
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
		AfxMessageBox(L"Ã»ÓÐÊý¾Ý£¬±£´æÊ§°Ü£¡");
		return 0;
	}
	EnterWorkPath();
	tt=CTime::GetCurrentTime();

	sprintf(szbuf,"SSToolText_%d-%d-%d-%d-%d-%d.txt",
		tt.GetYear(),tt.GetMonth(),tt.GetDay(),tt.GetHour(),tt.GetMinute(),tt.GetSecond());

	m_strTextFile=szbuf;
	strSubSrcPath=AppGetCurPath()+CACHE_FILE_NAME;
	strSubDestPath=AppGetCurPath()+L"\\"+m_strTextFile;

	if(GetFileAttributes(strSubSrcPath)!=0xFFFFFFFF)
	{
		CopyFile(strSubSrcPath,strSubDestPath,FALSE);
		DWORD FileAttr = GetFileAttributes(strSubDestPath);
		SetFileAttributes(strSubDestPath,FileAttr | FILE_ATTRIBUTE_NORMAL);

		if(!m_CFile.Open(m_strTextFile,CFile::modeWrite))
		{
			AfxMessageBox(L"´´½¨ÎÄ¼þÊ§°Ü£¡");
			return 0;
		}
	}
	else
	{
		if(!m_CFile.Open(m_strTextFile,CFile::modeCreate|CFile::modeWrite))
		{
			AfxMessageBox(L"´´½¨ÎÄ¼þÊ§°Ü£¡");
			return 0;
		}
	}
	m_CFile.SeekToEnd();
	m_CFile.Write((LPCTSTR)m_RecieveData,m_RecieveData.GetLength()*sizeof(TCHAR));

	m_CFile.Flush();
	m_CFile.Close();

	MessageBox(_T("ÒÑ±£´æ!"),NULL, MB_OK);

	return TRUE;
}

void CSSToolEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CEdit::OnLButtonDown(nFlags, point);
}
