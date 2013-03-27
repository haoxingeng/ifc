#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HyperLink.h"
#include "IrisCodeCounter.h"
#include "Global.h"

class CAboutDlg : public CDialog
{
public:
    enum { IDD = IDD_ABOUTBOX };
private:
    CFont m_AppNameFont;
    CStatic m_EmailIcon;
    CStatic m_HomepageIcon;
    CHyperLink m_EmailLink;
    CHyperLink m_HomepageLink;
protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
protected:
    afx_msg void OnBnClickedButtonAboutOk();
    DECLARE_MESSAGE_MAP()
public:
    CAboutDlg();
};
