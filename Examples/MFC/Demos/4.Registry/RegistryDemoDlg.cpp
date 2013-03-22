// RegistryDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegistryDemo.h"
#include "RegistryDemoDlg.h"

#include "ifc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRegistryDemoDlg dialog

CRegistryDemoDlg::CRegistryDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegistryDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegistryDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegistryDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CRegistryDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRegistryDemoDlg message handlers

BOOL CRegistryDemoDlg::OnInitDialog()
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

void CRegistryDemoDlg::OnPaint()
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
HCURSOR CRegistryDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRegistryDemoDlg::OnBnClickedButton1()
{
    CRegistry Reg;
    CString strKey;

    Reg.SetRootKey(HKEY_CURRENT_USER);

    strKey = TEXT("\\RegistryDemo\\Test");
    if (Reg.OpenKey(strKey, true))
    {
        // Write values
        Reg.WriteString(TEXT("str"), TEXT("this is a string."));
        Reg.WriteInteger(TEXT("int"), 12345);

        // Read values
        CString strValue = Reg.ReadString(TEXT("str"));
        int nValue = Reg.ReadInteger(TEXT("int"));

        ATLASSERT(strValue == TEXT("this is a string."));
        ATLASSERT(nValue == 12345);

        Reg.DeleteKey(TEXT("\\RegistryDemo"));
        ShowMessage(TEXT("OK!"));
    }
    else
    {
        ShowMessage(TEXT("Can not open the key!"));
    }
}
