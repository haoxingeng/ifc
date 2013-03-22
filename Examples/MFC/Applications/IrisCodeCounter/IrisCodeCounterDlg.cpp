// IrisCodeCounterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IrisCodeCounter.h"
#include "IrisCodeCounterDlg.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// m_ListCtrl 的列号
const int LIST_COL_FILENAME         = 0;
const int LIST_COL_PATH             = 1;
const int LIST_COL_TOTAL_LINES      = 2;
const int LIST_COL_CODE_LINES       = 3;
const int LIST_COL_COMMENTS_LINES   = 4;
const int LIST_COL_BLANK_LINES      = 5;

// CIrisCodeCounterDlg 对话框

CIrisCodeCounterDlg::CIrisCodeCounterDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CIrisCodeCounterDlg::IDD, pParent),
    m_pCodeCntThread(NULL)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIrisCodeCounterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_FILETYPE, m_FileTypeComboBox);
    DDX_Control(pDX, IDC_COMBO_SRC_PATH, m_SrcPathComboBox);
    DDX_Control(pDX, IDC_LIST_RESULT, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CIrisCodeCounterDlg, CDialog)
    //{{AFX_MSG_MAP(CIrisCodeCounterDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CIrisCodeCounterDlg::OnBnClickedButtonBrowse)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_RESULT, &CIrisCodeCounterDlg::OnNMRClickListResult)
    ON_COMMAND(ID_LISTCTRL_REMOVE, &CIrisCodeCounterDlg::OnListCtrlRemove)
    ON_BN_CLICKED(IDC_BUTTON_START, &CIrisCodeCounterDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CIrisCodeCounterDlg::OnBnClickedButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CIrisCodeCounterDlg::OnBnClickedButtonOutput)
    ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CIrisCodeCounterDlg::OnBnClickedButtonAbout)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, &CIrisCodeCounterDlg::OnBnClickedButtonExit)
    ON_CBN_SELENDOK(IDC_COMBO_SRC_PATH, &CIrisCodeCounterDlg::OnCbnSelendokComboSrcdir)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_RESULT, &CIrisCodeCounterDlg::OnLvnKeydownListResult)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CIrisCodeCounterDlg::OnHdnItemclickListResult)
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_CODECNT_FINISHED_FILE, OnCodeCntFinishedFileMsg)
    ON_MESSAGE(WM_CODECNT_FINISHED_ALL, OnCodeCntFinishedAllMsg)
END_MESSAGE_MAP()

// m_ListCtrl 的排序回调函数
int CALLBACK ListCtrlSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    int nResult = 0;

    int nColumn = lParamSort;
    int nItemId1 = lParam1;
    int nItemId2 = lParam2;
    CCodeCntResultItem Item1, Item2;
    CodeCountingResult.GetItemById(nItemId1, Item1);
    CodeCountingResult.GetItemById(nItemId2, Item2);
    CString s1, s2;

    struct {
        int operator() (int v1, int v2)
        {
            if (v1 < v2) return -1;
            else if (v1 > v2) return 1;
            else return 0;
        }
    } CompareInt;

    switch(nColumn)
    {
    case LIST_COL_FILENAME: // 文件名
        s1 = ExtractFileName(Item1.strFileName);
        s2 = ExtractFileName(Item2.strFileName);
        nResult = s1.Compare(s2);
        break;

    case LIST_COL_PATH: // 路径
        s1 = ExtractFilePath(Item1.strFileName);
        s2 = ExtractFilePath(Item2.strFileName);
        nResult = s1.Compare(s2);
        break;

    case LIST_COL_TOTAL_LINES: // 总行数
        nResult = CompareInt(Item1.Values.nTotalLines, Item2.Values.nTotalLines);
        break;

    case LIST_COL_CODE_LINES: // 代码行数
        nResult = CompareInt(Item1.Values.nCodeLines, Item2.Values.nCodeLines);
        break;

    case LIST_COL_COMMENTS_LINES: // 注释行数
        nResult = CompareInt(Item1.Values.nCommentsLines, Item2.Values.nCommentsLines);
        break;

    case LIST_COL_BLANK_LINES: // 空行数
        nResult = CompareInt(Item1.Values.nBlankLines, Item2.Values.nBlankLines);
        break;

    default:
        break;
    }

    if (nResult == 0 && nColumn != LIST_COL_FILENAME)
    {
        s1 = ExtractFileName(Item1.strFileName);
        s2 = ExtractFileName(Item2.strFileName);
        nResult = s1.Compare(s2);
    }

    return nResult;
}

