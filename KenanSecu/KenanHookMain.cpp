#include "pch.h"
#include "KenanHookMain.h"
#include "detours.h"
#include "Global.h"
#include "ClaSecuPolicy.h"
#include "kncrypto.h"

#include <winternl.h>
#include <ntstatus.h>
#include <winhttp.h>
#include <Winspool.h>
#include <wingdi.h>
#include <commdlg.h>
#include <shellapi.h>
#include "ClaSecuAgent.h"
#include "ClaProcess.h"
#include "ClaPathMgr.h"

#define CRYPT_SECU
#define DENY_SECU_OUT

#pragma comment(lib, "detours.lib")

#pragma comment(lib, "ntdll.lib")

wchar_t g_wszExePath[MAX_PATH];

typedef struct tagHandleInfo {
	HANDLE m_hHandle;
	BOOL m_bWide;
	wchar_t m_wszPathSrc[MAX_PATH];
	wchar_t m_wszPathTemp[60];
}ST_HANDLE_INFO;
CArray<ST_HANDLE_INFO> lv_handleList;

CArray<HANDLE> lv_handleOpenList;

void ss_insert_handleW(HANDLE p_hFile, const wchar_t* p_wszPathOrg, const wchar_t* p_wszTemp)
{
	if (p_hFile == NULL || p_hFile == INVALID_HANDLE_VALUE) return;

	ST_HANDLE_INFO stInfo; memset(&stInfo, 0, sizeof(stInfo));

	stInfo.m_hHandle = p_hFile;
	stInfo.m_bWide = TRUE;
	wcscpy_s(stInfo.m_wszPathSrc, MAX_PATH, p_wszPathOrg);
	wcscpy_s(stInfo.m_wszPathTemp, 60, p_wszTemp);
	
	lv_handleList.Add(stInfo);
}
void ss_insert_handleA(HANDLE p_hFile, const char* p_szPathOrg, const char* p_szTemp)
{
	if (p_hFile == NULL || p_hFile == INVALID_HANDLE_VALUE) return;

	ST_HANDLE_INFO stInfo; memset(&stInfo, 0, sizeof(stInfo));

	stInfo.m_hHandle = p_hFile;
	stInfo.m_bWide = FALSE;
	swprintf_s(stInfo.m_wszPathSrc, MAX_PATH, L"%S", p_szPathOrg);
	swprintf_s(stInfo.m_wszPathTemp, 60, L"%S", p_szTemp);

	lv_handleList.Add(stInfo);
}

BOOL ss_get_handle(HANDLE p_hFile, wchar_t* p_wszPathOrg, wchar_t* p_wszTemp)
{
	if (p_hFile == NULL || p_hFile == INVALID_HANDLE_VALUE) return FALSE;
	
	for (int i = 0; i < lv_handleList.GetCount(); i++) {
		if (lv_handleList[i].m_hHandle == p_hFile) {
			wcscpy_s(p_wszPathOrg, MAX_PATH, lv_handleList[i].m_wszPathSrc);
			wcscpy_s(p_wszTemp, MAX_PATH, lv_handleList[i].m_wszPathTemp);
			return TRUE;
		}
	}
	return FALSE;
}

void ss_remove_handle(HANDLE p_hFile)
{
	for (int i = 0; i < lv_handleList.GetCount(); i++) {
		if (lv_handleList[i].m_hHandle == p_hFile) {
			lv_handleList.RemoveAt(i);
			break;
		}
	}
}

void add_open_handle(HANDLE p_hHandle) {
	lv_handleOpenList.Add(p_hHandle);
}
BOOL is_exist_open_handle(HANDLE p_hHandle) {
	for (int i = 0; i < lv_handleOpenList.GetCount(); i++) {
		if (lv_handleOpenList[i] == p_hHandle)
			return TRUE;
	}
	return FALSE;
}
void remove_open_handle(HANDLE p_hHandle) {
	for (int i = 0; i < lv_handleOpenList.GetCount(); i++) {
		if (lv_handleOpenList[i] == p_hHandle) {
			lv_handleOpenList.RemoveAt(i);
			return;
		}
	}
}
typedef BOOL
(WINAPI *FN_CreateProcessW)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
);
FN_CreateProcessW lv_orgCreateProcessW;

typedef BOOL
(WINAPI *FN_CreateProcessA)(
	_In_opt_ LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOA lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);
FN_CreateProcessA lv_orgCreateProcessA;

BOOL WINAPI hook_CreateProcessW(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
	CString strCmd = lpCommandLine == NULL?L"":lpCommandLine;
	if (ClaPathMgr::CmpProcName(g_wszExePath, L"chrome.exe") == 0
	&&  lpApplicationName == NULL
	&&  strCmd.Find(L"--type=utility") >= 0
	&&  strCmd.Find(L"--utility-sub-type=chrome.mojom.UtilWin") >= 0
	) {
		//	This is for chrome open file dialog box.
		SetLastError(2);
		return FALSE;
	}

	BOOL bRet = lv_orgCreateProcessW(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags | CREATE_SUSPENDED,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
	);
	if (bRet) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PROC, L"Add Secu [%s][%d]", 
			lpApplicationName, 
			lpProcessInformation->dwProcessId);
		ClaSecuPolicy::AddSecuProcess(lpProcessInformation->dwProcessId);
		ClaSecuPolicy::InjectDll(lpProcessInformation->hProcess);
		ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

