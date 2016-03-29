// Seat.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Seat.h"
#include <math.h>
#include ".\seat.h"
#include "IBAHelpper.h"
#include "ActiveMember.h"

#include "IBACommDefine.h"
using namespace NS_IBACommDef;
// CSeat

CBitmap * CSeat::m_StatusBitmap = NULL;
BOOL CSeat::m_bIsBmpAttach = FALSE;
CSeat::CSeat(LPCTSTR szSeatPath)
{
	m_crBlack = 0;
	m_crWhite = RGB(255,255,255);

	m_cpCenter = CPoint(0,0);
	m_nRadius = 0;
	m_nDirection = 0;
	// m_StatusBitmap = NULL;
	//m_bIsBmpAttach = FALSE;
	m_ComputerStatus = 0;
	m_bStatusChange = FALSE;

	if(!m_bIsBmpAttach)
	{
		if (InitImage(szSeatPath))
		{
			m_bIsBmpAttach = TRUE;
		}
		else
		{
			m_bIsBmpAttach = FALSE;
		}
	}
}

CSeat::~CSeat()
{
	//if (m_StatusBitmap)
	//{
	//	//m_StatusBitmap->Detach();
	//	// 2014-1-21 - qsc只Detach没有释放对象会有内存泄露，终端多了很严重
	//	m_StatusBitmap->DeleteObject();
	//	delete m_StatusBitmap;
	//	m_StatusBitmap = NULL;
	//}

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

void CSeat::DrawRect(CDC * pDC, double XRatio, double YRatio, BOOL bDrawTermID)
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

	if(bDrawTermID)
	{
		DrawTermID(pDC, ptLT.x, ptLT.y);
	}
}

void CSeat::DrawRect(CDC * pDC, CPoint Centerpoint, double XRatio, double YRatio, BOOL bDrawTermID)
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

	if(bDrawTermID)
	{
		DrawTermID(pDC, ptLT.x, ptLT.y);
	}
}

// 绑定对应座椅图
BOOL CSeat::InitImage(LPCTSTR szSeatPath)
{
	CString szPath;
	if(NULL == szSeatPath)
	{
		szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\STATUS.BMP");
	}
	else
	{
		szPath = szSeatPath;
	}
	
	
	// 2014-8-19 - qsc 全局只要一份就足够了
	if(NULL == m_StatusBitmap)
	{
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
	return TRUE;

	//HBITMAP bmp;
	//bmp = (HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//if (bmp)
	//{	
	//	m_StatusBitmap = new CBitmap;
	//	m_StatusBitmap->Attach(bmp);
	//	
	//	return TRUE;
	//}
	//else
	//{
	//	return FALSE;
	//}
	
}

void CSeat::DrawBmp(CDC * pDC, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}

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

	DrawTermID(pDC, left, top);
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

	DrawTermID(pDC, left, top);
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
	int nTmp = -1;
	if(CLocalServer::GetInstance()->ComputerList.GetCount() <= 0)
		return FALSE;

	CComputerInfo info = GetComputerInfo();
	CComputerInfo::ECOMPUTERSTATUS status = info.GetComputerStatus();
	
	//// 2014-1-15-qsc
	//// 状态图比国内多了个会员用户在线
	//// 所以会员用户之后的状态值比国内的版本多了1

	// 2015-0831 liyajun 改用国内的新图片
	int nNewStatus = -1;
	switch (status)
	{
	case CComputerInfo::ECS_ONLINE:
		nNewStatus = 4;                 //默认是临时卡？
		if(CNetBarConfig::GetInstance()->GetIsMember( info.GetUserClassId()))	// 判断是否是会员
		{
			nNewStatus = 5;              //VIP（会员）
		}

		//2015-0906 liyajun应该是ComputerInfo.m_nMemberType
		//nTmp = info.GetUserClassId();
		if(3 == CNetBarConfig::GetInstance()->GetUserClassId(info.GetMemberType()))  //判断是不是员工卡
		{
			nNewStatus = 6;
		}

		nTmp = info.GetPayType();
		switch(info.GetPayType())
		{
		case NS_IBACommDef::EPayType_Capped:		// 累计封顶
			nNewStatus = 7;
			break;

		case NS_IBACommDef::EPayType_Withholding:	// 足额预扣
			nNewStatus = 8;
			break;
		
		//case NS_IBACommDef::EPayType_Coupon:			// 配套包时
		//	nNewStatus = 12;
		//	break;

		default:
			break;
		}
		break;

	case CComputerInfo::ECS_OFFLINE:                 //离线
		nNewStatus = 0;
		break;

	case CComputerInfo::ECS_LOCKED:                  //锁屏
		nNewStatus = 1;
		break;

	case CComputerInfo::ECS_SUSPEND:                 //挂机
		nNewStatus = 2;
		break;

	case CComputerInfo::ECS_LITTERMONEY:            //余额不足
		nNewStatus = 3;
		break;

	//case CComputerInfo::                          //普通用户TempUser?
		//nNewStatus = 4;
		//break;

	case CComputerInfo::ECS_VIP:                    //VIP
		nNewStatus = 5;
		break;

	case CComputerInfo::ECS_THIEF:                 // Thief 
		nNewStatus = 9;
		break;

	case CComputerInfo::ECS_CONFLICT:             // Conflict - One kind of Maintenance
		nNewStatus = 10;
		break;

	case CComputerInfo::ECS_UNLOCK:               // Unlock - One kind of Maintenance?
		nNewStatus = 10;
		break;


	//case CComputerInfo::ECS_BLACK:
	//	nNewStatus = 7 + 1;
	//	break;

	//case CComputerInfo::ECS_NORECORD:
	//	nNewStatus = 6 + 1;
	//	break;

	}

	if(nNewStatus != m_ComputerStatus)
	{
		m_bStatusChange = TRUE;
	}
	else
	{
		m_bStatusChange = FALSE;
	}
	m_ComputerStatus = nNewStatus;
	return TRUE;

}

