#pragma once
#include "afxdialogex.h"
#include "kn_conf.h"


// ClaDlgSetting dialog

class ClaDlgSetting : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgSetting)

public:
	ClaDlgSetting(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgSetting();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	ST_SETTING m_stSetting;
	BOOL m_bDlpFile;
	BOOL m_bDlpCapture;
	BOOL m_bDlpClp;
	BOOL m_bDlpPrintPDF;
	BOOL m_bDlpPrintDev;
	CString m_strEnc;
	BOOL m_bLog;
	DWORD m_dwLogLevel;
	DWORD m_dwLogArea;
	afx_msg void OnBnClickedBtnEnc();
	afx_msg void OnBnClickedBtnEncImport();
	afx_msg void OnBnClickedBtnEncExport();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedChkLog();
	BOOL m_bMornitorGlobal;
};
