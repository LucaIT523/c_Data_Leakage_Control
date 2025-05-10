
// KenanDiskMgrDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanDiskMgr.h"
#include "KenanDiskMgrDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanDiskMgrDlg dialog



CKenanDiskMgrDlg::CKenanDiskMgrDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTDISKMGR_DIALOG, pParent)
	, m_strImage1(_T("D:\\1.dat"))
	, m_strImage2(_T("D:\\1.dat"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanDiskMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_IMG_PATH1, m_strImage1);
	DDX_Text(pDX, IDC_EDT_IMG_PATH2, m_strImage2);
}

BEGIN_MESSAGE_MAP(CKenanDiskMgrDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_IMG_PATH1, &CKenanDiskMgrDlg::OnBnClickedBtnImgPath1)
	ON_BN_CLICKED(IDC_BTN_CREATE_IMAGE, &CKenanDiskMgrDlg::OnBnClickedBtnCreateImage)
	ON_BN_CLICKED(IDC_BTN_IMG_PATH2, &CKenanDiskMgrDlg::OnBnClickedBtnImgPath2)
	ON_BN_CLICKED(IDC_BTN_MOUNT, &CKenanDiskMgrDlg::OnBnClickedBtnMount)
	ON_BN_CLICKED(IDC_BTN_DISMOUNT, &CKenanDiskMgrDlg::OnBnClickedBtnDismount)
END_MESSAGE_MAP()

typedef unsigned int (*FN_VD_CREATE)(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, int p_nSize);
typedef unsigned int (*FN_VD_MOUNT_INIT)();
typedef unsigned int (*FN_VD_MOUNT)(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, wchar_t& p_zDrv);

FN_VD_CREATE		VD_Create;
FN_VD_MOUNT_INIT	VD_MountInit;
FN_VD_MOUNT			VD_Mount;

// CKenanDiskMgrDlg message handlers

BOOL CKenanDiskMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
#ifdef _WIN64
	HMODULE hMod = LoadLibrary(L"KenanVDMgr64.dll");
#else
	HMODULE hMod = LoadLibrary(L"KenanVDMgr32.dll");
#endif // _WIN64
	VD_Create	 = (FN_VD_CREATE	)GetProcAddress(hMod, "VD_CREATE");
	VD_MountInit = (FN_VD_MOUNT_INIT)GetProcAddress(hMod, "VD_MOUNT_INIT");
	VD_Mount	 = (FN_VD_MOUNT		)GetProcAddress(hMod, "VD_MOUNT");

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanDiskMgrDlg::OnPaint()
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
HCURSOR CKenanDiskMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKenanDiskMgrDlg::OnBnClickedBtnImgPath1()
{
	UpdateData(TRUE);

	CFileDialog dlg(FALSE, m_strImage1);
	if (dlg.DoModal() != IDOK) {
		return;
	}

	m_strImage1 = dlg.GetPathName();
	UpdateData(FALSE);
}

void CKenanDiskMgrDlg::OnBnClickedBtnCreateImage()
{
	UpdateData(TRUE);

	unsigned long lErr = VD_Create(m_strImage1, L"Test1234", 200);
	CString strMsg;
	strMsg.Format(L"%08X", lErr);
	AfxMessageBox(strMsg);
}


void CKenanDiskMgrDlg::OnBnClickedBtnImgPath2()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, m_strImage2);
	if (dlg.DoModal() != IDOK) {
		return;
	}

	m_strImage2 = dlg.GetPathName();
	UpdateData(FALSE);
}


void CKenanDiskMgrDlg::OnBnClickedBtnMount()
{
	UpdateData(TRUE);

	wchar_t wzDrv;
	unsigned long lErr = VD_Mount(m_strImage2, L"Test1234", wzDrv);
	CString strMsg;
	strMsg.Format(L"%08X - %c:\\", lErr, wzDrv);
	AfxMessageBox(strMsg);

}


void CKenanDiskMgrDlg::OnBnClickedBtnDismount()
{
	unsigned long lErr = VD_MountInit();
	CString strMsg;
	strMsg.Format(L"%08X", lErr);
	AfxMessageBox(strMsg);
}
