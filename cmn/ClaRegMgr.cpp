#include "pch.h"
#include "ClaRegMgr.h"
#include "ClaProcess.h"
#include <sddl.h>

void ClaRegMgr::createKey(LPCWSTR p_wszSubKey)
{
	HKEY newKey;
	LONG result = RegCreateKeyEx(_hRootKey, p_wszSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &newKey, nullptr);
	if (result != ERROR_SUCCESS) {
		
	}

	return;
}

unsigned long ClaRegMgr::writeStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, const wchar_t* p_wszValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_SET_VALUE, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_SZ, (const BYTE*)p_wszValue, (wcslen(p_wszValue) + 1) * sizeof(wchar_t));
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}

unsigned long ClaRegMgr::readStringW(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, wchar_t* p_wszValue)
{
	DWORD dataSize = 0;
	LONG result = RegGetValue(_hRootKey, p_wszSubKey, p_wszValueName, REG_MULTI_SZ, nullptr, nullptr, &dataSize);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	wchar_t* pTemp = (wchar_t*)malloc(dataSize + 2);
	memset(pTemp, 0, dataSize + 2);
	result = RegGetValue(_hRootKey, p_wszSubKey, p_wszValueName, REG_MULTI_SZ, nullptr, pTemp, &dataSize);
	if (result != ERROR_SUCCESS) {
		free(pTemp);
		return 2;
	}

	wcscpy_s(p_wszValue, MAX_PATH, pTemp);
	free(pTemp);
	return 0;
}

unsigned long ClaRegMgr::writeInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD p_dwValue)
{
	HKEY openedKey;
	LONG result = RegOpenKeyEx(_hRootKey, p_wszSubKey, 0, KEY_SET_VALUE, &openedKey);
	if (result != ERROR_SUCCESS) {
		return 1;
	}

	result = RegSetValueEx(openedKey, p_wszValueName, 0, REG_DWORD, (const BYTE*)&p_dwValue, 4);
	RegCloseKey(openedKey); // Close the key handle
	if (result != ERROR_SUCCESS) {
		return 2;
	}
	return 0;
}

unsigned long ClaRegMgr::readInt(const wchar_t* p_wszSubKey, const wchar_t* p_wszValueName, DWORD& p_dwValue)
{
	DWORD dataSize = 0;
	LONG result = RegGetValue(_hRootKey, p_wszSubKey, p_wszValueName, REG_DWORD, nullptr, nullptr, &dataSize);
	if (result != ERROR_SUCCESS) {
		return 1;
	}
	if (dataSize != 4) return 2;

	DWORD dwValue;
	result = RegGetValue(_hRootKey, p_wszSubKey, p_wszValueName, REG_DWORD, nullptr, &dwValue, &dataSize);
	if (result != ERROR_SUCCESS) {
		return 3;
	}

	p_dwValue = dwValue;
	return 0;
}

CString ClaRegMgr::getSIDString(void)
{
	DWORD dwSID;
	ProcessIdToSessionId(GetCurrentProcessId(), &dwSID);

	int nExpPID = ClaProcess::FindProcessID(L"explorer.exe", dwSID);
	HANDLE hExp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nExpPID);
	HANDLE hToken;

	if (!OpenProcessToken(hExp, TOKEN_QUERY, &hToken)) {
		return L"";
	}

	DWORD dwSize = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
	if (dwSize == 0) {
//		std::cerr << "Error getting token information size: " << GetLastError() << std::endl;
		CloseHandle(hToken);
		return L"";
	}

	PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwSize);
	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize)) {
//		std::cerr << "Error getting token information: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

	LPWSTR pSidString;
	if (!ConvertSidToStringSid(pTokenUser->User.Sid, &pSidString)) {
//		std::cerr << "Error converting SID to string: " << GetLastError() << std::endl;
		free(pTokenUser);
		CloseHandle(hToken);
		return L"";
	}

//	std::wcout << "SID: " << pSidString << std::endl;

	LocalFree(pSidString);
	free(pTokenUser);
	CloseHandle(hToken);
	return pSidString;
}