// CIrisCodeCounterDlg 消息处理程序

BOOL CIrisCodeCounterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    // 在此添加额外的初始化代码
    ConfigMgr.Load();
    SetWindowText(APP_NAME);
    InitFileTypeComboBox();
    InitListCtrl();
    LoadRecentSrcPaths();
    UpdateUIState(false);
    UpdateListResultCaption(-1);
    ((CButton*)GetDlgItem(IDC_CHECK_RECURSE))->SetCheck(true);
    m_SrcPathComboBox.SetFocus();

    return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIrisCodeCounterDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIrisCodeCounterDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


BOOL CIrisCodeCounterDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
        return TRUE;

    return CDialog::PreTranslateMessage(pMsg);
}

void CIrisCodeCounterDlg::InitFileTypeComboBox()
{
    CCodeCounter CodeCnt;
    CStrList ExtList;

    CCodeCntResultVals a;
    CodeCnt.Perform(TEXT("test.cpp"), a);

    struct {
        CString operator() (const CStrList& ExtList)
        {
            CString strResult;
            strResult.Preallocate(100);
            for (int i = 0; i < ExtList.GetCount(); i++)
            {
                if (i > 0) strResult += TEXT("; ");
                strResult += '*';
                strResult += ExtList[i];
            }
            strResult.ReleaseBuffer();
            return strResult;
        }
    } MakeFileExtString;

    m_FileTypeComboBox.AddString(TEXT("(All Types)"));
    CodeCnt.GetCppFileExtList(ExtList);
    m_FileTypeComboBox.AddString(MakeFileExtString(ExtList));
    CodeCnt.GetPascalFileExtList(ExtList);
    m_FileTypeComboBox.AddString(MakeFileExtString(ExtList));
    CodeCnt.GetJavaFileExtList(ExtList);
    m_FileTypeComboBox.AddString(MakeFileExtString(ExtList));
    CodeCnt.GetCsFileExtList(ExtList);
    m_FileTypeComboBox.AddString(MakeFileExtString(ExtList));

    m_FileTypeComboBox.SetCurSel(0);
}

void CIrisCodeCounterDlg::InitListCtrl()
{
    DWORD nStyle = GetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE);
    nStyle &= ~LVS_SINGLESEL;
    SetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE, nStyle);

    nStyle = m_ListCtrl.GetExtendedStyle();
    nStyle |= LVS_EX_FULLROWSELECT;
    nStyle |= LVS_EX_GRIDLINES;
    m_ListCtrl.SetExtendedStyle(nStyle);

    m_ListCtrl.InsertColumn(0, TEXT("File Name"), 0, 150);
    m_ListCtrl.InsertColumn(1, TEXT("Path"), 0, 162);
    m_ListCtrl.InsertColumn(2, TEXT("Total Lines"), 0, 90);
    m_ListCtrl.InsertColumn(3, TEXT("Code Lines"), 0, 90);
    m_ListCtrl.InsertColumn(4, TEXT("Comment Lines"), 0, 90);
    m_ListCtrl.InsertColumn(5, TEXT("Blank Lines"), 0, 90);
}

void CIrisCodeCounterDlg::LoadRecentSrcPaths()
{
    CStrList List;

    ConfigMgr.GetRecentSrcPaths(List);
    m_SrcPathComboBox.ResetContent();
    for (int i = 0; i < List.GetCount(); i++)
        m_SrcPathComboBox.AddString(List[i]);
    AddSpecItemForSrcPathComboBox();
}

void CIrisCodeCounterDlg::SaveRecentSrcPaths()
{
    CStrList List;
    for (int i = 0; i < m_SrcPathComboBox.GetCount() - 1; i++)
    {
        CString strItem;
        m_SrcPathComboBox.GetLBText(i, strItem);
        List.Add(strItem);
    }

    ConfigMgr.SetRecentSrcPaths(List);
}

