// StringListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "TextResouseTool.h"
#include "StringListCtrl.h"
#include ".\stringlistctrl.h"
#include "GxxString.h"
#include "DlgHit.h"
#include <algorithm>

// CStringListCtrl

IMPLEMENT_DYNAMIC(CStringListCtrl, CGxxListCtrl)
CStringListCtrl::CStringListCtrl()
{
	m_bInit = FALSE;
	m_bHaveChanged = FALSE;
}

CStringListCtrl::~CStringListCtrl()
{
}


BEGIN_MESSAGE_MAP(CStringListCtrl, CGxxListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHdnItemclick)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CStringListCtrl 消息处理程序


BOOL CStringListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd* pWnd = GetColChildWnd(COL_ID);
		CWnd* pFocus = GetFocus();
		if (pWnd && pWnd == GetFocus())
		{
			CString strCtrl;
			pWnd->GetWindowText(strCtrl);
			if (strCtrl.IsEmpty())
			{
				MessageBox(_T("ID值不能为空"));
				return TRUE;
			}
			else
			{
				OnClickItem(-1,-1);
				return TRUE;
			}
		}
		pWnd = GetColChildWnd(COL_CONTENT);
		pFocus = GetFocus();
		if (pWnd && pWnd == GetFocus())
		{
			OnClickItem(-1,-1);
			return TRUE;
		}
		pWnd = GetColChildWnd(COL_MASK);
		pFocus = GetFocus();
		if (pWnd && pWnd == GetFocus())
		{
			OnClickItem(-1,-1);
			return TRUE;
		}
		
		if (pFocus == this)
		{
			OnClickItem(-1,-1);
			return TRUE;
		}
	}

	return CGxxListCtrl::PreTranslateMessage(pMsg);
}

CString CStringListCtrl::GetChildWndText(CWnd*pWnd,int nItem,int nSubItem)
{
	CString strRes;

	CString strCtrl;
	pWnd->GetWindowText(strCtrl);
	
	if (nSubItem == COL_ID)
	{
		strCtrl.TrimLeft(_T('0')); // 去掉左边的0
		if (CheckModify(strCtrl, nItem))
		{
			strRes = strCtrl;
		}
		else
		{
			strRes = GetItemText(nItem,nSubItem);
		}
	}

	if (nSubItem == COL_CONTENT)
	{
		if (strCtrl.IsEmpty())
		{
			strRes = GetItemText(nItem, nSubItem);
		}
		else
		{
			strRes = strCtrl;
		}
	}

	if (nSubItem == COL_MASK)
	{
		strRes = strCtrl;
	}

	switch(nSubItem)
	{
	case COL_ID:
		{
			CMyString str = strRes;
			UINT nOld = m_items[nItem].nID;
			m_items[nItem].nID = str.ParseUInt();
			if (nOld != m_items[nItem].nID)
			{
				SetChanged();
			}
		}
		break;
	case COL_MASK:
		{
			CMyString strOld = m_items[nItem].strMark;
			m_items[nItem].strMark = strRes;
			if (!(strOld==m_items[nItem].strMark))
			{
				SetChanged();
			}
		}
		
		break;
	case COL_CONTENT:
		{
			CMyString strOld = m_items[nItem].str;
			CMyString strNew = strRes;
			
			// 验证重要的格式是否发生改变
			BOOL bShouldCheck = CGlobalParam::GetInstance()->GetCheckChange();
			if (bShouldCheck)
			{
				CMyString strFmtChs = CGlobalParam::GetInstance()->GetFormatChars();
				vector<CMyString> arrFmts;
				strFmtChs.Split(_T(";"),arrFmts);

				CMyString strLastF,strLastS;
				for(size_t i=0; i < arrFmts.size(); i++)
				{
					CMyString& strFmt = arrFmts[i];
					int iOld = 0;
					iOld = strOld.FindString(strFmt,iOld);
					while(iOld != -1)
					{
						strLastF += strFmt;
						iOld = strOld.FindString(strFmt,iOld+strFmt.GetLength());
					}
					int iNew = 0;
					iNew = strNew.FindString(strFmt,iNew);
					while(iNew != -1)
					{
						strLastS += strFmt;
						iNew = strNew.FindString(strFmt,iNew+strFmt.GetLength());
					}
				}

				if (strLastF != strLastS)
				{
					MessageBox(_T("更改后的内容的格式必须与更改前的格式一致!\n格式符为:")+strFmtChs,
						NULL,MB_OK|MB_ICONWARNING);
					return strOld.GetData();
				}


				
				/*for(int i = 0; i < strFmtChs.GetLength(); i++)
				{
					TCHAR ch = strFmtChs.GetAt(i);
					for(int n = 0 ; n < strOld.GetLength(); n++)
					{
						if (ch == strOld.GetAt(n))
							strLastF.AppendChar(ch);
					}
					for(int n = 0 ; n < strRes.GetLength(); n++)
					{
						if (ch == strRes.GetAt(n))
							strLastS.AppendChar(ch);
					}
					if (strLastF != strLastS)
					{
						MessageBox(_T("更改后的内容的格式必须与更改前的格式一致!\n格式符为:")+strFmtChs,
							NULL,MB_OK|MB_ICONWARNING);
						return strOld.GetData();
					}
				}*/
			}
			if (!(strOld == strRes.GetString()))
			{
				m_items[nItem].str = strRes;
				SetChanged();
			}
		}
		
		break;
	case COL_ORG:
		return strCtrl;

	default:
		GXX_ASSERT(0,"未知的列");
		break;
	}

	return strRes;
}

