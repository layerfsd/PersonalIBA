#include "StdAfx.h"
#include "WxMsgTips.h"
#include "resource.h"
#include "Dal/RightsManagement.h"
#include "CreditDlg.h"

using namespace NS_DAL;


//////////////////////////////////////////////////////////////////////////
// class CSpecStatic

IMPLEMENT_DYNAMIC(CSpecStatic, CStatic)
CSpecStatic::CSpecStatic()
{
	m_hCursor = ::LoadCursor(NULL, IDC_HAND);
}

BEGIN_MESSAGE_MAP(CSpecStatic, CStatic)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CSpecStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!m_hCursor)
		m_hCursor = ::LoadCursor(NULL, IDC_HAND);

	::SetCursor(m_hCursor);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// class CWxMsgTips

IMPLEMENT_DYNAMIC(CWxMsgTips, CWnd)
CWxMsgTips::CWxMsgTips()
{
	m_bCreated = FALSE;

	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&m_ScreenRect, 0); 

	m_WndRect.SetRect(0, 0, 0, 0);
	m_nShowTimes = 5;
	m_nCurTimes = 0;

	m_bMouseTrack = TRUE;
	m_bMouseIn = FALSE;

	m_strUserID = _T("");
	m_strPayAmount = _T("");
	m_strGiveAmount = _T("");
	m_strTermID = _T("");
	m_strTime = _T("");
}

CWxMsgTips::~CWxMsgTips()
{
	m_FntText.DeleteObject();
	m_FntDetailText.DeleteObject();
}

BEGIN_MESSAGE_MAP(CWxMsgTips, CWnd)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &CWxMsgTips::OnMouseLeave)
	ON_BN_CLICKED(IDC_TIPS_CLOSEBTN, &CWxMsgTips::OnBtnCloseClicked)
	ON_BN_CLICKED(IDC_WXMSGTIPS_5, &CWxMsgTips::OnBtnDetailClicked)
END_MESSAGE_MAP()

BOOL CWxMsgTips::Create()
{
	if (m_bCreated)
	{
		KillTimer(TimerIDOfPerSec);
		m_nCurTimes = 0;
		return TRUE;
	}

	LPCTSTR szClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	if (!Create(szClassName, _T("WxMsgTips"), WS_VISIBLE, CRect(0, 0, 0, 0), NULL, NULL, NULL))
	{
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	m_bCreated = TRUE;

	return TRUE;
}

void CWxMsgTips::SetUserID(LPCTSTR lpszID)
{
	m_strUserID = lpszID;
}

void CWxMsgTips::SetPayAmount(LPCTSTR lpszAmount)
{
	m_strPayAmount = lpszAmount;
}

void CWxMsgTips::SetGiveAmount(LPCTSTR lpszAmount)
{
	m_strGiveAmount = lpszAmount;
}

void CWxMsgTips::SetTermID(LPCTSTR lpszTermID)
{
	m_strTermID = lpszTermID;
}

void CWxMsgTips::SetTime(LPCTSTR lpszTime)
{
	m_strTime = lpszTime;
}

void CWxMsgTips::ShowRBCorner()
{
	if (!m_bCreated)
		return;

	int nScreenY = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&CWnd::wndTopMost, 
		m_ScreenRect.right - m_WndRect.Width() - 10, 
		m_ScreenRect.bottom - m_WndRect.Height(), 
		m_WndRect.Width(), m_WndRect.Height(), 0);

	ConstructorInfoText();
	
	ShowWindow(SW_SHOWNORMAL);

	SetTimer(TimerIDOfPerSec, TimerIDOfPerSecInterval, NULL);
}

