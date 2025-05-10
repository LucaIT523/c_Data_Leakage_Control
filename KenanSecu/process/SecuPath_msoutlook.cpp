#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_msoutlook(const wchar_t* p_wszExePath) {
	// user
	CString strAppTemp = ClaPathMgr::GetUserAppData(L"Temp");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	const wchar_t* AppLocal[] = {
		L"Temp",
		L"Microsoft\\Windows\\INetCache",
		L"Microsoft\\OneAuth",
		L"Microsoft\\IdentityCache",
		L"Microsoft\\FontCache",
		L"Microsoft\\Office",
		L"Microsoft\\TokenBroker\\Cache",
		L"Microsoft\\FORMS",
		NULL,
	};

	for (int i = 0; AppLocal[i] != NULL; i++) {
		strAppTemp = ClaPathMgr::GetUserAppData(AppLocal[i]);
		ClaSecuPolicy::AddExceptPath(strAppTemp);
	}

	strAppTemp = ClaPathMgr::GetUserRoaming(L"Microsoft\\Windows\\Recent\\CustomDestinations");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\Endpoint");

	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}