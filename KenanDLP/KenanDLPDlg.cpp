
// KenanDLPDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "KenanDLP.h"
#include "KenanDLPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanDLPDlg dialog



CKenanDLPDlg::CKenanDLPDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SFTDLP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKenanDLPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKenanDLPDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CKenanDLPDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CKenanDLPDlg message handlers

BOOL CKenanDLPDlg::OnInitDialog()
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

void CKenanDLPDlg::OnPaint()
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
HCURSOR CKenanDLPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <winternl.h>
#include <ntstatus.h>

typedef NTSTATUS(NTAPI* FN_NtCreateFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength);
FN_NtCreateFile lv_orgNtCreateFile;
FN_NtCreateFile lv_orgZwCreateFile;
typedef NTSTATUS(NTAPI* FN_NtClose)(HANDLE);
FN_NtClose lv_orgNtClose;
typedef NTSTATUS(NTAPI* FN_NtReadFile)(
	HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID,
	PIO_STATUS_BLOCK, PVOID, ULONG,
	PLARGE_INTEGER, PULONG);
FN_NtReadFile lv_orgNtReadFile;
typedef NTSTATUS(NTAPI* FN_NtWriteFile)(
	HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID,
	PIO_STATUS_BLOCK, PVOID, ULONG,
	PLARGE_INTEGER, PULONG);
FN_NtWriteFile lv_orgNtWriteFile;

typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, * PFILE_STANDARD_INFORMATION;

typedef NTSTATUS(NTAPI* FN_NtQueryInformationFile)(
	HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation, ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass);
FN_NtQueryInformationFile lv_orgNtQueryInformationFile;

#pragma comment(lib, "ntdll.lib")

void CKenanDLPDlg::OnBnClickedOk()
{
	/*
	HMODULE hModNtDll = LoadLibraryW(L"ntdll.dll");
	lv_orgNtCreateFile = (FN_NtCreateFile)GetProcAddress(hModNtDll, "NtCreateFile");
	lv_orgZwCreateFile = (FN_NtCreateFile)GetProcAddress(hModNtDll, "ZwCreateFile");

	CString strMessage;
	strMessage.Format(L"nt - %llx, zw - %llx", lv_orgNtCreateFile, lv_orgZwCreateFile);
	AfxMessageBox(strMessage);
	return;
	lv_orgNtReadFile = (FN_NtReadFile)GetProcAddress(hModNtDll, "NtReadFile");
	lv_orgNtWriteFile = (FN_NtWriteFile)GetProcAddress(hModNtDll, "NtWriteFile");
	lv_orgNtClose = (FN_NtClose)GetProcAddress(hModNtDll, "NtClose");
	lv_orgNtQueryInformationFile = (FN_NtQueryInformationFile)GetProcAddress(hModNtDll, "NtQueryInformationFile");

	UNICODE_STRING fileName;
	RtlInitUnicodeString(&fileName, L"\\??\\Z:\\.tmp\\1");

	OBJECT_ATTRIBUTES objAttr;
	InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	HANDLE hFile;
	IO_STATUS_BLOCK ioStatusBlock;

	DWORD a1 = FILE_GENERIC_READ | DELETE;
	NTSTATUS status = lv_orgNtCreateFile(
		&hFile,
		a1,
		&objAttr,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE,
		NULL,
		0
	);

	if (status != STATUS_SUCCESS) {
		return;
	}

	FILE_STANDARD_INFORMATION fileInfo = { 0 };
	status = lv_orgNtQueryInformationFile(
		hFile,
		&ioStatusBlock,
		&fileInfo,
		sizeof(fileInfo),
		(FILE_INFORMATION_CLASS)5//FileStandardInformation
	);
	lv_orgNtClose(hFile);

	if (status != STATUS_SUCCESS) {
		return;
	}
	// TODO: Add your control notification handler code here
//	CDialog::OnOK();
*/
}
