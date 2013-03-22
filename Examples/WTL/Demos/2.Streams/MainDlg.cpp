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
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnBnClickedButtonMemoryStream(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CMemoryStream stream;

    for (int i = 0; i < 10; i++)
        stream.Write(&i, sizeof(int));

    stream.SaveToFile(GetAppPath() + TEXT("TestMemoryStream.dat"));
    ShowMessage(TEXT("OK!"));

    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonFileStream(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString strFileName = GetAppPath() + TEXT("TestFileStream.dat");
    CFileStream stream;

    if (stream.Open(strFileName, FM_CREATE))
    {
        for (int i = 0; i < 10; i++)
            stream.Write(&i, sizeof(int));
        
        ShowMessage(TEXT("OK!"));
    }

    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonResourceStream(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CResourceStream stream(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), RT_GROUP_ICON);
    stream.SaveToFile(GetAppPath() + TEXT("TestResourceStream.dat"));
    ShowMessage(TEXT("OK!"));

    return 0;
}