void CWxMsgTips::InitResource()
{
	m_BtnClose.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 0, 0), this, IDC_TIPS_CLOSEBTN);
	m_StcTitle.Create(NULL, WS_CHILD | WS_VISIBLE , CRect(0, 0, 0, 0), this, IDC_TIPS_TITLESTC);

	m_FntText.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		0,						   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));			   // lpszFacename

	m_FntDetailText.CreateFont(14, 0, 0, 0, 0, FALSE, TRUE, 0, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));

	m_StcTitle.SetFont(&m_FntText, TRUE);
	m_StcTitle.SetWindowText(_T("支付信息"));
	m_BtnClose.SetToolTipText(_T("关闭"));
	m_BtnClose.LoadBitmap((HINSTANCE)GetModuleHandle(NULL), IDB_BMP_WXMSGCLOSE, 2);

	m_ImgBack.LoadFromOtherResource(NULL, IDR_PNG_WXMSGBK, _T("PNG"));

	m_WndRect.SetRect(0, 0, m_ImgBack.GetWidth(), m_ImgBack.GetHeight());

	// 创建文字显示控件
	int nUpperEdge = 35;
	int nDownEdge = 5;
	int nLeftEdge = 10;
	int nRightEdge = 5;
	int nTextItemWidth = m_WndRect.Width() - nLeftEdge - nRightEdge;
	int nTextItemHeight = (m_WndRect.Height() - TextItemSpace * (TextItemCount-1) - nUpperEdge - nDownEdge) / TextItemCount;

	int nTop = 0;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	for (int nID = IDC_WXMSGTIPS_1, nIndex = 0;
		nID <= IDC_WXMSGTIPS_5; 
		++nID, ++nIndex)
	{
		nTop = nUpperEdge + nIndex * (nTextItemHeight + TextItemSpace);
		
		if (nID != IDC_WXMSGTIPS_5)
		{
			m_aTexts[nIndex].Create(
				NULL, 
				dwStyle, 
				CRect(nLeftEdge, nTop, nLeftEdge + nTextItemWidth, nTop + nTextItemHeight),
				this,
				nID);

			m_aTexts[nIndex].SetFont(&m_FntText, TRUE);
		}
		else
		{
			m_StcDetail.Create(
				NULL, 
				dwStyle | SS_NOTIFY, 
				CRect(nLeftEdge + nTextItemWidth - 60, nTop, nLeftEdge + nTextItemWidth, nTop + nTextItemHeight), 
				this, 
				nID);

			m_StcDetail.SetFont(&m_FntDetailText, TRUE);
		}
	}
}

void CWxMsgTips::AdjustPosition()
{
	//CRgn MyRgn;
	//MoveWindow(&m_WndRect, TRUE);
	//MyRgn.CreateRoundRectRgn(0, 0, m_WndRect.right, m_WndRect.bottom, 16, 16);
	//SetWindowRgn(MyRgn, TRUE);

	CImageEx imgClose;
	imgClose.LoadFromResource((HINSTANCE)GetModuleHandle(NULL), IDB_BMP_WXMSGCLOSE);

	int nWidth = imgClose.GetWidth() / 2;
	m_BtnClose.MoveWindow(m_WndRect.right - nWidth, 0, nWidth, imgClose.GetHeight(), TRUE);
	
	m_StcTitle.MoveWindow(40, 8, 160, 52);
}

BOOL CWxMsgTips::IsMouseIn()
{
	CRect windowRect;
	GetWindowRect(&windowRect);	

	CPoint mousePoint;
	GetCursorPos(&mousePoint);

	if (windowRect.PtInRect(mousePoint))
		return TRUE;

	return FALSE;
}

void CWxMsgTips::ConstructorInfoText()
{
	CString strText(_T("微信支付:"));
	m_aTexts[0].SetWindowText(strText);

	strText.Format(_T("账号: %s"), m_strUserID);
	m_aTexts[1].SetWindowText(strText);

	int nPayAmount = _ttoi(m_strPayAmount);
	int nGiveAmount = _ttoi(m_strGiveAmount);
	float fPayAmount = (float)nPayAmount / 100.0f;
	float fGiveAmount = (float)nGiveAmount / 100.0f;
	strText.Format(_T("支付金额: %.2f 元  赠送 %.2f 元"), fPayAmount, fGiveAmount);
	m_aTexts[2].SetWindowText(strText);

	strText.Format(_T("机器号: %s  时间:%s"), m_strTermID, m_strTime);
	m_aTexts[3].SetWindowText(strText);

	strText = _T("点击详情");
	m_StcDetail.SetWindowText(strText);

	Invalidate();
}

