
// KenanLauncherV1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanLauncherV1.h"
#include "KenanLauncherV1Dlg.h"
#include "afxdialogex.h"
#include "ClaDlgSetting.h"
#include "ClaDlgAppConf.h"
#include "KenanDlp.h"
#include "kn_conf.h"
#include "ClaRegMgr.h"
#include "ClaLogPolicy.h"
#include "Global.h"

#include "ClaPathMgr.h"
#include "CLaSecuSvc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanLauncherV1Dlg dialog

CKenanLauncherV1Dlg::CKenanLauncherV1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_KENANLAUNCHERV1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanLauncherV1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_APP, m_lstApp);
}

BEGIN_MESSAGE_MAP(CKenanLauncherV1Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SETTING, &CKenanLauncherV1Dlg::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDC_BTN_CONF, &CKenanLauncherV1Dlg::OnBnClickedBtnConf)
	ON_BN_CLICKED(IDC_BTN_LAUNCH, &CKenanLauncherV1Dlg::OnBnClickedBtnLaunch)
	ON_BN_CLICKED(IDCANCEL, &CKenanLauncherV1Dlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LST_APP, &CKenanLauncherV1Dlg::OnNMClickLstApp)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_APP, &CKenanLauncherV1Dlg::OnNMDblclkLstApp)
	ON_BN_CLICKED(IDC_BTN_RESET, &CKenanLauncherV1Dlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CKenanLauncherV1Dlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_LOGOUT, &CKenanLauncherV1Dlg::OnBnClickedBtnLogout)
END_MESSAGE_MAP()


// CKenanLauncherV1Dlg message handlers

BOOL CKenanLauncherV1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launcher start");
	_initUI();

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"dlp init start");
	if (dlp_init() != 0) {
		LogPrintW(GD_LOG_LEVEL_ERROR, GD_LOG_AREA_LAUNCHER, L"dlp init failed");
		EndDialog(IDCANCEL);
		return TRUE;
	}

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"load config start");
	if (_loadConfig() == false) {
		LogPrintW(GD_LOG_LEVEL_ERROR, GD_LOG_AREA_LAUNCHER, L"load config failed");
		EndDialog(IDCANCEL);
		return TRUE;
	}

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launcher start running");
	_showAppInfo();
	_enableUI();


	CString strExePath32, strExePath64;
	strExePath32.Format(L"%s\\KenanAgent32.exe", ClaPathMgr::GetDP());
	strExePath64.Format(L"%s\\KenanAgent64.exe", ClaPathMgr::GetDP());

	ClaSecuSvc::Launch(L"winlogon.exe", strExePath32, L"kenan-global", NULL);
	ClaSecuSvc::Launch(L"winlogon.exe", strExePath64, L"kenan-global", NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanLauncherV1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKenanLauncherV1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKenanLauncherV1Dlg::OnBnClickedBtnSetting()
{
	if (dlp_is_exist_secu()) {
		AfxMessageBox(L"There are security process now, please stop all secu process first");
		return;
	}

	ClaDlgSetting dlgSetting;

	memcpy(&dlgSetting.m_stSetting, conf_get_setting_ptr(), sizeof(ST_SETTING));

	if (dlgSetting.DoModal() == IDOK) {
		memcpy(conf_get_setting_ptr(), &dlgSetting.m_stSetting, sizeof(ST_SETTING));
		conf_save();
		AfxMessageBox(L"Update the settings successfully.", MB_OK | MB_ICONINFORMATION);
	}
}

void CKenanLauncherV1Dlg::OnBnClickedBtnConf()
{
	if (_nSelectedItem == -1) {
		return;
	}

	ST_CONF_APP* pApp = (ST_CONF_APP*)m_lstApp.GetItemData(_nSelectedItem);

	ClaDlgAppConf dlgConf;
	memcpy(&dlgConf.m_stApp, pApp, sizeof(ST_CONF_APP));

	if (dlgConf.DoModal() == IDOK) {
		memcpy(pApp, &dlgConf.m_stApp, sizeof(ST_CONF_APP));
		conf_save();

		_showAppInfo();
	}
}

void CKenanLauncherV1Dlg::OnBnClickedBtnLaunch()
{
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch start");

	if (_nSelectedItem == -1) return;

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch get app");

	ST_CONF_APP* p = (ST_CONF_APP*)m_lstApp.GetItemData(_nSelectedItem);
	if (p->m_nStatus == 1) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_LAUNCHER, L"launch not support [%s]", p->m_wszPath);
		AfxMessageBox(L"This program was not supported now."); return;
	}

	if (p->m_nStatus != 0xFF) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_LAUNCHER, L"launch unknown exception");
		AfxMessageBox(L"Unknown Exception."); return;
	}

	if (GetFileAttributes(p->m_wszPath) == INVALID_FILE_ATTRIBUTES) {
		LogPrintW(GD_LOG_LEVEL_WARN, GD_LOG_AREA_LAUNCHER, L"launch exe not found");
		AfxMessageBox(L"Cannot find the executable files."); return;
	}

	//.	mount driver and create security environment.
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch get config");
	
	ST_SETTING *conf = conf_get_setting_ptr();
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch conf - file[%d], clp[%d], log[%d], scr[%d], global[%d], pdf[%d], prt[%d]", 
		conf->m_bFile, conf->m_bClipboard, conf->m_bLog, conf->m_bCapture, conf->m_bGlobal, conf->m_bPrint2PDF, conf->m_bPrint2Dev
		);

	unsigned int lSts = dlp_mount(conf);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch mount = %d", lSts);
	if (lSts != 0) {
		return;
	}

	lSts = dlp_launch_new(p);
	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_LAUNCHER, L"launch new = %d", lSts);
}

