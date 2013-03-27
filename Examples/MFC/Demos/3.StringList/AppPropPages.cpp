// AppPropPages.cpp
//

#include "stdafx.h"
#include "StringListDemo.h"
#include "AppPropPages.h"

#include "ifc.h"

///////////////////////////////////////////////////////////////////////////////
// CCommaTextPropPage

IMPLEMENT_DYNAMIC(CCommaTextPropPage, CPropertyPage)

CCommaTextPropPage::CCommaTextPropPage()
	: CPropertyPage(CCommaTextPropPage::IDD)
{
}

CCommaTextPropPage::~CCommaTextPropPage()
{
}

void CCommaTextPropPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_Edit1);
    DDX_Control(pDX, IDC_EDIT2, m_Edit2);
    DDX_Control(pDX, IDC_EDIT_COMMA_TEXT, m_CommaTextEdit);
}

BOOL CCommaTextPropPage::OnInitDialog()
{
    BOOL bResult = CPropertyPage::OnInitDialog();

	// Init the text of m_Edit1
    CStrList StrList;
    StrList.Add(TEXT("abc"));
    StrList.Add(TEXT("123,4"));
    StrList.Add(TEXT("x y z"));
    StrList.Add(TEXT("a=b"));
    m_Edit1.SetWindowText(StrList.GetText());

    return bResult;
}

BEGIN_MESSAGE_MAP(CCommaTextPropPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_GET_COMMATEXT, &CCommaTextPropPage::OnBnClickedButtonGetCommatext)
    ON_BN_CLICKED(IDC_BUTTON_SET_COMMATEXT, &CCommaTextPropPage::OnBnClickedButtonSetCommatext)
END_MESSAGE_MAP()

void CCommaTextPropPage::OnBnClickedButtonGetCommatext()
{
    CStrList StrList;
    CString strEditText, strCommaText;

    m_Edit1.GetWindowText(strEditText);
    StrList.SetText(strEditText);
    strCommaText = StrList.GetCommaText();
    
    m_CommaTextEdit.SetWindowText(strCommaText);
}

void CCommaTextPropPage::OnBnClickedButtonSetCommatext()
{
    CStrList StrList;
    CString strCommaText;

    m_CommaTextEdit.GetWindowText(strCommaText);
    StrList.SetCommaText(strCommaText);

    m_Edit2.SetWindowText(StrList.GetText());
}

///////////////////////////////////////////////////////////////////////////////
// CIndexOfPropPage

IMPLEMENT_DYNAMIC(CIndexOfPropPage, CPropertyPage)

CIndexOfPropPage::CIndexOfPropPage()
	: CPropertyPage(CIndexOfPropPage::IDD)
{
}

CIndexOfPropPage::~CIndexOfPropPage()
{
}

void CIndexOfPropPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_TEXT, m_TextEdit);
    DDX_Control(pDX, IDC_EDIT_INDEXOF_PARAM, m_IndexOfParamEdit);
    DDX_Control(pDX, IDC_EDIT_INDEXOFNAME_PARAM, m_IndexOfNameParamEdit);
    DDX_Control(pDX, IDC_EDIT_INDEXOF_RESULT, m_IndexOfResultEdit);
    DDX_Control(pDX, IDC_EDIT_INDEXOFNAME_RESULT, m_IndexOfNameResultEdit);
}

BOOL CIndexOfPropPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

	// Init the text of m_TextEdit
    CStrList StrList;
    StrList.Add(TEXT("abc"));
    StrList.Add(TEXT("123,4"));
    StrList.Add(TEXT("x y z"));
    StrList.Add(TEXT("a=b"));
    StrList.Add(TEXT("a=c"));
    StrList.Add(TEXT("b=test1"));
    StrList.Add(TEXT("str = test2"));
    StrList.Add(TEXT("str=test2"));
    StrList.Add(TEXT("string=test3"));
    m_TextEdit.SetWindowText(StrList.GetText());

    // Init m_IndexOfParamEdit
    m_IndexOfParamEdit.SetWindowText(TEXT("a=c"));

    // Init m_IndexOfNameParamEdit
    m_IndexOfNameParamEdit.SetWindowText(TEXT("str"));

    return TRUE;
}

BEGIN_MESSAGE_MAP(CIndexOfPropPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_INDEXOF, &CIndexOfPropPage::OnBnClickedButtonIndexof)
    ON_BN_CLICKED(IDC_BUTTON_INDEXOFNAME, &CIndexOfPropPage::OnBnClickedButtonIndexofname)
END_MESSAGE_MAP()

void CIndexOfPropPage::OnBnClickedButtonIndexof()
{
    CString strText;
    m_TextEdit.GetWindowText(strText);
    CString strParam;
    m_IndexOfParamEdit.GetWindowText(strParam);

    CStrList StrList;
    StrList.SetText(strText);

    int nIndex = StrList.IndexOf(strParam);
    m_IndexOfResultEdit.SetWindowText(IntToStr(nIndex));
}

