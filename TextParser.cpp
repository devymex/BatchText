#include "stdafx.h"
#include "TextParser.h"
#include <map>
#include <string>
#include <algorithm>

typedef UINT (*CONVCODE)(WCHAR, LPARAM);
typedef void (*OPTRFUNC)(WCHAR, LPARAM);

struct LEXICALNODE
{
	UINT nStat;
	UINT nNext;
	UINT nSymb;
	OPTRFUNC pOptr;
};

bool LessLexNode(LEXICALNODE &n1, LEXICALNODE &n2)
{
	if (n1.nStat < n2.nStat)
	{
		return true;
	}
	else if (n1.nStat == n2.nStat && n1.nSymb < n2.nSymb)
	{
		return true;
	}
	return false;
}

void LexicalAnalize(CStringW &str, LEXICALNODE *pLexTbl, UINT nStatNum,
					UINT nSymbNum, CONVCODE pConvFunc, LPARAM lParam)
{
	UINT nStatus = 0;
	for (int i = 0; nStatus < nStatNum; ++i)
	{
		WCHAR cCode = -1;
		if (i < str.GetLength())
		{
			cCode = str[i];
		}
		UINT nSymb = pConvFunc(cCode, lParam);
		LEXICALNODE &Node = pLexTbl[nStatus * nSymbNum + nSymb];
		if (Node.pOptr != NULL)
		{
			Node.pOptr(cCode, lParam);
		}
		nStatus = Node.nNext;
	}
}

void OP_PT_N(WCHAR cCode, LPARAM lParam)
{
	((STRARRAY*)lParam)->push_back(CStringW());
}

void OP_PT_F(WCHAR cCode, LPARAM lParam)
{
	((STRARRAY*)lParam)->push_back(CStringW());
	((STRARRAY*)lParam)->back().AppendChar(_T('f'));
	((STRARRAY*)lParam)->back().AppendChar(cCode);
}

void OP_PT_W(WCHAR cCode, LPARAM lParam)
{
	((STRARRAY*)lParam)->push_back(CStringW(cCode));
}

void OP_PT_C(WCHAR cCode, LPARAM lParam)
{
	STRARRAY &Seg = *(STRARRAY*)lParam;
	if (Seg.empty())
	{
		Seg.push_back(CStringW());
	}
	Seg.back().AppendChar(cCode);
}

UINT ConvPattern(WCHAR cCode, LPARAM lParam)
{
	UINT nSymb = 0;
	if (cCode == '%')
	{ //PER
		nSymb = 1;
	}
	else if (cCode >= '0' && cCode <= '9')
	{ //NUM
		nSymb = 2;
	}
	else if (cCode == 'f')
	{ //NUM
		nSymb = 3;
	}
	else if (cCode == WCHAR(-1))
	{ //END
		nSymb = 4;
	}
	return nSymb;
}

void ParsePattern(CStringW &str, STRARRAY &Segs)
{
	static std::map<std::string, UINT> SymbMap;
	if (SymbMap.size() == 0)
	{
		SymbMap["OTH"] = 0;
		SymbMap["PER"] = 1;
		SymbMap["NUM"] = 2;
		SymbMap["LCF"] = 3;
		SymbMap["END"] = 4;
	}
	static std::map<std::string, OPTRFUNC> OptrMap;
	if (OptrMap.size() == 0)
	{
		OptrMap["NULL"] = NULL;
		OptrMap["CHAR"] = OP_PT_C;
		OptrMap["WORD"] = OP_PT_W;
		OptrMap["NEWN"] = OP_PT_N;
		OptrMap["NEWF"] = OP_PT_F;
	}
	static LEXICALNODE LexNodes[] = {
		{0, 1, SymbMap["PER"], OptrMap["NULL"]},
		{0, 4, SymbMap["END"], OptrMap["NULL"]},
		{0, 0, SymbMap["OTH"], OptrMap["CHAR"]},
		{1, 2, SymbMap["NUM"], OptrMap["WORD"]},
		{2, 0, SymbMap["OTH"], OptrMap["WORD"]},
		{1, 4, SymbMap["END"], OptrMap["NULL"]},
		{2, 4, SymbMap["END"], OptrMap["NEWN"]},
		{2, 2, SymbMap["NUM"], OptrMap["CHAR"]},
		{1, 0, SymbMap["PER"], OptrMap["CHAR"]},
		{2, 0, SymbMap["PER"], OptrMap["NEWN"]},
		{0, 0, SymbMap["NUM"], OptrMap["CHAR"]},
		{1, 0, SymbMap["OTH"], OptrMap["CHAR"]},
		{1, 3, SymbMap["LCF"], OptrMap["NULL"]},
		{3, 2, SymbMap["NUM"], OptrMap["NEWF"]},
		{3, 1, SymbMap["PER"], OptrMap["NULL"]},
		{3, 0, SymbMap["OTH"], OptrMap["CHAR"]},
		{0, 0, SymbMap["LCF"], OptrMap["CHAR"]},
		{2, 0, SymbMap["LCF"], OptrMap["WORD"]},
		{3, 4, SymbMap["END"], OptrMap["NULL"]},
		{3, 0, SymbMap["LCF"], OptrMap["CHAR"]},
	};
	UINT nNodesNum = sizeof(LexNodes) / sizeof(LexNodes[0]);
	UINT nStatNum = nNodesNum / SymbMap.size();
	std::sort(&LexNodes[0], &LexNodes[nNodesNum], LessLexNode);

	Segs.push_back(CStringW());
	LexicalAnalize(str, LexNodes, nStatNum, SymbMap.size(),
		ConvPattern, (LPARAM)&Segs);
}

