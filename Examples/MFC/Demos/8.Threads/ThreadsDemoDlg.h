// ThreadsDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "MyThread.h"

// CThreadsDemoDlg dialog
class CThreadsDemoDlg : public CDialog
{
// Construction
public:
	CThreadsDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_THREADSDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    CMyThread *m_pThread;
private:
    void AddLog(LPCTSTR lpszStr);
    void UpdateUIState();
public:
    CEdit m_LogEdit;
public:
    LRESULT OnThreadWorking(WPARAM wParam, LPARAM lParam);
    LRESULT OnThreadBeforeTerm(WPARAM wParam, LPARAM lParam);
    LRESULT OnThreadBeforeKill(WPARAM wParam, LPARAM lParam);
    LRESULT OnThreadDeleted(WPARAM wParam, LPARAM lParam);

    afx_msg void OnBnClickedButtonRun();
    afx_msg void OnBnClickedButtonTerminate();
    afx_msg void OnBnClickedButtonKill();
};
