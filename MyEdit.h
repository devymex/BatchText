#pragma once


// CMyEdit

class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)
public:
	enum ME_TYPE{MET_EDITOR, MET_FOLDER, MET_FILENAME};
	CMyEdit(ME_TYPE Type);
	virtual ~CMyEdit();
	void SetMaxLength(UINT nMaxLength);

protected:
	UINT m_nMaxLen;
	const ME_TYPE m_Type;

	void ChooseFolder();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};


