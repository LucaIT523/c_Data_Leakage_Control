#include "pch.h"
#include "KenanVD.h"
#include "KenanVDMgr.h"
#include "ClaFileMap.h"
#include "ClaEvent.h"
#include "ClaProcess.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)
#define LD_VF_NAME_ORG	L"vf.exe"
#define LD_V_NAME_ORG	L"v.exe"

#define LD_VF_NAME		getPath(LD_VF_NAME_ORG)
#define LD_V_NAME		getPath(LD_V_NAME_ORG)

#define LD_FILEMAP_VD	L"Global\\filemap-vd"
#define LD_EVENT_VD_REQ	L"Global\\event-vd-req"
#define LD_EVENT_VD_RSP	L"Global\\event-vd-rsp"
ClaFileMap lv_filemap_vd;

#define LD_MODE_CREATE	1
#define LD_MODE_CLEAR	2
#define LD_MODE_MOUNT	3
#define LD_MODE_EXIT	4

HANDLE lv_hEventReq = NULL, lv_hEventRsp = NULL;

typedef struct tagVdParam {
	unsigned int m_nMODE;

	union {
		struct {
			wchar_t m_wszCreatePath[MAX_PATH];
			wchar_t m_wszCreatePwd[MAX_PATH];
			int m_nCreateSize;
		};
		struct {
			wchar_t m_wszMountPath[MAX_PATH];
			wchar_t m_wszMountPwd[MAX_PATH];
			wchar_t m_wzMountDrv;
		};
		unsigned char m_pBuff[1500];
	};

	unsigned int m_nCode;
}ST_VD_PARAM;
ST_VD_PARAM* lv_pVdParam;

int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam)
{
	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = L"open";
	ShExecInfo.lpFile = p_szEXE;
	ShExecInfo.lpParameters = p_pszCommandParam; //
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE; // SW_NORMAL

	if (ShellExecuteEx(&ShExecInfo)) {
		// Wait for the process to exit
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	return 1;
}

CString getPath(const wchar_t* p_wszName) {
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));

	GetModuleFileName(NULL, wszPath, MAX_PATH);
	wchar_t * pPos = wcsrchr(wszPath, L'\\');
	pPos[1] = 0x0;
	wcscat_s(wszPath, MAX_PATH, p_wszName);
	return wszPath;
}


unsigned int VD_WAIT_REQ() {
	WaitForSingleObject(lv_hEventReq, INFINITE);

	if (lv_pVdParam->m_nMODE == LD_MODE_EXIT) {
		return -1;
	}
	
	return 0;
}

void _open_event() {
	if (lv_hEventReq == NULL) {
		lv_hEventReq = ClaEvent::Open(LD_EVENT_VD_REQ);
	}
	if (lv_hEventRsp == NULL) {
		lv_hEventRsp = ClaEvent::Create(LD_EVENT_VD_RSP);
	}
}
unsigned int VD_SEND_REQ_CREATE(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, int p_nSize/*MB*/) {
	_open_event();

	lv_filemap_vd.openMap(LD_FILEMAP_VD, (void**)&lv_pVdParam);
	wcscpy_s(lv_pVdParam->m_wszCreatePath, MAX_PATH, p_wszPath);
	wcscpy_s(lv_pVdParam->m_wszCreatePwd, MAX_PATH, p_wszPwd);
	lv_pVdParam->m_nCreateSize = p_nSize;
	lv_pVdParam->m_nMODE = LD_MODE_CREATE;
	SetEvent(lv_hEventReq);
	return 0;
}
unsigned int VD_SEND_REQ_CLEAR(void) {
	_open_event();

	lv_filemap_vd.openMap(LD_FILEMAP_VD, (void**)&lv_pVdParam);
	lv_pVdParam->m_nMODE = LD_MODE_CLEAR;
	SetEvent(lv_hEventReq);
	return 0;
}
unsigned int VD_SEND_REQ_MOUNT(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, const wchar_t p_wzDrv) {
	_open_event();

	lv_filemap_vd.openMap(LD_FILEMAP_VD, (void**)&lv_pVdParam);
	wcscpy_s(lv_pVdParam->m_wszMountPath, MAX_PATH, p_wszPath);
	wcscpy_s(lv_pVdParam->m_wszMountPwd, MAX_PATH, p_wszPwd);
	lv_pVdParam->m_wzMountDrv = p_wzDrv;
	lv_pVdParam->m_nMODE = LD_MODE_MOUNT;
	SetEvent(lv_hEventReq);
	return 0;
}
unsigned int VD_WAIT_RSP(DWORD p_dwTimeout = INFINITE) {
	WaitForSingleObject(lv_hEventRsp, p_dwTimeout);

	return lv_pVdParam->m_nCode;
}
unsigned int VD_SEND_RSP() {
	SetEvent(lv_hEventRsp);
	return 0;
}

