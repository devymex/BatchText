
// BatchText.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

DWORD MultiByteToCString(UINT nDefCP, LPCSTR pBuf, CString &strOut);

DWORD LoadTextFile(LPCTSTR lpFileName, UINT nDefCP, CStringW &strOut);

// CBatchTextApp:
// See BatchText.cpp for the implementation of this class
//

class CBatchTextApp : public CWinAppEx
{
public:
	CBatchTextApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CBatchTextApp theApp;