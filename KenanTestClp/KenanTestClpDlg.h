
// KenanTestClpDlg.h : header file
//

#pragma once


// CKenanTestClpDlg dialog
class CKenanTestClpDlg : public CDialog
{
// Construction
public:
	CKenanTestClpDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SFTTESTCLP_DIALOG };
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
	afx_msg void OnBnClickedBtnHook();
	afx_msg void OnBnClickedBtnUnhook();
};
