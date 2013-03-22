// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

    m_LogEdit.Attach(GetDlgItem(IDC_EDIT_LOG));
    UpdateUIState();

	return TRUE;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnThreadWorking(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CString str;
    str.Format(TEXT("Thread is running... %d"), wParam);
    AddLog(str);

    return 0;
}

LRESULT CMainDlg::OnThreadBeforeTerm(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    AddLog(TEXT("CThread::BeforeTerminate() is called."));
    return 0;
}

LRESULT CMainDlg::OnThreadBeforeKill(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    AddLog(TEXT("CThread::BeforeKill() is called."));
    return 0;
}

LRESULT CMainDlg::OnThreadDeleted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    AddLog(TEXT("CThread object is deleted."));
    m_pThread = NULL;

    UpdateUIState();
    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_pThread = new CMyThread(m_hWnd);
    m_pThread->Run();

    UpdateUIState();

    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonTerminate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (m_pThread != NULL)
        m_pThread->Terminate();

    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonKill(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (m_pThread != NULL)
        m_pThread->Kill();

    return 0;
}

void CMainDlg::AddLog(LPCTSTR lpszStr)
{
    CString str(lpszStr);
    str += TEXT("\r\n");

    m_LogEdit.SetSel(MAXLONG, MAXLONG, 0);
    m_LogEdit.ReplaceSel(str);
}

void CMainDlg::UpdateUIState()
{
    bool bIsRunning = (m_pThread != NULL);

    GetDlgItem(IDC_BUTTON_RUN).EnableWindow(!bIsRunning);
    GetDlgItem(IDC_BUTTON_TERMINATE).EnableWindow(bIsRunning);
    GetDlgItem(IDC_BUTTON_KILL).EnableWindow(bIsRunning);
}
