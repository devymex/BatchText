#pragma once

#include <vector>

typedef std::vector<int> NUMARRAY;
typedef std::vector<CStringW> STRARRAY;
typedef std::vector<std::vector<CStringW>> STRTABLE;

void ParseVarArray(CStringW &str, STRTABLE &VarTbl);
void ParsePattern(CStringW &str, STRARRAY &Segs);