BOOL WINAPI hook_CreateProcessA(
	_In_opt_ LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOA lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
	BOOL bRet = lv_orgCreateProcessA(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags | CREATE_SUSPENDED,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
	);
	if (bRet) {
		LogPrintA(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PROC, "Add Secu [%s][%d]",
			lpApplicationName,
			lpProcessInformation->dwProcessId);
		ClaSecuPolicy::AddSecuProcess(lpProcessInformation->dwProcessId);

		ClaSecuPolicy::InjectDll(lpProcessInformation->hProcess);
		ResumeThread(lpProcessInformation->hThread);

	}
	return bRet;
}

typedef BOOL
(WINAPI *FN_CreateProcessAsUserW)(
	_In_opt_      HANDLE                hToken,
	_In_opt_      LPCWSTR                lpApplicationName,
	_Inout_opt_   LPWSTR                 lpCommandLine,
	_In_opt_      LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_      LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_          BOOL                  bInheritHandles,
	_In_          DWORD                 dwCreationFlags,
	_In_opt_      LPVOID                lpEnvironment,
	_In_opt_      LPCWSTR                lpCurrentDirectory,
	_In_          LPSTARTUPINFOW        lpStartupInfo,
	_Out_         LPPROCESS_INFORMATION lpProcessInformation
	);
FN_CreateProcessAsUserW lv_orgCreateProcessAsUserW;
typedef BOOL
(WINAPI *FN_CreateProcessAsUserA)(
	_In_opt_      HANDLE                hToken,
	_In_opt_      LPCSTR                lpApplicationName,
	_Inout_opt_   LPSTR                 lpCommandLine,
	_In_opt_      LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_      LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_          BOOL                  bInheritHandles,
	_In_          DWORD                 dwCreationFlags,
	_In_opt_      LPVOID                lpEnvironment,
	_In_opt_      LPCSTR                lpCurrentDirectory,
	_In_          LPSTARTUPINFOA        lpStartupInfo,
	_Out_         LPPROCESS_INFORMATION lpProcessInformation
	);
FN_CreateProcessAsUserA lv_orgCreateProcessAsUserA;

BOOL WINAPI hook_CreateProcessAsUserA(
	_In_opt_      HANDLE                hToken,
	_In_opt_      LPCSTR                lpApplicationName,
	_Inout_opt_   LPSTR                 lpCommandLine,
	_In_opt_      LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_      LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_          BOOL                  bInheritHandles,
	_In_          DWORD                 dwCreationFlags,
	_In_opt_      LPVOID                lpEnvironment,
	_In_opt_      LPCSTR                lpCurrentDirectory,
	_In_          LPSTARTUPINFOA        lpStartupInfo,
	_Out_         LPPROCESS_INFORMATION lpProcessInformation
)
{
	BOOL bRet = lv_orgCreateProcessAsUserA(
		hToken,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags | CREATE_SUSPENDED,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
	);
	if (bRet) {
		//LogPrintW(L"create process as user and Inject dll to [%S]A", lpApplicationName);
		LogPrintA(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PROC, "Add Secu [%s][%d]",
			lpApplicationName,
			lpProcessInformation->dwProcessId);
		ClaSecuPolicy::AddSecuProcess(lpProcessInformation->dwProcessId);
		wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
		swprintf_s(wszPath, MAX_PATH, L"%S", lpApplicationName);
		ClaSecuAgent::SetSignalToAgent(
			lpProcessInformation->dwProcessId, 
			wszPath,
			ClaProcess::IsProcess64Bit(lpProcessInformation->hProcess));

//		ClaSecuPolicy::InjectDll(lpProcessInformation->hProcess);
		ResumeThread(lpProcessInformation->hThread);

	}
	return bRet;
}

BOOL WINAPI hook_CreateProcessAsUserW(
	_In_opt_      HANDLE                hToken,
	_In_opt_      LPCWSTR               lpApplicationName,
	_Inout_opt_   LPWSTR                lpCommandLine,
	_In_opt_      LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_      LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_          BOOL                  bInheritHandles,
	_In_          DWORD                 dwCreationFlags,
	_In_opt_      LPVOID                lpEnvironment,
	_In_opt_      LPCWSTR               lpCurrentDirectory,
	_In_          LPSTARTUPINFOW        lpStartupInfo,
	_Out_         LPPROCESS_INFORMATION lpProcessInformation
)
{
	DWORD dwCreateFlg;
	wchar_t wszExe[MAX_PATH]; memset(wszExe, 0, sizeof(wszExe));
	wcscpy_s(wszExe, MAX_PATH, lpApplicationName);
	_wcsupr_s(wszExe, MAX_PATH);
	BOOL bInject = TRUE;
	if (ClaPathMgr::CmpProcName(lpApplicationName, L"ACROBAT.EXE") == 0) 
	{
		dwCreateFlg = dwCreationFlags;
	}
	else {
		dwCreateFlg = dwCreationFlags | CREATE_SUSPENDED;
	}
	if (ClaPathMgr::CmpProcName(lpApplicationName, L"chrome.exe") == 0) {
		bInject = FALSE;
	}

	BOOL bRet = lv_orgCreateProcessAsUserW(
		hToken,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags | CREATE_SUSPENDED,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
	);
	if (bRet) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PROC, L"Add Secu in as userw [%s][%d]",
			lpApplicationName,
			lpProcessInformation->dwProcessId);
		ClaSecuPolicy::AddSecuProcess(lpProcessInformation->dwProcessId);
		if (bInject)
			ClaSecuAgent::SetSignalToAgent(
				lpProcessInformation->dwProcessId, 
				lpApplicationName,
				ClaProcess::IsProcess64Bit(lpProcessInformation->hProcess));

