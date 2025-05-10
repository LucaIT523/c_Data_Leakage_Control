// ClaDlgAppConf.cpp : implementation file
//

#include "pch.h"
#include "KenanLauncherV1.h"
#include "afxdialogex.h"
#include "ClaDlgAppConf.h"


// ClaDlgAppConf dialog

IMPLEMENT_DYNAMIC(ClaDlgAppConf, CDialog)

ClaDlgAppConf::ClaDlgAppConf(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_APP, pParent)
	, m_strName(_T(""))
	, m_strPath(_T(""))
	, m_strInfo(_T(""))
{

}

ClaDlgAppConf::~ClaDlgAppConf()
{
}

void ClaDlgAppConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDT_PATH, m_strPath);
	DDX_Text(pDX, IDC_STC_INFO, m_strInfo);
}


BEGIN_MESSAGE_MAP(ClaDlgAppConf, CDialog)
	ON_BN_CLICKED(IDC_BTN_PATH, &ClaDlgAppConf::OnBnClickedBtnPath)
	ON_BN_CLICKED(IDOK, &ClaDlgAppConf::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &ClaDlgAppConf::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, &ClaDlgAppConf::OnBnClickedButton5)
END_MESSAGE_MAP()


// ClaDlgAppConf message handlers


BOOL ClaDlgAppConf::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_strName = m_stApp.m_wszName;
	m_strPath = m_stApp.m_wszPath;
	m_strInfo.Format(L"%S", m_stApp.m_szInfo);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgAppConf::OnBnClickedBtnPath()
{
	CFileDialog dlg(TRUE, L"exe", NULL, 6, L"Executable File|*.exe||");
	if (dlg.DoModal() != IDOK) {
		return;
	}

	char szInfo[512]; memset(szInfo, 0, sizeof(szInfo));
	if (conf_get_app_info(dlg.GetPathName(), szInfo, 512) != true) {
		AfxMessageBox(L"Failed to get file information");
		return;
	}

	CString strInfo; strInfo.Format(L"%S", szInfo); strInfo.MakeUpper();
	CString strName = m_strName; strName.MakeUpper();
//	if (strInfo.Find(strName) == -1) {
//		AfxMessageBox(L"Invalid file was selected."); return;
//	}

	m_strPath = dlg.GetPathName();
	m_strInfo.Format(L"%S", szInfo);
	UpdateData(FALSE);
}


void ClaDlgAppConf::OnBnClickedOk()
{
	wcscpy_s(m_stApp.m_wszPath, m_strPath);
	sprintf_s(m_stApp.m_szInfo, "%S", m_strInfo);

	CDialog::OnOK();
}


void ClaDlgAppConf::OnBnClickedButton2()
{
	m_stApp.m_nStatus = 0xFF;
}


void ClaDlgAppConf::OnBnClickedButton5()
{
	m_stApp.m_nStatus = 1;
}
