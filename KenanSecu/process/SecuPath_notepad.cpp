#include "pch.h"
#include "ClaPathMgr.h"
#include "ClaSecuPolicy.h"

void init_secu_path_notepad(const wchar_t* p_wszExePath) {
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\Local\\Temp"));
}