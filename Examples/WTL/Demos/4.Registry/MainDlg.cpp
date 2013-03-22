// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include "ifc.h"

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

	return TRUE;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CRegistry Reg;
    CString strKey;

    Reg.SetRootKey(HKEY_CURRENT_USER);

    strKey = TEXT("\\RegistryDemo\\Test");
    if (Reg.OpenKey(strKey, true))
    {
        // Write values
        Reg.WriteString(TEXT("str"), TEXT("this is a string."));
        Reg.WriteInteger(TEXT("int"), 12345);

        // Read values
        CString strValue = Reg.ReadString(TEXT("str"));
        int nValue = Reg.ReadInteger(TEXT("int"));

        ATLASSERT(strValue == TEXT("this is a string."));
        ATLASSERT(nValue == 12345);

        Reg.DeleteKey(TEXT("\\RegistryDemo"));
        ShowMessage(TEXT("OK!"));
    }
    else
    {
        ShowMessage(TEXT("Can not open the key!"));
    }

    return 0;
}
