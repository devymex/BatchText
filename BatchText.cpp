
// BatchText.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BatchText.h"
#include "BatchTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DWORD MultiByteToCString(UINT nDefCP, LPCSTR pBuf, CStringW &strOut)
{
	int nLen = MultiByteToWideChar(nDefCP, 0, pBuf, -1, NULL, 0);
	if (nLen == 0)
	{
		return GetLastError();
	}
	LPWSTR pContent = LPWSTR(VirtualAlloc(NULL, nLen * sizeof(WCHAR),
		MEM_COMMIT, PAGE_READWRITE));
	MultiByteToWideChar(nDefCP, 0, pBuf, -1, pContent, nLen);
	strOut = pContent;
	VirtualFree(pContent, 0, MEM_RELEASE);
	return 0;
}


DWORD LoadTextFile(LPCTSTR lpFileName, UINT nDefCP, CStringW &strOut)
{
	CFile File;
	CFileException e;
	if (!File.Open(lpFileName, CFile::modeRead, &e))
	{
		return e.m_cause;
	}
	LONG lSize = LONG(File.GetLength());
	if (lSize == 0)
	{
		return ERROR_INVALID_DATA;
	}
	LPSTR pBuf = (LPSTR)VirtualAlloc(NULL, lSize + 1,
		MEM_COMMIT, PAGE_READWRITE);
	if (pBuf == NULL)
	{
		return GetLastError();
	}
	File.Read(pBuf, lSize);

	if (pBuf[0] == '\xEF' && pBuf[1] == '\xBB' && pBuf[2] == '\xBF')
	{
		MultiByteToCString(CP_UTF8, pBuf + 3, strOut);
	}
	else if (pBuf[0] == '\xFF' && pBuf[1] == '\xFE')
	{
		strOut = LPWSTR(pBuf + 2);
	}
	else
	{
		MultiByteToCString(nDefCP, pBuf, strOut);
	}
	VirtualFree(pBuf, 0, MEM_RELEASE);
	File.Close();
	return 0;
}


BEGIN_MESSAGE_MAP(CBatchTextApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CBatchTextApp::CBatchTextApp()
{
}
CBatchTextApp theApp;

BOOL CBatchTextApp::InitInstance()
{
	CWinAppEx::InitInstance();

	CBatchTextDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}
