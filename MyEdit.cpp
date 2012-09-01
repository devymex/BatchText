
#include "stdafx.h"
#include "BatchText.h"
#include "MyEdit.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit(ME_TYPE Type)
: m_Type(Type)
, m_nMaxLen(0)
{

}

CMyEdit::~CMyEdit()
{
}

void CMyEdit::SetMaxLength(UINT nMaxLength)
{
	m_nMaxLen = nMaxLength;
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CMyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (m_Type)
	{
	case MET_EDITOR:
		if (nChar == '\t')
		{
			ReplaceSel(_T("\t")); 
			return;
		}
		break;
	case MET_FILENAME:
		break;
	case MET_FOLDER:
		break;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyEdit::ChooseFolder()
{
	TCHAR szPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = GetParent()->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("Choose destination folder");
	bi.ulFlags = BIF_USENEWUI;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl = SHBrowseForFolder(&bi);
	if (NULL != pidl)
	{
		if (SHGetPathFromIDList(pidl, szPath))
		{
			SetWindowText(szPath);
		}
	}
}

void CMyEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	switch (m_Type)
	{
	case MET_FOLDER:
		ChooseFolder();
		return;
	case MET_FILENAME:
		break;
	case MET_EDITOR:
		break;
	}
	CEdit::OnLButtonDblClk(nFlags, point);
}

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg)
{
	if (m_Type == MET_FILENAME && pMsg->message == WM_KEYDOWN)
	{
		UINT nChar = MapVirtualKey(pMsg->wParam, MAPVK_VK_TO_CHAR);
		char szCode[] = "\\/:*?\"<>|";
		int nCodeNum = sizeof(szCode) - 1;
		if (*std::find(&szCode[0], &szCode[nCodeNum], (char)nChar) != 0)
		{
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
