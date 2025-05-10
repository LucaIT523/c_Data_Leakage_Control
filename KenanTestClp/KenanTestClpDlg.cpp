
// KenanTestClpDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanTestClp.h"
#include "KenanTestClpDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanTestClpDlg dialog

typedef void (*FN_HOOK)(void);
FN_HOOK setGlobalHook;
typedef void (*FN_UNHOOK)(void);
FN_UNHOOK unsetGlobalHook;

CKenanTestClpDlg::CKenanTestClpDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTTESTCLP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanTestClpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKenanTestClpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_HOOK, &CKenanTestClpDlg::OnBnClickedBtnHook)
	ON_BN_CLICKED(IDC_BTN_UNHOOK, &CKenanTestClpDlg::OnBnClickedBtnUnhook)
END_MESSAGE_MAP()


// CKenanTestClpDlg message handlers

HMODULE lv_hModDllTest = NULL;
HOOKPROC lv_addrTest = NULL;

BOOL CKenanTestClpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
#ifdef _WIN64
	HMODULE hMod = LoadLibrary(L"KenanClp64");
	lv_hModDllTest = LoadLibrary(L"KenanGlobalHookTest64.dll");
#else//_WIN64
	HMODULE hMod = LoadLibrary(L"KenanClp32");
	lv_hModDllTest = LoadLibrary(L"KenanGlobalHookTest32.dll");
#endif//_WIN64

	setGlobalHook = (FN_HOOK)GetProcAddress(hMod, "SetGlobalHook");
	unsetGlobalHook = (FN_HOOK)GetProcAddress(hMod, "UnsetGlobalHook");
	lv_addrTest = (HOOKPROC)GetProcAddress(lv_hModDllTest, "NextHook");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanTestClpDlg::OnPaint()
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
HCURSOR CKenanTestClpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HHOOK lv_hookTest;

void CKenanTestClpDlg::OnBnClickedBtnHook()
{
	// TODO: Add your control notification handler code here
	setGlobalHook();
//	lv_hookTest = SetWindowsHookEx(WH_KEYBOARD, lv_addrTest, lv_hModDllTest, 0); // Or WH_KEYBOARD if you prefer to trigger the hook manually

}


void CKenanTestClpDlg::OnBnClickedBtnUnhook()
{
	// TODO: Add your control notification handler code here
	unsetGlobalHook();
//	BOOL unhook = UnhookWindowsHookEx(lv_hookTest);
}
