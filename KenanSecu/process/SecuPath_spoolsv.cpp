#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_spoolsv(const wchar_t* p_wszExePath) {
	// user
	ClaSecuPolicy::AddExceptPath(L"C:\\");
}