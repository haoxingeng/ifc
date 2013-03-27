#pragma once
#include "afxdlgs.h"

#include "AppPropPages.h"

class CAppPropSheet : public CPropertySheet
{
private:
    CCommaTextPropPage m_CommaTextPage;
    CIndexOfPropPage m_IndexOfPropPage;
    CSortPropPage m_SortPropPage;
    CLoadSavePropPage m_LoadSavePropPage;
public:
    CAppPropSheet();
    virtual ~CAppPropSheet();

    virtual void PreSubclassWindow();
	virtual BOOL OnInitDialog();
};
