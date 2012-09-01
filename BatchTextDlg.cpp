
// BatchTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BatchText.h"
#include "BatchTextDlg.h"
#include "TextParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CODEPAGE_GB2312 936

CBatchTextDlg::CBatchTextDlg(CWnd* pParent)
	: CDialog(CBatchTextDlg::IDD, pParent)
	, m_ctrlPattern(CMyEdit::MET_EDITOR)
	, m_ctrlFolder(CMyEdit::MET_FOLDER)
	, m_ctrlFileName(CMyEdit::MET_FILENAME)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CBatchTextDlg::~CBatchTextDlg()
{
}

void CBatchTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LC_VARIABLES, m_ctrlVariables);
	DDX_Control(pDX, IDC_ED_PATTERN, m_ctrlPattern);
	DDX_Control(pDX, IDC_ED_DEST_FOLDER, m_ctrlFolder);
	DDX_Control(pDX, IDC_ED_FILE_NAME, m_ctrlFileName);
	DDX_Control(pDX, IDC_BN_START, m_ctrlStart);
	DDX_Control(pDX, IDC_CB_ENCODING, m_ctrlEnc);
}

BEGIN_MESSAGE_MAP(CBatchTextDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LC_VARIABLES, &CBatchTextDlg::OnDblClkCtrlVariables)
	ON_BN_CLICKED(IDC_BN_START, &CBatchTextDlg::OnBnStartClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBatchTextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctrlVariables.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS |
		LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctrlPattern.SetLimitText(100000000);
	m_ctrlPattern.SetWindowText(_T("Type or paste your pattern text here."));
	m_ctrlFolder.SetWindowText(_T("Double click here to choose a destination folder."));
	m_ctrlFileName.SetWindowText(_T("Type pattern of files name here."));
	m_ctrlVariables.InsertColumn(0, _T("Double click below to load data file"));
	m_ctrlVariables.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	m_ctrlEnc.SetCurSel(0);

	return TRUE;
}

void CBatchTextDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CBatchTextDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBatchTextDlg::OnDblClkCtrlVariables(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString strFile;
	if (SelectTextFile(strFile))
	{
		m_VarTbl.clear();
		CStringW strContent;
		LoadTextFile(strFile, CODEPAGE_GB2312, strContent);
		ParseVarArray(strContent, m_VarTbl);
		for (STRTABLE::iterator i = m_VarTbl.begin(); i != m_VarTbl.end();)
		{
			if (i->empty())
			{
				i = m_VarTbl.erase(i);
			}
			else
			{
				++i;
			}
		}
		UpdateVarTbl();
	}
	*pResult = 0;
}

void CBatchTextDlg::UpdateVarTbl()
{
	m_ctrlVariables.DeleteAllItems();
	int nColumnCount = m_ctrlVariables.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
	   m_ctrlVariables.DeleteColumn(0);
	}
	size_t nColCnt = 0;
	for (size_t i = 0; i < m_VarTbl.size(); ++i)
	{
		if (m_VarTbl[i].size() > nColCnt)
		{
			nColCnt = m_VarTbl[i].size();
		}
	}

	for (size_t i = 0; i < nColCnt; ++i)
	{
		CString str;
		str.Format(_T("%%%d%%"), i);
		m_ctrlVariables.InsertColumn(i, str);
	}

	for (size_t i = 0; i < m_VarTbl.size(); ++i)
	{
		m_ctrlVariables.InsertItem(i, _T(""));
		for (size_t j = 0; j < m_VarTbl[i].size(); ++j)
		{
			m_ctrlVariables.SetItemText(i, j, CString(m_VarTbl[i][j]));
		}
	}
	for (size_t i = 0; i < nColCnt; ++i)
	{
		m_ctrlVariables.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
}

BOOL CBatchTextDlg::SelectTextFile(CString &strFile)
{
	BOOL br = FALSE;
	CFileDialog dlgOpenFile(TRUE, NULL, NULL, OFN_EXPLORER | OFN_ENABLESIZING,
		_T("Text Documents (*.txt)|*.TXT|All Files (*.*)|*.*||"), this);
	if (IDOK == dlgOpenFile.DoModal())
	{
		strFile = dlgOpenFile.GetPathName();
		br = TRUE;
	}
	return br;
}

