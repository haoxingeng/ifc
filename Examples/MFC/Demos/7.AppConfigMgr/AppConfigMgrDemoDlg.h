// AppConfigMgrDemoDlg.h : header file
//

#pragma once

// CAppConfigMgrDemoDlg dialog
class CAppConfigMgrDemoDlg : public CDialog
{
// Construction
public:
	CAppConfigMgrDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_APPCONFIGMGRDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};
