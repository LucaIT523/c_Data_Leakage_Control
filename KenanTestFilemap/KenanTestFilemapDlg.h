
// KenanTestFilemapDlg.h : header file
//

#pragma once


// CKenanTestFilemapDlg dialog
class CKenanTestFilemapDlg : public CDialog
{
// Construction
public:
	CKenanTestFilemapDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SFTTESTFILEMAP_DIALOG };
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
	afx_msg void OnBnClickedBtnWrite();
	afx_msg void OnBnClickedBtnRead();
};
