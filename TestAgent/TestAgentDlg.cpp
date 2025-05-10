
// TestAgentDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestAgent.h"
#include "TestAgentDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestAgentDlg dialog



CTestAgentDlg::CTestAgentDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TESTAGENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestAgentDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestAgentDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &CTestAgentDlg::OnBnClickedOk2)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CTestAgentDlg message handlers

BOOL CTestAgentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	OnBnClickedOk();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestAgentDlg::OnPaint()
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
HCURSOR CTestAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef unsigned int(*FN_SetGlobalHook);
unsigned int(*SetGlobalHook)(void);
unsigned int(*UnsetGlobalHook)(void);

unsigned int(*SetGlobalHook1)(void);
unsigned int(*UnsetGlobalHook1)(void);

void CTestAgentDlg::OnBnClickedOk()
{
#ifdef _WIN64
	HMODULE mod = LoadLibrary(L"KenanGlobalHookTest64.dll");
#else
	HMODULE mod = LoadLibrary(L"KenanGlobalHookTest32.dll");
#endif

	SetGlobalHook = (unsigned int(*)(void))GetProcAddress(mod, "SetGlobalHook");
	UnsetGlobalHook = (unsigned int(*)(void))GetProcAddress(mod, "UnsetGlobalHook");

#ifdef _WIN64
	HMODULE mod1 = LoadLibrary(L"TestGlobalHookDLL64.dll");
#else
	HMODULE mod1 = LoadLibrary(L"TestGlobalHookDLL32.dll");
#endif

	SetGlobalHook1 = (unsigned int(*)(void))GetProcAddress(mod1, "SetGlobalHook");
	UnsetGlobalHook1 = (unsigned int(*)(void))GetProcAddress(mod1, "UnsetGlobalHook");

	SetGlobalHook();
	SetGlobalHook1();
}


void CTestAgentDlg::OnBnClickedOk2()
{
	UnsetGlobalHook();
	UnsetGlobalHook1();
}


void CTestAgentDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}
