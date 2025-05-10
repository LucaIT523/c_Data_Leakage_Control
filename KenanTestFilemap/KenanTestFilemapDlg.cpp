
// KenanTestFilemapDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanTestFilemap.h"
#include "KenanTestFilemapDlg.h"
#include "afxdialogex.h"
#include "ClaSecuPolicy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanTestFilemapDlg dialog



CKenanTestFilemapDlg::CKenanTestFilemapDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTTESTFILEMAP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanTestFilemapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKenanTestFilemapDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_WRITE, &CKenanTestFilemapDlg::OnBnClickedBtnWrite)
	ON_BN_CLICKED(IDC_BTN_READ, &CKenanTestFilemapDlg::OnBnClickedBtnRead)
END_MESSAGE_MAP()


// CKenanTestFilemapDlg message handlers

BOOL CKenanTestFilemapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKenanTestFilemapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKenanTestFilemapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CKenanTestFilemapDlg::OnBnClickedBtnWrite()
{
	ClaSecuPolicy::RegisterSecuProcess(100);
	ClaSecuPolicy::RegisterClipboardFlag(TRUE);
}


void CKenanTestFilemapDlg::OnBnClickedBtnRead()
{
	BOOL bSts = ClaSecuPolicy::IsSecuProcess(100);
}
