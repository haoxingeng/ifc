// StreamsDemoDlg.h : header file
//

#pragma once


// CStreamsDemoDlg dialog
class CStreamsDemoDlg : public CDialog
{
// Construction
public:
	CStreamsDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STREAMSDEMO_DIALOG };

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
    afx_msg void OnBnClickedButtonMemoryStream();
    afx_msg void OnBnClickedButtonFileStream();
    afx_msg void OnBnClickedButtonResourceStream();
};
