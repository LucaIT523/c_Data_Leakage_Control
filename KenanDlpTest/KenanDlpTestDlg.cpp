
// KenanDlpTestDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanDlpTest.h"
#include "KenanDlpTestDlg.h"
#include "afxdialogex.h"
#include "ClaProcess.h"
#include "ClaPathMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanDlpTestDlg dialog

#define LD_MSGID_DRAG		(WM_USER+0x3001)

CKenanDlpTestDlg::CKenanDlpTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTDLPTEST_DIALOG, pParent)
	, m_strProcName(_T(""))
	, m_nPID(0)
	, m_strWnd(_T(""))
	, m_strDllPath(_T("E:\\proj\\game\\4\\result\\src\\client\\Debug\\client.dll"))
	, m_strExePath(_T("D:\\vm\\ubuntu-vietnam\\Client\\client.exe"))
	, m_strCaption(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanDlpTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_PROC, m_strProcName);
	DDX_Text(pDX, IDC_EDT_PID, m_nPID);
	DDX_Text(pDX, IDC_EDT_WND, m_strWnd);
	DDX_Text(pDX, IDC_EDT_DLL_PATH, m_strDllPath);
	DDX_Text(pDX, IDC_EDT_EXE_PATH, m_strExePath);
	DDX_Control(pDX, IDC_STC_DRAG, m_stcDrag);
	DDX_Text(pDX, IDC_EDT_CAPTION, m_strCaption);
}

BEGIN_MESSAGE_MAP(CKenanDlpTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FIND, &CKenanDlpTestDlg::OnBnClickedBtnFind)
	ON_BN_CLICKED(IDC_BTN_DLL_PATH, &CKenanDlpTestDlg::OnBnClickedBtnDllPath)
	ON_BN_CLICKED(IDC_BTN_INJECT, &CKenanDlpTestDlg::OnBnClickedBtnInject)
	ON_BN_CLICKED(IDC_BTN_EXE_PATH, &CKenanDlpTestDlg::OnBnClickedBtnExePath)
	ON_BN_CLICKED(IDC_BTN_INJECT_RUN, &CKenanDlpTestDlg::OnBnClickedBtnInjectRun)
	ON_BN_CLICKED(IDC_BTN_G_HOOK, &CKenanDlpTestDlg::OnBnClickedBtnGHook)
	ON_BN_CLICKED(IDC_BTN_G_UNHOOK, &CKenanDlpTestDlg::OnBnClickedBtnGUnhook)
	ON_BN_CLICKED(IDOK, &CKenanDlpTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CKenanDlpTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_RUN, &CKenanDlpTestDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BUTTON2, &CKenanDlpTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CKenanDlpTestDlg::OnBnClickedBtnHide)
	ON_BN_CLICKED(IDC_BTN_SHOW, &CKenanDlpTestDlg::OnBnClickedBtnShow)
	ON_BN_CLICKED(IDC_BTN_SHOW2, &CKenanDlpTestDlg::OnBnClickedBtnShow2)
	ON_BN_CLICKED(IDC_BUTTON3, &CKenanDlpTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CKenanDlpTestDlg message handlers

BOOL CKenanDlpTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_stcDrag.setCallBack(this, LD_MSGID_DRAG);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanDlpTestDlg::OnPaint()
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
HCURSOR CKenanDlpTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKenanDlpTestDlg::OnBnClickedBtnFind()
{
}


void CKenanDlpTestDlg::OnBnClickedBtnDllPath()
{
	CFileDialog dlg(TRUE, L"dll", L"KenanSecu64.dll", 6, L"Dll File|*.dll||");
	if (dlg.DoModal() != IDOK) return;
	UpdateData(TRUE);
	m_strDllPath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CKenanDlpTestDlg::OnBnClickedBtnInject()
{
	UpdateData(TRUE);

	ClaProcess::InjectDLL(m_nPID, m_strDllPath);
}


void CKenanDlpTestDlg::OnBnClickedBtnExePath()
{
	CFileDialog dlg(TRUE, L"exe", NULL, 6, L"Exe File|*.exe||");
	if (dlg.DoModal() != IDOK) return;

	UpdateData(TRUE);
	m_strExePath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CKenanDlpTestDlg::OnBnClickedBtnInjectRun()
{
	UpdateData(TRUE);

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo;

	CString strCmd; strCmd.Format(L"\"%s\" ur;name=test;ip=192.168.10.250;port=8888", m_strExePath);
	CreateProcess(m_strExePath, strCmd.GetBuffer(),
		NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
		ClaPathMgr::GetDP(m_strExePath), &StartupInfo, &ProcessInfo);

	ClaProcess::InjectDll(ProcessInfo.hProcess, m_strDllPath);
	ResumeThread(ProcessInfo.hThread);
}


void CKenanDlpTestDlg::OnBnClickedBtnGHook()
{
	// TODO: Add your control notification handler code here
}


void CKenanDlpTestDlg::OnBnClickedBtnGUnhook()
{
	// TODO: Add your control notification handler code here
}


void CKenanDlpTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

HWND hhhh;
LRESULT CKenanDlpTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == LD_MSGID_DRAG) {
		UpdateData(TRUE);

		HWND hWnd = (HWND)wParam;
		hhhh = (HWND)wParam;

		wchar_t wszName[MAX_PATH]; memset(wszName, 0, sizeof(wszName));
		::GetWindowTextW(hWnd, wszName, MAX_PATH);

		m_strCaption = wszName;
		m_strWnd.Format(L"0x%08X", hWnd);

		DWORD dwProcID;
		DWORD dwPID = GetWindowThreadProcessId(hWnd, &dwProcID);
		m_nPID = dwProcID;
		
		UpdateData(FALSE);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void CKenanDlpTestDlg::OnBnClickedButton1()
{
	::SetWindowText(hhhh, L"aaaa");
}


void CKenanDlpTestDlg::OnBnClickedBtnRun()
{
	UpdateData(TRUE);
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo;

	CString strCmd; strCmd.Format(L"\"%s\"", m_strExePath);
	CreateProcess(NULL, strCmd.GetBuffer(),
		NULL, NULL, FALSE, 0, NULL,
		L"C:\\Windows\\system32", &StartupInfo, &ProcessInfo);

}


void CKenanDlpTestDlg::OnBnClickedButton2()
{
	LoadLibrary(L"kenanClp32.dll");
}


void CKenanDlpTestDlg::OnBnClickedBtnHide()
{
	BOOL s = SetWindowDisplayAffinity(hhhh, WDA_EXCLUDEFROMCAPTURE);
	CString strMsg; strMsg.Format(L"r = %d[%08X]", s, GetLastError());
	AfxMessageBox(strMsg);
}


void CKenanDlpTestDlg::OnBnClickedBtnShow()
{
	BOOL s = SetWindowDisplayAffinity(hhhh, WDA_NONE);
	CString strMsg; strMsg.Format(L"r = %d[%08X]", s, GetLastError());
	AfxMessageBox(strMsg);
}


void CKenanDlpTestDlg::OnBnClickedBtnShow2()
{
	BOOL s = SetWindowDisplayAffinity(hhhh, WDA_MONITOR);
	CString strMsg; strMsg.Format(L"r = %d[%08X]", s, GetLastError());
	AfxMessageBox(strMsg);
}


void CKenanDlpTestDlg::OnBnClickedButton3()
{
	OPENFILENAME ofn;
	WCHAR szFile[MAX_PATH] = { 0 };

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open File Dialog
	BOOL bSts = GetOpenFileName(&ofn);

	CString str = ofn.lpstrFile;
}