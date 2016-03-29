// Seat.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Seat2.h"
#include <math.h>
#include "IBAHelpper.h"
#include "..\IBARes\resource.h"


// CSeat2

CSeat2::CSeat2(Seat2Type type)
	:m_nBmpWidth(100)
	,m_nBmpHeight(100)
{
	m_crBlack = 0;
	m_crWhite = RGB(255,255,255);

	m_cpCenter = CPoint(0,0);
	m_cpLT = CPoint(0,0);
	m_nRadius = 0;
	m_StatusBitmap = NULL;
	m_bIsBmpAttach = FALSE;

	m_Seat2Type = type;
	if (InitImage())
	{
		m_bIsBmpAttach = TRUE;
	}
	else
	{
		m_bIsBmpAttach = FALSE;
	}
	
}

CSeat2::~CSeat2()
{
	if (m_StatusBitmap)
	{
		//m_StatusBitmap->Detach();
		// 2014-1-21 - qsc只Detach没有释放对象会有内存泄露
		m_StatusBitmap->DeleteObject();
		delete m_StatusBitmap;
		m_StatusBitmap = NULL;
	}

}

void CSeat2::GetRect(CRect& denRect)
{
	CRect tRect(CPoint(0, 0), CPoint(0, 0));
	/*tRect.left  = GetPosition().x - GetRadius();
	tRect.top = GetPosition().y - GetRadius();
	tRect.right = GetPosition().x + GetRadius();
	tRect.bottom = GetPosition().y + GetRadius();*/
	/*tRect.top = GetPosition().y - m_nBmpHeight / 2;
	tRect.bottom = GetPosition().y + m_nBmpHeight / 2;
	tRect.left  = GetPosition().x - m_nBmpWidth / 2;
	tRect.right = GetPosition().x + m_nBmpWidth / 2;*/
	
	tRect.left  = GetPosition().x;
	tRect.top = GetPosition().y;
	tRect.right = GetPosition().x + m_nBmpWidth;
	tRect.bottom = GetPosition().y + m_nBmpHeight;
	tRect.NormalizeRect();
	denRect = tRect;
}

void CSeat2::DrawRect(CDC * pDC, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	CRect tRect(0, 0, 0, 0);
	GetRect(tRect);
	CPoint ptLT(tRect.left * XRatio, tRect.top * YRatio);
	CPoint ptBD(tRect.right * XRatio, tRect.bottom * YRatio);
	CRect retDraw(ptLT, ptBD);
	retDraw.NormalizeRect();

	CBrush Newbrush;
	Newbrush.CreateSolidBrush(RGB(0, 0, 0));
	CBrush * pOldBrush = pDC->SelectObject(&Newbrush);

	CPen NewPen;
	NewPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen * pOldPen = pDC->SelectObject(&NewPen);

	pDC->Rectangle(retDraw);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	Newbrush.DeleteObject();
	NewPen.DeleteObject();

	/*if(bDrawSeatName)
	{
		DrawSeatName(pDC, ptLT.x, ptLT.y);
	}*/
}

void CSeat2::DrawRect(CDC * pDC, CPoint Centerpoint, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	CRect tRect(0, 0, 0, 0);
	/*tRect.top = Centerpoint.y - GetRadius();
	tRect.bottom = Centerpoint.y + GetRadius();
	tRect.left  = Centerpoint.x - GetRadius();
	tRect.right = Centerpoint.x + GetRadius();*/
	/*tRect.top = Centerpoint.y - m_nBmpHeight / 2;
	tRect.bottom = Centerpoint.y + m_nBmpHeight / 2;
	tRect.left  = Centerpoint.x - m_nBmpWidth / 2;
	tRect.right = Centerpoint.x + m_nBmpWidth / 2;*/
	tRect.top = Centerpoint.y - m_nBmpHeight / 2;
	tRect.bottom = Centerpoint.y + m_nBmpHeight / 2;
	tRect.left  = Centerpoint.x - m_nBmpWidth / 2;
	tRect.right = Centerpoint.x + m_nBmpWidth / 2;
	tRect.NormalizeRect();

	CPoint ptLT(tRect.left * XRatio, tRect.top * YRatio);
	CPoint ptBD(tRect.right * XRatio, tRect.bottom * YRatio);
	CRect retDraw(ptLT, ptBD);
	retDraw.NormalizeRect();

	CBrush Newbrush;
	Newbrush.CreateSolidBrush(RGB(0, 0, 0));
	CBrush * pOldBrush = pDC->SelectObject(&Newbrush);

	CPen NewPen;
	NewPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen * pOldPen = pDC->SelectObject(&NewPen);

	pDC->Rectangle(retDraw);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	Newbrush.DeleteObject();
	NewPen.DeleteObject();

	/*if(bDrawSeatName)
	{
		DrawSeatName(pDC, ptLT.x, ptLT.y);
	}*/
}

