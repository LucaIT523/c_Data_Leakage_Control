// KenanSecu.h : main header file for the KenanSecu DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKenanSecuApp
// See KenanSecu.cpp for the implementation of this class
//

class CKenanSecuApp : public CWinApp
{
public:
	CKenanSecuApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