// 跟UpdateComputerStatus做的操作差不多，不过是为了单独保存m_PreComputerStatus的状态
// 所以独立一个函数
BOOL CSeat::UpdateComputerStatus2()
{
	int n = CLocalServer::GetInstance()->ComputerList.GetCount();
	if(CLocalServer::GetInstance()->ComputerList.GetCount() <= 0)
		return FALSE;

	CComputerInfo info = GetComputerInfo();
	CComputerInfo::ECOMPUTERSTATUS status = info.GetComputerStatus();

	//// 2014-1-15-qsc
	//// 状态图比国内多了个会员用户在线
	//// 所以会员用户之后的状态值比国内的版本多了1

	// 2015-0831 liyajun 改用国内的新图片
	int nNewStatus = -1;
	switch (status)
	{
	case CComputerInfo::ECS_ONLINE:
		nNewStatus = 4;                 //默认是临时卡？
		if(CNetBarConfig::GetInstance()->GetIsMember( info.GetUserClassId()))	// 判断是否是会员
		{
			nNewStatus = 5;              //VIP（会员）
		}

		//2015-0906 liyajun应该是ComputerInfo.m_nMemberType
		//nTmp = info.GetUserClassId();
		if(3 == CNetBarConfig::GetInstance()->GetUserClassId(info.GetMemberType()))  //判断是不是员工卡
		{
			nNewStatus = 6;
		}

		//nTmp = info.GetPayType();
		switch(info.GetPayType())
		{
		case NS_IBACommDef::EPayType_Capped:		// 累计封顶
			nNewStatus = 7;
			break;

		case NS_IBACommDef::EPayType_Withholding:	// 足额预扣
			nNewStatus = 8;
			break;

			//case NS_IBACommDef::EPayType_Coupon:			// 配套包时
			//	nNewStatus = 12;
			//	break;

		default:
			break;
		}
		break;

	case CComputerInfo::ECS_OFFLINE:                 //离线
		nNewStatus = 0;
		break;

	case CComputerInfo::ECS_LOCKED:                  //锁屏
		nNewStatus = 1;
		break;

	case CComputerInfo::ECS_SUSPEND:                 //挂机
		nNewStatus = 2;
		break;

	case CComputerInfo::ECS_LITTERMONEY:            //余额不足
		nNewStatus = 3;
		break;

		//case CComputerInfo::                          //普通用户TempUser?
		//nNewStatus = 4;
		//break;

	case CComputerInfo::ECS_VIP:                    //VIP
		nNewStatus = 5;
		break;

	case CComputerInfo::ECS_THIEF:                 // Thief 
		nNewStatus = 9;
		break;

	case CComputerInfo::ECS_CONFLICT:             // Conflict - One kind of Maintenance
		nNewStatus = 10;
		break;

	case CComputerInfo::ECS_UNLOCK:               // Unlock - One kind of Maintenance?
		nNewStatus = 10;
		break;


		//case CComputerInfo::ECS_BLACK:
		//	nNewStatus = 7 + 1;
		//	break;

		//case CComputerInfo::ECS_NORECORD:
		//	nNewStatus = 6 + 1;
		//	break;

	}

	if(nNewStatus != m_PreComputerStatus)
	{
		m_bStatusChange = TRUE;
	}
	else
	{
		m_bStatusChange = FALSE;
	}
	m_PreComputerStatus = nNewStatus;
	return TRUE;

}
void CSeat::DrawTermID(CDC* pDC, int x, int y)
{
	// 2013-11-26 11:27:05-qsc画上终端号
	//y += GetRadius() * 2 - 3;
	//2015-9-1 liyajun
	y += GetRadius() * 2 -1;
	CRect rc(x, y, x + GetRadius() * 2, y + 18);
	CFont font;
	CIBAHelpper::CreateFont(font, 12);
	HGDIOBJ oldFont = pDC->SelectObject((HGDIOBJ)font);
	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldClr = pDC->SetTextColor(RGB(20, 20, 20));
	pDC->DrawText(GetTerminalID(), rc, DT_CENTER);
	pDC->SetBkMode(nOldMode);
	pDC->SetTextColor(oldClr);
}

BOOL CSeat::IsStatusChange()
{
	return m_bStatusChange;
}

// 更新了返回TRUE
BOOL CSeat::InvalidateIfStatusChange(HWND hWnd)
{
	UpdateComputerStatus2();
	if(IsStatusChange())
	{
		CRect rc;
		GetRect(rc);
		InvalidateRect(hWnd, rc, TRUE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

