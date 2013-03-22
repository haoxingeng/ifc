#pragma once

#include "Resource.h"

///////////////////////////////////////////////////////////////////////////////

class CCommaTextPropPage : public CPropertyPageImpl<CCommaTextPropPage>
{
public:
    enum { IDD = IDD_PROPPAGE_COMMATEXT };
public:
    CEdit m_Edit1;
    CEdit m_Edit2;
    CEdit m_CommaTextEdit;
public:
    CCommaTextPropPage() {}
    ~CCommaTextPropPage() {}

    BEGIN_MSG_MAP(CCommaTextPropPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(IDC_BUTTON_GET_COMMATEXT, BN_CLICKED, OnBnClickedButtonGetCommatext)
        COMMAND_HANDLER(IDC_BUTTON_SET_COMMATEXT, BN_CLICKED, OnBnClickedButtonSetCommatext)
        CHAIN_MSG_MAP(CPropertyPageImpl<CCommaTextPropPage>)
    END_MSG_MAP()

    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
    LRESULT OnBnClickedButtonGetCommatext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonSetCommatext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

///////////////////////////////////////////////////////////////////////////////

class CIndexOfPropPage : public CPropertyPageImpl<CIndexOfPropPage>
{
public:
    enum { IDD = IDD_PROPPAGE_INDEXOF };
public:
    CEdit m_TextEdit;
    CEdit m_IndexOfParamEdit;
    CEdit m_IndexOfNameParamEdit;
    CEdit m_IndexOfResultEdit;
    CEdit m_IndexOfNameResultEdit;
public:
    CIndexOfPropPage() {}
    ~CIndexOfPropPage() {}

    BEGIN_MSG_MAP(CIndexOfPropPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(IDC_BUTTON_INDEXOF, BN_CLICKED, OnBnClickedButtonIndexof)
        COMMAND_HANDLER(IDC_BUTTON_INDEXOFNAME, BN_CLICKED, OnBnClickedButtonIndexofname)
        CHAIN_MSG_MAP(CPropertyPageImpl<CIndexOfPropPage>)
    END_MSG_MAP()

    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
    LRESULT OnBnClickedButtonIndexof(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonIndexofname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

///////////////////////////////////////////////////////////////////////////////

class CSortPropPage : public CPropertyPageImpl<CSortPropPage>
{
public:
    enum { IDD = IDD_PROPPAGE_SORT };
public:
    CEdit m_TextEdit;
public:
    CSortPropPage() {}
    ~CSortPropPage() {}

    BEGIN_MSG_MAP(CSortPropPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(IDC_BUTTON_SORT, BN_CLICKED, OnBnClickedButtonSort)
        CHAIN_MSG_MAP(CPropertyPageImpl<CSortPropPage>)
    END_MSG_MAP()

    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
    LRESULT OnBnClickedButtonSort(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

///////////////////////////////////////////////////////////////////////////////

class CLoadSavePropPage : public CPropertyPageImpl<CLoadSavePropPage>
{
public:
    enum { IDD = IDD_PROPPAGE_LOADSAVE };
public:
    CEdit m_TextEdit;
public:
    CLoadSavePropPage() {}
    ~CLoadSavePropPage() {}

    BEGIN_MSG_MAP(CLoadSavePropPage)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER(IDC_BUTTON_LOAD, BN_CLICKED, OnBnClickedButtonLoad)
        COMMAND_HANDLER(IDC_BUTTON_SAVE, BN_CLICKED, OnBnClickedButtonSave)
        CHAIN_MSG_MAP(CPropertyPageImpl<CLoadSavePropPage>)
    END_MSG_MAP()

    BOOL OnInitDialog(HWND hwndFocus, LPARAM lParam);
    LRESULT OnBnClickedButtonLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
