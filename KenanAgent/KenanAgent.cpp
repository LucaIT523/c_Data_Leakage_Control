
// KenanAgent.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "KenanAgent.h"
#include "KenanAgentDlg.h"
#include "ClaSecuPolicy.h"
#include "ClaMutex.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKenanAgentApp

BEGIN_MESSAGE_MAP(CKenanAgentApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKenanAgentApp construction

CKenanAgentApp::CKenanAgentApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKenanAgentApp object

CKenanAgentApp theApp;


// CKenanAgentApp initialization

BOOL CKenanAgentApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"initinstance");

#ifdef _WIN64
	if (ClaMutex::IsCreate(L"Global\\Agent64") == TRUE) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"already created x64");
		return FALSE;
	}
	ClaMutex::Create(L"Global\\Agent64");
#else
	if (ClaMutex::IsCreate(L"Global\\Agent32") == TRUE) {
		LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_AGENT, L"already created x86");
		return FALSE;
	}
	ClaMutex::Create(L"Global\\Agent32");
#endif

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CString strCmdLine = GetCommandLine();
	wchar_t wszPath[MAX_PATH]; memset(wszPath, 0, sizeof(wszPath));
	GetModuleFileNameW(NULL, wszPath, MAX_PATH);

	if (strCmdLine.Find(L"kenan-global") >= 0) {
#ifdef _DEBUG
#endif //_DEBUG
		CKenanAgentDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
			TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
		}
	}
	else {

	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

