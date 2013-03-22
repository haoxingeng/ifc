#pragma once

#include "Resource.h"
#include "afxwin.h"

///////////////////////////////////////////////////////////////////////////////

class CCommaTextPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCommaTextPropPage)

public:
	CCommaTextPropPage();
	virtual ~CCommaTextPropPage();

	enum { IDD = IDD_PROPPAGE_COMMATEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    CEdit m_Edit1;
    CEdit m_Edit2;
    CEdit m_CommaTextEdit;
public:
    afx_msg void OnBnClickedButtonGetCommatext();
    afx_msg void OnBnClickedButtonSetCommatext();
};

///////////////////////////////////////////////////////////////////////////////

class CIndexOfPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CIndexOfPropPage)

public:
	CIndexOfPropPage();
	virtual ~CIndexOfPropPage();

	enum { IDD = IDD_PROPPAGE_INDEXOF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    CEdit m_TextEdit;
    CEdit m_IndexOfParamEdit;
    CEdit m_IndexOfNameParamEdit;
    CEdit m_IndexOfResultEdit;
    CEdit m_IndexOfNameResultEdit;
public:
    afx_msg void OnBnClickedButtonIndexof();
    afx_msg void OnBnClickedButtonIndexofname();
};

///////////////////////////////////////////////////////////////////////////////

class CSortPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSortPropPage)

public:
	CSortPropPage();
	virtual ~CSortPropPage();

	enum { IDD = IDD_PROPPAGE_SORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    CEdit m_TextEdit;
public:
    afx_msg void OnBnClickedButtonSort();
};

///////////////////////////////////////////////////////////////////////////////

class CLoadSavePropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLoadSavePropPage)

public:
	CLoadSavePropPage();
	virtual ~CLoadSavePropPage();

	enum { IDD = IDD_PROPPAGE_LOADSAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_TextEdit;
public:
    afx_msg void OnBnClickedButtonLoad();
    afx_msg void OnBnClickedButtonSave();
};
