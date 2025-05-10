#pragma once
#include "afxdialogex.h"
#include "kn_conf.h"

// ClaDlgAppConf dialog

class ClaDlgAppConf : public CDialog
{
	DECLARE_DYNAMIC(ClaDlgAppConf)

public:
	ClaDlgAppConf(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ClaDlgAppConf();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_APP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	ST_CONF_APP m_stApp;
	afx_msg void OnBnClickedBtnPath();
	afx_msg void OnBnClickedOk();
	CString m_strName;
	CString m_strPath;
	CString m_strInfo;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton5();
};
