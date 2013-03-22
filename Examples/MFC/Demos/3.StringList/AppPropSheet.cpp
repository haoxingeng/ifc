#include "StdAfx.h"
#include "AppPropSheet.h"

CAppPropSheet::CAppPropSheet()
{
    AddPage(&m_CommaTextPage);
    AddPage(&m_IndexOfPropPage);
    AddPage(&m_SortPropPage);
    AddPage(&m_LoadSavePropPage);
}

CAppPropSheet::~CAppPropSheet()
{
}

void CAppPropSheet::PreSubclassWindow()
{
    CPropertySheet::PreSubclassWindow();
    ModifyStyle(0, WS_MINIMIZEBOX);
}

BOOL CAppPropSheet::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();

    // Button: IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP
    
    GetDlgItem(ID_APPLY_NOW)->ShowWindow(FALSE);
    GetDlgItem(IDOK)->ShowWindow(FALSE);
    GetDlgItem(IDHELP)->ShowWindow(FALSE);

    const int nSpacing = 6;  // space between two buttons...
    CRect rectBtn;
    CWnd *pBtn = GetDlgItem(IDCANCEL);
    
    pBtn->GetWindowRect(rectBtn);

    ScreenToClient (&rectBtn);
    int btnWidth = rectBtn.Width();
    rectBtn.left = rectBtn.left + (btnWidth + nSpacing) * 2;
    rectBtn.right = rectBtn.right + (btnWidth + nSpacing) * 2;

    pBtn->MoveWindow(rectBtn);
    pBtn->SetWindowText(TEXT("Close"));

    // Set window icon
    HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    SetIcon(hIcon, TRUE);       // Set big icon
    SetIcon(hIcon, FALSE);      // Set small icon

    // Set window title
    SetWindowText(TEXT("StringListDemo"));

    return bResult;
}
