// BCGPLinkStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "BCGPLinkStatic.h"


// CBCGPLinkStatic

IMPLEMENT_DYNAMIC(CBCGPLinkStatic, CBCGPStatic)
CBCGPLinkStatic::CBCGPLinkStatic():
	m_pFont(NULL),
	m_pHoverFont(NULL),
	m_strLinkAddress(_T("")),
	m_bAutoOpen(FALSE),
	m_clrBk(GetSysColor(COLOR_3DFACE)),
	m_clrLink(RGB(0,0,0)),
	m_clrHover(RGB(0,0,255)),
	m_clrVisited(RGB(0,0,0)),
	m_bVCenter(TRUE),
	m_bTransparent(TRUE)
{
	m_nMouseState = emLinkNormal;
	m_bHaveVisited = FALSE;
}

CBCGPLinkStatic::~CBCGPLinkStatic()
{
	
}



BEGIN_MESSAGE_MAP(CBCGPLinkStatic, CBCGPStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CBCGPLinkStatic 消息处理程序


void CBCGPLinkStatic::OnPaint()
{
	CRect rect;
	GetClientRect(rect);
	CString str;
	GetWindowText(str);

	CPaintDC dc(this); 
	DrawLinkBk(&dc,rect,m_bTransparent,m_nMouseState);
	DrawLinkName(&dc,rect,str,m_nMouseState);
	return;
	
}

void CBCGPLinkStatic::DrawLinkBk(CDC* pDC,RECT rect,BOOL bTransparent,UINT nMouseState)
{
	if( !m_bTransparent )
	{
		pDC->FillSolidRect(&rect,m_clrBk);
		return;
	}

	CRect rc;
	GetWindowRect(rc);
	globalData.DrawParentBackground(this,pDC,rc);

}

void CBCGPLinkStatic::DrawLinkName(CDC* pDC,RECT rect,CString strName,UINT nMouseState)
{
	pDC->SetBkMode(TRANSPARENT);

	UINT uiDTFlags = DT_WORDBREAK;

	const DWORD dwStyle = GetStyle ();

	if (dwStyle & SS_CENTER)
	{
		uiDTFlags |= DT_CENTER;
	}
	else if (dwStyle & SS_RIGHT)
	{
		uiDTFlags |= DT_RIGHT;
	}

	if (dwStyle & SS_NOPREFIX)
	{
		uiDTFlags |= DT_NOPREFIX;
	}

	CString strText(strName);

	COLORREF clr;
	CFont* pOldFont = NULL;
	CFont* pFont = NULL;

	if( nMouseState == emLinkHover )
	{
		if( m_clrHover == 0)
			clr = globalData.clrBarText;
		else
			clr = m_clrHover;

		if( m_pHoverFont == NULL )
			pFont = m_pFont;
		else
			pFont = m_pHoverFont;
	}
	else if( nMouseState == emLinkVisited || m_bHaveVisited)
	{
		if( m_clrVisited == 0)
			clr = globalData.clrBarText;
		else
			clr = m_clrVisited;

		pFont = m_pFont;
	}
	else if( nMouseState == emLinkNormal)
	{
		if( m_clrLink == 0)
			clr = globalData.clrBarText;
		else
			clr = m_clrLink;

		pFont = m_pFont; 
	}
	else
		clr = globalData.clrBarText;

	if( !IsWindowEnabled() )
		clr = globalData.clrGrayedText;

	if( pFont != NULL )
		pOldFont = pDC->SelectObject(pFont);
	else
		pOldFont = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);

	pDC->SetTextColor(clr);

	if( m_bVCenter )
	{
		TEXTMETRIC tag;
		pDC->GetTextMetrics(&tag);

		rect.top = rect.top + (rect.bottom - rect.top - tag.tmHeight)/2;
		rect.bottom = rect.top + tag.tmHeight;
	}
	
	pDC->DrawText(strText,&rect,uiDTFlags);
	//CBCGPVisualManager::GetInstance ()->DrawTextOnGlass (
	//	pDC, strText, rect, uiDTFlags, 6,clr); 

	pDC->SelectObject (pOldFont);
}

void CBCGPLinkStatic::SetLinkAddress(CString strLink,BOOL bAutoOpen/*=FALSE*/)
{
	m_strLinkAddress = strLink;
	m_bAutoOpen = bAutoOpen;
}

void CBCGPLinkStatic::SetTransparent(BOOL bTransparent)
{
	m_bTransparent = bTransparent;
}

void CBCGPLinkStatic::SetBkColor(COLORREF clr)
{
	m_clrBk = clr;
}

void CBCGPLinkStatic::SetLinkColor(COLORREF clr)
{
	m_clrLink = clr;
}

void CBCGPLinkStatic::SetHoverColor(COLORREF clr)
{
	m_clrHover = clr;
}

void CBCGPLinkStatic::SetVisitedColor(COLORREF clr)
{
	m_clrVisited = clr;
}

void CBCGPLinkStatic::SetFont(CFont *pFont)
{
	m_pFont = pFont;
}

CFont* CBCGPLinkStatic::GetFont()
{
	return m_pFont;
}
void CBCGPLinkStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	SetCapture();
	SetTimer(1,20,NULL);

	CStatic::OnMouseMove(nFlags, point);
}


void CBCGPLinkStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bHaveVisited = TRUE;
	m_nMouseState = emLinkVisited;
	Invalidate();

	if( m_bAutoOpen )
	{
		ShellExecute(NULL,_T("open"),m_strLinkAddress,NULL,NULL,SW_SHOWNORMAL);
	}

	CWnd* pParent = GetParent();

	if (pParent != NULL)
	{
		pParent->SendMessage(WM_COMMAND,
			MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	}

	//CStatic::OnLButtonUp(nFlags, point);
}

BOOL CBCGPLinkStatic::OnEraseBkgnd(CDC* pDC)
{

	return TRUE;
}

LRESULT CBCGPLinkStatic::OnBCGSetControlAero(WPARAM, LPARAM)
{
	m_bOnGlass = FALSE;
	return 0;
}


BOOL CBCGPLinkStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hc = 0;
	
	hc = ::LoadCursor(NULL,IDC_HAND);
		::SetCursor(hc);

	return TRUE;
	//return CWnd::OnSetCursor(pWnd,nHitTest,message);
}

void CBCGPLinkStatic::SetHoverFont(CFont *pFont)
{
	m_pHoverFont = pFont;
}

CFont* CBCGPLinkStatic::GetHoverFont()
{
	return m_pHoverFont;
}

void CBCGPLinkStatic::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==1)
	{
		if (::GetCapture()!=m_hWnd)
		{
			if( m_bHaveVisited )
			{
				m_nMouseState = emLinkVisited;
			}
			else
			{
				m_nMouseState = emLinkNormal;
			}
			RedrawWindow();

			KillTimer(1);
		}
		else
		{
			if( m_nMouseState!=emLinkHover )
			{
				m_nMouseState = emLinkHover;
				RedrawWindow();
			}

			ReleaseCapture();
		}
	}
	return CStatic::OnTimer(nIDEvent);
}

void CBCGPLinkStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CWnd::OnLButtonDown(nFlags, point);
}