#include "StdAfx.h"
#include ".\fileanalyzer.h"
#include <string>
using namespace std;

static const TCHAR* g_szDLGID_DIVIDE = _T("_&&_");

CFileAnalyzer::CFileAnalyzer(void)
{
}

CFileAnalyzer::~CFileAnalyzer(void)
{
}

void CFileAnalyzer::AnalyzeStrings(const WCHAR* lpwResource, const WCHAR* lpwSplit/*=L"\0\0"*/)
{
	//
}

std::wstring Utf8_to_WideChar( std::string& strUtf)
{
	wchar_t *pWch = new wchar_t[strUtf.length()+1];
	int nLen = MultiByteToWideChar(CP_UTF8,0,strUtf.c_str(),
		(int)strUtf.size(),pWch,strUtf.length());
	if( nLen <= 0 )
	{
		delete pWch;
		return L"";
	}
	else {
		pWch[nLen] = L'\0';
	}
	std::wstring strRes(pWch);
	delete pWch;
	return strRes;
}

BOOL CFileAnalyzer::AddString(StringItem sitem)
{
	bool bCanAdd = TRUE;
	for (vector<StringItem>::iterator it = m_arrayStrings.begin(); 
		it != m_arrayStrings.end();	++it)
	{
		if (it->nID == sitem.nID)
		{
			bCanAdd = FALSE;
			break;
		}
	}
	if (bCanAdd)
	{
		m_arrayStrings.push_back(sitem);
		return TRUE;
	}

	return FALSE;
}

BOOL CFileAnalyzer::LoadFile(const WCHAR* lpFileName, BOOL bOpenAlways/*=TRUE*/)
{
	AnalyzeStrings(L"301=充值退款\\0\\0302=断网处理");
	return TRUE;
}