// 绑定对应座椅图
BOOL CSeat2::InitImage()
{
	HBITMAP bmp;
	CString szPath;
	switch(m_Seat2Type)
	{
	case MainIBA:
		//szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\MainIBA.bmp");
		bmp = (HBITMAP)::LoadBitmap(theApp.GetResHandle(), MAKEINTRESOURCE(IDB_MAINIBA));
		break;
	case WC:
		//szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\WC.bmp");
		bmp = (HBITMAP)::LoadBitmap(theApp.GetResHandle(), MAKEINTRESOURCE(IDB_WC));
		break;

	case Entrance:
		//szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\Entrance.bmp");
		bmp = (HBITMAP)::LoadBitmap(theApp.GetResHandle(), MAKEINTRESOURCE(IDB_ENTERANCE));
		break;

	case Refrigerator:
		//szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\Refrigerator.bmp");
		bmp = (HBITMAP)::LoadBitmap(theApp.GetResHandle(), MAKEINTRESOURCE(IDB_REFRIGERATOR));
		break;
	default:
		break;
	}

	
	if (bmp)
	{	
		m_StatusBitmap = new CBitmap;
		m_StatusBitmap->Attach(bmp);
		BITMAP bmpInfo;
		GetObject(bmp, sizeof(BITMAP), &bmpInfo);
		m_nBmpHeight = bmpInfo.bmHeight;
		m_nBmpWidth	= bmpInfo.bmWidth;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void CSeat2::DrawBmp(CDC * pDC, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	INT_PTR radius = GetRadius();

	INT_PTR left = 0, top = 0, nWidth = 0, nHeight = 0;
	/*left = GetPosition().x * XRatio - radius;
	top = GetPosition().y * YRatio - radius;*/
	left = GetPosition().x * XRatio;
	top = GetPosition().y * YRatio;
	/*nWidth = radius * 2;
	nHeight = radius * 2;*/
	nWidth = m_nBmpWidth;
	nHeight = m_nBmpHeight;

	INT_PTR posx = 0, posy = 0;

	COLORREF crColour = RGB(255, 255, 255);

	///////////////////////////////////////////////////////////
	COLORREF crOldBack = pDC->SetBkColor(m_crWhite);
	COLORREF crOldText = pDC->SetTextColor(m_crBlack);
	CDC dcImage, dcTrans;

	// 创建兼容DC
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// 选择
	CBitmap* pOldBitmapImage = dcImage.SelectObject(m_StatusBitmap);

	// 标记图创建
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// 选入
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// 建立背景
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, posx, posy, SRCCOPY);

	// 贴图
	pDC->BitBlt(left, top, nWidth, nHeight, &dcImage, posx, posy, SRCINVERT);
	pDC->BitBlt(left, top, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(left, top, nWidth, nHeight, &dcImage, posx, posy, SRCINVERT);

	// 还原
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);

	DrawSeatName(pDC, left, top);
}

void CSeat2::DrawBmp(CDC * pDC, CPoint ptCenter, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	INT_PTR radius = GetRadius();

	INT_PTR left = 0, top = 0, nWidth = 0, nHeight = 0;
	/*left = ptCenter.x * XRatio - radius;
	top = ptCenter.y * YRatio - radius;*/
	left = ptCenter.x * XRatio;
	top = ptCenter.y * YRatio;
	/*nWidth = radius * 2;
	nHeight = radius * 2;*/
	nWidth = m_nBmpWidth;
	nHeight = m_nBmpHeight;

	INT_PTR posx = 0, posy = 0;

	COLORREF crColour = RGB(255, 255, 255);

	///////////////////////////////////////////////////////////
	COLORREF crOldBack = pDC->SetBkColor(m_crWhite);
	COLORREF crOldText = pDC->SetTextColor(m_crBlack);
	CDC dcImage, dcTrans;

	// 创建兼容DC
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// 选择
	CBitmap* pOldBitmapImage = dcImage.SelectObject(m_StatusBitmap);

	// 标记图创建
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// 选入
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// 建立背景
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, posx, posy, SRCCOPY);

	// 贴图
	pDC->BitBlt(left, top, nWidth, nHeight, &dcImage, posx, posy, SRCINVERT);
	pDC->BitBlt(left, top, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(left, top, nWidth, nHeight, &dcImage, posx, posy, SRCINVERT);

	// 还原
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);

	DrawSeatName(pDC, left, top);
}

void CSeat2::Draw(CDC * pDC, double XRatio, double YRatio)
{
	if (m_bIsBmpAttach)
	{
		DrawBmp(pDC, XRatio, YRatio);
	} 
	else
	{
		DrawRect(pDC, XRatio, YRatio);
	}
}

void CSeat2::Draw(CDC * pDC, CPoint ptCenter, double XRatio, double YRatio)
{
	if (m_bIsBmpAttach)
	{
		DrawBmp(pDC, ptCenter, XRatio, YRatio);
	} 
	else
	{
		DrawRect(pDC, ptCenter, XRatio, YRatio);
	}
}

void CSeat2::DrawSeatName(CDC* pDC, int x, int y)
{
	// 2013-11-26 11:27:05-qsc画上座位名
	/*y += GetRadius() * 2 - 3;
	x += 3;
	CFont font;
	CIBAHelpper::CreateFont(font, 12);
	HGDIOBJ oldFont = pDC->SelectObject((HGDIOBJ)font);
	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldClr = pDC->SetTextColor(RGB(20, 20, 20));
	pDC->TextOut(x, y, m_strSeatName);
	pDC->SetBkMode(nOldMode);
	pDC->SetTextColor(oldClr);*/
}

CPoint CSeat2::GetPosition() const
{
	return m_cpLT;
}

CPoint CSeat2::GetCenterPos() const
{
	return m_cpCenter;
}

void CSeat2::SetCenterPos(CPoint newPosition)
{
	m_cpCenter = newPosition;
	m_cpLT.x = newPosition.x - m_nBmpWidth / 2;
	m_cpLT.y = newPosition.y - m_nBmpHeight / 2;
}

void CSeat2::SetPosition( CPoint newPosition )
{
	m_cpLT = newPosition;
	m_cpCenter.x = newPosition.x + m_nBmpWidth / 2;
	m_cpCenter.y = newPosition.y + m_nBmpHeight / 2;
}
