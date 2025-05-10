#pragma once


class ClaPathMgr
{
public:
	ClaPathMgr() {};
	~ClaPathMgr() {};

public:
	static CString GetFN(const wchar_t* p_wszPath = NULL, BOOL p_bWithExt = TRUE);
	static CString GetDP(const wchar_t* p_wszPath = NULL);
	static CString GetUserAppData(const wchar_t* p_wszAdditional = NULL);
	static CString GetUserPath(const wchar_t* p_wszAdditional = NULL);
	static CString GetUserRoaming(const wchar_t* p_wszAdditional = NULL);
	static int CmpProcName(const wchar_t* p_wszPath, const wchar_t* p_wszExe);
};