BOOL CStringListCtrl::CheckModify(CString newString, int nRow)
{
	if (newString.IsEmpty())
	{
		MessageBox(_T("ID值不能为空"));
		return FALSE;
	}
	BOOL bFound = FALSE;
	for(int i = 0 ; i < GetItemCount() ; i++)
	{
		if (nRow == i)
		{
			continue;
		}
		CString strTxt = GetItemText(i, 0);
		if (strTxt == newString)
		{
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		MessageBox(_T("ID重复"));
		return FALSE;
	}
	return TRUE;
}
void CStringListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CGxxListCtrl::OnNMClick(pNMHDR, pResult);
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem == -1 && pNMItemActivate->iSubItem >=0)
	{	
		// 自动生成新的1行

		int nCount = GetItemCount();
		int nNewID = 101;
		if (nCount > 0)
		{
			CString strValue = GetItemText(nCount-1, 1);
			if (strValue == _T(""))
			{
				// 清除最后的无效字符串
				DeleteItem(nCount-1);
				m_items.pop_back();
				return;
			}
			CString str = GetItemText(nCount-1, 0);
			CGxxString<TCHAR> strN(str);
			int nID = strN.ParseInt();
			do 
			{
				nID++;
				strN = nID;
			} while (!CheckModify(strN.GetData(), -1));
			
			nNewID = nID;
		}
		CString strID;
		strID.Format(_T("%d"), nNewID);
		StringItem tmpSi;
		tmpSi.nID = nNewID;
		AddItem(tmpSi);
		OnClickItem(nCount, COL_ID);
	}
}

