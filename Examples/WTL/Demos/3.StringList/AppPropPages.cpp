// StringListDemoPropPages.cpp
//

#include "stdafx.h"
#include "StringListDemo.h"
#include "AppPropPages.h"

#include "ifc.h"

///////////////////////////////////////////////////////////////////////////////

BOOL CCommaTextPropPage::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
    m_Edit1.Attach(GetDlgItem(IDC_EDIT1));
    m_Edit2.Attach(GetDlgItem(IDC_EDIT2));
    m_CommaTextEdit.Attach(GetDlgItem(IDC_EDIT_COMMA_TEXT));

	// Init the text of m_Edit1
    CStrList StrList;
    StrList.Add(TEXT("abc"));
    StrList.Add(TEXT("123,4"));
    StrList.Add(TEXT("x y z"));
    StrList.Add(TEXT("a=b"));
    m_Edit1.SetWindowText(StrList.GetText());

    return TRUE;
}

LRESULT CCommaTextPropPage::OnBnClickedButtonGetCommatext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CStrList StrList;
    CString strEditText, strCommaText;

    m_Edit1.GetWindowText(strEditText);
    StrList.SetText(strEditText);
    strCommaText = StrList.GetCommaText();
    
    m_CommaTextEdit.SetWindowText(strCommaText);

    return 0;
}

LRESULT CCommaTextPropPage::OnBnClickedButtonSetCommatext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CStrList StrList;
    CString strCommaText;

    m_CommaTextEdit.GetWindowText(strCommaText);
    StrList.SetCommaText(strCommaText);

    m_Edit2.SetWindowText(StrList.GetText());

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CIndexOfPropPage::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
    m_TextEdit.Attach(GetDlgItem(IDC_EDIT_TEXT));
    m_IndexOfParamEdit.Attach(GetDlgItem(IDC_EDIT_INDEXOF_PARAM));
    m_IndexOfNameParamEdit.Attach(GetDlgItem(IDC_EDIT_INDEXOFNAME_PARAM));
    m_IndexOfResultEdit.Attach(GetDlgItem(IDC_EDIT_INDEXOF_RESULT));
    m_IndexOfNameResultEdit.Attach(GetDlgItem(IDC_EDIT_INDEXOFNAME_RESULT));

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


LRESULT CIndexOfPropPage::OnBnClickedButtonIndexof(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString strText;
    m_TextEdit.GetWindowText(strText);
    CString strParam;
    m_IndexOfParamEdit.GetWindowText(strParam);

    CStrList StrList;
    StrList.SetText(strText);

    int nIndex = StrList.IndexOf(strParam);
    m_IndexOfResultEdit.SetWindowText(IntToStr(nIndex));

    return 0;
}

LRESULT CIndexOfPropPage::OnBnClickedButtonIndexofname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString strText;
    m_TextEdit.GetWindowText(strText);
    CString strParam;
    m_IndexOfNameParamEdit.GetWindowText(strParam);

    CStrList StrList;
    StrList.SetText(strText);

    int nIndex = StrList.IndexOfName(strParam);
    m_IndexOfNameResultEdit.SetWindowText(IntToStr(nIndex));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CSortPropPage::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
    m_TextEdit.Attach(GetDlgItem(IDC_EDIT_TEXT));

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

LRESULT CSortPropPage::OnBnClickedButtonSort(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString strText;
    m_TextEdit.GetWindowText(strText);

    CStrList StrList;
    StrList.SetText(strText);

    StrList.SetSorted(true);

    m_TextEdit.SetWindowText(StrList.GetText());

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CLoadSavePropPage::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
    m_TextEdit.Attach(GetDlgItem(IDC_EDIT_TEXT));
    return TRUE;
}

LRESULT CLoadSavePropPage::OnBnClickedButtonLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    const int MAX_BUFFER_SIZE = 1024;

    CFileDialog FileDialog(TRUE);
    CString strBuffer;

    FileDialog.m_ofn.lpstrFile = strBuffer.GetBuffer(MAX_BUFFER_SIZE);
    FileDialog.m_ofn.nMaxFile = MAX_BUFFER_SIZE;
    FileDialog.m_ofn.lpstrDefExt = TEXT(".txt");
    FileDialog.m_ofn.lpstrFilter = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");
    FileDialog.m_ofn.Flags |= OFN_FILEMUSTEXIST;

    INT_PTR nRet = FileDialog.DoModal();
    strBuffer.ReleaseBuffer();

    if (nRet == IDOK)
    {
        CString strFileName = strBuffer;
        
        CStrList StrList;
        StrList.LoadFromFile(strFileName, false);

        m_TextEdit.SetWindowText(StrList.GetText());
    }

    return 0;
}

LRESULT CLoadSavePropPage::OnBnClickedButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    const int MAX_BUFFER_SIZE = 1024;

    CFileDialog FileDialog(FALSE);
    CString strBuffer;

    FileDialog.m_ofn.lpstrFile = strBuffer.GetBuffer(MAX_BUFFER_SIZE);
    FileDialog.m_ofn.nMaxFile = MAX_BUFFER_SIZE;
    FileDialog.m_ofn.lpstrDefExt = TEXT(".txt");
    FileDialog.m_ofn.lpstrFilter = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");

    INT_PTR nRet = FileDialog.DoModal();
    strBuffer.ReleaseBuffer();

    if (nRet == IDOK)
    {
        CString strFileName = strBuffer;
        
        CString strText;
        m_TextEdit.GetWindowText(strText);

        CStrList StrList;
        StrList.SetText(strText);

        StrList.SaveToFile(strFileName, false);
    }

    return 0;
}
