
// KenanAgentDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanAgent.h"
#include "KenanAgentDlg.h"
#include "afxdialogex.h"
#include "KenanSecuDll.h"
#include "KenanClpDll.h"
#include "ClaOsInfo.h"
#include "ClaProcess.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"
#include "ClaSecuAgent.h"
#include "ClaMutex.h"
#include "Global.h"
#include "KenanVdDll.h"
#include "ClaRegMgr.h"
#include "ClaMutex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanAgentDlg dialog
BOOL lv_chrome_devmode_state = TRUE;

void disable_chrome_devmode() {
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.createKey(L"SOFTWARE\\Policies\\Google\\Chrome");
	reg.writeInt(L"SOFTWARE\\Policies\\Google\\Chrome", L"DeveloperToolsDisabled", 1);
}
void enable_chrome_devmode() {
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.writeInt(L"SOFTWARE\\Policies\\Google\\Chrome", L"DeveloperToolsDisabled", 0);
}

void InjectDllToSpoolsv()
{
#ifdef _WIN64
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	swprintf_s(wszPath, MAX_PATH, L"%s\\KenanSecu64.dll", ClaPathMgr::GetDP());

	unsigned long lSts = ClaProcess::FindProcessID(L"spoolsv.exe");
	if (lSts == 0) {
		//.	process not found
		return;
	}
	ClaSecuPolicy::AddSecuProcess(lSts);
	ClaProcess::InjectDLL(lSts, wszPath);
#endif//_WIN64
}

void InjectDllToExplorer()
{
#ifdef _WIN64
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	swprintf_s(wszPath, MAX_PATH, L"%s\\KenanSecu64.dll", ClaPathMgr::GetDP());

	unsigned long lSts = ClaProcess::FindProcessID(L"explorer.exe");
	if (lSts == 0) {
		//.	process not found
		return;
	}
	ClaSecuPolicy::AddSecuProcess(lSts);
	ClaProcess::InjectDLL(lSts, wszPath);
#endif//_WIN64
}
CKenanAgentDlg::CKenanAgentDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTAGENT_DIALOG, pParent)
	, m_strProcName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_PROC, m_strProcName);
}

BEGIN_MESSAGE_MAP(CKenanAgentDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

HANDLE lv_thdInjectDll = NULL;

unsigned int TF_WatchCloseFlg(void* p) {
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg start");
	while (true) {

		HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Global\\Kenan_Mutex_Stop");
		if (hMutex != NULL) {
			LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg Close Flag was set");
			CloseHandle(hMutex);
			break;
		}

		if (ClaMutex::IsCreate(L"Global\\Mutex-DLP") == false) {
#ifndef _DEBUG
			ClaMutex::Create(L"Global\\Kenan_Mutex_Stop");
#endif
		}
		Sleep(100);
	}

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg Close Flag was set & start to terminate");
	Sleep(1000);

	CLP_UnsetHook();
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg unhook clipboard finished.");

	SECU_UnsetHook();
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg unhook secu finished.");

	Sleep(500);
	TerminateThread(lv_thdInjectDll, 0);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg terminate injection dll thread");

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_WatchCloseFlg terminate current process");

	enable_chrome_devmode();

	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}

unsigned int TF_InjectDll(void* p) {

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_InjectDll start");

	unsigned int nPID;
	CString strDllPath;
	wchar_t wszPath[MAX_PATH];

#ifdef _WIN64
	strDllPath.Format(L"%s\\KenanSecu64.dll", ClaPathMgr::GetDP());
#else
	strDllPath.Format(L"%s\\KenanSecu32.dll", ClaPathMgr::GetDP());
#endif

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_InjectDll dllpath = [%s]", strDllPath);

	while (true) {
		memset(wszPath, 0, sizeof(wszPath));
		if (ClaSecuAgent::WaitSignalFromClient(nPID, wszPath) == 0){
			//.
			LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_InjectDll receive the dll inject signal from [%d]", nPID);

			ClaProcess::InjectDLL(nPID, strDllPath);

			LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_InjectDll dll was injected to [%d]", nPID);

			ClaSecuAgent::SetSignalToClient();

			LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"TF_InjectDll send response to client");
		}
	}

	return 0;
}

// CKenanAgentDlg message handlers

BOOL CKenanAgentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

#ifdef _DEBUG
//	ASSERT(0);
#endif

	// TODO: Add extra initialization here

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent start");

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent secu_init start");
	SECU_Init();

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent clp_init start");
	CLP_Init();

#ifdef _WIN64
	//.	start global hook
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent secu_sethook");
	if (SECU_SetHook() != 0) {
		LogPrintW(GD_LOG_LEVEL_ERROR, GD_LOG_AREA_AGENT, L"agent secu_sethook fail");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent clp_sethook");
	if (CLP_SetHook() != 0) {
		LogPrintW(GD_LOG_LEVEL_ERROR, GD_LOG_AREA_AGENT, L"agent clp_sethook fail");
		SECU_UnsetHook();
		EndDialog(IDCANCEL);
		return FALSE;
	}
#endif//_64

#ifdef _WIN64
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent virtual disk manager");
	VD_Init();
	VD_Wait();
#endif
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent create watch close thread");
	DWORD dwTID;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_WatchCloseFlg, NULL, 0, &dwTID);

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"agent create dll injection thread");
	lv_thdInjectDll = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_InjectDll, NULL, 0, &dwTID);

	OpenFileMapAgent();

	disable_chrome_devmode();

	InjectDllToSpoolsv();

//	InjectDllToExplorer();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanAgentDlg::OnPaint()
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
HCURSOR CKenanAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKenanAgentDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}