//		ClaSecuPolicy::InjectDll(lpProcessInformation->hProcess);
		ResumeThread(lpProcessInformation->hThread);
	}
	return bRet;
}

typedef NTSTATUS(NTAPI* FN_NtCreateFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength);
FN_NtCreateFile lv_orgNtCreateFile;
typedef NTSTATUS(NTAPI* FN_NtClose)(HANDLE);

typedef NTSTATUS(NTAPI *FN_NtOpenFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
	);
FN_NtOpenFile lv_orgZwOpenFile;

FN_NtClose lv_orgNtClose;
typedef NTSTATUS(NTAPI* FN_NtReadFile)(
	HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, 
	PIO_STATUS_BLOCK, PVOID, ULONG, 
	PLARGE_INTEGER, PULONG);
FN_NtReadFile lv_orgNtReadFile;
typedef NTSTATUS(NTAPI* FN_NtWriteFile)(
	HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, 
	PIO_STATUS_BLOCK, PVOID, ULONG, 
	PLARGE_INTEGER, PULONG);
FN_NtWriteFile lv_orgNtWriteFile;

typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, * PFILE_STANDARD_INFORMATION;

typedef NTSTATUS(NTAPI*FN_NtQueryInformationFile)(
	HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation, ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass);
FN_NtQueryInformationFile lv_orgNtQueryInformationFile;

__int64 _get_file_size(const wchar_t* p_wszPath) {
	if (GetFileAttributes(p_wszPath) == INVALID_FILE_ATTRIBUTES) {
		return -1;
	}

	UNICODE_STRING fileName;
	RtlInitUnicodeString(&fileName, p_wszPath);

	OBJECT_ATTRIBUTES objAttr;
	InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	HANDLE hFile;
	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return -1;
	}

	FILE_STANDARD_INFORMATION fileInfo;

	status = lv_orgNtQueryInformationFile(
		hFile,
		&ioStatusBlock,
		&fileInfo,
		sizeof(fileInfo),
		(FILE_INFORMATION_CLASS)5//FileStandardInformation
	);
	lv_orgNtClose(hFile);

	if (status != STATUS_SUCCESS) {
		return -1;
	}

	return fileInfo.EndOfFile.QuadPart;

}

int _read_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen) {
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttr;
	IO_STATUS_BLOCK ioStatusBlock;
	UNICODE_STRING filePath;

	RtlInitUnicodeString(&filePath, p_wszPath);
	InitializeObjectAttributes(&objAttr, &filePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return 0;
	}

	DWORD bytesRead = 0;
	status = lv_orgNtReadFile(
		hFile,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		p_pBuff,
		p_nLen,
		NULL,
		0
	);

	lv_orgNtClose(hFile);

	return p_nLen;
}

int _write_file(const wchar_t* p_wszPath, void* p_pBuff, int p_nLen) {
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttr;
	IO_STATUS_BLOCK ioStatusBlock;
	UNICODE_STRING filePath;

	RtlInitUnicodeString(&filePath, p_wszPath);
	InitializeObjectAttributes(&objAttr, &filePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		FILE_GENERIC_WRITE,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OVERWRITE_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return 0;
	}

	DWORD bytesWritten = 0;
	status = lv_orgNtWriteFile(
		hFile,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		p_pBuff,
		p_nLen,
		NULL,
		0
	);

	lv_orgNtClose(hFile);

	return p_nLen;
}

void _encryptBuffer(void* p_pBuff, int p_nLen) {
	kn_encrypt((unsigned char*)p_pBuff, p_nLen);
/*
	for (int i = 0; i < p_nLen; i++) {
		((char*)p_pBuff)[i] = ((char*)p_pBuff)[i] ^ 0xCC;
	}
*/
}

void _decryptBuffer(void* p_pBuff, int p_nLen) {
	kn_decrypt((unsigned char*)p_pBuff, p_nLen);
/*
	for (int i = 0; i < p_nLen; i++) {
		((char*)p_pBuff)[i] = ((char*)p_pBuff)[i] ^ 0xCC;
	}
*/
}

