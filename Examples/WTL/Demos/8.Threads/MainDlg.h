// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MyThread.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
    CMainDlg() : m_pThread(NULL) {}
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

        MESSAGE_HANDLER(WM_THREAD_WORKING, OnThreadWorking)
		MESSAGE_HANDLER(WM_THREAD_BEFORE_TERM, OnThreadBeforeTerm)
		MESSAGE_HANDLER(WM_THREAD_BEFORE_KILL, OnThreadBeforeKill)
		MESSAGE_HANDLER(WM_THREAD_DELETED, OnThreadDeleted)

        COMMAND_HANDLER(IDC_BUTTON_RUN, BN_CLICKED, OnBnClickedButtonRun)
        COMMAND_HANDLER(IDC_BUTTON_TERMINATE, BN_CLICKED, OnBnClickedButtonTerminate)
        COMMAND_HANDLER(IDC_BUTTON_KILL, BN_CLICKED, OnBnClickedButtonKill)
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnThreadWorking(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnThreadBeforeTerm(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnThreadBeforeKill(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnThreadDeleted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnBnClickedButtonRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonTerminate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonKill(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
private:
    CMyThread *m_pThread;
    CEdit m_LogEdit;
private:
    void AddLog(LPCTSTR lpszStr);
    void UpdateUIState();
};
