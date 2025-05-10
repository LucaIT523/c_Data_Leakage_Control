// TestGlobalHookDLL.h : main header file for the TestGlobalHookDLL DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestGlobalHookDLLApp
// See TestGlobalHookDLL.cpp for the implementation of this class
//

class CTestGlobalHookDLLApp : public CWinApp
{
public:
	CTestGlobalHookDLLApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