void CIrisCodeCounterDlg::AddSpecItemForSrcPathComboBox()
{
    LPCTSTR lpszStr = TEXT("<Delete All Items>");

    int nCount = m_SrcPathComboBox.GetCount();
    if (nCount > 0)
    {
        CString strItem;
        m_SrcPathComboBox.GetLBText(nCount - 1, strItem);
        if (strItem.Compare(lpszStr) != 0)
            m_SrcPathComboBox.AddString(lpszStr);
    }
}

void CIrisCodeCounterDlg::AddRecentSrcPathToComboBox()
{
    CString strText;
    m_SrcPathComboBox.GetWindowText(strText);
    if (!strText.IsEmpty())
    {
        int i = m_SrcPathComboBox.FindStringExact(0, strText);
        if (i >= 0)
            m_SrcPathComboBox.DeleteString(i);
        m_SrcPathComboBox.InsertString(0, strText);
        m_SrcPathComboBox.SetWindowText(strText);
        AddSpecItemForSrcPathComboBox();
    }
}

void CIrisCodeCounterDlg::UpdateUIState(bool bCounting)
{
    m_FileTypeComboBox.EnableWindow(!bCounting);
    m_SrcPathComboBox.EnableWindow(!bCounting);
    GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(!bCounting);
    GetDlgItem(IDC_CHECK_RECURSE)->EnableWindow(!bCounting);
    m_ListCtrl.EnableWindow(!bCounting);
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(!bCounting);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(bCounting);
    GetDlgItem(IDC_BUTTON_OUTPUT)->EnableWindow(m_ListCtrl.GetItemCount() > 0);
}

void CIrisCodeCounterDlg::UpdateListResultCaption(int nItemCount)
{
    CString strCaption;
    if (nItemCount >= 0)
        strCaption.Format(TEXT("Results (%d items)"), nItemCount);
    else
        strCaption = TEXT("Results");

    (*(CStatic*)GetDlgItem(IDC_STATIC_COUNTING)).SetWindowText(strCaption);
}

void CIrisCodeCounterDlg::ClearCodeCountingResult()
{
    CodeCountingResult.Clear();

    m_ListCtrl.DeleteAllItems();
    GetDlgItem(IDC_EDIT_TOTAL)->SetWindowText(TEXT(""));
    GetDlgItem(IDC_EDIT_CODE)->SetWindowText(TEXT(""));
    GetDlgItem(IDC_EDIT_COMMENTS)->SetWindowText(TEXT(""));
    GetDlgItem(IDC_EDIT_BLANK)->SetWindowText(TEXT(""));
}

void CIrisCodeCounterDlg::ShowCodeCountingResult()
{
    int nItemCount = CodeCountingResult.GetCount();
    int nListCount = m_ListCtrl.GetItemCount();

    while (nListCount < nItemCount)
    {
        int nIndex = nListCount;
        CCodeCntResultItem FileItem;
        CodeCountingResult.GetItem(nIndex, FileItem);

        m_ListCtrl.InsertItem(nIndex, ExtractFileName(FileItem.strFileName));
        m_ListCtrl.SetItemText(nIndex, 1, CString(TEXT(".\\")) +
            ExtractRelativePath(CodeCountingResult.GetSourcePath(), ExtractFilePath(FileItem.strFileName)));
        m_ListCtrl.SetItemText(nIndex, 2, IntToStr(FileItem.Values.nTotalLines));
        m_ListCtrl.SetItemText(nIndex, 3, IntToStr(FileItem.Values.nCodeLines));
        m_ListCtrl.SetItemText(nIndex, 4, IntToStr(FileItem.Values.nCommentsLines));
        m_ListCtrl.SetItemText(nIndex, 5, IntToStr(FileItem.Values.nBlankLines));
        m_ListCtrl.SetItemData(nIndex, FileItem.nItemId);
        m_ListCtrl.EnsureVisible(nIndex, FALSE);

        nListCount++;
    }

    UpdateListResultCaption(nListCount);
    ShowCodeCountingSumVals();
}

void CIrisCodeCounterDlg::ShowCodeCountingSumVals()
{
    CCodeCntResultVals SumVals = CodeCountingResult.GetSumVals();

    GetDlgItem(IDC_EDIT_TOTAL)->SetWindowText(AddThousandSep(SumVals.nTotalLines));
    GetDlgItem(IDC_EDIT_CODE)->SetWindowText(AddThousandSep(SumVals.nCodeLines));
    GetDlgItem(IDC_EDIT_COMMENTS)->SetWindowText(AddThousandSep(SumVals.nCommentsLines));
    GetDlgItem(IDC_EDIT_BLANK)->SetWindowText(AddThousandSep(SumVals.nBlankLines));
}