void CStringListCtrl::InitList()
{
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	ModifyStyle(0,LVS_SINGLESEL);

	for(int i = 0 ; i < COL_COUNT; i++ )
	{
		switch(i)
		{
		case COL_ID:
			InsertColumn(0, _T("ID"), LVCFMT_LEFT, 80, TRUE,TRUE,TRUE,NULL,CGxxListCtrl::GLC_Edit);
			break;
		case COL_MASK:
			InsertColumn(1, LOAD_STRING(IDS_MASK), LVCFMT_LEFT, 250, TRUE,TRUE,TRUE,NULL,CGxxListCtrl::GLC_Edit);
			break;
		case COL_CONTENT:
			InsertColumn(2, LOAD_STRING(IDS_CONTENT), LVCFMT_LEFT, 150, TRUE,TRUE,TRUE,NULL,CGxxListCtrl::GLC_Edit);
			break;
		case COL_ORG:
			InsertColumn(3, LOAD_STRING(IDS_CONTENT_OLD), LVCFMT_LEFT, 150, TRUE,TRUE,TRUE,NULL,CGxxListCtrl::GLC_Edit);
			break;
		default:
			GXX_ASSERT(0,"未知的列");
			break;
		}
		GetColChildWnd(i)->SetFont(GetFont());
	}


	
	//SetWindowLong(GetColChildWnd(COL_ORG)->GetSafeHwnd(), GWL_STYLE, ES_READONLY);
	
	((CEdit*)GetColChildWnd(COL_ORG))->SetReadOnly(TRUE);
	
	SetItemsHeight(18);
	GetColChildWnd(COL_ID)->ModifyStyle(0, ES_NUMBER);
	m_bInit = TRUE;

}
BOOL CStringListCtrl::AddItems(vector<StringItem>& items)
{
	CMyString strResult;
	if (CGlobalParam::GetInstance()->GetReplace())
	{
		strResult += _T("允许新导入的ID替换重复的ID对应的项!\n\n");
		
		int nReplace = 0;
		int nNew = 0;
		for(size_t i = 0 ; i < items.size(); i++)
		{
			if (AddItem(items[i]) == 1)
			{
				nReplace++;
			}
			else
			{
				nNew++;
			}
		}
		strResult.AppendFormat(_T("导入字符串总数为:%d\n\n"),nReplace+nNew);
		strResult.AppendFormat(_T("  替换:%d 个\n"), nReplace);
		strResult.AppendFormat(_T("  新增:%d 个\n"), nNew);

		MessageBox(strResult,NULL,MB_OK|MB_ICONINFORMATION);
		return nReplace > 0 || nNew > 0;
	}
	else
	{
		strResult += _T("忽略导入ID重复的项!\n\n");

		int nIgnore = 0;
		int nNew = 0;
		for(size_t i = 0; i < items.size(); i++)
		{
			if (AddItem(items[i]) == 0)
			{
				nIgnore++;
			}
			else
			{
				nNew++;
			}
		}
		strResult.AppendFormat(_T("导入字符串总数为:%d\n\n"),nIgnore+nNew);
		strResult.AppendFormat(_T("  忽略:%d 个\n"), nIgnore);
		strResult.AppendFormat(_T("  新增:%d 个\n"), nNew);

		MessageBox(strResult,NULL,MB_OK|MB_ICONINFORMATION);
		return nNew > 0;

	}
	return FALSE;
}

