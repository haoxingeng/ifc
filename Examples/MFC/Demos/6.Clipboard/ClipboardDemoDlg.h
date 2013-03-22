// ClipboardDemoDlg.h : header file
//

#pragma once


// CClipboardDemoDlg dialog
class CClipboardDemoDlg : public CDialog
{
// Construction
public:
	CClipboardDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIPBOARDDEMO_DIALOG };

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
public:
    afx_msg void OnBnClickedButtonCopy();
    afx_msg void OnBnClickedButtonPaste();
};