void CIndexOfPropPage::OnBnClickedButtonIndexofname()
{
    CString strText;
    m_TextEdit.GetWindowText(strText);
    CString strParam;
    m_IndexOfNameParamEdit.GetWindowText(strParam);

    CStrList StrList;
    StrList.SetText(strText);

    int nIndex = StrList.IndexOfName(strParam);
    m_IndexOfNameResultEdit.SetWindowText(IntToStr(nIndex));
}

///////////////////////////////////////////////////////////////////////////////
// CSortPropPage

IMPLEMENT_DYNAMIC(CSortPropPage, CPropertyPage)

CSortPropPage::CSortPropPage()
	: CPropertyPage(CSortPropPage::IDD)
{
}

CSortPropPage::~CSortPropPage()
{
}

void CSortPropPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_TEXT, m_TextEdit);
}

BOOL CSortPropPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

	// Init the text of m_TextEdit
    CStrList StrList;
    StrList.Add(TEXT("abc"));
    StrList.Add(TEXT("x y z"));
    StrList.Add(TEXT("123,4"));
    StrList.Add(TEXT("1234567890"));
    StrList.Add(TEXT("0123456789"));
    StrList.Add(TEXT("zzZZzzzZZZ"));
    StrList.Add(TEXT("test3"));
    StrList.Add(TEXT("test2"));
    StrList.Add(TEXT("test1"));
    StrList.Add(TEXT("str1=test2"));
    StrList.Add(TEXT("str2=test1"));
    m_TextEdit.SetWindowText(StrList.GetText());

    return TRUE;
}

BEGIN_MESSAGE_MAP(CSortPropPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_SORT, &CSortPropPage::OnBnClickedButtonSort)
END_MESSAGE_MAP()

void CSortPropPage::OnBnClickedButtonSort()
{
    CString strText;
    m_TextEdit.GetWindowText(strText);

    CStrList StrList;
    StrList.SetText(strText);

    StrList.SetSorted(true);

    m_TextEdit.SetWindowText(StrList.GetText());
}

///////////////////////////////////////////////////////////////////////////////
// CLoadSavePropPage

IMPLEMENT_DYNAMIC(CLoadSavePropPage, CPropertyPage)

CLoadSavePropPage::CLoadSavePropPage()
	: CPropertyPage(CLoadSavePropPage::IDD)
{
}

CLoadSavePropPage::~CLoadSavePropPage()
{
}

void CLoadSavePropPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_TEXT, m_TextEdit);
}

BEGIN_MESSAGE_MAP(CLoadSavePropPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &CLoadSavePropPage::OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CLoadSavePropPage::OnBnClickedButtonSave)
END_MESSAGE_MAP()

void CLoadSavePropPage::OnBnClickedButtonLoad()
{
    const int MAX_BUFFER_SIZE = 1024;

    CFileDialog FileDialog(TRUE);
    CString strBuffer;

    FileDialog.GetOFN().lpstrFile = strBuffer.GetBuffer(MAX_BUFFER_SIZE);
    FileDialog.GetOFN().nMaxFile = MAX_BUFFER_SIZE;
    FileDialog.GetOFN().lpstrDefExt = TEXT(".txt");
    FileDialog.GetOFN().lpstrFilter = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");
    FileDialog.GetOFN().Flags |= OFN_FILEMUSTEXIST;

    INT_PTR nRet = FileDialog.DoModal();
    strBuffer.ReleaseBuffer();

    if (nRet == IDOK)
    {
        CString strFileName = FileDialog.GetPathName();
        
        CStrList StrList;
        StrList.LoadFromFile(strFileName, false);

        m_TextEdit.SetWindowText(StrList.GetText());
    }
}

void CLoadSavePropPage::OnBnClickedButtonSave()
{
    const int MAX_BUFFER_SIZE = 1024;

    CFileDialog FileDialog(FALSE);
    CString strBuffer;

    FileDialog.GetOFN().lpstrFile = strBuffer.GetBuffer(MAX_BUFFER_SIZE);
    FileDialog.GetOFN().nMaxFile = MAX_BUFFER_SIZE;
    FileDialog.GetOFN().lpstrDefExt = TEXT(".txt");
    FileDialog.GetOFN().lpstrFilter = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");

    INT_PTR nRet = FileDialog.DoModal();
    strBuffer.ReleaseBuffer();

    if (nRet == IDOK)
    {
        CString strFileName = FileDialog.GetPathName();
        
        CString strText;
        m_TextEdit.GetWindowText(strText);

        CStrList StrList;
        StrList.SetText(strText);

        StrList.SaveToFile(strFileName, false);
    }
}
