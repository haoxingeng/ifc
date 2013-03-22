#pragma once

#include "AppPropPages.h"

class CAppPropSheet : public CPropertySheetImpl<CAppPropSheet>
{
private:
    CCommaTextPropPage m_CommaTextPage;
    CIndexOfPropPage m_IndexOfPropPage;
    CSortPropPage m_SortPropPage;
    CLoadSavePropPage m_LoadSavePropPage;
public:
    CAppPropSheet();
    virtual ~CAppPropSheet();

    BEGIN_MSG_MAP(CAppPropSheet)
		MSG_WM_SHOWWINDOW(OnShowWindow)
        CHAIN_MSG_MAP(CPropertySheetImpl<CAppPropSheet>)
    END_MSG_MAP()

    void OnSheetInitialized();
	static int CALLBACK PropSheetCallback(HWND hWnd, UINT uMsg, LPARAM lParam);

	void OnShowWindow(BOOL, int) 
    {
		CenterWindow(GetDesktopWindow());
		SetMsgHandled(FALSE);
	}
};
