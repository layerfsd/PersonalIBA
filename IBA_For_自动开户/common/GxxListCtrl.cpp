// CGxxHeaderCtrl
#include "stdafx.h"
#include "GxxListCtrl.h"
#include ".\gxxlistctrl.h"
using namespace NS_COMMON;

////--begin-CGxxListCtrl
//
IMPLEMENT_DYNAMIC(CGxxListCtrl,CListCtrl)
 CGxxListCtrl::CGxxListCtrl()
{
	m_nItemHeight = 16;
	m_nInnerMaxCtrlID = 10000;
}

 CGxxListCtrl::~CGxxListCtrl()
{
	for(int iCol = 0 ; iCol < (int)m_vecColsInfo.size() ; iCol ++ )
	{
		ColInfo& colInf = m_vecColsInfo[iCol];
		/*if( colInf.bHaveChildWnd && colInf.bShared )
		{
			if( colInf.nChildWndKind != GLC_CUSTOM && colInf.pChildWnd != NULL)
			{
				delete colInf.pChildWnd;
			}
		}*/
		if (colInf.pChildWnd != NULL)
		{
			delete colInf.pChildWnd;
			colInf.pChildWnd = NULL;
		}
	}
	for(int iItem = 0 ; iItem < (int)m_vecSubItems.size() ; iItem ++)
	{
		vector<SubItemStruct>& rowSubitems = m_vecSubItems[iItem];
		ASSERT( rowSubitems.size() == m_vecColsInfo.size() );
		for(int iCol = 0 ;iCol < (int)rowSubitems.size() ; iCol ++)
		{
			ColInfo& colInf = m_vecColsInfo[iCol];
			if( colInf.bHaveChildWnd && colInf.bShared == FALSE \
				&& rowSubitems[iCol].pChildWnd != NULL
				)
			{
				delete rowSubitems[iCol].pChildWnd;
				rowSubitems[iCol].pChildWnd = NULL;
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CGxxListCtrl,CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(HDN_ENDTRACKA, 0, &CGxxListCtrl::OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, &CGxxListCtrl::OnHdnEndtrack)
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_REFLECT(NM_CLICK, &CGxxListCtrl::OnNMClick)
	ON_WM_TIMER()
END_MESSAGE_MAP()
void CGxxListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CGxxListCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

void CGxxListCtrl::PreSubclassWindow()
{
	ModifyStyle(0,LVS_OWNERDRAWFIXED );
	CListCtrl::PreSubclassWindow();
}

void CGxxListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CRect rc;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CDC* pDC = &dc;

	for(int i = 0 ; i < GetColumnCount() ; i ++)
	{
		DrawSubItem(pDC,lpDrawItemStruct->itemID,i,lpDrawItemStruct->itemState);
	}

	dc.Detach();
}


void CGxxListCtrl::DrawSubItem( CDC* pDC,int nItem,int nSubItem,UINT itemState )
{
	CRect rc = GetSubItemRect(nItem,nSubItem);
	BOOL bDrawText = TRUE;

	COLORREF clrBk=RGB(255,255,255);

	
	if( itemState & ODS_SELECTED )
	{
		clrBk = RGB(57,128,244);
	}
	else if( itemState & ODS_FOCUS )
	{
		clrBk = RGB(200,200,200);
	}
	else
	{
		clrBk = GetSubItemBkColor(nItem,nSubItem);
	}

	if( !GetColIsShared(nSubItem) ) //添加此判断为了避免闪烁
	{
		CWnd *pWnd = GetSubItemWnd(nItem,nSubItem);
		if( pWnd != NULL && IsWindow(pWnd->m_hWnd) )
		{
			CRect rcWn;
			pWnd->GetClientRect(rcWn);
			if( rcWn.Width() < rc.Width() )
			{
				//rc.left = rc.left + rcWn.Width()+1;
				pDC->FillSolidRect(rc,clrBk);
			}
			bDrawText = FALSE;
		}
	}
	if( bDrawText )
	{
		pDC->FillSolidRect(rc,clrBk);
		CString str = GetItemText(nItem,nSubItem);
		DrawSubItemText(pDC,nItem,nSubItem,str,rc,itemState );
	}
}

void CGxxListCtrl::DrawSubItemText( CDC* pDC,int nItem,int nSubItem,LPCTSTR lpTxt,LPRECT lpRect,UINT itemState )
{
	CRect rc = lpRect;
	CRect rcTest(0,0,0,0);
	pDC->DrawText(lpTxt,rcTest,DT_CALCRECT);
	CRect rcTxt = rc;
	rcTxt.DeflateRect(2,0,0,0);
	rcTxt.top = rc.top + (rc.Height() - rcTest.Height())/2;
	rcTxt.bottom = rcTxt.top + rcTest.Height();
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor( GetSubItemTxtColor(nItem,nSubItem) );
	pDC->DrawText(lpTxt,rcTxt,m_vecColsInfo[nSubItem].txtAlign);
}

RECT CGxxListCtrl::GetSubItemRect( int iItem,int iSubItem )
{
	CRect rc;
	CListCtrl::GetSubItemRect(iItem,iSubItem,LVIR_BOUNDS,rc);
	CRect rcW;
	CListCtrl::GetHeaderCtrl()->GetItemRect(iSubItem,rcW);
	//m_headCtrl.GetItemRect(,);
	rc.right = rc.left + rcW.Width();
	return rc;
}


void CGxxListCtrl::SetItemsHeight( int iHeight )
{
	m_nItemHeight = iHeight;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CGxxListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	
	ShowAllSubWindows();	
}

void CGxxListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

	ShowAllSubWindows();
}
void CGxxListCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	SetTimer(10000,20,NULL);
}

void CGxxListCtrl::ShowAllSubWindows()
{
	CRect rcClient;
	GetClientRect(rcClient);
	for(int iSubItem = 0 ; iSubItem < GetColumnCount() ; iSubItem ++)
	{
		if( GetColHasChildWnd(iSubItem) &&
			GetColIsShared(iSubItem) == FALSE)  //非共享式
		{
			for(int iItem = 0 ; iItem < GetItemCount() ; iItem ++)
			{
				CWnd* pWnd = GetSubItemWnd(iItem,iSubItem);
				if(pWnd)
				{
					CRect rc;
					rc = GetSubItemRect(iItem,iSubItem);
					CRect rcNew = rc;
					if (GetColIsAutosize(iSubItem))
					{
						rcNew = rc;
					}
					else
					{
						SIZE sz = GetSubItemStruct(iItem,iSubItem)->szWnd;
						const int iMargin = 2;
						if (rc.Width() <= (sz.cx+iMargin*2))
						{
							rcNew.left = rc.left;
							rcNew.right = rc.right;
						}
						else
						{
							switch(m_vecColsInfo[iSubItem].txtAlign)
							{
							case DT_LEFT:
								rcNew.left = rc.left + iMargin;
								rcNew.right = rcNew.left + sz.cx;
								break;
							case DT_RIGHT:
								rcNew.right = rc.right - iMargin;
								rcNew.left = rcNew.right - sz.cx;
								break;
							case DT_CENTER:
								rcNew.left = rc.left + (rc.Width()- sz.cx)/2;
								rcNew.right = rc.right - (rc.Width() - sz.cx)/2;
								break;
							}
						}
						if (rc.Height() <= sz.cy)
						{
							rcNew.top = rc.top;
							rcNew.bottom = rc.bottom;
						}
						else
						{
							rcNew.top = rc.top + (rc.Height()-sz.cy)/2;
							rcNew.bottom = rc.bottom - (rc.Height()-sz.cy)/2;
						}
					}

					CHeaderCtrl *pHead = GetHeaderCtrl();
					CRect rcHead;
					pHead->GetClientRect(rcHead);
					if( rcNew.top >= rcHead.Height() )
					{
						pWnd->ShowWindow(TRUE);
					}
					else
					{
						pWnd->ShowWindow(FALSE);
					}

					pWnd->MoveWindow(rcNew);

					//pWnd->Invalidate();
				}
			}
		}
		else if( GetColHasChildWnd(iSubItem) &&
			GetColIsShared(iSubItem) == TRUE)  //共享式
		{
			int iShowAtItem = GetColChildWndAtItem(iSubItem);
			CWnd *pWnd = GetColChildWnd(iSubItem);
			if( pWnd != NULL && iShowAtItem != -1)
			{
				CRect rc;
				rc = GetSubItemRect(iShowAtItem,iSubItem);
				if( GetColChildWndShown(iSubItem) )
				{	
					CRect rc;
					rc = GetSubItemRect(iShowAtItem,iSubItem);
					CRect rcNew = rc;
					if (GetColIsAutosize(iSubItem))
					{
						rcNew = rc;
					}
					else
					{
						SIZE sz = GetSubItemStruct(iShowAtItem,iSubItem)->szWnd;
						const int iMargin = 2;
						if (rc.Width() <= (sz.cx+iMargin*2))
						{
							rcNew.left = rc.left;
							rcNew.right = rc.right;
						}
						else
						{
							switch(m_vecColsInfo[iSubItem].txtAlign)
							{
							case DT_LEFT:
								rcNew.left = rc.left + iMargin;
								rcNew.right = rcNew.left + sz.cx;
								break;
							case DT_RIGHT:
								rcNew.right = rc.right - iMargin;
								rcNew.left = rcNew.right - sz.cx;
								break;
							case DT_CENTER:
								rcNew.left = rc.left + (rc.Width()- sz.cx)/2;
								rcNew.right = rc.right - (rc.Width() - sz.cx)/2;
								break;
							}
						}
						if (rc.Height() <= sz.cy)
						{
							rcNew.top = rc.top;
							rcNew.bottom = rc.bottom;
						}
						else
						{
							rcNew.top = rc.top + (rc.Height()-sz.cy)/2;
							rcNew.bottom = rc.bottom - (rc.Height()-sz.cy)/2;
						}
					}

					pWnd->MoveWindow(rcNew);

					if( rc.top >= GetHeaderHeight() )
					{					
						pWnd->ShowWindow(TRUE);					
					}
					else
					{
						pWnd->ShowWindow(FALSE);
					}
				}
				else
				{
					pWnd->ShowWindow(FALSE);
				}
			}
			if(pWnd && iShowAtItem == -1)
			{
				pWnd->ShowWindow(FALSE);
			}
			pWnd->Invalidate();
		}
	}

	//*m_headCtrl->SetWindowPos(NULL,0,0,rcC.Width(),rcC.Height(),SWP_NOMOVE)*/
	

}

int CGxxListCtrl::GetColumnCount()
{
	return CListCtrl::GetHeaderCtrl()->GetItemCount();
}

int CGxxListCtrl::InsertColumn( int nCol, LPCTSTR lpszColumnTxt, int nFormat/*=LVCFMT_LEFT */,int		nWidth)
{	
	return CGxxListCtrl::InsertColumn(nCol,lpszColumnTxt,nFormat,nWidth,FALSE);
}

int CGxxListCtrl::InsertColumn( 
							   int nCol, 
							   LPCTSTR lpszColumnTxt, 
							   int nFormat,
							   int nWidth, 
							   BOOL bHaveChildWnd/*=FALSE*/, 
							   BOOL bShared/*=FALSE*/, 
							   BOOL bAutoAdjust/*=FALSE*/, 
							   LPSIZE  lpCtrlSize/*=NULL*/,
							   int nChildWndKind/*=GLC_CUSTOM*/, 
							   CWnd* pChildWnd/*=NULL*/ )
{
	//保存表头的文本对齐方式
	DWORD nTxtAlign = DT_LEFT;
	switch(nFormat)
	{
	case LVCFMT_LEFT:
		nTxtAlign = DT_LEFT;
		break;
	case LVCFMT_CENTER:
		nTxtAlign = DT_CENTER; 
		break;
	case LVCFMT_RIGHT:
		nTxtAlign = DT_RIGHT;
		break;
	default:
		break;
	}
	
	/// 
	if( bHaveChildWnd )  //有子窗口
	{
		ColInfo colInfo;
		colInfo.txtAlign = nTxtAlign;
		colInfo.bHaveChildWnd = TRUE;
		colInfo.bShared = bShared;
		colInfo.bAutoAdjust = bAutoAdjust;
		if (!bAutoAdjust)
		{
			if (lpCtrlSize!=NULL)
			{
				colInfo.szWnd = *lpCtrlSize;
			}
			
		}
		if( bShared )
		{
			colInfo.nChildWndKind = nChildWndKind;
			if( nChildWndKind == GLC_CUSTOM )
			{
				ASSERT(pChildWnd != NULL);
				colInfo.pChildWnd = pChildWnd;
			}
			else
			{
				colInfo.pChildWnd = CreateChildWnd(nChildWndKind);
			}
		}
		m_vecColsInfo.insert(m_vecColsInfo.begin()+nCol,colInfo); //插入这一列的信息
	}
	else  //无子窗口
	{
		ColInfo colInfo;
		colInfo.txtAlign = nTxtAlign;
		m_vecColsInfo.insert(m_vecColsInfo.begin()+nCol,colInfo); //插入这一列的信息
	}
	if( GetItemCount() > 0 )
	{
		int nCount =  GetItemCount();
		for(int iItem = 0 ; iItem < nCount; iItem ++)
		{
			SubItemStruct si;
			if( bHaveChildWnd && bShared )
			{
				si.nChildWndKind = m_vecColsInfo[nCol].nChildWndKind;
				si.pChildWnd = m_vecColsInfo[nCol].pChildWnd;
			}
			m_vecSubItems[iItem].insert( m_vecSubItems[iItem].begin() + nCol,si);
		}
	}
	return CListCtrl::InsertColumn(nCol,lpszColumnTxt,nFormat,nWidth);
}

CWnd* CGxxListCtrl::CreateChildWnd( int nChildWndKind )
{
	CWnd *pWnd = NULL;
	switch( nChildWndKind )
	{
	case GLC_Button:
		{
			pWnd = new CButton;
			((CButton *)pWnd)->Create(L"",BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD|BS_NOTIFY,\
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
		}
		break;
	case GLC_CheckBox:
		{
			pWnd = new CButton;
			((CButton *)pWnd)->Create(L"",BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|BS_NOTIFY,\
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
		}
		break;
	case GLC_RadioBox:
		{
			pWnd = new CButton;
			((CButton *)pWnd)->Create(L"",BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|BS_NOTIFY,\
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
		}
		break;
	case GLC_ComboBox:
		{
			pWnd = new CComboBox;
			((CComboBox *)pWnd)->Create(CBS_DROPDOWNLIST|WS_VISIBLE|WS_CHILD,\
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
		}
		break;
	case GLC_Static:
		{
			pWnd = new CStatic;
			((CStatic *)pWnd)->Create(L"",WS_VISIBLE|WS_CHILD, \
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
		}
		break;
	case GLC_Edit:
		{
			pWnd = new CEdit;
			((CEdit *)pWnd)->Create(WS_TABSTOP|WS_VISIBLE|WS_CHILD|ES_AUTOHSCROLL,\
				CRect(0,0,0,0),this,++m_nInnerMaxCtrlID );
			pWnd->ModifyStyleEx(0,WS_EX_CLIENTEDGE,   SWP_DRAWFRAME);
		}
		break;
	default:
		break;
	}
		TRACE("\nCGxxListCtrl: Creating child window! hWnd=%x,nID=%d\n\n",pWnd->m_hWnd,pWnd->GetDlgCtrlID());
	return pWnd;
}

CWnd * CGxxListCtrl::GetSubItemWnd( int iItem,int iSubItem )
{
	return m_vecSubItems[iItem].at(iSubItem).pChildWnd;
}

BOOL CGxxListCtrl::GetColHasChildWnd( int iCol )
{
	return m_vecColsInfo[iCol].bHaveChildWnd;
}

BOOL CGxxListCtrl::GetColIsShared( int iCol )
{
	return m_vecColsInfo[iCol].bShared;
}

int CGxxListCtrl::GetColChildWndKind( int iCol )
{
	return m_vecColsInfo[iCol].nChildWndKind;
}

CWnd* CGxxListCtrl::GetColChildWnd( int iCol )
{
	return m_vecColsInfo[iCol].pChildWnd;
}

BOOL CGxxListCtrl::GetColChildWndShown( int iCol )
{
	return m_vecColsInfo[iCol].bShown;
}

void CGxxListCtrl::SetColChildWndShown(int iCol, BOOL bShow )
{
	m_vecColsInfo[iCol].bShown = bShow;
}

int CGxxListCtrl::GetColChildWndAtItem( int iCol )
{
	return m_vecColsInfo[iCol].iShowAtItem;
}

void CGxxListCtrl::SetColChildWndAtItem( int iCol,int iAtItem )
{
	m_vecColsInfo[iCol].iShowAtItem = iAtItem;
}

int CGxxListCtrl::InsertItem( 
							 int nItem, 
							 CString str,
							 int nChildWndKind/*=GLC_CUSTOM*/,
							 CWnd* pChildWnd/*=NULL*/,
							 LPSIZE lpSize/*=NULL*/)
{
	vector<SubItemStruct> vecRow;
	int nCount = GetColumnCount();
	vecRow.resize( GetColumnCount() );
	SubItemStruct *pSubItem = &vecRow[0];

	if ( GetColHasChildWnd(0) && !GetColIsShared(0))
	{
		pSubItem->nChildWndKind = nChildWndKind;
		if( nChildWndKind == GLC_CUSTOM )
		{
			//ASSERT(pChildWnd != NULL);
			pSubItem->pChildWnd = pChildWnd;
		}
		else
		{
			pSubItem->pChildWnd = CreateChildWnd( nChildWndKind );
			if( pSubItem->pChildWnd )
				pSubItem->pChildWnd->SetWindowText(str);
			ASSERT(pSubItem->pChildWnd);
		}

		if (!m_vecColsInfo[0].bAutoAdjust)
		{
			if (lpSize==NULL)
			{
				pSubItem->szWnd = m_vecColsInfo[0].szWnd;
			}
			else
			{
				pSubItem->szWnd = *lpSize;
			}
		}
	}
	
	for(int i = 0; i < GetColumnCount(); i++)
	{
		pSubItem = &vecRow[i];
		if (GetColHasChildWnd(i) && GetColIsShared(i))
		{
			pSubItem->nChildWndKind = GetColChildWndKind(i);
			pSubItem->pChildWnd = GetColChildWnd(i);

			if (!m_vecColsInfo[i].bAutoAdjust)
			{
				pSubItem->szWnd = m_vecColsInfo[i].szWnd;
			}
		}
	}

	m_vecSubItems.insert(m_vecSubItems.begin()+nItem,vecRow);
	return CListCtrl::InsertItem(nItem,str);
}


BOOL CGxxListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	BOOL bRes = CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	ShowAllSubWindows();
	CGxxListCtrl::RedrawWindow();
	return bRes;
}

void CGxxListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	OnClickItem( pNMItemActivate->iItem,pNMItemActivate->iSubItem);



}

void CGxxListCtrl::OnClickItem( int nItem,int nSubItem )
{
	BOOL bShouldShown = FALSE;

	//单击某一列的单元格，就需要把其他列的共享控件隐藏
	for(int iCol = 0 ; iCol < GetColumnCount() ; iCol ++)
	{
		if( iCol == nSubItem && nItem != -1) //本身单击的这一列不做处理
		{
			continue;
		}
		if( GetColHasChildWnd(iCol) &&  //列必须支持控件并且是共享式的
			GetColIsShared(iCol))
		{
			CWnd* pWnd = GetColChildWnd(iCol); //获取共享式的控件指针
			if( pWnd!= NULL && GetColChildWndShown(iCol) ==TRUE)
			{
				int iShowAt = GetColChildWndAtItem(iCol);  //获取控件处于哪一行
				if( iShowAt != -1)
				{
					ASSERT(iShowAt<=GetItemCount());
					CString str;
					str = GetChildWndText(pWnd,iShowAt,iCol);
					SetItemText(iShowAt,iCol,str);
					pWnd->ShowWindow(FALSE);
					SetColChildWndShown(iCol,FALSE);
				}
				bShouldShown = TRUE;
			}
			SetColChildWndAtItem(iCol,-1); //设置控件不处于任何位置
		}
	}
	if( nItem != -1 && nSubItem != -1)
	{
		if( GetColHasChildWnd(nSubItem) &&
			GetColIsShared(nSubItem))
		{
			CWnd* pWnd = GetColChildWnd(nSubItem);
			if(pWnd != NULL)
			{
				int iShowAt = GetColChildWndAtItem(nSubItem);
				if( iShowAt != -1 && GetColChildWndShown(nSubItem) && iShowAt != nItem)
				{	//要求控件处于显示状态，并且不等于当前单击的这一行，才执行下面
					
					//把控件的文本设置到列单元格
					CString str;
					str = GetChildWndText(pWnd,iShowAt,nSubItem);
					SetItemText(iShowAt,nSubItem,str);
				}
				SetColChildWndShown(nSubItem,TRUE);
				SetColChildWndAtItem(nSubItem,nItem);
				CString str;
				str = GetItemText(nItem,nSubItem);
				SetChildWndText(pWnd,str,nItem,nSubItem);			
				bShouldShown = TRUE;
			}
		}
	}
	else
	{
		/*if( GetColHasChildWnd(nSubItem) &&
			GetColIsShared(nSubItem))
		{
			CWnd* pWnd = GetColChildWnd(nSubItem);
			if(pWnd != NULL)
			{
				int iShowAt = GetColChildWndAtItem(nSubItem);
				if( iShowAt != -1)
				{
					CString str;
					pWnd->GetWindowText(str);
					SetItemText(iShowAt,nSubItem,str);
				}
				SetColChildWndShown(nSubItem,FALSE);
				SetColChildWndAtItem(nSubItem,nItem);
			}
		}
		ShowAllSubWindows();*/
	}
	if( bShouldShown )
		ShowAllSubWindows();
}

UINT CGxxListCtrl::GetSubItemWndId( int iItem,int iSubItem )
{
	CWnd *pWnd = GetSubItemWnd(iItem,iSubItem);
	if( pWnd == NULL || !IsWindow(pWnd->GetSafeHwnd()))
	{
		return 0;
	}
	return pWnd->GetDlgCtrlID();
}

BOOL CGxxListCtrl::DeleteItem( int nItem )
{
	ASSERT( nItem < GetItemCount() );

	vector<SubItemStruct>& pItemSubs = m_vecSubItems[nItem];
	for(int iCol = 0 ;iCol < (int)pItemSubs.size() ; iCol ++)
	{
		///有子窗口的非共享列的子窗口要删除
		if( 
			GetColHasChildWnd(iCol) && 
			GetColIsShared(iCol) == FALSE &&
			pItemSubs[iCol].pChildWnd != NULL
		  )
		{	
			pItemSubs[iCol].pChildWnd->SendMessage(WM_CLOSE,0,0);
			delete pItemSubs[iCol].pChildWnd;	
			pItemSubs[iCol].pChildWnd = NULL;
		}

		if( GetColHasChildWnd(iCol) &&
			GetColIsShared(iCol) == TRUE)
		{
			if( GetColChildWndAtItem(iCol) == nItem )
			{
				SetColChildWndAtItem(iCol,-1);
			}
		}
	}

	m_vecSubItems.erase( m_vecSubItems.begin() + nItem);

	BOOL bRes = CListCtrl::DeleteItem(nItem);
	ShowAllSubWindows();
	ASSERT(bRes);
	return bRes;
}

BOOL CGxxListCtrl::DeleteAllItems()
{
	for(int i = 0 ; i < GetItemCount() ; i++)
	{
		vector<SubItemStruct>& pItemSubs = m_vecSubItems[i];
		for(int iCol = 0 ;iCol < (int)pItemSubs.size() ; iCol ++)
		{
			if( 
				GetColHasChildWnd(iCol) && 
				GetColIsShared(iCol) == FALSE &&
				pItemSubs[iCol].pChildWnd != NULL
				)
			{	///有子窗口的非共享列的子窗口要删除，并且是非自定义的子窗口

				//TRACE("删除子窗口%x\n",pItemSubs[iCol].pChildWnd,m_hWnd);
				pItemSubs[iCol].pChildWnd->SendMessage(WM_CLOSE,0,0);
				delete pItemSubs[iCol].pChildWnd;	
				pItemSubs[iCol].pChildWnd = NULL;
			}
			if( GetColHasChildWnd(iCol) &&
				GetColIsShared(iCol) == TRUE)
			{
				if( GetColChildWndAtItem(iCol) == i )
				{
					SetColChildWndAtItem(iCol,-1);
				}
			}
		}
	}

	m_vecSubItems.clear();

	return CListCtrl::DeleteAllItems();
}


int CGxxListCtrl::GetSubItemWndKind( int iItem,int iSubItem )
{
	return m_vecSubItems[iItem][iSubItem].nChildWndKind;
}

int CGxxListCtrl::SetItem( 
						  int nItem,
						  int nSubItem,
						  CString str,
						  int nChildWndKind/*=GLC_CUSTOM*/,
						  CWnd* pChildWnd/*=NULL*/,
						  LPSIZE lpSize/*=NULL*/)
{
	SubItemStruct *pSubItem = GetSubItemStruct(nItem, nSubItem);

	if( GetColHasChildWnd(nSubItem) )
	{
		if( GetColIsShared(nSubItem) )  //列共享式子窗口
		{
			pSubItem->nChildWndKind = GetColChildWndKind(nSubItem);
			pSubItem->pChildWnd = GetColChildWnd(nSubItem);

			if (!m_vecColsInfo[nItem].bAutoAdjust)
			{
				pSubItem->szWnd = m_vecColsInfo[nSubItem].szWnd;
			}
		}
		else   //非共享式的
		{
			//先删除以前的
			if( pSubItem->pChildWnd != NULL)
			{
				delete pSubItem->pChildWnd;
				pSubItem->pChildWnd = NULL;
			}
			pSubItem->nChildWndKind = nChildWndKind;
			if( nChildWndKind == GLC_CUSTOM )
			{
				//ASSERT(pChildWnd != NULL);
				pSubItem->pChildWnd = pChildWnd;
			}
			else
			{
				pSubItem->pChildWnd = CreateChildWnd( nChildWndKind );
				ASSERT( pSubItem->pChildWnd != NULL);
			}
			if( pSubItem->pChildWnd )
			{
				pSubItem->pChildWnd->SetWindowText(str);
			}

			if (!m_vecColsInfo[nSubItem].bAutoAdjust)
			{
				if (lpSize==NULL)
				{
					pSubItem->szWnd = m_vecColsInfo[nSubItem].szWnd;
				}
				else
				{
					pSubItem->szWnd = *lpSize;
				}
			}
		}	
	}
	return (int)CListCtrl::SetItemText(nItem,nSubItem,str);
}

int CGxxListCtrl::DeleteColumn( int nCol )
{
	for(int iItem = 0 ; iItem < GetItemCount() ; iItem ++)
	{
		vector<SubItemStruct>& vecRow = m_vecSubItems[iItem];

		if( GetColHasChildWnd(nCol) &&
			GetColIsShared(nCol) == FALSE &&
			GetSubItemWnd(iItem,nCol) != NULL)
		{
			delete GetSubItemWnd(iItem,nCol);
		}

		vecRow.erase(vecRow.begin() + nCol);

	}
	
	if( GetColHasChildWnd(nCol) && GetColIsShared(nCol) && GetColChildWndKind(nCol) != GLC_CUSTOM)
	{
		if( GetColChildWnd(nCol) )
		{
			delete GetColChildWnd(nCol);
		}
	}
	m_vecColsInfo.erase( m_vecColsInfo.begin() + nCol);

	int iRes = (int)CListCtrl::DeleteColumn(nCol);

	ShowAllSubWindows();

	return iRes;
}

COLORREF CGxxListCtrl::GetSubItemTxtColor( int iItem,int iSubItem )
{
	return m_vecSubItems[iItem].at(iSubItem).clrTxt;
}

COLORREF CGxxListCtrl::GetSubItemBkColor( int iItem,int iSubItem )
{
	return m_vecSubItems[iItem].at(iSubItem).clrBk;
}

void CGxxListCtrl::SetSubItemTxtColor( int iItem,int iSubItem,COLORREF clr )
{
	m_vecSubItems[iItem][iSubItem].clrTxt = clr;
}

void CGxxListCtrl::SetSubItemBkColor( int iItem,int iSubItem,COLORREF clr )
{
	m_vecSubItems[iItem][iSubItem].clrBk = clr;
}


void CGxxListCtrl::SetHeaderFont( CFont* pFont )
{
	GetHeaderCtrl()->SetFont(pFont);
}

CString CGxxListCtrl::GetChildWndText( CWnd*pWnd ,int nItem,int nSubItem)
{
	ASSERT(pWnd!=NULL);
	CString str;
	pWnd->GetWindowText(str);
	return str;
}

void CGxxListCtrl::SetChildWndText( CWnd*pWnd,CString str,int nItem,int nSubItem )
{
	ASSERT(pWnd!=NULL);
	pWnd->SetWindowText(str);
}

void CGxxListCtrl::SetSubItemData( int iItem,int iSubItem,DWORD dataPtr )
{
	GetSubItemStruct(iItem,iSubItem)->dataPtr = dataPtr;
}

DWORD CGxxListCtrl::GetSubItemData(int iItem,int iSubItem)
{
	return GetSubItemStruct(iItem,iSubItem)->dataPtr;
}

CGxxListCtrl::SubItemStruct* CGxxListCtrl::GetSubItemStruct(int iItem, int iSubItem)
{
	ASSERT(iItem>=0);
	ASSERT(iSubItem < GetColumnCount());
	return &m_vecSubItems[iItem][iSubItem];
}


void CGxxListCtrl::GetItemIdByChildWndID( UINT nID,int* pItem,int* pSubItem /*= NULL*/ )
{
	if(pItem!= NULL)
		*pItem = -1;
	if(pSubItem!= NULL)
		*pSubItem = -1;
	for(int iCol = 0 ;iCol <GetColumnCount() ; iCol ++)
	{
		if( GetColHasChildWnd(iCol) )
		{
			if( GetColIsShared(iCol) )
			{
				CWnd *pWnd = GetColChildWnd(iCol);
				if( pWnd && pWnd->GetDlgCtrlID() == nID)
				{
					if(pItem!= NULL)
						*pItem =0;
					if(pSubItem!= NULL)
						*pSubItem = iCol;
					return; //找到
				}
			}
			else
			{
				for(int i = 0 ; i < GetItemCount() ; i ++)
				{
					CWnd *pWnd = GetSubItemWnd(i,iCol);
					if( pWnd && pWnd->GetDlgCtrlID() == nID)
					{
						if(pItem!= NULL)
							*pItem = i;
						if(pSubItem!= NULL)
							*pSubItem = iCol;
						return;
					}
				}
			}
		}
		else
			continue;
	}
}

void CGxxListCtrl::HideAllSharedWnd()
{
	OnClickItem(-1,-1);
}
//
//}

int CGxxListCtrl::GetHeaderHeight()
{
	CRect rc;
	GetHeaderCtrl()->GetClientRect(rc);
	return rc.Height();
}

void CGxxListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==10000)
	{
		Invalidate();
		ShowAllSubWindows();
		KillTimer(nIDEvent);
	}
}
BOOL CGxxListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_COMMAND)
	{
		int iItem,iItem2;
		GetItemIdByChildWndID( LOWORD(pMsg->wParam), &iItem, &iItem2);
		if (iItem!=-1 && iItem2!=-1)
		{	
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}
	if (pMsg->message == WM_NOTIFY)
	{
		UINT nID = ((LPNMHDR)pMsg->lParam)->idFrom;
		int iItem,iItem2;
		GetItemIdByChildWndID( nID, &iItem, &iItem2);
		if (iItem!=-1 && iItem2!=-1)
		{	
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

BOOL NS_COMMON::CGxxListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR lpHdr = (LPNMHDR)lParam;
	int iItem,iItem2;
	GetItemIdByChildWndID( lpHdr->idFrom, &iItem, &iItem2);
	if (iItem!=-1 && iItem2!=-1)
	{
		NMHDR_GLC_CONTROL glcHdr;
		glcHdr.hdr.code = GLCN_CHILD_CONTROL;
		glcHdr.hdr.hwndFrom = m_hWnd;
		glcHdr.hdr.idFrom = GetDlgCtrlID();
		glcHdr.nRowID = iItem;
		glcHdr.nColID = iItem2;
		glcHdr.nKind = GetSubItemWndKind(iItem, iItem2);
		glcHdr.pCtrlHdr = lpHdr;
		*pResult = GetParent()->SendMessage(WM_NOTIFY,WPARAM(m_hWnd), LPARAM(&glcHdr));
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

BOOL NS_COMMON::CGxxListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID = LOWORD(wParam);
	int iItem,iItem2;
	GetItemIdByChildWndID( nID, &iItem, &iItem2);
	if (iItem!=-1 && iItem2!=-1)
	{
		NMHDR_GLC_CONTROL glcHdr;
		glcHdr.hdr.code = GLCN_CHILD_CONTROL;
		glcHdr.hdr.hwndFrom = m_hWnd;
		glcHdr.hdr.idFrom = GetDlgCtrlID();
		glcHdr.nRowID = iItem;
		glcHdr.nColID = iItem2;
		glcHdr.nKind = GetSubItemWndKind(iItem, iItem2);
		NMHDR hdr;
		hdr.code = HIWORD(wParam);
		hdr.hwndFrom = (HWND)lParam;
		hdr.idFrom = nID;
		glcHdr.pCtrlHdr = &hdr;
		GetParent()->SendMessage(WM_NOTIFY,WPARAM(m_hWnd), LPARAM(&glcHdr));
		return TRUE;
	}

	return CListCtrl::OnCommand(wParam, lParam);
}

BOOL NS_COMMON::CGxxListCtrl::GetColIsAutosize(int iCol)
{
	return m_vecColsInfo[iCol].bAutoAdjust;
}

UINT NS_COMMON::CGxxListCtrl::RegeisterCtrlID()
{
	return ++m_nInnerMaxCtrlID;
}