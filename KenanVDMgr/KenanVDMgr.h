// KenanVDMgr.h : main header file for the KenanVDMgr DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanVDMgrApp
// See KenanVDMgr.cpp for the implementation of this class
//

class CKenanVDMgrApp : public CWinApp
{
public:
	CKenanVDMgrApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