void _decryptFileW(const wchar_t* p_wszPathOrg, wchar_t* p_wszPathTemp)
{
	ClaSecuPolicy::GenerateTempFilePathW(p_wszPathTemp);

	//.	if file not exist, return only temp path

	__int64 nSize = _get_file_size(p_wszPathOrg);
	if (nSize == -1) {
		DeleteFile(p_wszPathOrg);
		return;
	}

	if (nSize == 0) {
		//.	if the file size is 0, as empty file
		HANDLE hFile;
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK ioStatusBlock;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, p_wszPathTemp);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		NTSTATUS status = lv_orgNtCreateFile(
			&hFile,
			FILE_GENERIC_WRITE,
			&objAttr,
			&ioStatusBlock,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_CREATE,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
		);
		if (NT_SUCCESS(status)) {
			lv_orgNtClose(hFile);
		}
		return;
	}

	unsigned char pHead[] = { 0x65, 0x87, 0x65 };
	unsigned char* pBuff = (unsigned char*)malloc(nSize); memset(pBuff, 0, nSize);
	int nRead = _read_file(p_wszPathOrg, pBuff, nSize);
	if (nRead >= 3 && memcmp(pBuff, pHead, 3) == 0) {
		//	if crypted file
		_decryptBuffer(&pBuff[3], nSize-3);
		_write_file(p_wszPathTemp, &pBuff[3], nRead-3);
	}
	else {
		_write_file(p_wszPathTemp, pBuff, nRead);
	}

	free(pBuff);
}

void _decryptFileA(const char* p_szPathOrg, char* p_szPathTemp)
{
	wchar_t wszPathOrg[MAX_PATH]; memset(wszPathOrg, 0, sizeof(wszPathOrg));
	swprintf_s(wszPathOrg, MAX_PATH, L"%S", p_szPathOrg);
	wchar_t wszPathTemp[64]; memset(wszPathTemp, 0, sizeof(wszPathTemp));
	_decryptFileW(wszPathOrg, wszPathTemp);
	sprintf_s(p_szPathTemp, 32, "%S", wszPathTemp);
}

void _encryptFileW(const wchar_t* p_wszPathTemp, const wchar_t* p_wszPathOrg)
{
	//.	if file not exist, return only temp path
/*
	if (GetFileAttributes(p_wszPathTemp) == INVALID_FILE_ATTRIBUTES) {
		DeleteFile(p_wszPathOrg);
		return;
	}
*/

	__int64 nSize = _get_file_size(p_wszPathTemp);
	if (nSize == -1) {
		DeleteFile(p_wszPathOrg); return;
	}

	if (nSize == 0) {
		HANDLE hFile;
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK ioStatusBlock;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, p_wszPathOrg);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		NTSTATUS status = lv_orgNtCreateFile(
			&hFile,
			FILE_GENERIC_WRITE,
			&objAttr,
			&ioStatusBlock,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			FILE_CREATE,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
		);
		if (NT_SUCCESS(status)) {
			lv_orgNtClose(hFile);
		}
		return;
	}

	unsigned char pHead[] = { 0x65, 0x87, 0x65 };
	unsigned char* pBuff = (unsigned char*)malloc(nSize+3);
	memcpy(pBuff, pHead, 3);
	int nRead = _read_file(p_wszPathTemp, &pBuff[3], nSize);
	//	if crypted file
	_encryptBuffer(&pBuff[3], nSize);
	_write_file(p_wszPathOrg, pBuff, nSize+3);
	free(pBuff);
}

NTSTATUS NTAPI hook_NtCreateFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
)
{	

	if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL || (CreateOptions & FILE_DIRECTORY_FILE)) {
		return lv_orgNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
			CreateOptions, EaBuffer, EaLength);
	}
	wchar_t* wszFileName = ObjectAttributes->ObjectName->Buffer;
	BOOL bExceptPath = ClaSecuPolicy::IsExceptPathW(wszFileName);
	BOOL bSecuPath = ClaSecuPolicy::IsSecuPathW(wszFileName);
	BOOL bCryptoPath = ClaSecuPolicy::IsCryptoPathW(wszFileName);
	BOOL bTempPath = ClaSecuPolicy::IsTempPathW(wszFileName);
	DWORD dwDesiredAccess = DesiredAccess;
	BOOL bWriteAble = 
		   (dwDesiredAccess & GENERIC_WRITE) 
		|| (dwDesiredAccess & GENERIC_ALL)
		|| (dwDesiredAccess & FILE_WRITE_DATA)
		|| (CreateDisposition == FILE_CREATE);

	if (wszFileName == NULL
	|| bExceptPath
	|| (!bSecuPath && !bCryptoPath && !bWriteAble)
	|| ((GetFileAttributes(wszFileName) & FILE_ATTRIBUTE_DIRECTORY) && GetFileAttributes(wszFileName) != INVALID_FILE_ATTRIBUTES)
	|| bTempPath
	|| wcsstr(wszFileName, L"C:\\ProgramData\\kenan.log") != NULL)
	{
		return lv_orgNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
			CreateOptions, EaBuffer, EaLength);
	}