BOOL CStringListCtrl::AddItem(StringItem& it)
{
	int nCount = GetItemCount();

	for(int i = 0; i < (int)m_items.size(); i++)
	{
		if (it.nID == m_items[i].nID)
		{
			BOOL bHit = CGlobalParam::GetInstance()->GetIsNextHit();
			if (bHit)
			{
				CDlgHit dh;
				dh.DoModal();
			}
			BOOL bReplace = CGlobalParam::GetInstance()->GetReplace();
			if (bReplace)
			{
				// 替换
				m_items[i].str = it.str;
				m_items[i].strMark = it.strMark;
		
				SetItemText(i, COL_MASK, it.strMark.GetData());		
				
				SetItemText(i, COL_CONTENT, it.str.GetData());
				SetItemText(i, COL_ORG, it.str.GetData());

				for(int j =1; j <= COL_CONTENT; j++)
					SetSubItemTxtColor(i,j,RGB(245,0,0));

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	m_items.push_back(it);
	CString str;
	str.Format(_T("%d"), it.nID);
	InsertItem(nCount, _T(""));
	SetItemText(nCount, COL_ID,      str);

	SetItemText(i, COL_MASK, it.strMark.GetData());
	if (it.bDlgItem)
	{
		SetSubItemTxtColor(i, COL_MASK, RGB(0,150,0));
	}
		
	SetItemText(nCount, COL_CONTENT, it.str.GetData());
	SetItemText(nCount, COL_ORG, it.str.GetData());

	if (nCount %2 == 1)
	{
		for(int i =0; i <= COL_ORG; i++)
			SetSubItemBkColor(nCount,i,RGB(229,242,254));
	}
	
	return 2;
}
void CStringListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CGxxListCtrl::OnSize(nType, cx, cy);

	if (m_bInit)
	{
		int nLastWidth = 0;
		int nWidth = 0;
		for (int i = 0 ; i < COL_COUNT ; i++)
		{
			if (i == COL_CONTENT || i == COL_ORG)
			{
				continue;	
			}
			nWidth += GetColumnWidth(i);
		}
		nWidth += 15;
		nLastWidth = cx - nWidth;
		
		SetColumnWidth(COL_CONTENT, nLastWidth / 2);
		SetColumnWidth(COL_ORG, nLastWidth / 2);
	}
}

void CStringListCtrl::SaveAs(CString strNewFilePath)
{
	CFile file;
	if (file.Open(strNewFilePath, CFile::modeReadWrite | CFile::modeCreate))
	{
		CMyString str;
		for(size_t i = 0 ; i < m_items.size(); i++)
		{
			StringItem& it = m_items[i];

			/*if (it.nID >= 65536)
			{
				str.AppendFormat(_T("[%d]\t%s\t\t\t=\"%s\"\r\n"), it.nID,it.strMark.GetData(),  _T("XXX"));
			}
			else*/
			str.AppendFormat(_T("[%d]\t%s\t\t\t=\"%s\"\r\n"), it.nID,it.strMark.GetData(),  it.str.GetData());
		}
		CGxxStringA strA = CT2A(str);
		file.Write(strA.GetData(), strA.GetLength()+1);
		file.Flush();
		file.Close();
		MessageBox(_T("保存成功!"));
	}
}

void CStringListCtrl::SetChanged(BOOL bChanged /*= TRUE*/)
{
	m_bHaveChanged = bChanged;
	GetParent()->SendMessage(WM_ITEMCHANGE, m_bHaveChanged, 0);
}

void CStringListCtrl::DeleteString(int nRow)
{
	OnClickItem(-1,-1);
	m_items.erase(m_items.begin()+nRow);
	DeleteItem(nRow);
	SetChanged(TRUE);
}

void CStringListCtrl::InsertString(int nRow)
{
	StringItem si;
	si.nID = GetNewID();
	m_items.insert(m_items.begin()+nRow,si);
	
	CString str;
	str.Format(_T("%d"), si.nID);
	InsertItem(nRow, _T(""));
	SetItemText(nRow, COL_ID,      str);
	SetItemText(nRow, COL_MASK,    si.strMark.GetData());
	SetItemText(nRow, COL_CONTENT, si.str.GetData());

	SetSelectionMark(nRow);
	SetItemState(nRow, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);

	SetChanged(TRUE);
}
void CStringListCtrl::AddString()
{
	InsertString(0);
}

UINT CStringListCtrl::GetNewID()
{
	if (m_items.size() == 0)
	{
		return 100;
	}
	UINT nMax3 = 100;
	UINT nMax4 = 999;
	UINT nMax5 = 10000;
	int nCount3 = 0;
	int nCount4 = 0;
	int nCount5 = 0;
	for(size_t i = 0; i < m_items.size(); i++)
	{
		UINT nID = m_items[i].nID;
		if ( 100 <= nID && nID <= 999 )
		{
			if (nID > nMax3)
			{
				nMax3 = nID;
			}
			nCount3++;
		}
		else if( 1000 <= nID && nID <= 9999)
		{
			if (nID > nMax4)
			{
				nMax4 = nID;
			}
			nCount4++;
		}
		else if( 10000 <= nID && nID <= 65535)
		{
			if (nID > nMax5)
			{
				nMax5 = nID;
			}
			nCount5++;
		}
	}
	if (nCount3 < 900 && nMax3 < 999)
	{
		return nMax3+1;
	}
	if (nCount4 < 9000 && nMax4 < 9999)
	{
		return nMax4+1;
	}
	if (nCount5 < 55536 && nMax5 < 65535)
	{
		return nMax5+1;
	}

	return m_items[m_items.size()-1].nID + 1;
}

BOOL CStringListCtrl::Search(CString strKeyword, BOOL bCase, BOOL bWholeMatch, int nLastResult, int& nNewResult)
{
	nNewResult = nLastResult;
	int nRow = -1;
	for(int i = 0; i < GetItemCount() && nRow == -1; i++)
	{
		bool bFound = false;
		for(int j = 0; j < GetColumnCount(); j++)
		{
			CString strItemText = GetItemText(i,j);
			if (!bCase)
			{
				// 不区分大小写
				strItemText.MakeUpper();
				strKeyword.MakeUpper();

			}

			if (bWholeMatch)
			{
				if (strItemText == strKeyword)
				{
					nRow = i;
					bFound = true;
					break;
				}
			}
			else
			{
				if (strItemText.Find(strKeyword.GetString()) >= 0)
				{
					nRow = i;
					bFound = true;
					break;
				}
			}	
		}	
		if (bFound)
		{
			if (nLastResult == -1)
			{
				break;
			}
			else
			{
				if (nRow <= nLastResult)
				{
					nRow = -1;
					continue;
				}
			}
		}
	}
	if (nRow >= 0)
	{
		nNewResult = nRow;
		SetSelectionMark(nRow);
		SetItemState(nRow, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		EnsureVisible(nRow,TRUE);
		return TRUE;
	}
	return FALSE;
}

class CCompare
{
public:
	CCompare(int nKind, bool bDe){ m_nKind = nKind; m_bDesc = bDe;}

	bool operator()(StringItem& left,StringItem& right)
	{
		if (m_nKind == CStringListCtrl::COL_ID)
		{
			return m_bDesc? (left.nID > right.nID) : (left.nID < right.nID);
		}
		else
		{
			return m_bDesc? (left.strMark.CompareNoCase(right.strMark) > 0) : (left.strMark.CompareNoCase(right.strMark) < 0);
		}
	}
	int m_nKind;
	bool m_bDesc;
};

void CStringListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
	
	static bool bDesc = false;

	if (phdr->iItem == COL_ID)
	{
		sort(m_items.begin(),m_items.end(),CCompare(COL_ID,bDesc));
	}
	else if(phdr->iItem == COL_MASK)
	{
		sort(m_items.begin(),m_items.end(),CCompare(COL_MASK,bDesc));
	}
	else
	{
		return;
	}

	std::vector<StringItem> vecTemp;
	vecTemp.resize(m_items.size());
	copy(m_items.begin(),m_items.end(),vecTemp.begin());

	m_items.clear();

	DeleteAllItems();
	for (size_t i = 0 ; i < vecTemp.size(); i++)
	{
		AddItem(vecTemp[i]);
	}

	bDesc = !bDesc;
}

void CStringListCtrl::DeleteAllStrings()
{
	m_items.clear();
	DeleteAllItems();
}


void CStringListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CGxxListCtrl::OnRButtonDown(nFlags, point);
}

void CStringListCtrl::OnClickItem(int nItem,int nSubItem)
{
	if (nItem >= 0 && nSubItem == COL_ID)
	{
		BOOL bForbid = CGlobalParam::GetInstance()->GetForbidEditID();
		((CEdit*)GetColChildWnd(COL_ID))->SetReadOnly(bForbid);
	}
	CGxxListCtrl::OnClickItem(nItem, nSubItem);
}