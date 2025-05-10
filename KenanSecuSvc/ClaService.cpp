#include "pch.h"
#include "ClaService.h"
#include <winsvc.h>
#include "CLaSecuSvc.h"
#include <TlHelp32.h>
#include "ClaProcess.h"
#include "ClaLogPolicy.h"
#include "Global.h"

#pragma comment(lib, "advapi32.lib")

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
DWORD WINAPI SvcCtrlHandlerEx(
	DWORD    dwControl,
	DWORD    dwEventType,
	LPVOID   lpEventData,
	LPVOID   lpContext
);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID SvcReportEvent(LPTSTR);

unsigned long ClaService::run()
{
	ClaLogPolicy::RegisterLogPolicy(TRUE, 0xFFFFFFFF, 0xFFFFFFFF);
	ClaSecuSvc::Register(L"", L"", L"", L"");

    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { GD_SERVICE_NAME_SHORT, (LPSERVICE_MAIN_FUNCTION)SvcMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.

    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
    }

	return 0;
}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	// Register the handler function for the service
	OutputDebugString(L"------------------------Start service 1---------------------");

	gSvcStatusHandle = RegisterServiceCtrlHandlerEx(
		GD_SERVICE_NAME_SHORT,
		SvcCtrlHandlerEx, NULL);

	OutputDebugString(L"------------------------Start service 2---------------------");

	if (!gSvcStatusHandle)
	{
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.

	SvcInit(dwArgc, lpszArgv);
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with 
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.

	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
		return;
	}

	// Report running status when initialization is complete.

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	OutputDebugString(L"------------------------Start service---------------------");

	// TO_DO: Perform work until service stops.

	while (1)
	{
		// Check whether to stop the service.

		WaitForSingleObject(ghSvcStopEvent, INFINITE);

		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

bool GetProcessToken(HANDLE& hToken, const wchar_t* p_wszTarget) {
	unsigned int nExpID = ClaProcess::FindProcessID(L"explorer.exe");
	DWORD dwActiveSID;
	ProcessIdToSessionId(nExpID, &dwActiveSID);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE || hProcessSnap == NULL) {
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)) {
		CloseHandle(hProcessSnap);
		return false;
	}


	do {
		if (_wcsicmp(pe32.szExeFile, p_wszTarget) == 0) {
			DWORD dwSID;
			ProcessIdToSessionId(pe32.th32ProcessID, &dwSID);

			if (dwSID != dwActiveSID) continue;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
			if (hProcess == NULL) {
				CloseHandle(hProcessSnap);
				return false;
			}

			if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken)) {
				CloseHandle(hProcess);
				CloseHandle(hProcessSnap);
				return false;
			}

			CloseHandle(hProcess);
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return true;
}

bool LaunchProgramAsTarget(
	const wchar_t* programPath, 
	const wchar_t* p_wszTarget, 
	const wchar_t* p_wszCmd, 
	const wchar_t* p_wszWorkDir) 
{
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - get token");

	HANDLE hToken;
	if (!GetProcessToken(hToken, p_wszTarget)) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_SERVICE, L"service - func error[0x%08X]", GetLastError());
		return false;
	}

	HANDLE hNewToken;
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - dup token");
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hNewToken)) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_SERVICE, L"service - dup token error[0x%08X]", GetLastError());
		CloseHandle(hToken);
		return false;
	}

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - create as user");
	if (!CreateProcessAsUser(hNewToken, programPath, (wchar_t*)p_wszCmd, NULL, NULL, FALSE, 0, NULL, p_wszWorkDir, &si, &pi)) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_SERVICE, L"service - create as user error[0x%08X]", GetLastError());
		CloseHandle(hNewToken);
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hNewToken);
	CloseHandle(hToken);
	return true;
}

void launchNewProcess(void)
{
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - create process");

	wchar_t wszTarget[64]; memset(wszTarget, 0, sizeof(wszTarget));
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	wchar_t wszCmd[MAX_PATH]; memset(wszCmd, 0, sizeof(wszCmd));
	wchar_t wszWorkDir[MAX_PATH]; memset(wszWorkDir, 0, sizeof(wszWorkDir));

	ClaSecuSvc::Get(wszTarget, wszPath, wszCmd, wszWorkDir);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - get target=[%s]", wszTarget);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - get path=[%s]", wszPath);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - get cmd=[%s]", wszCmd);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_SERVICE, L"service - get wording=[%s]", wszWorkDir);

	if (GetFileAttributes(wszPath) != INVALID_FILE_ATTRIBUTES) {
		LaunchProgramAsTarget(wszPath, wszTarget, wszCmd, wszWorkDir);
	}
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
DWORD WINAPI SvcCtrlHandlerEx(
	DWORD    dwControl,
	DWORD    dwEventType,
	LPVOID   lpEventData,
	LPVOID   lpContext
)
{
	// Handle the requested control code. 

	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

		return 0;

	case 128:
		launchNewProcess();
		
		break;

	default:
		break;
	}
	return 0;
}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, GD_SERVICE_NAME_SHORT);

	if (NULL != hEventSource)
	{
		swprintf_s(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = GD_SERVICE_NAME_SHORT;
		lpszStrings[1] = Buffer;

		ReportEvent(hEventSource,        // event log handle
			EVENTLOG_ERROR_TYPE, // event type
			0,                   // event category
			1001/*SVC_ERROR*/,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data

		DeregisterEventSource(hEventSource);
	}
}