//	LogPrintW(L"CreateFileW - [%s]-%08X-%08X-%08X\n",
//		wszFileName, dwDesiredAccess, ShareAccess, FileAttributes);
	if (!bSecuPath && !bCryptoPath && bWriteAble) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_FILE, L"BLOCK %s\n", wszFileName);
		SetLastError(ERROR_ACCESS_DENIED);
		return STATUS_ACCESS_DENIED;
	}

	//.	it is in security directory which need to be crypted

	//	create file decrypted to temp directory.
	wchar_t wszPathT[MAX_PATH]; memset(wszPathT, 0, sizeof(wszPathT));
	_decryptFileW(wszFileName, wszPathT);

	//.	if flag is not for write, delete file on close handle
	if (!bWriteAble) {
		OBJECT_ATTRIBUTES objAttr;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, wszPathT);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		DWORD a1 = FILE_GENERIC_READ | DELETE;
		DWORD o = CreateOptions | FILE_DELETE_ON_CLOSE;
		DWORD c = CreateDisposition;

		NTSTATUS e = lv_orgNtCreateFile(
			FileHandle,
			a1,
			&objAttr,
			IoStatusBlock,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			0,
			FILE_OPEN,
			o,
			NULL,
			0
		);
		/*
		NTSTATUS e = lv_orgNtCreateFile(FileHandle, d, &objAttr,
			IoStatusBlock, AllocationSize, FileAttributes, 0, CreateDisposition,
			o, EaBuffer, EaLength);
			*/
		return e;
	}
	else {
		//	save handle
		OBJECT_ATTRIBUTES objAttr;
		UNICODE_STRING fileName;

		RtlInitUnicodeString(&fileName, wszPathT);
		InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

		NTSTATUS nsRet = lv_orgNtCreateFile(FileHandle, DesiredAccess, &objAttr,
			IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition,
			CreateOptions, EaBuffer, EaLength);

		if (NT_SUCCESS(nsRet)) {
			ss_insert_handleW(*FileHandle, wszFileName, wszPathT);
		}
		return nsRet;
	}
}

NTSTATUS NTAPI hook_NtOpenFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
)
{
	if (ObjectAttributes == NULL || ObjectAttributes->ObjectName == NULL) {
		return lv_orgZwOpenFile(FileHandle, DesiredAccess, ObjectAttributes,
			IoStatusBlock, ShareAccess, OpenOptions);
	}

	wchar_t* wszFileName = ObjectAttributes->ObjectName->Buffer;
	BOOL bExceptPath = ClaSecuPolicy::IsExceptPathW(wszFileName);
	BOOL bSecuPath = ClaSecuPolicy::IsSecuPathW(wszFileName);
	BOOL bCryptoPath = ClaSecuPolicy::IsCryptoPathW(wszFileName);
	BOOL bTempPath = ClaSecuPolicy::IsTempPathW(wszFileName);
	DWORD dwDesiredAccess = DesiredAccess;

	NTSTATUS hRet = lv_orgZwOpenFile(FileHandle, DesiredAccess, ObjectAttributes,
		IoStatusBlock, ShareAccess, OpenOptions);
	if ((bExceptPath || bSecuPath || bCryptoPath || bTempPath) && (dwDesiredAccess & DELETE) && NT_SUCCESS(hRet)) {
		add_open_handle(*FileHandle);
	}
	return hRet;
}

BOOL NTAPI hook_NtClose(HANDLE hObject)
{
	BOOL		bRet = FALSE;
	wchar_t		wszPathOrg[MAX_PATH]; memset(wszPathOrg, 0, sizeof(wszPathOrg));
	wchar_t		wszPathTemp[MAX_PATH]; memset(wszPathTemp, 0, sizeof(wszPathTemp));

	bRet = lv_orgNtClose(hObject);
	if (ss_get_handle(hObject, wszPathOrg, wszPathTemp) == TRUE) {
		_encryptFileW(wszPathTemp, wszPathOrg);
		ss_remove_handle(hObject);
		DeleteFile(wszPathTemp);
	}
	remove_open_handle(hObject);
	return bRet;
}

typedef NTSTATUS(NTAPI *FN_NtSetInformationFile)(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
	);
FN_NtSetInformationFile lv_orgNtSetInformationFile;

typedef struct _FILE_RENAME_INFORMATION {
	BOOLEAN ReplaceIfExists; // Set to TRUE to replace a file with the same name, FALSE otherwise
	HANDLE RootDirectory;    // Optional handle to the root directory for relative paths
	ULONG FileNameLength;    // Length of the new name in bytes
	WCHAR FileName[1];       // New name for the file (variable length)
} FILE_RENAME_INFORMATION, * PFILE_RENAME_INFORMATION;