void CIrisCodeCounterDlg::GetCodeCountingParams(CCodeCountingParams& Params)
{
    GetFileExtListFromUI(Params.FileExtList);
    m_SrcPathComboBox.GetWindowText(Params.strPath);
    Params.bRecurseSubDir = ((CButton*)GetDlgItem(IDC_CHECK_RECURSE))->GetCheck() == BST_CHECKED;
    Params.hMsgRecver = this->m_hWnd;
}

void CIrisCodeCounterDlg::GetFileExtListFromUI(CStrList& List)
{
    CString strText;

    List.Clear();
    int nSelIndex = m_FileTypeComboBox.GetCurSel();
    if (nSelIndex == 0)
    {
        for (int i = 1; i < m_FileTypeComboBox.GetCount(); i++)
        {
            CString strItem;
            m_FileTypeComboBox.GetLBText(i, strItem);
            if (i > 1) strText += ';';
            strText += strItem;
        }
    }
    else if (nSelIndex >= 1)
        m_FileTypeComboBox.GetLBText(nSelIndex, strText);

    SplitString(strText, ';', List);
    for (int i = 0; i < List.GetCount(); i++)
    {
        CString str = List[i];
        str.Trim();
        if (!str.IsEmpty() && str[0] == '*') str.Delete(0, 1);
        List.SetString(i, str);
    }
}

void CIrisCodeCounterDlg::OutputCodeCountingResult(LPCTSTR lpszFileName)
{
    const int MAX_NUM_WIDTH = 13;

    // AdjustFieldStr()
    struct {
        void operator() (CStringA& str, int nMaxChars)
        {
            if (str.GetLength() > nMaxChars)
            {
                str = str.Left(nMaxChars - 3);
                str += TEXT("...");
            }
            if (str.GetLength() <= nMaxChars)
                str.Append(CStringA(StringOfChar(' ', nMaxChars - str.GetLength() + 1)));
        }
    } AdjustFieldStr;

    struct {
        CString operator() (LPCTSTR lpszFileName)
        {
            CString strResult(ExtractRelativePath(CodeCountingResult.GetSourcePath(), ExtractFilePath(lpszFileName)));
            strResult = CString(TEXT(".\\")) + strResult;
            return strResult;
        }
    } GetRelativePath;

    // 计算文件名和路径的最大长度
    int nMaxFileNameLen = 0;
    int nMaxPathLen = 0;
    for (int i = 0; i < CodeCountingResult.GetCount(); i++)
    {
        CCodeCntResultItem Item;
        CodeCountingResult.GetItem(i, Item);
        CStringA strFileName(ExtractFileName(Item.strFileName));
        CStringA strPath(GetRelativePath(Item.strFileName));

        int nLen = strFileName.GetLength();
        if (nMaxFileNameLen < nLen) nMaxFileNameLen = nLen;
        nLen = strPath.GetLength();
        if (nMaxPathLen < nLen) nMaxPathLen = nLen;
    }
    nMaxPathLen += 3;

    CStrList StrList;
    StrList.Add(FormatString(TEXT("Source Dir: %s"), CodeCountingResult.GetSourcePath()));
    StrList.Add(TEXT(""));

    int nItemCount = CodeCountingResult.GetCount();
    for (int i = -2; (i < nItemCount) && (nItemCount > 0); i++)
    {
        CStringA strFileName, strPath;
        CStringA strTotalLines, strCodeLines, strCommentsLines, strBlankLines;
        CStringA strLine;

        CCodeCntResultItem Item;
        if (i >= 0)
            CodeCountingResult.GetItem(i, Item);

        if (i == -2)
        {
            strFileName = TEXT("File Name");
            strPath = TEXT("Path");
            strTotalLines = TEXT("Total Lines");
            strCodeLines = TEXT("Code Lines");
            strCommentsLines = TEXT("Comment Lines");
            strBlankLines = TEXT("Blank Lines");
        }
        else if (i == -1)
        {
            strFileName = StringOfChar('-', nMaxFileNameLen);
            strPath = StringOfChar('-', nMaxPathLen);
            strTotalLines = StringOfChar('-', MAX_NUM_WIDTH);
            strCodeLines = StringOfChar('-', MAX_NUM_WIDTH);
            strCommentsLines = StringOfChar('-', MAX_NUM_WIDTH);
            strBlankLines = StringOfChar('-', MAX_NUM_WIDTH);
        }
        else
        {
            strFileName = ExtractFileName(Item.strFileName);
            strPath = GetRelativePath(Item.strFileName);
            strTotalLines = IntToStr(Item.Values.nTotalLines);
            strCodeLines = IntToStr(Item.Values.nCodeLines);
            strCommentsLines = IntToStr(Item.Values.nCommentsLines);
            strBlankLines = IntToStr(Item.Values.nBlankLines);
        }

        AdjustFieldStr(strFileName, nMaxFileNameLen);
        AdjustFieldStr(strPath, nMaxPathLen);
        AdjustFieldStr(strTotalLines, MAX_NUM_WIDTH);
        AdjustFieldStr(strCodeLines, MAX_NUM_WIDTH);
        AdjustFieldStr(strCommentsLines, MAX_NUM_WIDTH);
        AdjustFieldStr(strBlankLines, MAX_NUM_WIDTH);

        strLine = strFileName + strPath + strTotalLines + strCodeLines + strCommentsLines + strBlankLines;
        StrList.Add(CString(strLine));
    }

    StrList.Add(TEXT(""));

    CCodeCntResultVals SumVals = CodeCountingResult.GetSumVals();
    StrList.Add(FormatString(TEXT("Total Lines:   %s"), AddThousandSep(SumVals.nTotalLines)));
    StrList.Add(FormatString(TEXT("Code Lines:    %s"), AddThousandSep(SumVals.nCodeLines)));
    StrList.Add(FormatString(TEXT("Comment Lines: %s"), AddThousandSep(SumVals.nCommentsLines)));
    StrList.Add(FormatString(TEXT("Blank Lines:   %s"), AddThousandSep(SumVals.nBlankLines)));

    StrList.SaveToFile(lpszFileName, false);
}

