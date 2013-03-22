#include "StdAfx.h"
#include "AppPropSheet.h"

CAppPropSheet::CAppPropSheet()
    : CPropertySheetImpl<CAppPropSheet>(TEXT("Demo"))
{
    m_psh.dwFlags |= PSH_NOAPPLYNOW;

    AddPage(m_CommaTextPage);
    AddPage(m_IndexOfPropPage);
    AddPage(m_SortPropPage);
    AddPage(m_LoadSavePropPage);
}

CAppPropSheet::~CAppPropSheet()
{
}

void CAppPropSheet::OnSheetInitialized()
{
    ModifyStyleEx(WS_EX_CONTEXTHELP, 0, SWP_FRAMECHANGED);

    // set icons
    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    GetDlgItem(IDOK).ShowWindow(FALSE);
    GetDlgItem(IDCANCEL).SetWindowText(TEXT("Close"));
}

int CALLBACK CAppPropSheet::PropSheetCallback(HWND hWnd, UINT uMsg, LPARAM lParam)
{
    if (uMsg == PSCB_PRECREATE)
    { 
        LPDLGTEMPLATE Template = (LPDLGTEMPLATE)lParam;
        Template->style |=  WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        return 0;
    }

    return CPropertySheetImpl<CAppPropSheet>::PropSheetCallback(hWnd, uMsg, lParam);
}
