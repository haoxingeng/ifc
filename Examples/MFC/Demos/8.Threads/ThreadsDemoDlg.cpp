// ThreadsDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ThreadsDemo.h"
#include "ThreadsDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CThreadsDemoDlg dialog

CThreadsDemoDlg::CThreadsDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThreadsDemoDlg::IDD, pParent)
    , m_pThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThreadsDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_LOG, m_LogEdit);
}

BEGIN_MESSAGE_MAP(CThreadsDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_THREAD_WORKING, OnThreadWorking)
    ON_MESSAGE(WM_THREAD_BEFORE_TERM, OnThreadBeforeTerm)
    ON_MESSAGE(WM_THREAD_BEFORE_KILL, OnThreadBeforeKill)
    ON_MESSAGE(WM_THREAD_DELETED, OnThreadDeleted)
    ON_BN_CLICKED(IDC_BUTTON_RUN, &CThreadsDemoDlg::OnBnClickedButtonRun)
    ON_BN_CLICKED(IDC_BUTTON_TERMINATE, &CThreadsDemoDlg::OnBnClickedButtonTerminate)
    ON_BN_CLICKED(IDC_BUTTON_KILL, &CThreadsDemoDlg::OnBnClickedButtonKill)
END_MESSAGE_MAP()


// CThreadsDemoDlg message handlers

BOOL CThreadsDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    UpdateUIState();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CThreadsDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CThreadsDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CThreadsDemoDlg::AddLog(LPCTSTR lpszStr)
{
    CString str(lpszStr);
    str += TEXT("\r\n");

    m_LogEdit.SetSel(MAXLONG, MAXLONG, 0);
    m_LogEdit.ReplaceSel(str);
}

void CThreadsDemoDlg::UpdateUIState()
{
    bool bIsRunning = (m_pThread != NULL);

    GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(!bIsRunning);
    GetDlgItem(IDC_BUTTON_TERMINATE)->EnableWindow(bIsRunning);
    GetDlgItem(IDC_BUTTON_KILL)->EnableWindow(bIsRunning);
}

LRESULT CThreadsDemoDlg::OnThreadWorking(WPARAM wParam, LPARAM lParam)
{
    CString str;
    str.Format(TEXT("Thread is running... %d"), wParam);
    AddLog(str);

    return 0;
}

LRESULT CThreadsDemoDlg::OnThreadBeforeTerm(WPARAM wParam, LPARAM lParam)
{
    AddLog(TEXT("CThread::BeforeTerminate() is called."));
    return 0;
}

LRESULT CThreadsDemoDlg::OnThreadBeforeKill(WPARAM wParam, LPARAM lParam)
{
    AddLog(TEXT("CThread::BeforeKill() is called."));
    return 0;
}

LRESULT CThreadsDemoDlg::OnThreadDeleted(WPARAM wParam, LPARAM lParam)
{
    AddLog(TEXT("CThread object is deleted."));
    m_pThread = NULL;

    UpdateUIState();
    return 0;
}

void CThreadsDemoDlg::OnBnClickedButtonRun()
{
    m_pThread = new CMyThread(m_hWnd);
    m_pThread->Run();

    UpdateUIState();
}

void CThreadsDemoDlg::OnBnClickedButtonTerminate()
{
    if (m_pThread != NULL)
        m_pThread->Terminate();
}

void CThreadsDemoDlg::OnBnClickedButtonKill()
{
    if (m_pThread != NULL)
        m_pThread->Kill();
}