void GenTextFromPat(const STRARRAY &Pat, const STRARRAY &Var, CStringW &strOut)
{
	if (!Pat.empty() || Pat.size() % 2 != 1)
	{
		strOut.Empty();

		STRARRAY::const_iterator i = Pat.begin();
		strOut.Append(*i++);
		for (; i != Pat.end(); ++i)
		{
			BOOL bFile = FALSE;
			CStringW strCurVar = *i;
			if (strCurVar[0] == _T('f') || strCurVar[0] == _T('F'))
			{
				bFile = TRUE;
				strCurVar.Delete(0, 1);
			}
			UINT nNum = atoi(CStringA(strCurVar));
			if (nNum < Var.size())
			{
				if (bFile)
				{
					CStringW strFile;
					if (0 == LoadTextFile(CString(Var[nNum]),
						CODEPAGE_GB2312, strFile))
					{
						strOut.Append(strFile);
					}
				}
				else
				{
					strOut.Append(Var[nNum]);
				}
			}
			strOut.Append(*++i);
		}
	}
}

void CBatchTextDlg::SaveTextFile(CString &strFile, CStringW &strContent)
{
	CFile file;
	file.Open(strFile, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
	file.SeekToEnd();

	int nLen;
	std::vector<BYTE> Buf;

	switch (m_ctrlEnc.GetCurSel())
	{
	case 0:
		nLen = WideCharToMultiByte(CODEPAGE_GB2312, 0,
			strContent, -1, NULL, 0, NULL, NULL) - 1;
		Buf.resize(nLen);
		if (nLen != 0)
		{
			WideCharToMultiByte(CODEPAGE_GB2312, 0,
				strContent, -1, (LPSTR)&Buf[0], nLen, NULL, NULL);
		}
		break;
	case 1:
		Buf.resize(strContent.GetLength() * 2 + 2);
		Buf[0] = 0xFE;
		Buf[1] = 0xFF;
		CopyMemory(&Buf[2], strContent.GetBuffer(), Buf.size());
		break;
	case 2:
		nLen = WideCharToMultiByte(CP_UTF8, 0,
			strContent, -1, NULL, 0, NULL, NULL) - 1;
		Buf.resize(nLen + 3);
		Buf[0] = 0xEF;
		Buf[1] = 0xBB;
		Buf[2] = 0xBF;
		WideCharToMultiByte(CP_UTF8, 0,
			strContent, -1, (LPSTR)&Buf[3], nLen, NULL, NULL);
		break;
	}
	file.Write(&Buf[0], Buf.size());
}

void CBatchTextDlg::OnBnStartClicked()
{
	if (m_VarTbl.empty())
	{
		return;
	}

	//处理文件内容模式
	CString str;
	m_ctrlPattern.GetWindowText(str);
	CStringW strContent(str);
	m_ContPat.clear();
	ParsePattern(strContent, m_ContPat);

	//处理文件名模式
	m_ctrlFileName.GetWindowText(str);
	CStringW strFileName(str);
	m_FilePat.clear();
	ParsePattern(strFileName, m_FilePat);

	//处理目标文件夹
	CString strFolder;
	m_ctrlFolder.GetWindowText(strFolder);
	if (FALSE == PathFileExists(strFolder))
	{
		CreateDirectory(strFolder, NULL);
	}
	if (strFolder[strFolder.GetLength() - 1] != _T('\\'))
	{
		strFolder.AppendChar(_T('\\'));
	}

	if (m_FilePat.size() == 1)
	{
		CString strPathName = strFolder + m_FilePat.front();
		DeleteFile(strPathName);
	}
	//开始生成
	for (STRTABLE::iterator i = m_VarTbl.begin();
		i != m_VarTbl.end(); ++i)
	{
		GenTextFromPat(m_ContPat, *i, strContent);
		GenTextFromPat(m_FilePat, *i, strFileName);
		CString strPathName = strFolder + CString(strFileName);
		SaveTextFile(strPathName, strContent);
	}
}
