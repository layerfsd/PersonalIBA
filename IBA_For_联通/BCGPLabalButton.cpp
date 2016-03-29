#include "stdafx.h"
#include ".\bcgplabalbutton.h"

BEGIN_MESSAGE_MAP(CBCGPLabelButton, CBCGPButton)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CBCGPLabelButton::CBCGPLabelButton(void)
: m_pLastActiveWnd(NULL)
{

}

CBCGPLabelButton::~CBCGPLabelButton(void)
{
}

void CBCGPLabelButton::OnDrawFocusRect(CDC* pDC, const CRect& rectClient)
{
	
}

void CBCGPLabelButton::OpenURL()
{
	OnClicked();
}


void CBCGPLabelButton::OnSetFocus(CWnd* pOldWnd)
{
	CBCGPURLLinkButton::OnSetFocus(pOldWnd);

	m_pLastActiveWnd = pOldWnd;
}

void CBCGPLabelButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CBCGPURLLinkButton::OnLButtonUp(nFlags, point);

	if (m_pLastActiveWnd != NULL)
	{
		m_pLastActiveWnd->SetFocus();
	}

	CWnd* pParent = GetParent();
	
	if (pParent != NULL)
	{
		pParent->PostMessage(WM_COMMAND,
			MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	}
}
