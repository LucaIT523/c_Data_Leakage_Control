
// KenanDiskMgrDlg.h : header file
//

#pragma once


// CKenanDiskMgrDlg dialog
class CKenanDiskMgrDlg : public CDialog
{
// Construction
public:
	CKenanDiskMgrDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SFTDISKMGR_DIALOG };
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
	CString m_strImage1;
	CString m_strImage2;
	afx_msg void OnBnClickedBtnImgPath1();
	int _execute(const wchar_t* p_szEXE, const wchar_t* p_pszCommandParam);
	afx_msg void OnBnClickedBtnCreateImage();
	afx_msg void OnBnClickedBtnImgPath2();
	afx_msg void OnBnClickedBtnMount();
	afx_msg void OnBnClickedBtnDismount();
};
