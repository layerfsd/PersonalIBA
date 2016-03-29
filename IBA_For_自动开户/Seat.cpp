// Seat.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Seat.h"
#include <math.h>
#include ".\seat.h"


// CSeat

CSeat::CSeat()
{
	m_crBlack = 0;
	m_crWhite = RGB(255,255,255);

	m_cpCenter = CPoint(0,0);
	m_nRadius = 0;
	m_nDirection = 0;
	m_StatusBitmap = NULL;
	m_bIsBmpAttach = FALSE;
	m_ComputerStatus = 0;

	if (InitImage())
	{
		m_bIsBmpAttach = TRUE;
	}
	else
	{
		m_bIsBmpAttach = FALSE;
	}
	
}

CSeat::~CSeat()
{
	if (m_StatusBitmap)
	{
		m_StatusBitmap->Detach();
		delete m_StatusBitmap;
		m_StatusBitmap = NULL;
	}

}

// CSeat 成员函数
CComputerInfo& CSeat::GetComputerInfo()
{
	INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_strTerminalID, m_strTerminalIP);
	ASSERT(nIndex >=0);
	return CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
}

void CSeat::GetRect(CRect& denRect)
{
	CRect tRect(CPoint(0, 0), CPoint(0, 0));
	tRect.left  = GetPosition().x - GetRadius();
	tRect.top = GetPosition().y - GetRadius();
	tRect.right = GetPosition().x + GetRadius();
	tRect.bottom = GetPosition().y + GetRadius();
	tRect.NormalizeRect();
	denRect = tRect;
}

void CSeat::DrawRect(CDC * pDC, double XRatio, double YRatio)
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
	
}

void CSeat::DrawRect(CDC * pDC, CPoint Centerpoint, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	CRect tRect(0, 0, 0, 0);
	tRect.top = Centerpoint.y - GetRadius();
	tRect.bottom = Centerpoint.y + GetRadius();
	tRect.left  = Centerpoint.x - GetRadius();
	tRect.right = Centerpoint.x + GetRadius();
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

}

// 绑定对应座椅图
BOOL CSeat::InitImage(void)
{
	CString szPath;
	szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\STATUS.BMP");

	HBITMAP bmp;
	bmp = (HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (bmp)
	{	
		m_StatusBitmap = new CBitmap;
		m_StatusBitmap->Attach(bmp);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void CSeat::DrawBmp(CDC * pDC, double XRatio, double YRatio)
{
	INT_PTR radius = GetRadius();

	INT_PTR left = 0, top = 0, nWidth = 0, nHeight = 0;
	left = GetPosition().x * XRatio - radius;
	top = GetPosition().y * YRatio - radius;
	nWidth = radius * 2;
	nHeight = radius * 2;

	UpdateComputerStatus();
	INT_PTR posx = 0, posy = 0;
	posx = radius * 2 * m_nDirection;
	posy = radius * 2 * m_ComputerStatus;

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
}

void CSeat::DrawBmp(CDC * pDC, CPoint ptCenter, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

	INT_PTR radius = GetRadius();

	INT_PTR left = 0, top = 0, nWidth = 0, nHeight = 0;
	left = ptCenter.x * XRatio - radius;
	top = ptCenter.y * YRatio - radius;
	nWidth = radius * 2;
	nHeight = radius * 2;

	UpdateComputerStatus();
	INT_PTR posx = 0, posy = 0;
	posx = radius * 2 * m_nDirection;
	posy = radius * 2 * m_ComputerStatus;

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

}

void CSeat::Draw(CDC * pDC, double XRatio, double YRatio)
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

void CSeat::Draw(CDC * pDC, CPoint ptCenter, double XRatio, double YRatio)
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

BOOL CSeat::UpdateComputerStatus()
{
	CComputerInfo info = GetComputerInfo();
	CComputerInfo::ECOMPUTERSTATUS status = info.GetComputerStatus();

	switch (status)
	{
	case CComputerInfo::ECS_ONLINE:
		m_ComputerStatus = 2;
		break;

	case CComputerInfo::ECS_OFFLINE:
		m_ComputerStatus = 0;
		break;

	case CComputerInfo::ECS_LOCKED:
		m_ComputerStatus = 1;
		break;

	case CComputerInfo::ECS_LITTERMONEY:
		m_ComputerStatus = 4;
		break;

	case CComputerInfo::ECS_THIEF:
		m_ComputerStatus = 7;
		break;

	case CComputerInfo::ECS_SUSPEND:
		m_ComputerStatus = 5;
		break;

	case CComputerInfo::ECS_BIRTHDAY:
		m_ComputerStatus = 3;
		break;

	case CComputerInfo::ECS_VIP:
		m_ComputerStatus = 8;
		break;

	case CComputerInfo::ECS_BLACK:
		m_ComputerStatus = 7;
		break;

	case CComputerInfo::ECS_NORECORD:
		m_ComputerStatus = 6;
		break;

	case CComputerInfo::ECS_CONFLICT:
		m_ComputerStatus = 3;
		break;

	case CComputerInfo::ECS_UNLOCK:
		m_ComputerStatus = 3;
		break;
	}

	return TRUE;

}