void CFileAnalyzer::LoadFromSource(CString strRCFile, CString strResourceFille,vector<StringItem>& vecItems,BOOL bMenuBefore)
{
	vecItems.clear();

	CMyString strOrg;
	CMyString strOrgID;

	// 打开资源文件,resource.h
	CFile file;
	if ( file.Open(strRCFile, CFile::modeRead) )
	{
		file.SeekToBegin();
		long nLen = long(file.GetLength());
		char * pChars = new char[nLen+1];
		file.Read(pChars, nLen);
		strOrg = CA2T(pChars);

		delete []pChars;
		file.Close();
	}
	else
	{
		return;
	}	
	if ( file.Open(strResourceFille, CFile::modeRead) )
	{
		file.SeekToBegin();
		long nLen = long(file.GetLength());
		char * pChars = new char[nLen+1];
		file.Read(pChars, nLen);
		strOrgID = CA2T(pChars);

		delete []pChars;
		file.Close();
	}
	else
	{
		return;
	}	

	vector<MarkName> vecMarks;
	

	if (bMenuBefore)
	{
		_AnalyzeMenu(strOrg, vecMarks);
		_AnalyzeStringTable(strOrg, vecMarks);
	}
	else
	{
		_AnalyzeStringTable(strOrg, vecMarks);
		_AnalyzeMenu(strOrg, vecMarks);
	}

	_AnalyzeDlg(strOrg, vecMarks);

	

	// 解析ID
	int nDefine = 0;
	int nFrom = 0;
	map<CMyString, UINT> mapMarksID;
	int nKwLen = (int)_tcslen(_T("#define"));
	CMyString kwLine = _T("\r\n");
	do 
	{
		nDefine = strOrgID.FindString(_T("#define"), nFrom);
		if (nDefine < 0)
		{
			break;
		}
		int nEnd = strOrgID.FindString(kwLine, nDefine+7);
		if (nEnd < 0)
		{
			break;
		}
		CMyString strMS = strOrgID.Mid(nDefine+nKwLen, nEnd - (nDefine+nKwLen));
		strMS.Trim();
		MarkID mi;
		int nFirstSpace = strMS.FindString(_T(" "), 0);
		int nLastSpace = strMS.ReverseFind(_T(' '));
		if ( nFirstSpace < 0 || nLastSpace < 0 ) 
		{
			TRACE("\nResource.h 语法错误! 解析终止\n");
			break;
		}
		CMyString strMark = strMS.Mid(0, nFirstSpace);
		CMyString strID = strMS.Mid(nLastSpace+1);
		strID.TrimLeft();
		UINT nID = 0;
		if (strID.GetLength() > 2)
		{
			strID.ToLower();
			if (strID.GetAt(0) == _T('0') && strID.GetAt(1) == _T('x'))
			{
				// 16进制形式
				strID.TrimLeft(_T("0x"));
				strID.FillCharFromLeft(8,_T('0'));
				BYTE bt[4] = {0};
				strID.Parse16ToBuf(bt, 4);
				swap(bt[0],bt[3]);swap(bt[1],bt[2]);
				nID = *((UINT*)bt);
			}
		}
		if (nID == 0)
		{
			nID = strMS.Mid(nLastSpace+1).ParseUInt();
		}
		

		mapMarksID[strMark] = nID;

		nFrom = nEnd + 1;
	} while (nDefine >= 0);

	// 标识符，ID，内容的连接
	for(size_t i = 0; i < vecMarks.size(); i++)
	{
		MarkName& mm = vecMarks[i];
		if (mm.strDlgMark.IsEmpty())
		{
			map<CMyString, UINT>::iterator it = mapMarksID.find(mm.strMark);
			if (it != mapMarksID.end())
			{
				StringItem si;
				si.nID = (*it).second;
				si.strMark = mm.strMark;
				si.str = mm.strConstent;
				vecItems.push_back(si);

/*				mapMarksID.erase(it);*/
			}
		}
		else
		{
			map<CMyString, UINT>::iterator it = mapMarksID.find(mm.strDlgMark);
			StringItem si;
			if (it != mapMarksID.end())
			{
				UINT nDlgID = (*it).second;
				si.bDlgItem = TRUE;
				si.str = mm.strConstent;
				si.strMark = mm.strDlgMark + g_szDLGID_DIVIDE + mm.strMark;
				
				bool bFound = false;
				if (mm.strMark.IsEmpty())
				{
					si.nID = 0;
					bFound = true;
				}
				else if (mm.strMark == _T("IDOK"))
				{
					si.nID = IDOK ;
					bFound = true;
				}
				else if(mm.strMark == _T("IDCANCEL"))
				{
					si.nID = IDCANCEL ;
					bFound = true;
				}
				else if(mm.strMark == _T("IDYES"))
				{
					si.nID = IDYES ;
					bFound = true;
				}
				else if(mm.strMark == _T("IDNO"))
				{
					si.nID = IDNO ;
					bFound = true;
				}
				else
				{
					it = mapMarksID.find(mm.strMark);
					if (it != mapMarksID.end())
					{
						si.nID = (*it).second ;
						bFound = true;
					}
				}
				if (bFound)
				{
					si.nID += (nDlgID << 16);
					vecItems.push_back(si);
				}
			}
		}
	}
}

void CFileAnalyzer::LoadFromGrcFile(CString strGrcFile, vector<StringItem>& vecItems)
{
	vecItems.clear();

	CMyString strOrg;

	CFile file;
	if ( file.Open(strGrcFile, CFile::modeRead) )
	{
		file.SeekToBegin();
		long nLen = long(file.GetLength());
		char * pChars = new char[nLen+1];
		memset(pChars,0,nLen+1);
		file.Read(pChars, nLen);

		wstring stW = Utf8_to_WideChar(string(pChars));
		strOrg.Empty();
		strOrg.Append(stW.c_str());
		delete []pChars;
		file.Close();
	}
	else
	{
		return;
	}

	const TCHAR *pSplit = _T("\r\n");
	vector<CMyString> vecLines;
	strOrg.Split(pSplit, vecLines);

	for(size_t i = 0; i < vecLines.size(); i++)
	{
		CMyString& strLine = vecLines[i];
		if (strLine.IsEmpty())
		{
			continue;
		}
		int nIdFrom = strLine.FindString(_T("["),0);
		int nIdEnd = strLine.FindString(_T("]"),1);
		if (nIdFrom < 0 || nIdEnd < 0 || nIdFrom >= nIdEnd)
		{
			continue;
		}
		int nEqual = strLine.FindString(_T("="),2);
		if (nEqual < 0)
		{
			continue;
		}
		int nRefFrom = strLine.FindString(_T("\""), nEqual);
		if (nRefFrom < 0)
		{
			continue;
		}
		int nRefEnd = strLine.ReverseFind(_T('\"'));
		if (nRefEnd < 0)
		{
			continue;
		}
		StringItem si;
		si.nID = strLine.Mid(nIdFrom+1, nIdEnd - nIdFrom - 1).ParseUInt();
		si.strMark = strLine.Mid(nIdEnd+1, nEqual - nIdEnd - 1);
		si.strMark.Trim(_T('\t'));
		si.strMark.Trim(_T(' '));
		si.str = strLine.Mid(nRefFrom+1, nRefEnd - nRefFrom - 1);
		
		vecItems.push_back(si);
	}
}

