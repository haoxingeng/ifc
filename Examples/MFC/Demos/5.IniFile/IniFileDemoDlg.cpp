// IniFileDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IniFileDemo.h"
#include "IniFileDemoDlg.h"

#include "ifc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIniFileDemoDlg dialog

CIniFileDemoDlg::CIniFileDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIniFileDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIniFileDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIniFileDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CIniFileDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CIniFileDemoDlg message handlers

BOOL CIniFileDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIniFileDemoDlg::OnPaint()
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
HCURSOR CIniFileDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIniFileDemoDlg::OnBnClickedButton1()
{
    CString strFileName = GetAppPath() + TEXT("test.ini");
    CIniFile IniFile(strFileName);

    // Write values
    IniFile.WriteString(TEXT("config"), TEXT("str"), TEXT("this is a string."));
    IniFile.WriteInteger(TEXT("config"), TEXT("int"), 12345);

    // Read values
    CString strValue = IniFile.ReadString(TEXT("config"), TEXT("str"));
    int nValue = IniFile.ReadInteger(TEXT("config"), TEXT("int"));

    ATLASSERT(strValue == TEXT("this is a string."));
    ATLASSERT(nValue == 12345);

    ShowMessage(TEXT("OK!"));
}
