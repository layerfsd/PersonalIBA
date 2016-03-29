
#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "CreditDlg.h"
#include "ServletHellper.h"
#include "Dal\IBADAL.h"
#include "SeatStatusBar.h"
#include "IBACommDefine.h"
#include ".\seatstatusbar.h"
#include "common\CGdiPlusBitmap.h"

using namespace NS_IBACommDef;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////////////////
// CSeatStatusBar

BEGIN_MESSAGE_MAP(CSeatStatusBar, CBCGPDockingControlBar)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeatStatusBar construction/destruction

CSeatStatusBar::CSeatStatusBar()
	:m_ptrSeatBitmap(new CGdiPlusBitmap)
{
	// TODO: add one-time construction code here
	m_ptrSeatBitmap->Load(theApp.GetWorkPath() + _T("\\IBAConfig\\STATUSTIPS.bmp"));
}

CSeatStatusBar::~CSeatStatusBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSeatStatusBar message handlers
//创建
int CSeatStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableGripper(FALSE);//不显示标题CAPTION
	return 0;
}

void CSeatStatusBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	// 2014-2-14 - qsc 不做任何处理，目的是不让默认的右键菜单弹出
}

void CSeatStatusBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDockingControlBar::OnPaint()
	
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(&dc);
	HBITMAP hBitNew = CreateCompatibleBitmap(dc.m_hDC, m_ptrSeatBitmap->GetWidth(),
		m_ptrSeatBitmap->GetHeight());
	HGDIOBJ hOldBitmap = dcMemory.SelectObject(hBitNew);
	Gdiplus::Graphics graphics(dcMemory.m_hDC);

	Gdiplus::RectF grect; grect.X=0, grect.Y=0; grect.Width = m_ptrSeatBitmap->GetWidth(); grect.Height = m_ptrSeatBitmap->GetHeight();
	graphics.DrawImage(m_ptrSeatBitmap->m_pBitmap, grect, 0, 0, m_ptrSeatBitmap->GetWidth(), m_ptrSeatBitmap->GetHeight(), Gdiplus::UnitPixel);
	dc.BitBlt(0, 0, m_ptrSeatBitmap->GetWidth(), 
		m_ptrSeatBitmap->GetHeight(), &dcMemory, 0, 0, SRCCOPY);

	dcMemory.SelectObject(hOldBitmap);
	DeleteObject(hBitNew);
}

void CSeatStatusBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CBCGPDockingControlBar::OnLButtonDown(nFlags, point);
}

BOOL CSeatStatusBar::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//pDC->SetBkColor(RGB(255, 0, 0));
	//return CBCGPWnd::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, GetSysColor(COLOR_WINDOW));
	return TRUE;
	//return CBCGPDockingControlBar::OnEraseBkgnd(pDC);
}
