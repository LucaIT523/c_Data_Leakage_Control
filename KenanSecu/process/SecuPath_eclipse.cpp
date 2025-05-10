#include "pch.h"
#include "ClaSecuPolicy.h"
#include "ClaPathMgr.h"

void init_secu_path_eclipse(const wchar_t* p_wszExePath) {

	CString strDirPath = ClaPathMgr::GetDP(p_wszExePath);

	//
	//.	Set Crypto Path
	//

	// clear crypto path
	ClaSecuPolicy::ClearCryptoPath();

	// 

	//
	//. Set Except Path
	//
	ClaSecuPolicy::ClearExceptPath();
	ClaSecuPolicy::AddExceptPath(L"\\\\.\\Global\\");

	ClaSecuPolicy::AddExceptPath(L"\\Device\\Afd\\Endpoint");
	ClaSecuPolicy::AddExceptPath(L"\\Device\\RasAcd");

	// user
	CString strAppTemp = ClaPathMgr::GetUserAppData(L"Temp");
	ClaSecuPolicy::AddExceptPath(strAppTemp);

	//.	icon path
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserAppData(L"Microsoft\\Windows\\Explorer"));
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserAppData(L"Eclipse"));
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserAppData(L"Temp"));
	CString userEclipse; userEclipse.Format(L"%s\\.eclipse", ClaPathMgr::GetUserPath());
	ClaSecuPolicy::AddExceptPath(userEclipse);
	CString userP2; userP2.Format(L"%s\\.p2", ClaPathMgr::GetUserPath());
	ClaSecuPolicy::AddExceptPath(userP2);


	//.	eclipse configuration
	CString confDir; confDir.Format(L"%s\\Configuration", strDirPath);
	ClaSecuPolicy::AddExceptPath(confDir);
	CString p2Dir; p2Dir.Format(L"%s\\p2", strDirPath);
	ClaSecuPolicy::AddExceptPath(p2Dir);
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserRoaming());

	//.	for print encryption.
	ClaSecuPolicy::AddExceptPath(L"C:\\Windows\\Temp");
	ClaSecuPolicy::AddExceptPath(ClaPathMgr::GetUserPath(L"AppData\\LocalLow\\Temp"));
}