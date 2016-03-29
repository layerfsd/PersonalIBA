// HoverButton.cpp : implementation file
//

#include "stdafx.h"
#include "ImageEx.h"
#include "HoverButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverButton

CHoverButton::CHoverButton()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_uHoverIndex = 2;
	m_uSelectedIndex = 1;
}

CHoverButton::~CHoverButton()
{
}

IMPLEMENT_DYNAMIC(CHoverButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButton, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
//	CHoverButton message handlers

void CHoverButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
		ActivateTooltip();
	}

	CButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButton::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButton::SetToolTipText(int nId, BOOL bActivate)
{
	CString sText;

	// load string resource
	sText.LoadString(nId);

	// If string resource is not empty
	if (!sText.IsEmpty())
		SetToolTipText(sText, bActivate);
}

// Set the tooltip with a CString
void CHoverButton::SetToolTipText(LPCTSTR lpszText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (lpszText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, lpszText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText(lpszText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CHoverButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

// Activate the tooltip
void CHoverButton::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0)
		return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip


void CHoverButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* mydc = CDC::FromHandle(lpDrawItemStruct->hDC);

	CDC* pMemDC = new CDC;
	pMemDC->CreateCompatibleDC(mydc);

	CBitmap* pOldBitmap = pMemDC->SelectObject(&m_Bitmap);

	CPoint point(0, 0);	

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		mydc->BitBlt(0, 0, m_ButtonSize.cx, m_ButtonSize.cy, pMemDC,
				m_ButtonSize.cx * m_uSelectedIndex, 0, SRCCOPY);
	}
	else
	{
		if (m_bHover)
		{
			mydc->BitBlt(0, 0, m_ButtonSize.cx, m_ButtonSize.cy, pMemDC,
					m_ButtonSize.cx * m_uHoverIndex, 0, SRCCOPY);
		}
		else
		{
			mydc->BitBlt(0, 0, m_ButtonSize.cx, m_ButtonSize.cy, pMemDC, 0, 0, SRCCOPY);
		}
	}

	// clean up
	pMemDC->SelectObject(pOldBitmap);
	
	delete pMemDC;
}

// Load a bitmap from the resources in the button, the bitmap has to have 3 buttonsstates next to each other: Up/Down/Hover
BOOL CHoverButton::LoadBitmap(UINT bitmapid, UINT bmpCount)
{
	DeleteObject(m_Bitmap.Detach());
	
	HGDIOBJ hImage = LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(bitmapid), IMAGE_BITMAP, 0, 0,LR_LOADMAP3DCOLORS);

	if (m_Bitmap.Attach(hImage))
	{
		InitButtonData(bmpCount);

		return TRUE;
	}
	
	return FALSE;
}

BOOL CHoverButton::LoadPng(UINT pngid, UINT bmpCount /*= 3*/)
{
	DeleteObject(m_Bitmap.Detach());
	
	CImageEx MyImage;

	if (MyImage.LoadFromOtherResource(AfxGetResourceHandle(), pngid, _T("PNG")))
	{
		m_Bitmap.Attach(MyImage.Detach());

		InitButtonData(bmpCount);

		return TRUE;
	}

	return FALSE;
}

LRESULT CHoverButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHover = TRUE;
	Invalidate();
	ActivateTooltip();
	return 0;
}


LRESULT CHoverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover = FALSE;
	Invalidate();
	ActivateTooltip(FALSE);

	return 0;
}

BOOL CHoverButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_HAND);

	::SetCursor(hCursor);

	return TRUE;
}

void CHoverButton::InitButtonData(UINT bmpCount)
{
	switch (bmpCount)
	{
	case 1:
		m_uHoverIndex = 0;
		m_uSelectedIndex = 0;
		break;
	case 2:
		m_uHoverIndex = 1;
		m_uSelectedIndex = 1;
		break;
	default:
		bmpCount = 3;
		m_uHoverIndex = 2;
		m_uSelectedIndex = 1;
	}

	BITMAP bitmapbits = {0};
	m_Bitmap.GetBitmap(&bitmapbits);
	m_ButtonSize.cy = bitmapbits.bmHeight;
	m_ButtonSize.cx = bitmapbits.bmWidth / bmpCount;
	SetWindowPos(NULL, 0, 0, m_ButtonSize.cx, m_ButtonSize.cy, SWP_NOMOVE | SWP_NOOWNERZORDER);
	SetWindowText(_T(""));
}
