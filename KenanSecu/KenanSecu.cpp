// KenanSecu.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "KenanSecu.h"
#include "KenanHookMain.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"
#include "SecuPath.h"
#include "Global.h"
#include "ClaRegMgr.h"
#include "ClaProcess.h"
#include "ClaSecuAgent.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "DrawMark.h"

HINSTANCE g_hInstance = NULL;

extern wchar_t g_wszDllPath[MAX_PATH]; 
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CKenanSecuApp

BEGIN_MESSAGE_MAP(CKenanSecuApp, CWinApp)
END_MESSAGE_MAP()


// CKenanSecuApp construction

CKenanSecuApp::CKenanSecuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanSecuApp object

CKenanSecuApp theApp;


// CKenanSecuApp initialization

BOOL CKenanSecuApp::InitInstance()
{
	CWinApp::InitInstance();
	g_hInstance = theApp.m_hInstance;

	memset(g_wszDllPath, 0, sizeof(g_wszDllPath));
	GetModuleFileName(theApp.m_hInstance, g_wszDllPath, MAX_PATH);

	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	GetModuleFileName(NULL, wszPath, MAX_PATH);

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Dll Main start[%s][%d]\n", wszPath, GetCurrentProcessId());

//	return TRUE;

	if (is_except_process()) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_INIT, L"Dll Main Except Process");
		return TRUE;
	}

	if (!ClaSecuPolicy::GetDlpGlobalFlag() 
	&&	ClaSecuPolicy::IsAddSecuProcess(GetCurrentProcessId()) == false) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_INIT, L"Dll Main Not registed Secu process");
		return TRUE;
	}

	//LogPrintW(L"not except process");

	//LogPrintW(L"init secu start");
	if (init_secu_path() == true) {
		//.	injected launch, so it need to be hook
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Dll Main start");
		if (ClaSecuPolicy::GetDlpFile()) {
			LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_INIT, L"Dll Main dlp file");
			hook_functions();
		}

		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Dll Main start");
		startDrawMark();

		//.	regist secu process
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Dll Main regist secu proces with mutex");
		ClaSecuPolicy::RegisterSecuProcess(GetCurrentProcessId());
	}

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Dll Main end");
	return TRUE;
}

void disable_chrome_devmode() {
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.createKey(L"SOFTWARE\\Policies\\Google\\Chrome");
	reg.writeInt(L"SOFTWARE\\Policies\\Google\\Chrome", L"DeveloperToolsDisabled", 1);
}
void enable_chrome_devmode() {
	ClaRegMgr reg(HKEY_LOCAL_MACHINE);
	reg.writeInt(L"SOFTWARE\\Policies\\Google\\Chrome", L"DeveloperToolsDisabled", 0);
}

