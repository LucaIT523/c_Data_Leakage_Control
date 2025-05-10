// KenanGlobalHookTest.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "KenanGlobalHookTest.h"

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

// CKenanGlobalHookTestApp

BEGIN_MESSAGE_MAP(CKenanGlobalHookTestApp, CWinApp)
END_MESSAGE_MAP()


// CKenanGlobalHookTestApp construction

CKenanGlobalHookTestApp::CKenanGlobalHookTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanGlobalHookTestApp object

CKenanGlobalHookTestApp theApp;

HHOOK g_hHookKeyboard = NULL;
HHOOK g_hHookMouse = NULL;
HHOOK g_hHookKeyboardLL = NULL;
HHOOK g_hHookMouseLL = NULL;
HHOOK g_hHookProc = NULL;

HINSTANCE g_hInstance = NULL;

// CKenanGlobalHookTestApp initialization

BOOL CKenanGlobalHookTestApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance = m_hInstance;

	return TRUE;
}

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
	}
	if (g_hHookMouse == NULL) {
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, HookProcMouse, g_hInstance, 0);
	}
	if (g_hHookKeyboardLL == NULL) {
		g_hHookKeyboardLL = SetWindowsHookEx(WH_KEYBOARD_LL, HookProcKeyboardLL, NULL, 0);
	}
	if (g_hHookMouseLL == NULL) {
		g_hHookMouseLL = SetWindowsHookEx(WH_MOUSE_LL, HookProcMouseLL, NULL, 0);
	}
	if (g_hHookProc == NULL) {
		g_hHookProc = SetWindowsHookEx(WH_CALLWNDPROC, HookProcWinProc, NULL, 0);
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