BOOL CFileAnalyzer::IntoFindMenu(CMyString& strBE,vector<MarkName>& vecMarks)
{
	vector<CMyString> strLines;
	strBE.Split(_T("\r\n"),strLines);
	if (strLines.size() == 0)
		return FALSE;

	CMyString strNewBE;

	bool bGotoBegin = false;
	int nBegin = 0;
	for(size_t i = 0 ; i < strLines.size(); i++)
	{
		CMyString& str = strLines[i];
		str.Trim(); // 去掉左右两边的空格
		if (str.Left(8) == _T("MENUITEM"))
		{
			if (bGotoBegin)
			{
				strNewBE += str + _T("\r\n");
				continue;
			}
			// 是菜单项
			int nFirstRef = str.FindString(_T("\""),0);
			if (nFirstRef < 0)
			{
				continue;
			}
			int nSecRef = str.ReverseFind(_T('\"'));
			if (nSecRef < 0)
			{
				continue;
			}
			int nEndSpace = str.ReverseFind(_T(' '));
			if (nEndSpace < 0)
			{
				continue;
			}
			CMyString strMask = str.Mid(nEndSpace+1);
			if (strMask.IsEmpty())
			{
				continue;
			}
			MarkName mn;
			mn.strConstent = str.Mid(nFirstRef+1, nSecRef - nFirstRef - 1);
			mn.strMark = strMask;
			vecMarks.push_back(mn);
		}
		else if (str.Left(5) == _T("BEGIN"))
		{
			if (bGotoBegin == false)
			{
				bGotoBegin = true;
			}
			else
			{
				strNewBE += str + _T("\r\n");
			}
			nBegin ++;
		}
		else if (str.Left(3) == _T("END"))
		{
			nBegin --;
			if (bGotoBegin && nBegin == 0)
			{
				IntoFindMenu(strNewBE,vecMarks);
				bGotoBegin = false;
				nBegin = 0;
				strNewBE.Empty();
			}
			else
			{
				strNewBE += str + _T("\r\n");
			}
		}
		else
		{
			if (bGotoBegin)
			{
				strNewBE += str + _T("\r\n");
			}	
		}
	}

	return TRUE;
}

void CFileAnalyzer::_AnalyzeMenu(CMyString& strOrg, vector<MarkName>& vecMarks)
{
	

	//
	// 解析菜单
	//
	{
		CMyString kwLine = _T("\r\n");
		CMyString kwMainBegin = _T("MENU \r\nBEGIN\r\n");
		CMyString kwMainEnd = _T("END\r\nEND");

		int nMainMenu = -1;
		int nEnd = 0;

		while( 1 )
		{
			BOOL bFound = FALSE;
			nMainMenu = strOrg.FindString(kwMainBegin, nMainMenu+1);
			if (nMainMenu >= 0)
			{
				// 找到一个主菜单的起点
				nEnd = strOrg.FindString(kwMainEnd, nMainMenu+1);
				if (nEnd >= 0)
				{
					bFound = TRUE;
				}
			}
			else
			{
				break;
			}

			CMyString strSubMain = strOrg.Mid(nMainMenu+ kwMainBegin.GetLength(), nEnd - nMainMenu - kwMainEnd.GetLength()+5);
			IntoFindMenu(strSubMain,vecMarks);

		}// end-while-MAIN-MENU
	}
}

