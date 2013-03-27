// ClipboardDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClipboardDemo.h"
#include "ClipboardDemoDlg.h"

#include "ifc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CClipboardDemoDlg dialog

CClipboardDemoDlg::CClipboardDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClipboardDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClipboardDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClipboardDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_COPY, &CClipboardDemoDlg::OnBnClickedButtonCopy)
    ON_BN_CLICKED(IDC_BUTTON_PASTE, &CClipboardDemoDlg::OnBnClickedButtonPaste)
END_MESSAGE_MAP()


// CClipboardDemoDlg message handlers

BOOL CClipboardDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Init IDC_EDIT_COPY
    GetDlgItem(IDC_EDIT_COPY)->SetWindowText(TEXT("This is a test string."));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClipboardDemoDlg::OnPaint()
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
HCURSOR CClipboardDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClipboardDemoDlg::OnBnClickedButtonCopy()
{
    CString strText;
    GetDlgItem(IDC_EDIT_COPY)->GetWindowText(strText);

    Clipboard().SetAsText(strText);
}

void CClipboardDemoDlg::OnBnClickedButtonPaste()
{
    CString strText = Clipboard().GetAsText();
    GetDlgItem(IDC_EDIT_PASTE)->SetWindowText(strText);
}