bool process_old_chrome() {
	if (ClaProcess::FindProcessID(L"chrome.exe")) {
		if (AfxMessageBox(L"Google chrome is running now, it will be force terminate.\nDo you want to continue?", MB_OKCANCEL) == IDOK) {
			ClaProcess::TerminateProcessByName(L"chrome.exe");
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

DWORD lv_dwSandbax = 1;
void disable_acrobat_sandbox() {
	ClaRegMgr reg(HKEY_CURRENT_USER);
	if (lv_dwSandbax == 0xFFFF) {
		reg.readInt(L"Software\\Adobe\\Adobe Acrobat\\DC\\Privileged", L"bProtectedMode", lv_dwSandbax);
	}
	reg.writeInt(L"Software\\Adobe\\Adobe Acrobat\\DC\\Privileged", L"bProtectedMode", 0);
}
void restore_acrobat_sandbox() {
	ClaRegMgr reg(HKEY_CURRENT_USER);
	if (lv_dwSandbax != 0xFFFF) {
		reg.writeInt(L"Software\\Adobe\\Adobe Acrobat\\DC\\Privileged", L"bProtectedMode", lv_dwSandbax);
	}
}

bool process_old_acrobat() {
	if (ClaProcess::FindProcessID(L"acrobat.exe")) {
		ClaProcess::TerminateProcessByName(L"acrobat.exe");
	}
	return true;
}

void CB_BEFORE_INJECT(unsigned int p_nPID) {
	ClaSecuPolicy::AddSecuProcess(p_nPID);
}

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT unsigned int SECU_CREATE_PROCESS_WITH_DLL(
	const wchar_t* p_wszProcPath,
	const wchar_t* p_wszCmd,
	const wchar_t* p_wszWorkDir,
	const wchar_t* p_wszDllPath,
	bool p_bShow,
	void* p_phProcessHandle
)
{
	int nRet = -1;
	if (ClaPathMgr::CmpProcName(p_wszProcPath, L"chrome.exe") == 0) {
		// if the target process is chrome, it is need to closed all the chrome process first
		if (process_old_chrome() == false) {
			return -2;
		}
		disable_chrome_devmode();
	}
	if (ClaPathMgr::CmpProcName(p_wszProcPath, L"acrobat.exe") == 0) {
		process_old_acrobat();
		disable_acrobat_sandbox();
	}

	DWORD dwCreateFlg = CREATE_SUSPENDED;
	if (wcsstr(p_wszProcPath, L"FoxitPDFReader.exe") != NULL
	|| wcsstr(p_wszProcPath, L"Acrobat.exe") != NULL
	|| wcsstr(p_wszProcPath, L"chrome.exe") != NULL) {
		dwCreateFlg = 0;
	}

	//.	create process with suspend option and using createprocessasuserfunction.
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi; memset(&pi, 0, sizeof(pi));

	if (CreateProcess(p_wszProcPath, (LPWSTR)p_wszCmd, NULL, NULL, FALSE, dwCreateFlg, NULL, p_wszWorkDir, &si, &pi) == FALSE) {
		return -1;
	}

	//	register to security process
	ClaSecuPolicy::AddSecuProcess(pi.dwProcessId);

	//.	check x86 or x64 && set signal to agent for dll injection.
	if (ClaSecuAgent::SetSignalToAgent(pi.dwProcessId, p_wszProcPath, ClaProcess::IsProcess64Bit(pi.hProcess)) != 0) {
		TerminateProcess(pi.hProcess, 0);
		AfxMessageBox(L"Security environment was currupted.");
		return -2;
	}
	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

DLL_EXPORT unsigned int SECU_FINISH(void)
{
	enable_chrome_devmode();
	restore_acrobat_sandbox();
	return 0;
}


DLL_EXPORT unsigned int SECU_REGISTER_PATH(const wchar_t* p_wszPath)
{
	return 0;
}

DLL_EXPORT unsigned int SECU_GET_REGISTER_PATH(wchar_t* p_wszPath, int p_cchPath)
{
	wcscpy_s(p_wszPath, p_cchPath, L"Z:\\");
	return 0;
}

HHOOK g_hHookKeyboard = NULL;
HHOOK g_hHookMouse = NULL;
HHOOK g_hHookKeyboardLL = NULL;
HHOOK g_hHookMouseLL = NULL;
HHOOK g_hHookProc = NULL;


extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookKeyboard, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcMouse(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookMouse, nCode, wParam, lParam);
}
extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcKeyboardLL(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookKeyboardLL, nCode, wParam, lParam);
}
extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcMouseLL(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookMouseLL, nCode, wParam, lParam);
}
extern "C" __declspec(dllexport) LRESULT CALLBACK HookProcWinProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		// Process the hook message here
	}
	return CallNextHookEx(g_hHookProc, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) unsigned int SetGlobalHook() {
	if (g_hHookKeyboard == NULL) {
		g_hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD, HookProcKeyboard, g_hInstance, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"keyboard hook = 0x%08X[%08X]", g_hHookKeyboard, GetLastError());
	}
	if (g_hHookMouse == NULL) {
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, HookProcMouse, g_hInstance, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"mouse hook = 0x%08X[%08X]", g_hHookMouse, GetLastError());
	}
	if (g_hHookKeyboardLL == NULL) {
		g_hHookKeyboardLL = SetWindowsHookEx(WH_KEYBOARD_LL, HookProcKeyboardLL, NULL, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"keyboardLL hook = 0x%08X[%08X]", g_hHookKeyboardLL, GetLastError());
	}
	if (g_hHookMouseLL == NULL) {
		g_hHookMouseLL = SetWindowsHookEx(WH_MOUSE_LL, HookProcMouseLL, NULL, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"MouseLL hook = 0x%08X[%08X]", g_hHookMouseLL, GetLastError());
	}
	if (g_hHookProc == NULL) {
		g_hHookProc = SetWindowsHookEx(WH_CALLWNDPROC, HookProcWinProc, NULL, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"WndProc hook = 0x%08X[%08X]", g_hHookProc, GetLastError());
	}

	if (g_hHookKeyboard == NULL || g_hHookMouse == NULL) {
		return -1;
	}
	return 0;
	//.	inject to whole process because it was too late to inject after global hook
//	_inject_this_to_whole_process();
}

extern "C" __declspec(dllexport) unsigned int UnsetGlobalHook() {

	if (g_hHookKeyboard) {
		UnhookWindowsHookEx(g_hHookKeyboard);
		g_hHookKeyboard = NULL;
	}
	if (g_hHookMouse) {
		UnhookWindowsHookEx(g_hHookMouse);
		g_hHookMouse = NULL;
	}

	if (g_hHookKeyboardLL) {
		UnhookWindowsHookEx(g_hHookKeyboardLL);
		g_hHookKeyboardLL = NULL;
	}
	if (g_hHookMouseLL) {
		UnhookWindowsHookEx(g_hHookMouseLL);
		g_hHookMouseLL = NULL;
	}
	if (g_hHookProc) {
		UnhookWindowsHookEx(g_hHookProc);
		g_hHookProc = NULL;
	}

	return 0;
}