void CFileAnalyzer::_AnalyzeStringTable(CMyString& strOrg, vector<MarkName>& vecMarks)
{
	//
	// 解析字符串表
	//
	{
		CMyString kwLine = _T("\r\n");
		CMyString kwStringTable = _T("STRINGTABLE \r\nBEGIN");
		CMyString kwEnd = _T("\r\nEND");

		int nTable = -1;
		int nEnd = 0;

		while( 1 )
		{
			BOOL bFound = FALSE;
			nTable = strOrg.FindString(kwStringTable, nTable+1);
			if (nTable >= 0)
			{
				nEnd = strOrg.FindString(kwEnd, nTable+1);
				if (nEnd >= 0)
				{
					BOOL bFoundEnd = FALSE;
					while(nEnd!=-1)
					{
						int nRefCount = 0;
						for(int i = nTable ; i<nEnd; i++)
						{
							if (strOrg.GetAt(i) == _T('\"'))
							{
								nRefCount++;
							}
						}
						if (nRefCount%2 == 1)
						{
							// 两引号之间有END, 此时找到的END无效
							nEnd = strOrg.FindString(kwEnd, nEnd+1); // 在此找END
						}
						else
						{
							bFoundEnd = TRUE;
							break;
						}
					}

					if (bFoundEnd)
					{
						bFound = TRUE;
					}
					else
					{
						//do over
					}
				}
				else
				{
					//do over
				}
			}
			else
			{
				break;
			}

			if (bFound)
			{
				CMyString strLines = strOrg.Mid(nTable+ kwStringTable.GetLength(), nEnd - nTable - kwStringTable.GetLength());
				strLines.TrimLeft(kwLine);

				vector<CMyString> vecLines;
				strLines.Split(kwLine, vecLines);

				for (size_t i = 0; i < vecLines.size(); i++)
				{
					CMyString& strLine = vecLines[i];
					if (strLine.IsEmpty())
					{
						continue;
					}
					strLine.Trim();
					int nFirstSpace = strLine.FindString(_T(" "), 0);
					if (nFirstSpace < 0)
					{
						// rc资源中，标识符和内容之间可能有换行
						if (i < (vecLines.size() - 1))
						{
							vecLines[i+1].Insert(0, strLine); // 交给下一次循环做处理
						}
						continue;
					}
					//IDS_ACTIVEMEMBERLISTHEAD

					int nFirstRef = strLine.FindString(_T("\""), nFirstSpace + 1);
					if (nFirstRef < 0)
					{
						// rc资源中，标识符和内容之间可能有换行
						if (i < (vecLines.size() - 1))
						{
							vecLines[i+1].Insert(0, strLine); // 交给下一次循环做处理
						}
						continue;
					}
					int nLastRef = strLine.ReverseFind(_T('\"'));
					if (nLastRef < 0)
					{
						continue;
					}

					MarkName mn;
					mn.strMark = strLine.Mid(0, nFirstSpace);

					mn.strConstent = strLine.Mid(nFirstRef+1, nLastRef-nFirstRef-1);

					vecMarks.push_back(mn);
				}
			}
		}// end-while-STRINGTALBE-BEGIN
	}
}

