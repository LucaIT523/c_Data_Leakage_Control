
// KenanLauncherV1Dlg.h : header file
//

#pragma once

#include "ClaListCtrl.h"

// CKenanLauncherV1Dlg dialog
class CKenanLauncherV1Dlg : public CDialog
{
// Construction
public:
	CKenanLauncherV1Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KENANLAUNCHERV1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	ClaListCtrl m_lstApp;
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedBtnConf();
	afx_msg void OnBnClickedBtnLaunch();
	afx_msg void OnBnClickedCancel();

protected:
	void _initUI();
	bool _loadConfig();
	void _showAppInfo();
	void _enableUI();
public:
	afx_msg void OnNMClickLstApp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkLstApp(NMHDR* pNMHDR, LRESULT* pResult);

	CImageList imgList;
	int _nSelectedItem;
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLogout();
};
