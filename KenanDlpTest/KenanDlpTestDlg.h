
// KenanDlpTestDlg.h : header file
//

#pragma once

#include "ClaStaticDrag.h"

// CKenanDlpTestDlg dialog
class CKenanDlpTestDlg : public CDialog
{
// Construction
public:
	CKenanDlpTestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SFTDLPTEST_DIALOG };
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
	CString m_strProcName;
	int m_nPID;
	CString m_strWnd;
	CString m_strDllPath;
	CString m_strExePath;
	afx_msg void OnBnClickedBtnFind();
	ClaStaticDrag m_stcDrag;
	afx_msg void OnBnClickedBtnDllPath();
	afx_msg void OnBnClickedBtnInject();
	afx_msg void OnBnClickedBtnExePath();
	afx_msg void OnBnClickedBtnInjectRun();
	afx_msg void OnBnClickedBtnGHook();
	afx_msg void OnBnClickedBtnGUnhook();
	afx_msg void OnBnClickedOk();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CString m_strCaption;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnHide();
	afx_msg void OnBnClickedBtnShow();
	afx_msg void OnBnClickedBtnShow2();
	afx_msg void OnBnClickedButton3();
};
