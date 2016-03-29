#include "StdAfx.h"
#include ".\stringcontainer.h"
#include "..\TextResouseTool\FileAnalyzer.h"
//#include "..\TextResouseTool\FileAnalyzer.cpp"

using namespace std;

CStringContainer::CStringContainer(void)
{
}

CStringContainer::~CStringContainer(void)
{
}

BOOL CStringContainer::LoadGrcFile(CString strFilePath)
{
	m_mapStrs.clear();
	vector<StringItem> vecItems;
	CFileAnalyzer::LoadFromGrcFile(strFilePath, vecItems);
	if (vecItems.size()==0)
	{
		return FALSE;
	}
	
	const nSize = 4;
	const TCHAR nChs[nSize] = {_T('\\'),_T('r'),_T('n'),_T('t')};
	const TCHAR nChsEx[nSize] = {_T('\\'),_T('\r'),_T('\n'),_T('\t')};

	for(size_t i = 0; i < vecItems.size(); i++)
	{
		StringItem& si = vecItems[i];
		if (si.str.FindString(_T("\\"),0) >= 0)
		{
			int nDiffCount = si.str.GetLength();

			// 如果存在转义符，字符串需要做处理
			TCHAR *pBase = &si.str[0];
			TCHAR *pTmp = pBase;
			while( *pTmp != _T('\0') )
			{
				if ( *pTmp == _T('\\') )
				{
					pTmp++;

					if ( *pTmp == _T('\0') )
						break;
					
					bool bFound = false;
					for (int n = 0; n < nSize; n++)
					{
						if (*pTmp == nChs[n])
						{
							*pBase++ = nChsEx[n];
							nDiffCount--;
							bFound = true;
							break;
						}
					}
					if (!bFound)
						*pBase++ = *pTmp++;
					else
						pTmp++;
				}
				else
				{
					*pBase++ = *pTmp++;
				}
			}

			si.str.SetAppendAt(nDiffCount);
		}
		m_mapStrs[si.nID] = CString(si.str.GetData());
	}
	return TRUE;
}

void CStringContainer::GetStringA_(UINT nID,char* pCh,const UINT nSize )const
{
	static const char* lpUndefined = "Unknown string ID";
	map<UINT,CString>::const_iterator it = m_mapStrs.find(nID);

	if ( it != m_mapStrs.end())
	{
		strncpy(pCh,CT2A(it->second),nSize);
	}
	else
	{
		strncpy(pCh,lpUndefined,nSize);
	}
}

void CStringContainer::GetStringW_(UINT nID, wchar_t* pCh,const UINT nSize )const
{
	static const wchar_t* lpUndefined = L"Unknown string ID";
	map<UINT,CString>::const_iterator it = m_mapStrs.find(nID);

	if ( it != m_mapStrs.end())
	{
		wcsncpy(pCh, CT2W(it->second), nSize);
	}
	else
	{
		wcsncpy(pCh, lpUndefined, nSize);
	}
}

CString CStringContainer::GetString_(UINT nID)const
{
	static const TCHAR* lpUndefined = _T("Unknown string ID");
	map<UINT,CString>::const_iterator it = m_mapStrs.find(nID);
	if ( it != m_mapStrs.end())
	{
		return it->second;
	}
	return lpUndefined;
}