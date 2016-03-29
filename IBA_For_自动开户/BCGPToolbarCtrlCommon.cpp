#include "stdafx.h"
#include ".\bcgptoolbarctrlcommon.h"


//CBCGPToolbarCtrlCommon::CBCGPToolbarCtrlCommon(void)
//{
//}

IMPLEMENT_SERIAL(CBCGPToolbarCtrlCommon, CBCGPToolbarButton, 1)

CBCGPToolbarCtrlCommon::CBCGPToolbarCtrlCommon()
{
	m_pWndParent  = NULL;
	m_dwCtrlStyle = 0;
	m_pWndCtrl    = 0;
	m_iWidth      = 0;
	m_cltrKind    = emBCGP_button;
}

CBCGPToolbarCtrlCommon::CBCGPToolbarCtrlCommon(
	UINT uiID, 
	int iImage, 
	emBCGPToolbarCtrlCommon ctrlKind, 
	CWnd *pParent /*= NULL*/, 
	DWORD dwStyle /*= 0*/, 
	int iWidth /*= 0*/) :
	CBCGPToolbarButton( uiID, iImage )
{
	m_strCtrlText = _T("");
	m_cltrKind    = ctrlKind;
	m_pWndParent  = pParent;
	m_iWidth      = (iWidth == 0)?iDefaultSize : iWidth;
	m_dwCtrlStyle = dwStyle | WS_CHILD | WS_VISIBLE;
	m_pWndCtrl    = NULL;
}


CBCGPToolbarCtrlCommon::~CBCGPToolbarCtrlCommon(void)
{
	if( m_pWndCtrl != NULL )
	{
		m_pWndCtrl->DestroyWindow();
		delete m_pWndCtrl;
	}
}

void CBCGPToolbarCtrlCommon::Serialize(CArchive& ar)
{
	CBCGPToolbarButton::Serialize(ar);

	if( ar.IsLoading() )
	{
		ar >> m_iWidth;
		ar >> m_dwCtrlStyle;

		int iKind = 0;
		ar >> iKind;
		m_cltrKind = (emBCGPToolbarCtrlCommon)iKind;
				
		m_rect.right = m_rect.left + m_iWidth;
		
	}
	else
	{
		ar << m_iWidth;
		ar << m_dwCtrlStyle;
		ar << (int)m_cltrKind;
	}
}




BOOL CBCGPToolbarCtrlCommon::CreateCtrlWnd()
{
	ASSERT( m_pWndParent!= NULL );

	if( m_pWndCtrl )
	{
		m_pWndCtrl->DestroyWindow();
		delete m_pWndCtrl;
		m_pWndCtrl = NULL;
	}

	switch( m_cltrKind )
	{
	case emBCGP_button:
		{
			//> 创建按钮控件

			CButton *pBtn = new CButton;
			if( !pBtn )
			{
				ASSERT(0);
				return FALSE;
			}
			BOOL bRes = pBtn->Create(m_strCtrlText,
				m_dwCtrlStyle|WS_CHILD|WS_VISIBLE,
				CRect(0,0,0,0),m_pWndParent,m_nID);
			
			if( !bRes )
			{
				delete pBtn;
				return FALSE;
			}

			m_pWndCtrl = pBtn;
		}
		break;

	case emBCGP_combobox:
		{
			//> 创建组合框

			CComboBox *pComb = new CComboBox;
			if( !pComb )
			{
				ASSERT(0);
				return FALSE;
			}
			BOOL bRes = pComb->Create(m_dwCtrlStyle,
				CRect(0,0,0,0),m_pWndParent,m_nID);

			if( !bRes )
			{
				delete pComb;
				return FALSE;
			}

			m_pWndCtrl = pComb;
		}
		break;

	case emBCGP_static:
		{
			CVCenterStatic *pSta = new CVCenterStatic;
			if( !pSta )
			{
				ASSERT(0);
				return FALSE;
			}
			BOOL bRes = pSta->Create(m_strCtrlText,m_dwCtrlStyle,CRect(0,0,0,0),m_pWndParent,m_nID);
			

			if( !bRes )
			{
				delete pSta;
				return FALSE;
			}

			m_pWndCtrl = pSta;
		}
		break;



	default:
		ASSERT(0);  // 不支持此类型
		return FALSE;
	}

	return TRUE;
}

SIZE CBCGPToolbarCtrlCommon::OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	CSize sz(sizeDefault);
	sz.cx = m_iWidth;
		return CBCGPToolbarButton::OnCalculateSize(pDC,sz,bHorz);
	//}

}