NTSTATUS NTAPI hook_NtSetInformationFile(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
)
{
	if (FileInformationClass == 10/*FileRenameInformation*/) {
		if (is_exist_open_handle(FileHandle)) {
			//.	oepn secu file
			FILE_RENAME_INFORMATION* renameInfo = (FILE_RENAME_INFORMATION*)FileInformation;
			BOOL bExceptPath = ClaSecuPolicy::IsExceptPathW(renameInfo->FileName);
			BOOL bSecuPath = ClaSecuPolicy::IsSecuPathW(renameInfo->FileName);
			BOOL bCryptoPath = ClaSecuPolicy::IsCryptoPathW(renameInfo->FileName);
			BOOL bTempPath = ClaSecuPolicy::IsTempPathW(renameInfo->FileName);

			if (!bExceptPath && !bSecuPath && !bCryptoPath && !bTempPath) {
				LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_FILE, L"BLOCK SetInformationFile %s\n", renameInfo->FileName);
				return STATUS_ACCESS_DENIED;
			}
		}
	}
	return lv_orgNtSetInformationFile(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass
	);
}
typedef int (WINAPI *FN_StartDocA)(
	HDC            hdc,
	const DOCINFOA* lpdi
);
FN_StartDocA lv_orgStartDocA;
typedef int (WINAPI *FN_StartDocW)(
	HDC            hdc,
	const DOCINFOW* lpdi
);
FN_StartDocW lv_orgStartDocW;
int WINAPI hook_StartDocA(HDC hdc, const DOCINFOA* lpdi)
{
	if (ClaSecuPolicy::GetDlpPrintPdf() && ClaSecuPolicy::GetDlpPrintDev()) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to print");
		SetLastError(ERROR_ACCESS_DENIED);
		return 0;
	}
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PRINT, L"Allow to print");
	return lv_orgStartDocA(hdc, lpdi);
}
int WINAPI hook_StartDocW(HDC hdc, const DOCINFOW* lpdi)
{
	if (ClaSecuPolicy::GetDlpPrintPdf() && ClaSecuPolicy::GetDlpPrintDev()) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to print");
		SetLastError(ERROR_ACCESS_DENIED);
		return 0;
	}
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PRINT, L"Allow to print");
	return lv_orgStartDocW(hdc, lpdi);
}

typedef HINTERNET (WINAPI *FN_WinHttpConnect)(
	HINTERNET     hSession,
	LPCWSTR       pswzServerName,
	INTERNET_PORT nServerPort,
	DWORD         dwReserved
	);
FN_WinHttpConnect lv_orgWinHttpConnect;
HINTERNET hook_WinHttpConnect(
	HINTERNET     hSession,
	LPCWSTR       pswzServerName,
	INTERNET_PORT nServerPort,
	DWORD         dwReserved
)
{
	if (pswzServerName != NULL && wcsstr(pswzServerName, L"d.docs.live.net") != NULL) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_FILE, L"Denied to connect");
		SetLastError(ERROR_WINHTTP_CONNECTION_ERROR);
		return NULL;
	}
	LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Allow to connect");
	return lv_orgWinHttpConnect(hSession, pswzServerName, nServerPort, dwReserved);
}

typedef BOOL (WINAPI *FN_OpenPrinterA)(
	_In_  LPSTR             pPrinterName,
	_Out_ LPHANDLE           phPrinter,
	_In_  LPPRINTER_DEFAULTS pDefault
);
FN_OpenPrinterA lv_orgOpenPrinterA;
typedef BOOL(WINAPI *FN_OpenPrinterW)(
	_In_  LPWSTR             pPrinterName,
	_Out_ LPHANDLE           phPrinter,
	_In_  LPPRINTER_DEFAULTS pDefault
	);
FN_OpenPrinterW lv_orgOpenPrinterW;

BOOL WINAPI hook_OpenPrinterA(
	_In_  LPSTR              pPrinterName,
	_Out_ LPHANDLE           phPrinter,
	_In_  LPPRINTER_DEFAULTS pDefault
)
{
	char szPrinter[MAX_PATH]; memset(szPrinter, 0, sizeof(szPrinter));
	if (pPrinterName != NULL) {
		strcpy_s(szPrinter, MAX_PATH, pPrinterName);
		_strupr_s(szPrinter, MAX_PATH);

		if (ClaSecuPolicy::GetDlpPrintPdf()) {
			if (strstr(szPrinter, "PDF") != NULL) {
				LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to open printer[%S]", pPrinterName);
				SetLastError(ERROR_ACCESS_DENIED);
				return FALSE;
			}
		}
		if (ClaSecuPolicy::GetDlpPrintDev()) {
			if (strstr(szPrinter, "PDF") == NULL) {
				LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to open printer[%S]", pPrinterName);
				SetLastError(ERROR_ACCESS_DENIED);
				return FALSE;
			}
		}
	}
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PRINT, L"Allow to open printer[%S]", pPrinterName);
	return lv_orgOpenPrinterA(pPrinterName, phPrinter, pDefault);
}

BOOL WINAPI hook_OpenPrinterW(
	_In_  LPWSTR             pPrinterName,
	_Out_ LPHANDLE           phPrinter,
	_In_  LPPRINTER_DEFAULTS pDefault
)
{
	wchar_t wszPrinter[MAX_PATH]; memset(wszPrinter, 0, sizeof(wszPrinter));
	if (pPrinterName != NULL) {
		wcscpy_s(wszPrinter, MAX_PATH, pPrinterName);
		_wcsupr_s(wszPrinter, MAX_PATH);

		if (ClaSecuPolicy::GetDlpPrintPdf()) {
			if (wcsstr(wszPrinter, L"PDF") != NULL) {
				LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to open printer[%s]", pPrinterName);
				SetLastError(ERROR_ACCESS_DENIED);
				return FALSE;
			}
		}
		if (ClaSecuPolicy::GetDlpPrintDev()) {
			if (wcsstr(wszPrinter, L"PDF") == NULL) {
				LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_S_PRINT, L"Denied to open printer[%s]", pPrinterName);
				SetLastError(ERROR_ACCESS_DENIED);
				return FALSE;
			}
		}
	}
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_PRINT, L"Allow to open printer[%s]", pPrinterName);
	return lv_orgOpenPrinterW(pPrinterName, phPrinter, pDefault);
}