BOOL CWxMsgTips::Create(
						LPCTSTR lpszClassName, 
						LPCTSTR lpszWindowName, 
						DWORD dwStyle, 
						const RECT& rect, 
						CWnd* pParentWnd, 
						UINT nID, 
						CCreateContext* pContext /* = NULL */)
{
	if (!CreateEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, lpszClassName, NULL, WS_POPUP,
		100, 100, 150, 50, NULL, NULL, NULL))
	{
		return FALSE;
	}

	InitResource();

	AdjustPosition();

	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | 0x80000);

	return TRUE;
}

void CWxMsgTips::PostNcDestroy()
{
	delete this;
}

void CWxMsgTips::OnTimer(UINT nIDEvent)
{
	if (TimerIDOfPerSec == nIDEvent)
	{
		++m_nCurTimes;
		if (m_nCurTimes >= m_nShowTimes)
		{
			// 这里不能直接用 IsMouseIn 判断
			// 因为在进行拖动时会返回 FALSE
			// 从而调用 OnBtnCloseClicked 而移动已销毁的窗口崩溃
			if (m_bMouseIn)
			{
				m_nCurTimes = m_nShowTimes;
			}
			else
			{
				OnBtnCloseClicked();
			}
		}
	}
}

BOOL CWxMsgTips::OnEraseBkgnd(CDC* pDC)
{
	if (!m_ImgBack.IsNull())
	{
		m_ImgBack.Draw(pDC->GetSafeHdc(), 0, 0, m_WndRect.Width(), m_WndRect.Height());

		return TRUE;
	}

	return CWnd::OnEraseBkgnd(pDC);
}

HBRUSH CWxMsgTips::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (pWnd == &m_StcTitle)
			pDC->SetTextColor(RGB(255, 255, 255));
		else if (pWnd == &m_StcDetail)
			pDC->SetTextColor(RGB(0, 0, 255));

		pDC->SetBkMode(TRANSPARENT);   //设置背景透明

		return HBRUSH(GetStockObject(HOLLOW_BRUSH));
	}

	return hbr;
}

void CWxMsgTips::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	if (bShow)
		SetLayeredWindowAttributes(RGB(255,255,255), 255, LWA_ALPHA);
}

void CWxMsgTips::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	m_bMouseIn = TRUE;

	if (m_bMouseTrack)
	{
		TRACKMOUSEEVENT mouseevent;	 // if we want to receive
		mouseevent.cbSize = sizeof(TRACKMOUSEEVENT);// WM_MOUSELEAVE message,
		mouseevent.dwFlags = TME_LEAVE;// | TME_HOVER;	 // TrackMouseEvent() must
		mouseevent.hwndTrack = m_hWnd;	 // be called.
		mouseevent.dwHoverTime = 1;
		m_bMouseTrack = _TrackMouseEvent(&mouseevent);
	}
}


LRESULT CWxMsgTips::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseTrack = TRUE;

	// 这里 MouseEnter 时也会接收到消息
	// 所以再判断下
	if (!IsMouseIn())
	{
		m_bMouseIn = FALSE;
	}

	return 0;
}

void CWxMsgTips::OnBtnCloseClicked()
{
	KillTimer(TimerIDOfPerSec);

	m_nCurTimes = 0;
	
	ShowWindow(SW_HIDE);
	DestroyWindow();
}

void CWxMsgTips::OnBtnDetailClicked()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_CREDIT())
	{
		CCreditDlg dlg;
		dlg.SetNetId(m_strUserID);
		dlg.DoModal();
	} 
}