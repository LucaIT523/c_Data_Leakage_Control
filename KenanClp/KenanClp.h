// KenanClp.h : main header file for the KenanClp DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanClpApp
// See KenanClp.cpp for the implementation of this class
//

class CKenanClpApp : public CWinApp
{
public:
	CKenanClpApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