typedef BOOL(APIENTRY* FN_GetOpenFileNameA)(LPOPENFILENAMEA);
typedef BOOL(APIENTRY* FN_GetOpenFileNameW)(LPOPENFILENAMEW);
FN_GetOpenFileNameA lv_orgGetOpenFileNameA;
FN_GetOpenFileNameW lv_orgGetOpenFileNameW;

BOOL APIENTRY hook_GetOpenFileNameA(LPOPENFILENAMEA lpOFN)
{
	BOOL bSts = lv_orgGetOpenFileNameA(lpOFN);
	if (bSts) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_CHROME, L"Open File Dialog[%S]", lpOFN->lpstrFile);
	}
	if (bSts == TRUE && ClaSecuPolicy::IsSecuPathA(lpOFN->lpstrFile)) {
		AfxMessageBox(L"Access denied");
		return FALSE;
	}
	return bSts;
}
BOOL APIENTRY hook_GetOpenFileNameW(LPOPENFILENAMEW lpOFN)
{
	BOOL bSts = lv_orgGetOpenFileNameW(lpOFN);
	if (bSts) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_CHROME, L"Open File Dialog[%s]", lpOFN->lpstrFile);
	}
	if (bSts == TRUE && ClaSecuPolicy::IsSecuPathW(lpOFN->lpstrFile)) {
		AfxMessageBox(L"Access denied");
		return FALSE;
	}
	return bSts;
}

typedef UINT (__stdcall* FN_DragQueryFileA)(HDROP hDrop, UINT  iFile, LPSTR lpszFile, UINT  cch); 
typedef UINT (__stdcall* FN_DragQueryFileW)(HDROP hDrop, UINT  iFile, LPWSTR lpszFile, UINT  cch);

FN_DragQueryFileA lv_orgDragQueryFileA;
FN_DragQueryFileW lv_orgDragQueryFileW;

UINT __stdcall hook_DragQueryFileA(HDROP hDrop, UINT  iFile, LPSTR lpszFile, UINT  cch)
{
	UINT nSts = lv_orgDragQueryFileA(hDrop, iFile, lpszFile, cch);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_CHROME, L"DragQueryFile [%S]", lpszFile);
	if (nSts != 0 && ClaSecuPolicy::IsSecuPathA(lpszFile)) {
		if (lpszFile) lpszFile[0] = 0;
		return 0;
	}
	return nSts;
}
UINT __stdcall hook_DragQueryFileW(HDROP hDrop, UINT  iFile, LPWSTR lpszFile, UINT  cch)
{
	UINT nSts = lv_orgDragQueryFileW(hDrop, iFile, lpszFile, cch);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_CHROME, L"DragQueryFile [%s]", lpszFile);
	if (nSts != 0 && ClaSecuPolicy::IsSecuPathW(lpszFile)) {
		if (lpszFile) lpszFile[0] = 0;
		return 0;
	}
	return nSts;
}