bool CIrisCodeCounterDlg::CheckValidBeforeStart()
{
    CString str;
    m_SrcPathComboBox.GetWindowText(str);
    if (str.IsEmpty())
    {
        AfxMessageBox(TEXT("Please select the source directory."), MB_OK | MB_ICONINFORMATION);
        m_SrcPathComboBox.SetFocus();
        return false;
    }

    if (!DirectoryExists(str))
    {
        AfxMessageBox(TEXT("The source directory does not exist."), MB_OK | MB_ICONINFORMATION);
        m_SrcPathComboBox.SetFocus();
        return false;
    }

    return true;
}

LRESULT CIrisCodeCounterDlg::OnCodeCntFinishedFileMsg(WPARAM wParam, LPARAM lParam)
{
    ASSERT(CodeCountingResult.GetCount() > 0);
    ShowCodeCountingResult();

    return 0;
}

LRESULT CIrisCodeCounterDlg::OnCodeCntFinishedAllMsg(WPARAM wParam, LPARAM lParam)
{
    ShowCodeCountingResult();

    m_ListCtrl.SortItems(ListCtrlSortFunc, LIST_COL_PATH);
    m_ListCtrl.EnsureVisible(0, FALSE);

    m_pCodeCntThread = NULL;
    UpdateUIState(false);

    return 0;
}

void CIrisCodeCounterDlg::OnBnClickedButtonBrowse()
{
    CString strPath;

    m_SrcPathComboBox.GetWindowText(strPath);
    if (SelectFolderDlg(m_hWnd, CString("Please select the source directory:"), false, strPath))
        m_SrcPathComboBox.SetWindowText(strPath);
}

void CIrisCodeCounterDlg::OnNMRClickListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = (LPNMITEMACTIVATE)pNMHDR;
    
    if (pNMItemActivate->iItem != -1)
    {
        DWORD dwPos = GetMessagePos();
        CPoint point(LOWORD(dwPos), HIWORD(dwPos));

        CMenu menu, *pPopup;
        VERIFY(menu.LoadMenu(IDR_MENU_LISTCTRL));
        pPopup = menu.GetSubMenu(0);
        ASSERT(pPopup != NULL);
        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
    }

    *pResult = 0;
}

