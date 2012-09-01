
// BatchTextDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "TextParser.h"
#include "myedit.h"

class CBatchTextDlg : public CDialog
{
public:
	CBatchTextDlg(CWnd* pParent = NULL);
	~CBatchTextDlg();
	enum {IDD = IDD_BATCHTEXT_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	HICON m_hIcon;
	CButton m_ctrlStart;
	CMyEdit m_ctrlPattern;
	CMyEdit m_ctrlFolder;
	CMyEdit m_ctrlFileName;
	CListCtrl m_ctrlVariables;
	CComboBox m_ctrlEnc;
	STRTABLE m_VarTbl;
	STRARRAY m_ContPat;
	STRARRAY m_FilePat;

	void LoadVarTbl(const CString &strBuf);
	void UpdateVarTbl();
	BOOL SelectTextFile(CString &strFile);
	void SaveTextFile(CString &strFile, CStringW &strContent);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDblClkCtrlVariables(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnStartClicked();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
};