void CBCGPToolbarCtrlCommon::OnDraw(CDC* pDC, const CRect& rect, CBCGPToolBarImages* pImages, BOOL bHorz /* = TRUE */, BOOL bCustomizeMode /* = FALSE */, BOOL bHighlight /* = FALSE */, BOOL bDrawBorder /* = TRUE */, BOOL bGrayDisabledButtons /* = TRUE */)
{
	if (m_pWndCtrl->GetSafeHwnd () == NULL ||
		(m_pWndCtrl->GetStyle () & WS_VISIBLE) == 0)
	{
		CBCGPToolbarButton::OnDraw (pDC, rect, pImages,
			bHorz, bCustomizeMode,
			bHighlight, bDrawBorder, bGrayDisabledButtons);
	}
	else if ((m_bTextBelow && bHorz) && !m_strText.IsEmpty())
	{
		//--------------------
		// Draw button's text:
		//--------------------
		BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
			(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

		pDC->SetTextColor (bDisabled ?
			globalData.clrGrayedText : 
		(bHighlight) ?	CBCGPToolBar::GetHotTextColor () :
		globalData.clrBarText);
		CRect rectText;
		rectText.left = (rect.left + rect.right - m_sizeText.cx) / 2;
		rectText.right = (rect.left + rect.right + m_sizeText.cx) / 2;
		rectText.top = rect.bottom + rect.top;
		rectText.bottom = rectText.top + m_sizeText.cy;
		pDC->DrawText (m_strText, &rectText, DT_CENTER | DT_WORDBREAK);
	}
}

void CBCGPToolbarCtrlCommon::OnMove()
{
	if (m_pWndCtrl->GetSafeHwnd () == NULL ||
		(m_pWndCtrl->GetStyle () & WS_VISIBLE) == 0)
	{
		return;
	}

	CRect rectWnd;
	m_pWndCtrl->GetWindowRect (rectWnd);
	
	int cx = m_rect.Width() - 2 * iHorzMargin;
	int cy = 0;

	CSize szImage = ((CBCGPToolBar*)m_pWndParent)->GetImageSize();

	if( ((m_bTextBelow && IsHorizontal()) && !m_strText.IsEmpty()) )
	{
		cy = szImage.cy + 2*iVertMargin;
	}
	else
	{
		cy = m_rect.Height();
	}
	

	m_pWndCtrl->SetWindowPos(NULL,
		m_rect.left + iHorzMargin,
		m_rect.top /*+ iVertMargin*/,
		cx,
		cy,
		SWP_NOZORDER | SWP_NOACTIVATE);

	AdjustCtrlRect ();
}

void CBCGPToolbarCtrlCommon::AdjustCtrlRect()
{
	if( m_pWndCtrl->GetSafeHwnd () == NULL ||
		(m_pWndCtrl->GetStyle () & WS_VISIBLE) == 0 ||
		m_rect.IsRectEmpty ())
	{
		return;
	}

	m_pWndCtrl->GetWindowRect( &m_rect );
	m_pWndCtrl->ScreenToClient( &m_rect );
	m_pWndCtrl->MapWindowPoints( m_pWndCtrl->GetParent (), &m_rect);
	m_rect.InflateRect ( iHorzMargin, iVertMargin );
}

void CBCGPToolbarCtrlCommon::SetCtrlWindowText(CString str)
{
	m_strCtrlText = str;
	if( m_pWndCtrl != NULL && m_pWndCtrl->GetSafeHwnd() != NULL )
	{
		m_pWndCtrl->SetWindowText( str );
	}
}


void CBCGPToolbarCtrlCommon::OnShow (BOOL bShow)
{
	if (m_pWndCtrl->GetSafeHwnd () != NULL)
	{
		if (bShow && m_bHorz)
		{
			m_pWndCtrl->ShowWindow (SW_SHOWNOACTIVATE);
			OnMove ();
		}
		else
		{
			m_pWndCtrl->ShowWindow (SW_HIDE);
		}
	}
}

void CBCGPToolbarCtrlCommon::OnChangeParentWnd(CWnd* pWndParent)
{
	CBCGPToolbarButton::OnChangeParentWnd (pWndParent);

	if (m_pWndCtrl->GetSafeHwnd () != NULL)
	{
		CWnd* pWndParentCurr = m_pWndCtrl->GetParent ();
		ASSERT (pWndParentCurr != NULL);

		if (pWndParent != NULL &&
			pWndParentCurr->GetSafeHwnd () == pWndParent->GetSafeHwnd ())
		{
			return;
		}

		m_pWndCtrl->DestroyWindow ();
		delete m_pWndCtrl;
		m_pWndCtrl = NULL;
	}

	if (pWndParent == NULL || pWndParent->GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rect = m_rect;
	rect.InflateRect (-2, 0);
	rect.bottom = rect.top + globalData.GetTextHeight() + 2 * iVertMargin;

	m_pWndParent = pWndParent;
	if ( !CreateCtrlWnd() )
	{
		ASSERT (FALSE);
		return;
	}

	AdjustCtrlRect();

	m_pWndCtrl->SetFont (&globalData.fontRegular);

}

void CBCGPToolbarCtrlCommon::CopyFrom(const CBCGPToolbarButton& src)
{
	CBCGPToolbarButton::CopyFrom(src);

	const CBCGPToolbarCtrlCommon& s = (const CBCGPToolbarCtrlCommon&)src;

	m_dwCtrlStyle = s.m_dwCtrlStyle;
	m_iWidth = s.m_iWidth;
	m_cltrKind = s.m_cltrKind;
	m_strCtrlText = s.m_strCtrlText;
}

//*************************************************************************

CVCenterStatic::CVCenterStatic()
{
	m_clrBk = GetSysColor( COLOR_3DFACE );
	m_clrTxt = RGB(0,0,0);
	m_pFont = NULL;
}

BEGIN_MESSAGE_MAP(CVCenterStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CVCenterStatic::OnPaint()
{
	CPaintDC dc(this); 

	DrawBkAndText(dc);
}
void CVCenterStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	DrawBkAndText( *pDC );
}


void CVCenterStatic::SetFont(CFont* pFont)
{
	m_pFont = pFont;
}

COLORREF CVCenterStatic::SetBkColor(const COLORREF clrBk,BOOL bRepaint /*= FALSE*/)
{
	COLORREF old = m_clrBk;
	m_clrBk = clrBk;
	if( bRepaint )
		Invalidate();
	return old;
}

COLORREF CVCenterStatic::SetTextColor(const COLORREF clrTxt,BOOL bRepaint /*= FALSE*/)
{
	COLORREF old = m_clrBk;
	m_clrTxt = clrTxt;
	if( bRepaint )
		Invalidate();
	return old;
}

void CVCenterStatic::DrawBkAndText(CDC& dc)
{
	CRect rcRect;
	CRect rcTxt;

	GetClientRect( rcRect );

	CDC memDC;  // Paint的内存DC
	CBitmap bmp;

	bmp.CreateCompatibleBitmap( &dc,rcRect.Width(),rcRect.Height() );
	memDC.CreateCompatibleDC(NULL);
	memDC.SelectObject(&bmp);

	memDC.FillSolidRect(rcRect,m_clrBk);

	CString strTxt;
	GetWindowText(strTxt);

	UINT nFlag = DT_LEFT;

	//DWORD nStytle = GetStyle();
	//if( nStytle & SS_RIGHT){
	//	nFlag = DT_RIGHT;
	//}
	//else if( nStytle & SS_CENTER )
	//{
	//	nFlag = DT_CENTER;
	//}

	CFont* pOld = memDC.SelectObject( m_pFont );

	// 绘制文本
	if( !strTxt.IsEmpty() )
	{
		memDC.SetTextColor(m_clrTxt);

		TEXTMETRIC tag;
		memDC.GetTextMetrics(&tag);

		rcTxt = rcRect;
		rcTxt.top = rcRect.top + (rcRect.Height() - tag.tmHeight)/2;
		rcTxt.bottom = rcTxt.top + tag.tmHeight;

		rcTxt.DeflateRect(1,0,0,1);

		memDC.DrawText(strTxt,rcTxt,nFlag);
	}

	memDC.SelectObject( pOld );

	CBCGPToolBar* pParent = (CBCGPToolBar*)GetParent();

	if( pParent )
	{
		if( !pParent->IsKindOf(RUNTIME_CLASS(CBCGPToolBar)) )
		{
			pParent = (CBCGPToolBar*)pParent->GetParent();
		}
		CDC* pDC = pParent->GetDC();
		WINDOWPLACEMENT wp;
		this->GetWindowPlacement(&wp);

		CRect rcParent;
		pParent->GetClientRect(rcParent);

		// 绘制背景
		CDC memParentDC;
		CBitmap bmpParent;
		bmpParent.CreateCompatibleBitmap(pDC,rcParent.Width(),rcParent.Height());
		memParentDC.CreateCompatibleDC(NULL);
		memParentDC.SelectObject(&bmpParent);

		CBCGPVisualManager::GetInstance ()->OnFillBarBackground (&memParentDC, pParent,
			rcParent,rcParent);

		CRect rt = wp.rcNormalPosition;

		//
		CDC memDCMark;
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC,rt.Width(),rt.Height());
		memDCMark.CreateCompatibleDC(NULL);
		memDCMark.SelectObject(&bmp);



		BOOL bRes = FALSE;

		if( memDCMark.BitBlt(0,0,rt.Width(),rt.Height(),&memParentDC,rt.left,rt.top,SRCCOPY) )
		{
			bRes = memDCMark.TransparentBlt(0,0,rt.Width(),rt.Height(),
				&memDC,0,0,rt.Width(),rt.Height(),m_clrBk);

			bRes = dc.BitBlt(0,0,rt.Width(),rt.Height(),&memDCMark,0,0,SRCCOPY);
		}
		else
		{
			dc.BitBlt(0,0,rt.Width(),rt.Height(),&memDC,0,0,SRCCOPY);
		}

		pParent->ReleaseDC(pDC);
	}
}
