// StringListDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "StringListDemo.h"
#include "AppPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStringListDemoApp

BEGIN_MESSAGE_MAP(CStringListDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStringListDemoApp construction

CStringListDemoApp::CStringListDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CStringListDemoApp object

CStringListDemoApp theApp;


// CStringListDemoApp initialization

BOOL CStringListDemoApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

    CAppPropSheet dlg;
    m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