DLL_EXPORT unsigned int VD_CREATE(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, int p_nSize/*MB*/)
{
	//.	if user is not admin, send request to agent.
	if (IsUserAnAdmin() == false && ClaProcess::FindProcessID(L"KenanAgent64.exe") != 0) {
		VD_SEND_REQ_CREATE(p_wszPath, p_wszPwd, p_nSize);
		return VD_WAIT_RSP();
	}

	int i;
	CString strCmd;

	if (GetFileAttributes(LD_VF_NAME) == INVALID_FILE_ATTRIBUTES) {
		return GD_ERR_VD_NO_VF;
	}

	strCmd.Format(
		L"/create \"%s\" /password \"%s\" /hash sha512 /encryption AES /quick /size %dM /filesystem NTFS /pim 0 /silent /force /fastcreatefile", 
		p_wszPath, p_wszPwd, p_nSize);
	_execute(LD_VF_NAME, strCmd);
	//	wait for 5s
	for (i = 0; i < 5; i++) {
		if (GetFileAttributes(p_wszPath) == INVALID_FILE_ATTRIBUTES) {
			Sleep(1000);
			continue;
		}
		break;
	}
	if (i == 5) {
		return GD_ERR_VD_FAIL_CREATE;
	}
	
	return GD_ERR_VD_SUCCESS;
}

DLL_EXPORT unsigned int VD_MOUNT_INIT()
{
	//.	if user is not admin, send request to agent.
	if (IsUserAnAdmin() == false && ClaProcess::FindProcessID(L"KenanAgent64.exe") != 0) {
		VD_SEND_REQ_CLEAR();
		return VD_WAIT_RSP();
	}

	//	dismount all 
	CString strCmd;
	wchar_t wszDrv[] = L"?:\\";

	if (GetFileAttributes(LD_V_NAME) == INVALID_FILE_ATTRIBUTES) {
		return GD_ERR_VD_NO_V;
	}

	strCmd.Format(L"/q /dismount /silent /force");
	_execute(LD_V_NAME, strCmd);
	//	wait for 3s
	//Sleep(3000);

	return GD_ERR_VD_SUCCESS;
}

DLL_EXPORT unsigned int VD_MOUNT(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, wchar_t& p_zDrv)
{
	//.	if user is not admin, send request to agent.
	if (IsUserAnAdmin() == false && ClaProcess::FindProcessID(L"KenanAgent64.exe") != 0) {
		VD_SEND_REQ_MOUNT(p_wszPath, p_wszPwd, p_zDrv);
		return VD_WAIT_RSP(3000);
	}

	int i;
	CString strCmd;
	wchar_t wszDrv[] = L"?:\\";

	if (GetFileAttributes(LD_V_NAME) == INVALID_FILE_ATTRIBUTES) {
		return GD_ERR_VD_NO_V;
	}

	for (wszDrv[0] = L'Z'; wszDrv[0] > L'C'; wszDrv[0]--) {
		if (GetFileAttributes(wszDrv) == INVALID_FILE_ATTRIBUTES) {
			break;
		}
	}

	strCmd.Format(
		L"/q /v \"%s\" /p \"%s\" /l %c  /a /silent",
		p_wszPath, p_wszPwd, wszDrv[0]);
	_execute(LD_V_NAME, strCmd);
	//	wait for 5s
	for (i = 0; i < 5; i++) {
		if (GetFileAttributes(wszDrv) == INVALID_FILE_ATTRIBUTES) {
			Sleep(1000);
			continue;
		}
		break;
	}
	if (i == 5) {
		return GD_ERR_VD_FAIL_MOUNT;
	}

	p_zDrv = wszDrv[0];
	return GD_ERR_VD_SUCCESS;
}

DWORD WINAPI TF_VD_WAIT(void*) {
	lv_filemap_vd.createMap(LD_FILEMAP_VD, sizeof(ST_VD_PARAM), (void**)&lv_pVdParam);
	lv_hEventReq = ClaEvent::Create(LD_EVENT_VD_REQ);
	lv_hEventRsp = ClaEvent::Create(LD_EVENT_VD_RSP);
	ST_VD_PARAM* param = lv_pVdParam;

	while (true)
	{
		if (VD_WAIT_REQ() != 0) {
			break;
		}

		if (param->m_nMODE == LD_MODE_CREATE) {
			lv_pVdParam->m_nCode = VD_CREATE(param->m_wszCreatePath, param->m_wszCreatePwd, param->m_nCreateSize);
			VD_SEND_RSP();
			continue;
		}

		if (param->m_nMODE == LD_MODE_MOUNT) {
			lv_pVdParam->m_nCode = VD_MOUNT(param->m_wszMountPath, param->m_wszMountPwd, param->m_wzMountDrv);
			VD_SEND_RSP();
			continue;
		}

		if (param->m_nMODE == LD_MODE_CLEAR) {
			lv_pVdParam->m_nCode = VD_MOUNT_INIT();
			VD_SEND_RSP();
			continue;
		}
	}

	return 0;
}

DLL_EXPORT unsigned int VD_WAIT()
{
	DWORD dwTID;
	CreateThread(NULL, 0, TF_VD_WAIT, NULL, 0, &dwTID);

	return 0;
}