void OP_VA_L(WCHAR cSymbol, LPARAM lParam)
{
	((STRTABLE*)lParam)->push_back(STRARRAY());
}

void OP_VA_W(WCHAR cSymbol, LPARAM lParam)
{
	STRTABLE &StrTbl = *((STRTABLE*)lParam);
	if (StrTbl.empty())
	{
		StrTbl.push_back(STRARRAY());
	}
	StrTbl.back().push_back(CStringW());
}

void OP_VA_C(WCHAR cSymbol, LPARAM lParam)
{
	STRTABLE &StrTbl = *((STRTABLE*)lParam);
	if (StrTbl.empty())
	{
		StrTbl.push_back(STRARRAY());
	}
	if (StrTbl.back().empty())
	{
		StrTbl.back().push_back(CStringW());
	}
	StrTbl.back().back().AppendChar(cSymbol);
}

UINT ConvVarArray(WCHAR cCode, LPARAM lParam)
{
	UINT nSymb = 0;
	switch (cCode)
	{
	case '\t':
		nSymb = 1;
		break;
	case '\x0D':
		nSymb = 2;
		break;
	case '\x0A':
		nSymb = 3;
		break;
	case '"':
		nSymb = 4;
		break;
	case WCHAR(-1):
		nSymb = 5;
		break;
	}
	return nSymb;
}

void ParseVarArray(CStringW &str, STRTABLE &VarTbl)
{
	static std::map<std::string, UINT> SymbMap;
	if (SymbMap.size() == 0)
	{
		SymbMap["OTH"] = 0;
		SymbMap["TAB"] = 1;
		SymbMap["0D"] = 2;
		SymbMap["0A"] = 3;
		SymbMap["QUO"] = 4;
		SymbMap["END"] = 5;
	}
	static std::map<std::string, OPTRFUNC> OptrMap;
	if (OptrMap.size() == 0)
	{
		OptrMap["NULL"] = NULL;
		OptrMap["CHAR"] = OP_VA_C;
		OptrMap["WORD"] = OP_VA_W;
		OptrMap["LINE"] = OP_VA_L;
	}
	static LEXICALNODE LexNodes[] = {
		{0, 1, SymbMap["0A"], OptrMap["CHAR"]},
		{0, 3, SymbMap["QUO"], OptrMap["NULL"]},
		{2, 0, SymbMap["0A"], OptrMap["LINE"]},
		{1, 2, SymbMap["0D"], OptrMap["NULL"]},
		{1, 1, SymbMap["OTH"], OptrMap["CHAR"]},
		{1, 0, SymbMap["TAB"], OptrMap["WORD"]},
		{3, 3, SymbMap["TAB"], OptrMap["CHAR"]},
		{3, 4, SymbMap["QUO"], OptrMap["NULL"]},
		{4, 3, SymbMap["QUO"], OptrMap["CHAR"]},
		{4, 0, SymbMap["TAB"], OptrMap["WORD"]},
		{4, 1, SymbMap["OTH"], OptrMap["CHAR"]},
		{0, 0, SymbMap["TAB"], OptrMap["WORD"]},
		{0, 2, SymbMap["0D"], OptrMap["NULL"]},
		{2, 1, SymbMap["QUO"], OptrMap["CHAR"]},
		{2, 0, SymbMap["TAB"], OptrMap["WORD"]},
		{0, 6, SymbMap["END"], OptrMap["NULL"]},
		{1, 6, SymbMap["END"], OptrMap["WORD"]},
		{2, 6, SymbMap["END"], OptrMap["NULL"]},
		{3, 6, SymbMap["END"], OptrMap["WORD"]},
		{4, 6, SymbMap["END"], OptrMap["WORD"]},
		{2, 2, SymbMap["0D"], OptrMap["NULL"]},
		{4, 5, SymbMap["0D"], OptrMap["NULL"]},
		{5, 0, SymbMap["0A"], OptrMap["LINE"]},
		{5, 0, SymbMap["TAB"], OptrMap["WORD"]},
		{5, 1, SymbMap["QUO"], OptrMap["CHAR"]},
		{5, 6, SymbMap["END"], OptrMap["WORD"]},
		{3, 3, SymbMap["OTH"], OptrMap["CHAR"]},
		{3, 3, SymbMap["0D"], OptrMap["CHAR"]},
		{3, 3, SymbMap["0A"], OptrMap["CHAR"]},
		{4, 1, SymbMap["0A"], OptrMap["CHAR"]},
		{5, 1, SymbMap["OTH"], OptrMap["CHAR"]},
		{5, 1, SymbMap["0D"], OptrMap["CHAR"]},
		{0, 1, SymbMap["OTH"], OptrMap["CHAR"]},
		{2, 1, SymbMap["OTH"], OptrMap["CHAR"]},
		{1, 1, SymbMap["QUO"], OptrMap["CHAR"]},
		{1, 1, SymbMap["0A"], OptrMap["CHAR"]},
	};
	UINT nNodesNum = sizeof(LexNodes) / sizeof(LexNodes[0]);
	UINT nStatNum = nNodesNum / SymbMap.size();
	std::sort(&LexNodes[0], &LexNodes[nNodesNum], LessLexNode);

	LexicalAnalize(str, LexNodes, nStatNum, SymbMap.size(),
		ConvVarArray, (LPARAM)&VarTbl);
}