unsigned int TF_DETACH(void*);
BOOL lv_bDetourSecu = FALSE;
extern wchar_t g_wszDllPath[MAX_PATH];
void hook_functions()
{
	memset(g_wszExePath, 0, sizeof(g_wszExePath));
	GetModuleFileName(NULL, g_wszExePath, MAX_PATH);

	BOOL bNonExplorer = ClaPathMgr::CmpProcName(g_wszExePath, L"explorer.exe");
		
	HMODULE hModKernel = LoadLibraryW(L"kernelbase.dll");
	if (hModKernel)
	{
		lv_orgCreateProcessA = (FN_CreateProcessA)GetProcAddress(hModKernel, "CreateProcessA");
		lv_orgCreateProcessW = (FN_CreateProcessW)GetProcAddress(hModKernel, "CreateProcessW");
		lv_orgCreateProcessAsUserA = (FN_CreateProcessAsUserA)GetProcAddress(hModKernel, "CreateProcessAsUserA");
		lv_orgCreateProcessAsUserW = (FN_CreateProcessAsUserW)GetProcAddress(hModKernel, "CreateProcessAsUserW");
	}

	HMODULE hModNtDll = LoadLibraryW(L"ntdll.dll");
	lv_orgNtCreateFile	= (FN_NtCreateFile)GetProcAddress(hModNtDll, "NtCreateFile");
	lv_orgNtReadFile	= (FN_NtReadFile)GetProcAddress(hModNtDll, "NtReadFile");
	lv_orgNtWriteFile	= (FN_NtWriteFile)GetProcAddress(hModNtDll, "NtWriteFile");
	lv_orgNtClose		= (FN_NtClose)GetProcAddress(hModNtDll, "NtClose");
	lv_orgNtQueryInformationFile = (FN_NtQueryInformationFile)GetProcAddress(hModNtDll, "NtQueryInformationFile");
	lv_orgZwOpenFile	= (FN_NtOpenFile)GetProcAddress(hModNtDll, "ZwOpenFile");
	lv_orgNtSetInformationFile = (FN_NtSetInformationFile)GetProcAddress(hModNtDll, "NtSetInformationFile");

	HMODULE hModGdi32Dll = LoadLibraryW(L"Gdi32.dll");
	lv_orgStartDocA = (FN_StartDocA)GetProcAddress(hModGdi32Dll, "StartDocA");
	lv_orgStartDocW = (FN_StartDocW)GetProcAddress(hModGdi32Dll, "StartDocW");

	HMODULE hModWinhttp = LoadLibraryW(L"winhttp.dll");
	lv_orgWinHttpConnect = (FN_WinHttpConnect)GetProcAddress(hModWinhttp, "WinHttpConnect");
	
	HMODULE hModSpooldrv = LoadLibraryW(L"Winspool.drv");
	lv_orgOpenPrinterA = (FN_OpenPrinterA)GetProcAddress(hModSpooldrv, "OpenPrinterA");
	lv_orgOpenPrinterW = (FN_OpenPrinterW)GetProcAddress(hModSpooldrv, "OpenPrinterW");

	HMODULE hModComdlg32 = LoadLibraryW(L"Comdlg32.dll");
	lv_orgGetOpenFileNameA = (FN_GetOpenFileNameA)GetProcAddress(hModComdlg32, "GetOpenFileNameA");
	lv_orgGetOpenFileNameW = (FN_GetOpenFileNameW)GetProcAddress(hModComdlg32, "GetOpenFileNameW");

	HMODULE hModShell32 = LoadLibraryW(L"Shell32.dll");
	lv_orgDragQueryFileA = (FN_DragQueryFileA)GetProcAddress(hModShell32, "DragQueryFileA");
	lv_orgDragQueryFileW = (FN_DragQueryFileW)GetProcAddress(hModShell32, "DragQueryFileW");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourAttach(&(PVOID&)lv_orgCreateProcessA, hook_CreateProcessA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgCreateProcessW, hook_CreateProcessW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	
	if (DetourAttach(&(PVOID&)lv_orgCreateProcessAsUserA, hook_CreateProcessAsUserA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgCreateProcessAsUserW, hook_CreateProcessAsUserW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (DetourAttach(&(PVOID&)lv_orgNtCreateFile, hook_NtCreateFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgZwOpenFile, hook_NtOpenFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgNtClose, hook_NtClose) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgNtSetInformationFile, hook_NtSetInformationFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgStartDocA, hook_StartDocA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgStartDocW, hook_StartDocW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgOpenPrinterA, hook_OpenPrinterA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgOpenPrinterW, hook_OpenPrinterW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourAttach(&(PVOID&)lv_orgWinHttpConnect, hook_WinHttpConnect) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (ClaPathMgr::CmpProcName(g_wszExePath, L"chrome.exe") == 0) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_C_HOOK, L"Chrome-HookFunctions");
		if (DetourAttach(&(PVOID&)lv_orgDragQueryFileA, hook_DragQueryFileA) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
		if (DetourAttach(&(PVOID&)lv_orgDragQueryFileW, hook_DragQueryFileW) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}

	if (DetourTransactionCommit() != NO_ERROR)	{
		OutputDebugStringW(L"err");
	}

	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DETACH, NULL, 0, &dwTID);
	lv_bDetourSecu = TRUE;
}

void unhook_secu_functions()
{
	if (!lv_bDetourSecu) return;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (DetourDetach(&(PVOID&)lv_orgCreateProcessA, hook_CreateProcessA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgCreateProcessW, hook_CreateProcessW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgCreateProcessAsUserA, hook_CreateProcessAsUserA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgCreateProcessAsUserW, hook_CreateProcessAsUserW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgNtCreateFile, hook_NtCreateFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgZwOpenFile, hook_NtOpenFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgNtClose, hook_NtClose) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgNtSetInformationFile, hook_NtSetInformationFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgStartDocA, hook_StartDocA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgStartDocW, hook_StartDocW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgOpenPrinterA, hook_OpenPrinterA) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgOpenPrinterW, hook_OpenPrinterW) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgWinHttpConnect, hook_WinHttpConnect) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	if (DetourDetach(&(PVOID&)lv_orgNtSetInformationFile, hook_NtSetInformationFile) != NO_ERROR) {
		OutputDebugStringW(L"err");
	}

	if (ClaPathMgr::CmpProcName(g_wszExePath, L"chrome.exe") == 0) {
		if (DetourDetach(&(PVOID&)lv_orgDragQueryFileA, hook_DragQueryFileA) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
		if (DetourDetach(&(PVOID&)lv_orgDragQueryFileW, hook_DragQueryFileW) != NO_ERROR) {
			OutputDebugStringW(L"err");
		}
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		OutputDebugStringW(L"err");
	}
	lv_bDetourSecu = FALSE;
}

unsigned int TF_DETACH(void*) {
	while (true)
	{
		Sleep(100);
		HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Global\\Kenan_Mutex_Stop");
		if (hMutex != NULL) {
			//.	terminate
			if (lv_bDetourSecu)
			{
				unhook_secu_functions();
				TerminateProcess(GetCurrentProcess(), 0);
			}
			CloseHandle(hMutex);
			break;
		}
	}
	return 0;
}