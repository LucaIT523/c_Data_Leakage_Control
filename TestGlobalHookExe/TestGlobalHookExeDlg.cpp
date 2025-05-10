
// TestGlobalHookExeDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestGlobalHookExe.h"
#include "TestGlobalHookExeDlg.h"
#include "afxdialogex.h"
#include "CLaSecuSvc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestGlobalHookExeDlg dialog



CTestGlobalHookExeDlg::CTestGlobalHookExeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TESTGLOBALHOOKEXE_DIALOG, pParent)
	, m_strPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestGlobalHookExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_PATH, m_strPath);
}

BEGIN_MESSAGE_MAP(CTestGlobalHookExeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PATH, &CTestGlobalHookExeDlg::OnBnClickedBtnPath)
	ON_BN_CLICKED(IDOK, &CTestGlobalHookExeDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTestGlobalHookExeDlg message handlers

BOOL CTestGlobalHookExeDlg::OnInitDialog()
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

void CTestGlobalHookExeDlg::OnPaint()
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
HCURSOR CTestGlobalHookExeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestGlobalHookExeDlg::OnBnClickedBtnPath()
{
	CFileDialog dlg(TRUE, NULL, NULL, 6, L"Exe File|*.exe||");
	if (dlg.DoModal() != IDOK) return;
	m_strPath = dlg.GetPathName();
	UpdateData(false);
}


void CTestGlobalHookExeDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	// TODO: Add your control notification handler code here
	if (m_strPath.IsEmpty()) {
		AfxMessageBox(L"Input the exe file path"); return;
	}
	if (GetFileAttributes(m_strPath) == INVALID_FILE_ATTRIBUTES) {
		AfxMessageBox(L"File not found"); return;
	}

	ClaSecuSvc::Launch(L"winlogon.exe", m_strPath, L"kenan-global", NULL);
//	CDialog::OnOK();
}
