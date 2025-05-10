#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_chrome(const wchar_t* p_wszExePath) {
	// user
	CString strAppTemp = ClaPathMgr::GetUserAppData(L"Temp");
	ClaSecuPolicy::AddExceptPath(strAppTemp);
//	strAppTemp = ClaPathMgr::GetUserAppData(L"Google\\Chrome\\User Data\\lockfile");
//	ClaSecuPolicy::AddExceptPath(strAppTemp);
	strAppTemp = ClaPathMgr::GetUserAppData(L"Google\\Chrome\\User Data");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\Endpoint");
	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\AsyncConnectHlp");
	ClaSecuPolicy::AddExceptPath(L"\\Device\\RasAcd");
	
	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}