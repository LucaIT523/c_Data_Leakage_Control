#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_acrobat(const wchar_t* p_wszExePath) {
	// user
	CString strAppTemp = ClaPathMgr::GetUserAppData(L"Temp");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\Endpoint");
	ClaSecuPolicy::AddExceptPath(L"\\\Device\\RasAcd");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Adobe\\AcroCef\\DC\\Acrobat"));

	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}