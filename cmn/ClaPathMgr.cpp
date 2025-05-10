#include "pch.h"
#include "ClaPathMgr.h"

CString ClaPathMgr::GetFN(const wchar_t* p_wszPath /*= NULL*/, BOOL p_bWithExt /*= TRUE*/)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	if (p_wszPath == NULL) {
		GetModuleFileName(NULL, wszPath, MAX_PATH);
	}
	else {
		wcscpy_s(wszPath, MAX_PATH, p_wszPath);
	}
	wchar_t* pPos = wcsrchr(wszPath, L'\\');

	if (pPos == NULL) return L"";

	if (!p_bWithExt) {
		wchar_t* pExt = wcsrchr(wszPath, L'.');
		if (pExt != NULL) pExt[0] = 0x0;
	}

	return &pPos[1];
}

CString ClaPathMgr::GetDP(const wchar_t* p_wszPath /*= NULL*/)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	if (p_wszPath == NULL) {
		GetModuleFileName(NULL, wszPath, MAX_PATH);
	}
	else {
		wcscpy_s(wszPath, MAX_PATH, p_wszPath);
	}
	wchar_t* pPos = wcsrchr(wszPath, L'\\');
	if (pPos != NULL) pPos[0] = 0;
	return wszPath;
}

CString ClaPathMgr::GetUserAppData(const wchar_t* p_wszAdditional)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	HRESULT result = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);
	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}

CString ClaPathMgr::GetUserPath(const wchar_t* p_wszAdditional)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	SHGetSpecialFolderPath(NULL, appDataPath, CSIDL_PROFILE, TRUE);

	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}
CString ClaPathMgr::GetUserRoaming(const wchar_t* p_wszAdditional /*= NULL*/)
{
	wchar_t appDataPath[MAX_PATH]; memset(appDataPath, 0, sizeof(appDataPath));
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);
	CString strRet;
	if (p_wszAdditional)
		strRet.Format(L"%s\\%s", appDataPath, p_wszAdditional);
	else
		strRet.Format(L"%s", appDataPath);
	return strRet;
}

int ClaPathMgr::CmpProcName(const wchar_t* p_wszPath, const wchar_t* p_wszExe)
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	wcscpy_s(wszPath, MAX_PATH, GetFN(p_wszPath));
	_wcsupr_s(wszPath, MAX_PATH);
	wchar_t wszExe[64]; memset(wszExe, 0, sizeof(wszExe));
	wcscpy_s(wszExe, 64, p_wszExe);
	_wcsupr_s(wszExe);

	return wcscmp(wszPath, wszExe);
}