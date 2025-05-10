#include "pch.h"
#include "KenanSvc.h"
#include "ClaPathMgr.h"
#include "ClaProcess.h"
#include "ClaSvcMgr.h"
#include "CLaSecuSvc.h"

/// <summary>
/// Initialize the service for kenan
/// Svc_Init function will check service exe exist, regist service, launch service and check the service status
/// </summary>
/// <param name="">void</param>
/// <returns>
/// success : 0
/// failed	: error_code
/// </returns>
unsigned long SVC_Init(void)
{
	return 0;

	/*
	CString strCmd = GetCommandLine();
	if (strCmd.Find(L"--system") >= 0) {
		//	if it was launched from service

	}
	else {
		unsigned int lSts;

		//	check service status
		CString strSvcPath; strSvcPath.Format(L"%s\\KenanSecuSvc64.exe", ClaPathMgr::GetDP());
		if (GetFileAttributes(strSvcPath) == INVALID_FILE_ATTRIBUTES) {
			return -1;
		}

		wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
		GetModuleFileName(NULL, wszPath, MAX_PATH);

		ClaProcess::CreateProcess_wait(strSvcPath, L" -f -u -i -b", ClaPathMgr::GetDP(), &lSts);

		ClaSecuSvc::Launch(L"winlogon.exe", wszPath, L"--system", NULL);
		ExitProcess(0);
	}


	return 0;
	*/
}