void CIrisCodeCounterDlg::OnLvnKeydownListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    if (pLVKeyDow->wVKey == VK_DELETE)
        OnListCtrlRemove();

    *pResult = 0;
}

void CIrisCodeCounterDlg::OnHdnItemclickListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

    m_ListCtrl.SortItems(ListCtrlSortFunc, phdr->iItem);
    m_ListCtrl.EnsureVisible(0, FALSE);

    *pResult = 0;
}

void CIrisCodeCounterDlg::OnListCtrlRemove()
{
    m_ListCtrl.SetRedraw(FALSE);

    for (int i = m_ListCtrl.GetItemCount() - 1; i >= 0; i--)
    {
        if(m_ListCtrl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            int nSelIndex = i;
            int nItemId = m_ListCtrl.GetItemData(nSelIndex);
            m_ListCtrl.DeleteItem(nSelIndex);
            CodeCountingResult.Remove(nItemId);
        }
    }

    ShowCodeCountingSumVals();
    
    m_ListCtrl.SetRedraw(TRUE);
    m_ListCtrl.Invalidate();
    m_ListCtrl.UpdateWindow();
}

void CIrisCodeCounterDlg::OnBnClickedButtonStart()
{
    if (!CheckValidBeforeStart()) return;

    ClearCodeCountingResult();

    UpdateUIState(true);
    UpdateListResultCaption(0);
    AddRecentSrcPathToComboBox();

    CCodeCountingParams Params;
    GetCodeCountingParams(Params);

    m_pCodeCntThread = new CCodeCountingThread(Params);
    m_pCodeCntThread->Run();
}

void CIrisCodeCounterDlg::OnBnClickedButtonStop()
{
    if (m_pCodeCntThread != NULL)
        m_pCodeCntThread->Terminate();
}

void CIrisCodeCounterDlg::OnBnClickedButtonOutput()
{
    const int MAX_BUFFER_SIZE = 1024;

    CFileDialog FileDialog(FALSE);
    CString strBuffer;

    FileDialog.GetOFN().lpstrFile = strBuffer.GetBuffer(MAX_BUFFER_SIZE);
    FileDialog.GetOFN().nMaxFile = MAX_BUFFER_SIZE;
    FileDialog.GetOFN().lpstrDefExt = TEXT(".txt");
    FileDialog.GetOFN().lpstrFilter = TEXT("Text File(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");

    INT_PTR nRet = FileDialog.DoModal();
    strBuffer.ReleaseBuffer();

    if (nRet == IDOK)
    {
        CString strFileName = FileDialog.GetPathName();
        OutputCodeCountingResult(strFileName);
    }
}

void CIrisCodeCounterDlg::OnBnClickedButtonAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CIrisCodeCounterDlg::OnBnClickedButtonExit()
{
    PostQuitMessage(0);
}

void CIrisCodeCounterDlg::OnDestroy()
{
    CDialog::OnDestroy();

    SaveRecentSrcPaths();
    ConfigMgr.Save();
}

void CIrisCodeCounterDlg::OnCbnSelendokComboSrcdir()
{
    int nIndex = m_SrcPathComboBox.GetCurSel();
    if (nIndex >= 0 && nIndex == m_SrcPathComboBox.GetCount() - 1)
        m_SrcPathComboBox.ResetContent();
}

void CIrisCodeCounterDlg::OnDropFiles(HDROP hDropInfo)
{
    DWORD nFileCount;
    TCHAR chBuffer[MAX_PATH];

    nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, chBuffer, MAX_PATH);
    if (nFileCount > 0)
    {
        ::DragQueryFile(hDropInfo, 0, chBuffer, MAX_PATH);
        CString strFileName(chBuffer);
        CString strPath;
        if (DirectoryExists(strFileName))
            strPath = strFileName;
        else
            strPath = ExtractFilePath(strFileName);

        m_SrcPathComboBox.SetWindowText(strPath);
        m_SrcPathComboBox.SetFocus();
    }

    ::DragFinish(hDropInfo);
    CDialog::OnDropFiles(hDropInfo);
}
