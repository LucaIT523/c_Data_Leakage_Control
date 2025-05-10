// KenanGlobalHookTest.h : main header file for the KenanGlobalHookTest DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanGlobalHookTestApp
// See KenanGlobalHookTest.cpp for the implementation of this class
//

class CKenanGlobalHookTestApp : public CWinApp
{
public:
	CKenanGlobalHookTestApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
