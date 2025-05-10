// KenanClp.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "KenanClp.h"
#include "KenanHookClp.h"
#include <TlHelp32.h>
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

// CKenanClpApp

BEGIN_MESSAGE_MAP(CKenanClpApp, CWinApp)
END_MESSAGE_MAP()


// CKenanClpApp construction

CKenanClpApp::CKenanClpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanClpApp object

CKenanClpApp theApp;

HHOOK g_hHookKeyboard = NULL;
HHOOK g_hHookMouse = NULL;
HHOOK g_hHookKeyboardLL = NULL;
HHOOK g_hHookMouseLL = NULL;
HHOOK g_hHookProc = NULL;

HINSTANCE g_hInstance = NULL;
wchar_t g_wszModulePath[MAX_PATH];


// CKenanClpApp initialization

BOOL CKenanClpApp::InitInstance()
{
	CWinApp::InitInstance();
	g_hInstance = theApp.m_hInstance;

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_C_INIT, L"initinstance");
	memset(g_wszModulePath, 0, sizeof(g_wszModulePath));
	GetModuleFileName(g_hInstance, g_wszModulePath, MAX_PATH);

//	return TRUE;

	hook_clip_functions();

	return TRUE;
}

typedef void (*FN_CALLBACK_ENUM_INT)(unsigned int p);

unsigned int _enum_process_with_callback(FN_CALLBACK_ENUM_INT fn)
{
	// Take a snapshot of all processes in the system.
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE || hProcessSnap == NULL) {
//		std::cerr << "CreateToolhelp32Snapshot failed." << std::endl;
		return 1;
	}

	// Set the size of the structure before using it.
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process.
	if (!Process32First(hProcessSnap, &pe32)) {
// 		std::cerr << "Process32First failed." << std::endl;
		CloseHandle(hProcessSnap); // Clean the snapshot object.
		return 1;
	}

	// Now walk the snapshot of processes, and display information about each process.
	do {
		fn(pe32.th32ProcessID);
// 		std::cout << "Process ID: " << pe32.th32ProcessID << std::endl;
// 		std::cout << "Process Name: " << pe32.szExeFile << std::endl;
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return 0;
}

void CB_PROCESS(unsigned int p_nID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, p_nID);
	if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE) return;

	HMODULE hKernel32 = GetModuleHandleA("Kernel32");
	FARPROC lb = GetProcAddress(hKernel32, "LoadLibraryW");
	void* pMem = VirtualAllocEx(hProcess, NULL, sizeof(g_wszModulePath), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pMem, g_wszModulePath, sizeof(g_wszModulePath), NULL);
	CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lb, pMem, 0, NULL);

}

void _inject_this_to_whole_process() {
	_enum_process_with_callback((FN_CALLBACK_ENUM_INT)CB_PROCESS);
}


#define DLL_EXPORT extern "C" __declspec(dllexport)

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
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_C_HOOK, L"keyboard = 0x%08X[%08X]", g_hHookKeyboard, GetLastError());
	}
	if (g_hHookMouse == NULL) {
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, HookProcMouse, g_hInstance, 0);
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_C_HOOK, L"Mouse = 0x%08X[%08X]", g_hHookMouse, GetLastError());
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
		g_hHookMouse= NULL;
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