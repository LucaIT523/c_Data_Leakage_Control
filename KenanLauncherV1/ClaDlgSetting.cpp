// ClaDlgSetting.cpp : implementation file
//

#include "pch.h"
#include "KenanLauncherV1.h"
#include "afxdialogex.h"
#include "ClaDlgSetting.h"


// ClaDlgSetting dialog

IMPLEMENT_DYNAMIC(ClaDlgSetting, CDialog)

ClaDlgSetting::ClaDlgSetting(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_SETTING, pParent)
	, m_bDlpFile(FALSE)
	, m_bDlpCapture(FALSE)
	, m_bDlpClp(FALSE)
	, m_bDlpPrintPDF(FALSE)
	, m_bDlpPrintDev(FALSE)
	, m_strEnc(_T(""))
	, m_bLog(FALSE)
	, m_dwLogLevel(0)
	, m_dwLogArea(0)
	, m_bMornitorGlobal(FALSE)
{

}

ClaDlgSetting::~ClaDlgSetting()
{
}

void ClaDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_FCO, m_bDlpFile);
	DDX_Check(pDX, IDC_CHK_CAPTURE, m_bDlpCapture);
	DDX_Check(pDX, IDC_CHK_CLP, m_bDlpClp);
	DDX_Text(pDX, IDC_EDT_ENC, m_strEnc);
	DDX_Check(pDX, IDC_CHK_LOG, m_bLog);
	DDX_Text(pDX, IDC_EDT_LOG_LEVEL, m_dwLogLevel);
	DDX_Text(pDX, IDC_EDT_LOG_AREA, m_dwLogArea);
	DDX_Check(pDX, IDC_CHK_PRINT_PDF, m_bDlpPrintPDF);
	DDX_Check(pDX, IDC_CHK_PRINT_DEV, m_bDlpPrintDev);
	DDX_Check(pDX, IDC_CHK_GLOBAL_MONITOR, m_bMornitorGlobal);
}


BEGIN_MESSAGE_MAP(ClaDlgSetting, CDialog)
	ON_BN_CLICKED(IDC_BTN_ENC, &ClaDlgSetting::OnBnClickedBtnEnc)
	ON_BN_CLICKED(IDC_BTN_ENC_IMPORT, &ClaDlgSetting::OnBnClickedBtnEncImport)
	ON_BN_CLICKED(IDC_BTN_ENC_EXPORT, &ClaDlgSetting::OnBnClickedBtnEncExport)
	ON_BN_CLICKED(IDC_BTN_SET, &ClaDlgSetting::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDOK, &ClaDlgSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_LOG, &ClaDlgSetting::OnBnClickedChkLog)
END_MESSAGE_MAP()


// ClaDlgSetting message handlers


BOOL ClaDlgSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_bDlpFile			= m_stSetting.m_bFile;
	m_bDlpCapture		= m_stSetting.m_bCapture;
	m_bDlpClp			= m_stSetting.m_bClipboard;
	m_bDlpPrintPDF		= m_stSetting.m_bPrint2PDF;
	m_bDlpPrintDev		= m_stSetting.m_bPrint2Dev;
	m_bMornitorGlobal	= m_stSetting.m_bGlobal;
	
	m_strEnc = L"";
	for (int i = 0; i < 32; i++) {
		m_strEnc.AppendFormat(L"%02X", m_stSetting.m_pKey[i]);
	}
	m_bLog = m_stSetting.m_bLog;
	m_dwLogLevel = m_stSetting.m_dwLogLevel;
	m_dwLogArea = m_stSetting.m_dwLogArea;

	UpdateData(FALSE);

	GetDlgItem(IDC_EDT_LOG_LEVEL)->EnableWindow(m_bLog);
	GetDlgItem(IDC_EDT_LOG_AREA)->EnableWindow(m_bLog);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ClaDlgSetting::OnBnClickedBtnEnc()
{
	UpdateData(TRUE);

	conf_get_new_key(m_stSetting.m_pKey);

	m_strEnc = L"";
	for (int i = 0; i < 32; i++) {
		m_strEnc.AppendFormat(L"%02X", m_stSetting.m_pKey[i]);
	}
	UpdateData(FALSE);
}


void ClaDlgSetting::OnBnClickedBtnEncImport()
{
	UpdateData(TRUE);

	unsigned char p[32];
	CFileDialog dlg(TRUE, L"key", NULL, 6, L"Crypto key file|*.key||");
	if (dlg.DoModal() != IDOK) {
		return ;
	}

	FILE* pFile = NULL;
	_wfopen_s(&pFile, dlg.GetPathName(), L"rb");
	if (pFile == NULL) {
		AfxMessageBox(L"Failed to open file."); return;
	}
	int n = fread(p, 1, 32, pFile);
	fclose(pFile);

	if (n != 32) {
		AfxMessageBox(L"Failed to read crypto file"); return;
	}

	memcpy(m_stSetting.m_pKey, p, 32);
	m_strEnc = L"";
	for (int i = 0; i < 32; i++) {
		m_strEnc.AppendFormat(L"%02X", m_stSetting.m_pKey[i]);
	}
	UpdateData(FALSE);

	AfxMessageBox(L"Import the crypto key file successfully.", MB_OK | MB_ICONINFORMATION);
}


void ClaDlgSetting::OnBnClickedBtnEncExport()
{
	UpdateData(TRUE);

	CFileDialog dlg(FALSE, L"key", NULL, 6, L"Crypto key file|*.key||");
	if (dlg.DoModal() != IDOK) {
		return;
	}

	FILE* pFile = NULL;
	_wfopen_s(&pFile, dlg.GetPathName(), L"rb");
	if (pFile == NULL) {
		AfxMessageBox(L"Failed to open file."); return;
	}
	fwrite(m_stSetting.m_pKey, 1, 32, pFile);
	fclose(pFile);

	AfxMessageBox(L"Export the crypto key file successfully.", MB_OK | MB_ICONINFORMATION);
}


void ClaDlgSetting::OnBnClickedBtnSet()
{
	UpdateData(TRUE);

	if (m_strEnc.GetLength() != 64) {
		AfxMessageBox(L"Please input the correct key"); return;
	}

	for (int i = 0; i < 32; i++) {
		wchar_t wszNum[3];
		wszNum[0] = m_strEnc[i * 2];
		wszNum[1] = m_strEnc[i * 2 + 1];
		wszNum[2] = 0x0;
		int n; swscanf_s(wszNum, L"%02X", &n);
		m_stSetting.m_pKey[i] = n;
	}

	AfxMessageBox(L"Set the crypto key file successfully.", MB_OK | MB_ICONINFORMATION);
}


void ClaDlgSetting::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_stSetting.m_bFile			= m_bDlpFile;
	m_stSetting.m_bCapture		= m_bDlpCapture;
	m_stSetting.m_bClipboard	= m_bDlpClp;
	m_stSetting.m_bPrint2PDF	= m_bDlpPrintPDF;
	m_stSetting.m_bPrint2Dev	= m_bDlpPrintDev;
	m_stSetting.m_bGlobal		= m_bMornitorGlobal;

	m_stSetting.m_bLog			= m_bLog;
	m_stSetting.m_dwLogLevel	= m_dwLogLevel;
	m_stSetting.m_dwLogArea		= m_dwLogArea;

	CDialog::OnOK();
}


void ClaDlgSetting::OnBnClickedChkLog()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_EDT_LOG_LEVEL)->EnableWindow(m_bLog);
	GetDlgItem(IDC_EDT_LOG_AREA)->EnableWindow(m_bLog);
}
