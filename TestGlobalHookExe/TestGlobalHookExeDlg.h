
// TestGlobalHookExeDlg.h : header file
//

#pragma once


// CTestGlobalHookExeDlg dialog
class CTestGlobalHookExeDlg : public CDialog
{
// Construction
public:
	CTestGlobalHookExeDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTGLOBALHOOKEXE_DIALOG };
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
	afx_msg void OnBnClickedBtnPath();
	afx_msg void OnBnClickedOk();
	CString m_strPath;
};
