#include "pch.h"
#include "SecuPath.h"
#include "ClaFileProperty.h"

void init_secu_path_notepad(const wchar_t* p_wszExePath);
void init_secu_path_notepadpp(const wchar_t* p_wszExePath);
void init_secu_path_eclipse(const wchar_t* p_wszExePath);
void init_secu_path_paint(const wchar_t* p_wszExePath);
void init_secu_path_acrobat(const wchar_t* p_wszExePath);
void init_secu_path_msword(const wchar_t* p_wszExePath);
void init_secu_path_mspowerpoint(const wchar_t* p_wszExePath);
void init_secu_path_msexcel(const wchar_t* p_wszExePath);
void init_secu_path_msoutlook(const wchar_t* p_wszExePath);
void init_secu_path_chrome(const wchar_t* p_wszExePath);
void init_secu_path_spoolsv(const wchar_t* p_wszExePath);
void init_secu_path_sap(const wchar_t* p_wszExePath);
void init_secu_path_foxit(const wchar_t* p_wszExePath);

bool init_secu_path()
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	GetModuleFileName(NULL, wszPath, MAX_PATH);
	_wcsupr_s(wszPath, MAX_PATH);
	CString description = ClaFileProperty::GetProperty(wszPath, L"FileDescription");
	description.Trim();

	if (wcsstr(wszPath, L"ECLIPSE.EXE") != NULL) {
		init_secu_path_eclipse(wszPath);
	}
	else if (wcsstr(wszPath, L"JAVAW.EXE") != NULL) {
		init_secu_path_eclipse(wszPath);
	}
	else if (description == L"Notepad" || description == L"Notepad.exe" || wcsstr(wszPath, L"NOTEPAD.EXE") != NULL) {
		init_secu_path_notepad(wszPath);
	}
	else if (description == L"Paint" || wcsstr(wszPath, L"MSPAINT.EXE") != NULL) {
		init_secu_path_paint(wszPath);
	}
	else if (description == L"Notepad++" || wcsstr(wszPath, L"NOTEPAD++.EXE") != NULL) {
		init_secu_path_notepadpp(wszPath);
	}
	else if (description == L"Google Chrome" || wcsstr(wszPath, L"CHROME.EXE") != NULL) {
		init_secu_path_chrome(wszPath);
	}
	else if (description == L"Microsoft Word" || wcsstr(wszPath, L"WINWORD.EXE") != NULL) {
		init_secu_path_msword(wszPath);
	}
	else if (description == L"Microsoft PowerPoint" || wcsstr(wszPath, L"POWERPNT.EXE") != NULL) {
		init_secu_path_mspowerpoint(wszPath);
	}
	else if (description == L"Microsoft Excel" || wcsstr(wszPath, L"EXCEL.EXE") != NULL) {
		init_secu_path_msexcel(wszPath);
	}
	else if (description == L"Microsoft Outlook" || wcsstr(wszPath, L"OUTLOOK.EXE") != NULL) {
		init_secu_path_msoutlook(wszPath);
	}
	else if (description == L"Adobe Acrobat" || wcsstr(wszPath, L"ACROBAT.EXE") != NULL) {
		init_secu_path_acrobat(wszPath);
	}
	else if (description == L"Adobe AcrCEF") {
		init_secu_path_acrobat(wszPath);
	}
	else if (wcsstr(wszPath, L"SPOOLSV.EXE") != NULL) {
		init_secu_path_spoolsv(wszPath);
	}
	else if (wcsstr(wszPath, L"SAPLOGON.EXE") != NULL) {
		init_secu_path_sap(wszPath);
	}
	else if (wcsstr(wszPath, L"FOXITPDFREADER.EXE") != NULL) {
		init_secu_path_foxit(wszPath);
	}
	//else if (wcsstr(wszPath, L"EXPLORER.EXE") != NULL) {

	//}
	else {
		return false;
	}
	return true;
}

bool is_except_process()
{
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	GetModuleFileName(NULL, wszPath, MAX_PATH);

	CString product = ClaFileProperty::GetProperty(wszPath, L"ProductName");
	if (product == L"Kenan Security Environment") {
		return true;
	}
	return false;
}