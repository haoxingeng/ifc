// IrisCodeCounter.cpp :
//

#include "stdafx.h"
#include "IrisCodeCounter.h"
#include "IrisCodeCounterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIrisCodeCounterApp

BEGIN_MESSAGE_MAP(CIrisCodeCounterApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CIrisCodeCounterApp theApp;

// CIrisCodeCounterApp Init

BOOL CIrisCodeCounterApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    AfxEnableControlContainer();

    CIrisCodeCounterDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    return FALSE;
}
