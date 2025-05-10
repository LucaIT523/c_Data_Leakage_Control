#include "pch.h"
#include "KenanHookClp.h"
#include "detours.h"
#include "ClaSecuPolicy.h"

#pragma comment(lib, "detours.lib")

unsigned int TF_DETACH(void*);

typedef HANDLE (WINAPI *FN_SetClipboardData)(
	UINT uFormat, HANDLE hMem);
FN_SetClipboardData lv_orgSetClipboardData = NULL;

typedef HANDLE (WINAPI *FN_GetClipboardData)(
	_In_ UINT uFormat);
FN_GetClipboardData  lv_orgGetClipboardData = NULL;

DWORD lv_dwCurrentPID = 0;
DWORD GetCurPID() {
	if (lv_dwCurrentPID == 0) {
		lv_dwCurrentPID = GetCurrentProcessId();
	}
	return lv_dwCurrentPID;
}

HANDLE WINAPI hook_SetClipboardData(UINT uFormat, HANDLE hMem)
{
	if (ClaSecuPolicy::GetDlpClipboard() == FALSE) {
		return lv_orgSetClipboardData(uFormat, hMem);
	}

	if (ClaSecuPolicy::IsSecuProcess(GetCurPID())) {
		ClaSecuPolicy::RegisterClipboardFlag(TRUE);
	}
	else {
		ClaSecuPolicy::RegisterClipboardFlag(FALSE);
	}
	return lv_orgSetClipboardData(uFormat, hMem);
}

HANDLE WINAPI hook_GetClipboardData(_In_ UINT uFormat)
{
	if (ClaSecuPolicy::GetDlpClipboard() == FALSE) {
		return hook_GetClipboardData(uFormat);
	}

	wchar_t wszExePath[MAX_PATH]; memset(wszExePath, 0, sizeof(wszExePath));
	GetModuleFileName(NULL, wszExePath, MAX_PATH);

	if ((ClaSecuPolicy::IsSecuProcess(GetCurPID()) == FALSE || wcsstr(wszExePath, L"chrome.exe") != NULL)
	&& ClaSecuPolicy::IsSecuClipboard() == TRUE) {
		SetLastError(ERROR_ACCESS_DENIED);
		return INVALID_HANDLE_VALUE;
	}
	return lv_orgGetClipboardData(uFormat);
}

BOOL lv_bDetourClp = FALSE;

void hook_clip_functions()
{
	if (lv_bDetourClp) return;

	if (DetourIsHelperProcess()) {
		return ;
	}

	DetourRestoreAfterWith();

	HMODULE hModUser32 = LoadLibrary(L"user32.dll");
	lv_orgSetClipboardData = (FN_SetClipboardData)GetProcAddress(hModUser32, "SetClipboardData");
	lv_orgGetClipboardData = (FN_GetClipboardData)GetProcAddress(hModUser32, "GetClipboardData");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourAttach(&(PVOID&)lv_orgSetClipboardData, hook_SetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgGetClipboardData, hook_GetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DETACH, NULL, 0, &dwTID);
	lv_bDetourClp = TRUE;
}

void unhook_clip_functions()
{
	if (!lv_bDetourClp) return;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&)lv_orgSetClipboardData, hook_SetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgGetClipboardData, hook_GetClipboardData) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	lv_bDetourClp = FALSE;
}

unsigned int TF_DETACH(void*) {
	while (true)
	{
		Sleep(100);
		HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Global\\Kenan_Mutex_Stop");
		if (hMutex != NULL) {
			//.	terminate
			unhook_clip_functions();
			CloseHandle(hMutex);
			break;
		}
	}
	return 0;
}