void CKenanLauncherV1Dlg::OnBnClickedCancel()
{
	dlp_finish(true);
	CDialog::OnCancel();
}

void CKenanLauncherV1Dlg::_initUI()
{
	m_lstApp.SetExtendedStyle(m_lstApp.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lstApp.setHead(L" ;Name;Path;Status");
	m_lstApp.autoFitWidth();

	imgList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);
}

bool CKenanLauncherV1Dlg::_loadConfig()
{
	if (conf_exist() == false) {
		conf_create_default();
		if (conf_save() == false) {
			AfxMessageBox(L"Failed to create configuration file.");
			return false;
		}
	}

	conf_load();

	ST_SETTING* p = conf_get_setting_ptr();
	ClaLogPolicy::RegisterLogPolicy(p->m_bLog, p->m_dwLogLevel, p->m_dwLogArea);

	return true;
}

CString getAppStatus(ST_CONF_APP* p_pApp) {
	if (p_pApp->m_nStatus == 0) {
		return L"Fail";
	}
	if (p_pApp->m_nStatus == 1) {
		return L"Not Support";
	}
	if (p_pApp->m_nStatus == 0xFF) {
		if (p_pApp->m_wszPath[0] == 0x0) {
			return L"No Executable File Selected";
		}

		if (GetFileAttributes(p_pApp->m_wszPath) == INVALID_FILE_ATTRIBUTES) {
			return L"No Executable File";
		}

		return L"OK";
	}
	return L"-";
}

void CKenanLauncherV1Dlg::_showAppInfo()
{
	m_lstApp.DeleteAllItems();
	int nAppCount = conf_get_app_count();

	while (imgList.GetImageCount() > 0)
		imgList.Remove(0);

	imgList.Add(AfxGetApp()->LoadIconW(IDI_BLOCK));

	for (int i = 0; i < nAppCount; i++) {
		HICON hIcon;
		ST_CONF_APP* pstAppInfo = conf_get_app_info_ptr(i);
		UINT nIcons = ExtractIconEx(pstAppInfo->m_wszPath, 0, NULL, &hIcon, 1);
		if (nIcons > 0 && hIcon != NULL) {
			// Successfully loaded the icon.
			// Proceed to the next steps.
			pstAppInfo->m_nIconIndex = imgList.Add(hIcon);
			DestroyIcon(hIcon); // After adding it to the image list, destroy the icon.
		}
		else {
			pstAppInfo->m_nIconIndex = -1;
		}
	}

	m_lstApp.SetImageList(&imgList, LVSIL_SMALL);

	for (int i = 0; i < nAppCount; i++) {
		ST_CONF_APP* pstAppInfo = conf_get_app_info_ptr(i);
		int nSts;
		if (pstAppInfo->m_nIconIndex == -1)
		{
			nSts = m_lstApp.addRecord(
				L"",
				pstAppInfo->m_wszName,
				pstAppInfo->m_wszPath,
				getAppStatus(pstAppInfo)
			);
		}
		else {
			nSts = m_lstApp.addRecordWithIcon(
				pstAppInfo->m_nIconIndex,
				L"",
				pstAppInfo->m_wszName,
				pstAppInfo->m_wszPath,
				getAppStatus(pstAppInfo)
			);
		}
		m_lstApp.SetItemData(nSts, (DWORD_PTR)pstAppInfo);
	}
	m_lstApp.autoFitWidth();
	_nSelectedItem = -1;
}

void CKenanLauncherV1Dlg::_enableUI()
{
	GetDlgItem(IDC_BTN_CONF)->EnableWindow(_nSelectedItem != -1);
	GetDlgItem(IDC_BTN_LAUNCH)->EnableWindow(_nSelectedItem != -1);
}

void CKenanLauncherV1Dlg::OnNMClickLstApp(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	_nSelectedItem = pNMItemActivate->iItem;

	_enableUI();
}


void CKenanLauncherV1Dlg::OnNMDblclkLstApp(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	_nSelectedItem = pNMItemActivate->iItem;
	_enableUI();

	if (_nSelectedItem == -1) return;

	ST_CONF_APP* p = (ST_CONF_APP*)m_lstApp.GetItemData(_nSelectedItem);
	if (p->m_wszPath[0] == 0) {
		OnBnClickedBtnConf();
	}
	else {
		OnBnClickedBtnLaunch();
	}
}


void CKenanLauncherV1Dlg::OnBnClickedBtnReset()
{
	conf_create_default();
	if (conf_save() == false) {
		AfxMessageBox(L"Failed to create configuration file."); return;
	}
	conf_load();

	_showAppInfo();
}


void CKenanLauncherV1Dlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	if (dlp_mount(conf_get_setting_ptr()) == 0) {
		AfxMessageBox(L"The Secu Area has been mounted.", MB_ICONINFORMATION);
	}
}


void CKenanLauncherV1Dlg::OnBnClickedBtnLogout()
{
	// TODO: Add your control notification handler code here
	if (dlp_is_exist_secu()) {
		if (AfxMessageBox(L"Do you really want to stop all the security process and logout virtual drive?", MB_YESNO) != IDYES)
			return;
	}

	dlp_finish(true);

	AfxMessageBox(L"The Secu Area has been dismounted.", MB_ICONINFORMATION);
}
