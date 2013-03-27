#include "StdAfx.h"
#include "AboutDlg.h"
#include "ifc.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // IDC_STATIC_APPNAME 的字体变粗 
    CStatic *pStatic = (CStatic*)GetDlgItem(IDC_STATIC_APPNAME);
    LOGFONT LogFont;
    pStatic->GetFont()->GetLogFont(&LogFont);
    LogFont.lfWeight = FW_BOLD;
    m_AppNameFont.CreateFontIndirect(&LogFont);
    pStatic->SetFont(&m_AppNameFont);

    // 设置文本
    pStatic = (CStatic*)GetDlgItem(IDC_STATIC_APPNAME);
    pStatic->SetWindowText(FormatString(TEXT("%s v%s"), APP_NAME, APP_VERSION));

    // 创建图标
    m_EmailIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_ICON, CRect(62, 86, 0, 0), this);
    HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
        MAKEINTRESOURCE(IDI_ICON_EMAIL), IMAGE_ICON, 16, 16, 0);   
    m_EmailIcon.SetIcon(hIcon);

    m_HomepageIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_ICON, CRect(62, 105, 0, 0), this);
    hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
        MAKEINTRESOURCE(IDI_ICON_HOMEPAGE), IMAGE_ICON, 16, 16, 0);   
    m_HomepageIcon.SetIcon(hIcon);

    // 设置超链接
    m_EmailLink.SetWindowText(APP_EMAIL);
    m_EmailLink.SetUrl(CString(TEXT("mailto:")) + APP_EMAIL);
    m_HomepageLink.SetWindowText(APP_HOMEPAGE);
    m_HomepageLink.SetUrl(APP_HOMEPAGE);

    return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_EMAIL, m_EmailLink);
    DDX_Control(pDX, IDC_STATIC_HOMEPAGE, m_HomepageLink);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ABOUT_OK, &CAboutDlg::OnBnClickedButtonAboutOk)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedButtonAboutOk()
{
    OnOK();
}
