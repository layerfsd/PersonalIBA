#pragma once
#include "G2XWnd.h"
// CG2XSuperWnd

G2XUI_BEGIN

template<class  baseWnd>
class CG2XSuperWnd : public baseWnd
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void CreateGidVirtualWnd(CRect rc);

	virtual void DrawVirualWindows()
	{
		CPaintDC dc(this);
		CRect rc;
		GetClientRect(rc);

		CRect rcClient = rc;
		CDC memDC;
		CBitmap bmp;
		memDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		memDC.SelectObject(&bmp);

		memDC.FillSolidRect(rcClient,GetSysColor(COLOR_3DFACE));

		m_gdiVirualWnd.OnDraw(&memDC);
		dc.BitBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);
	}

public:
	CG2XWnd m_gdiVirualWnd;      // ¶¥²ãÐéÄâ´°¿Ú
};

template<class  baseWnd>
void CG2XSuperWnd<baseWnd>::CreateGidVirtualWnd(CRect rc)
{
	m_gdiVirualWnd.CreateSuperWnd(this, rc, this->GetDlgCtrlID());
}
template<class baseWnd>
BOOL CG2XSuperWnd<baseWnd>::PreTranslateMessage(MSG* pMsg)
{
	if (m_gdiVirualWnd.PreTranslateMessage(pMsg))
	{
		return TRUE;
	}
	return baseWnd::PreTranslateMessage(pMsg);
}

class  CGdiVirtualSysBtnWnd:public CG2XWnd
{
public:
	CGdiVirtualSysBtnWnd(INT nCaptionHeight=30)
	{
		m_nCaptionHeight = nCaptionHeight;
	}
protected:
	INT m_nCaptionHeight;

protected:
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point)
	{
		CRect rcClient = GetClientRect();
		rcClient.bottom = rcClient.top + m_nCaptionHeight;

		if (rcClient.PtInRect(point))
		{
			GetSuperWnd()->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); //·¢ËÍÏûÏ¢
			return TRUE;
		}

		return FALSE;
	}
};

template<class  TDialog>
class  CGdiVirualWndDialog : public TDialog
{
public:
	CGdiVirualWndDialog()
	{
		m_pGdiVirtualWnd = NULL;
		m_bVirtualWndInit = FALSE;
	}
	CGdiVirualWndDialog(LPCTSTR lpszTemplateName, CWnd* pParent = NULL)
		:TDialog(lpszTemplateName, pParent)
	{
		m_pGdiVirtualWnd = NULL;
		m_bVirtualWndInit = FALSE;
	}
	CGdiVirualWndDialog(UINT nIDTemplate, CWnd* pParent=NULL)
		:TDialog(nIDTemplate, pParent)
	{
		m_pGdiVirtualWnd = NULL;
		m_bVirtualWndInit = FALSE;
	}
	virtual ~CGdiVirualWndDialog()
	{
		if (m_pGdiVirtualWnd)
		{
			delete m_pGdiVirtualWnd;
			m_pGdiVirtualWnd = NULL;
		}
	}


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->hwnd == m_pGdiVirtualWnd->GetSuperWnd()->GetSafeHwnd())
		{
			if (!m_pGdiVirtualWnd->PreTranslateMessage(pMsg))
			{
				return TDialog::PreTranslateMessage(pMsg);
			}
			else
			{
				return TRUE;
			}
		}
		return TDialog::PreTranslateMessage(pMsg);
	}

	void CreateGdiVirtualWnd(CRect rc)
	{
		if (m_pGdiVirtualWnd)
		{
			delete m_pGdiVirtualWnd;
			m_pGdiVirtualWnd = NULL;
		}
		m_pGdiVirtualWnd = new CG2XWnd;
		m_pGdiVirtualWnd->CreateSuperWnd(this, rc, this->GetDlgCtrlID());

		m_pGdiVirtualWnd->SetTransparent(TRUE);
	}
	void CreateGdiVirtualWnd(CG2XWnd* pGdiWnd, CRect rc)
	{	
		if (m_pGdiVirtualWnd)
		{
			delete m_pGdiVirtualWnd;
			m_pGdiVirtualWnd = NULL;
		}
	
		ASSERT(pGdiWnd);
		m_pGdiVirtualWnd = pGdiWnd;
		m_pGdiVirtualWnd->CreateSuperWnd(this, rc, this->GetDlgCtrlID());

		m_pGdiVirtualWnd->SetTransparent(TRUE);
	}

	virtual BOOL OnInitDialog()
	{
		BOOL bRes = TDialog::OnInitDialog();
		CRect rc;
		GetClientRect(rc);
		CreateGdiVirtualWnd(rc);
		m_bVirtualWndInit = TRUE;
		return bRes;
	}

	virtual void DrawVirualWindows()
	{
		CPaintDC dc(this);
		CRect rc;
		GetClientRect(rc);

		CRect rcClient = rc;
		CDC memDC;
		CBitmap bmp;
		memDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		memDC.SelectObject(&bmp);

		memDC.FillSolidRect(rcClient,GetSysColor(COLOR_3DFACE));

		Gdiplus::Graphics gh(memDC.GetSafeHdc());
		Gdiplus::Bitmap gBmp(rc.Width(),rc.Height());
		Gdiplus::Graphics ghBmp(&gBmp);

		m_pGdiVirtualWnd->OnDraw(&ghBmp);
		
		gh.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		
		{
			
			CBitmap bmp;
			if (!bmp.LoadBitmap(IDB_BK3))
			{
				return ;
			}
			Gdiplus::Image*pImg = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bmp.GetSafeHandle(),NULL);

			if (pImg)
			{
				gh.DrawImage(pImg,Gdiplus::Rect(rc.left,rc.top,rc.Width(),rc.Height()),
					0,0,pImg->GetWidth(),pImg->GetHeight(),Gdiplus::UnitPixel);
			}
			
			//Gdiplus::TextureBrush brTexture(pImg);
			//gh.FillRectangle(&brTexture,0,0,rc.Width(),rc.Height());

		}
		gh.DrawImage(&gBmp,0,0,0,0,rc.Width(),rc.Height(),Gdiplus::UnitPixel);


		

		dc.BitBlt(0,0,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);
	}

public:
	CG2XWnd* m_pGdiVirtualWnd;  // ¶¥²ãÐéÄâ´°¿Ú

protected:
	BOOL m_bVirtualWndInit;

};

G2X_END
