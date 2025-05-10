#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_paint(const wchar_t* p_wszExePath) {
	// user
	CString strAppTemp = ClaPathMgr::GetUserAppData(L"Temp");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}