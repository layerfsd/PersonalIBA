// XPEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "XPEdit.h"


// CXPEdit

IMPLEMENT_DYNAMIC(CXPEdit, CEdit)
CXPEdit::CXPEdit()
{
	m_bIsFocused = FALSE;
	m_bIsMouseOver = FALSE;

	bldo=FALSE;
}

CXPEdit::~CXPEdit()
{
}


BEGIN_MESSAGE_MAP(CXPEdit, CEdit)
//	ON_WM_DRAWITEM()
ON_WM_NCPAINT()
ON_WM_MOUSEMOVE()
ON_WM_SETFOCUS()
ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CXPEdit 消息处理程序



void CXPEdit::OnNcPaint()
{
	// TODO: 在此添加消息处理程序代码

	Default(); 
	DrawEdge();
}

void CXPEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	CEdit::OnMouseMove(nFlags, point);

	if ( GetCapture() != this )
	{
		m_bIsMouseOver = TRUE;
		SetCapture();
		DrawEdge();
	}
	else
	{
		CRect rect;
		GetClientRect( rect );
		
		if ( !rect.PtInRect( point ) )
		{
			m_bIsMouseOver = FALSE;
			DrawEdge();
			ReleaseCapture();
		}
	}
}

void CXPEdit::DrawEdge(void)
{
	if ( !( GetStyle() & WS_BORDER ) && !( GetExStyle() & WS_EX_CLIENTEDGE ) )
		return;

	BOOL bHaveScrollBar = FALSE;
	
	CRect rect;
	GetWindowRect ( rect );
	ScreenToClient( rect );

	CDC *pDC = GetDC ();
	int nSaveDC = pDC->SaveDC();

	COLORREF BorderColor, HotColor;
	
	BorderColor = RGB(244, 112, 28);
	HotColor = pDC->GetPixel( rect.left + 2, rect.top + 2 );
	
	if ( !( GetStyle() & ES_READONLY ) && IsWindowEnabled() ) 
	{
		HotColor = RGB( 255, 255, 255 );

	if (!bldo)
	{		
		if ( m_bIsFocused )
			BorderColor = RGB(244, 157, 28);

		if ( m_bIsMouseOver )
			HotColor = RGB( 253, 157, 80 );
		else if ( (GetStyle() & WS_VSCROLL) || (GetStyle() & WS_HSCROLL) ) 
			bHaveScrollBar = TRUE;
	}


	}

	CPen pen1, pen2, pen3;
	pen1.CreatePen( PS_SOLID, 1, BorderColor );
	pen2.CreatePen( PS_SOLID, 1, HotColor );
	pen3.CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNFACE) );

	pDC->SelectStockObject( NULL_BRUSH );
	pDC->SelectObject( &pen1 );

	pDC->Rectangle( rect );
    rect.DeflateRect( 1, 1 );
	pDC->SelectObject( &pen2 );
	
	if ( !bHaveScrollBar )
		pDC->Rectangle( rect );
	else
	{
		rect.DeflateRect(0,0,1,1);

		pDC->MoveTo( rect.left, rect.bottom );
		pDC->LineTo( rect.left, rect.top );
		pDC->LineTo( rect.right, rect.top );

		if ( GetStyle() & WS_VSCROLL )
			pDC->SelectObject( &pen3 );
		pDC->LineTo( rect.right, rect.bottom );

		pDC->SelectObject( &pen2 );
		if ( GetStyle() & WS_HSCROLL )
			pDC->SelectObject( &pen3 );
		pDC->LineTo( rect.left, rect.bottom );
	}

	pDC->RestoreDC( nSaveDC );
	ReleaseDC ( pDC);
}


void CXPEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	m_bIsFocused = TRUE;
	DrawEdge();
}

void CXPEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: 在此添加消息处理程序代码
	m_bIsFocused = FALSE;
	DrawEdge();
}