void CFileAnalyzer::_AnalyzeDlg(CMyString& strOrg, vector<MarkName>& vecMarks)
{
	//
	// 解析字符串表
	//
	CMyString kwLine = _T("\r\n");
	CMyString kwDialog = _T(" DIALOGEX ");
	CMyString kwEnd = _T("\r\nEND");

	int nDialog = -1;
	int nEnd = 0;

	while( 1 )
	{
		BOOL bFound = FALSE;
		nDialog = strOrg.FindString(kwDialog, nDialog+1);

		if (nDialog == -1)
		{
			break;
		}
		int nLine1 = strOrg.ReverseFind(_T('\n'),nDialog-1);
		int nLine2 = strOrg.FindString(kwLine, nDialog+1);
		if (nLine1 == -1 && nLine2 == -1)
		{
			continue;
		}

		CMyString strDlgMark = strOrg.Mid(nLine1+1, nDialog-nLine1-1);
		strDlgMark.Trim();

		int nBegin = strOrg.FindString(_T("BEGIN\r\n"),nLine2+1);
		if (nBegin == -1)
		{
			continue;
		}
		int nEnd = strOrg.FindString(_T("END\r\n"),nBegin+1);
		if (nEnd == -1)
		{
			continue;
		}

		{
			CMyString strCaption = strOrg.Mid(nLine2, nBegin - nLine2);
			int nCap = strCaption.FindString(_T("\r\nCAPTION "),0);
			if (nCap >=0)
			{
				int nLine = strCaption.FindString(_T("\r\n"), nCap+1);
				if (nLine >= 0)
				{
					int nLeftRef = strCaption.FindString(_T("\""), nCap);
					int nRightRef = strCaption.ReverseFind(_T('\"'), nLine);
					if (nLeftRef >= 0 && nRightRef >= 0)
					{
						MarkName mn;
						mn.strConstent = strCaption.Mid(nLeftRef+1,nRightRef-nLeftRef-1);
						mn.strDlgMark = strDlgMark;
						vecMarks.push_back(mn);
					}
				}
			}
			
		}

		vector<CMyString> arrLines;
		strOrg.Mid(nBegin+7, nEnd - nBegin -7).Split(_T("\r\n"),arrLines);

		vector<CMyString> strControls;
		strControls.push_back(CMyString(_T("LTEXT")));
		strControls.push_back(CMyString(_T("RTEXT")));
		strControls.push_back(CMyString(_T("CTEXT")));
		strControls.push_back(CMyString(_T("PUSHBUTTON")));
		strControls.push_back(CMyString(_T("DEFPUSHBUTTON")));
		strControls.push_back(CMyString(_T("GROUPBOX")));
		strControls.push_back(CMyString(_T("CONTROL")));
		
		if (arrLines.size() == 0)
		{
			continue;
		}

		for(int i= (int)arrLines.size()-1; i >= 0; i--)
		{
			CMyString& strLine = arrLines[i];

 			strLine.Trim();
			strLine.Trim(_T('\t'));

			int nSpace = strLine.FindString(_T(" "),0);
			if (nSpace == -1)
			{
				arrLines[i-1] += _T(" ") + strLine; // 追加到前面一行
				continue;
			}

			CMyString strType = strLine.Left(nSpace);
			if (strType.IsEmpty())
			{
				continue;
			}
			
			BOOL bTxtType = FALSE;
			for(size_t n = 0; n < strControls.size(); n++)
			{
				if (strControls[n].CompareNoCase(strType) == 0)
				{
					bTxtType = TRUE;
					break;
				}
			}

			if (!bTxtType)
			{
				if (i == 0)
				{
					break;
				}
				arrLines[i-1] += _T(" ") + strLine; // 追加到前面一行
				continue;
			}

			MarkName mn;

			if (strType.Right(4) == _T("TEXT") ||
				strType.Right(6) == _T("BUTTON") ||
				strType == _T("GROUPBOX"))
			{
				mn.strDlgMark = strDlgMark;

				int nLeftRef = strLine.FindString(_T("\""),nSpace+1);
				if (nLeftRef == -1)
				{
					continue;
				}
				int nRightRef = strLine.ReverseFind(_T('\"'));
				if (nRightRef == -1)
				{
					continue; //无效的行
				}
				mn.strConstent = strLine.Mid(nLeftRef+1, nRightRef-nLeftRef-1);
				int nDou1 = strLine.FindString(_T(","),nRightRef+1);
				if (nDou1 == -1)
				{
					continue;
				}
				int nDou2 = strLine.FindString(_T(","),nDou1+1);
				if (nDou2 == -1)
				{
					continue;
				}

				mn.strMark = strLine.Mid(nDou1+1, nDou2 - nDou1 - 1).Trim();
			}
			else if (strType == _T("CONTROL"))
			{
				mn.strDlgMark = strDlgMark;

				int nRightRef2 = strLine.ReverseFind(_T('\"'));
				if (nRightRef2 == -1)
				{
					continue;
				}
				int nLeftRef2 = strLine.ReverseFind(_T('\"'), nRightRef2-1);
				if (nLeftRef2 == -1)
				{
					continue;
				}
				CMyString strClassName = strLine.Mid(nLeftRef2+1, nRightRef2-nLeftRef2-1);
				strClassName.ToLower();
				if (!(
					strClassName == _T("button") ||
					strClassName == _T("static")))
				{
					continue;
				}
				
				int nLeftRef = strLine.FindString(_T("\""),nSpace+1);
				if (nLeftRef == -1)
				{
					continue;
				}
				int nRightRef = strLine.ReverseFind(_T('\"'),nLeftRef2-1);
				if (nRightRef == -1)
				{
					continue; //无效的行
				}
				mn.strConstent = strLine.Mid(nLeftRef+1, nRightRef-nLeftRef-1);
				int nDou1 = strLine.FindString(_T(","),nRightRef+1);
				if (nDou1 == -1)
				{
					continue;
				}
				int nDou2 = strLine.FindString(_T(","),nDou1+1);
				if (nDou2 == -1)
				{
					continue;
				}

				mn.strMark = strLine.Mid(nDou1+1, nDou2 - nDou1 - 1).Trim();
			}
			else
			{
				continue;
			}

			vecMarks.push_back(mn);
		}

	}
}