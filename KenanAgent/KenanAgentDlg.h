
// KenanAgentDlg.h : header file
//

#pragma once


// CKenanAgentDlg dialog
class CKenanAgentDlg : public CDialog
{
// Construction
public:
	CKenanAgentDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SFTAGENT_DIALOG };
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
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
