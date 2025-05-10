#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_foxit(const wchar_t* p_wszExePath) {
	// user
	ClaSecuPolicy::AddExceptPath(L"C:\\");
	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\Endpoint");
	ClaSecuPolicy::AddExceptPath(L"Foxit PDF Reader");
}