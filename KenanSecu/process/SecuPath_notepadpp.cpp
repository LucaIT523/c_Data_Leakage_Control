#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_notepadpp(const wchar_t* p_wszExePath) {

	CString strAppTemp = ClaPathMgr::GetUserRoaming(L"Notepad++\\session.xml");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}