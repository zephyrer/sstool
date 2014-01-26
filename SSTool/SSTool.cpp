
// SSTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SSTool.h"
#include "SSToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSSToolApp

BEGIN_MESSAGE_MAP(CSSToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSSToolApp construction

CSSToolApp::CSSToolApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CSSToolApp theApp;

BOOL CSSToolApp::InitInstance()
{

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CSSToolDlg dlg(L"SSTool:zjl@hikeen.com.cn");
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		
	}
	else if (nResponse == IDCANCEL)
	{
	
	}
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	return FALSE;
}

