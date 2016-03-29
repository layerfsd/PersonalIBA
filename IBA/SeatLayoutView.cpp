// CSeatLayoutView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "AddTerminalDlg.h"
#include "Socket\LocalServer.h"
#include "Cashier.h"
#include ".\seatlayoutview.h"
#include "ComputerListView.h"
#include "SeatList2.h"
#include "Dal\IBADAL.h"
#include "BossPwdDlg.h"
#include <algorithm>
#include "Servlet\WinHttpRequest.h"
// CSeatLayoutView

IMPLEMENT_DYNCREATE(CSeatLayoutView, CScrollView)


int CSeatLayoutView::m_nMaxBottom = 0;
int CSeatLayoutView::m_nMaxRight = 0;
CSeatLayoutView::CSeatLayoutView()
: m_bTotalView(FALSE)
,m_pSeatBitmap(NULL)
,m_pSeatList(NULL)
,m_sizeTotal(0, 0)
,m_ptPrePoint(0, 0)
,m_bMoveRect(FALSE)
,m_nIndex(-1),m_nIndex2(-1)
{
	CLocalServer::GetInstance()->SetSeatLayoutView(this);//传递指针，作外部调用
	//2015-0819 liyajun 
	CSize size(0,0); 
	SetScrollSizes(MM_TEXT,size); 

	m_CommandMode = M_SEAT_COMPUTER_MENU;
	//m_pSeatBitmap = new CBitmap;
	m_pSeatBitmap = new CGdiPlusBitmap;
	//m_pSeatTipsBitmap = new CBitmap;
	m_pSeatList = new CSeatList;
	//m_pSeatList->ReadList();	// 2014-1-16-qsc

	m_pSeatList2 = new CSeatList2;
	//m_pSeatList2->ReadList();

	m_sizeTotal.cx = 0;
	m_sizeTotal.cy = 0;
}

CSeatLayoutView::~CSeatLayoutView()
{
	if (m_pSeatBitmap)
	{
		//m_pSeatBitmap->Detach();
		delete m_pSeatBitmap;
		m_pSeatBitmap = NULL;
	}

	/*if (m_pSeatTipsBitmap)
	{
		m_pSeatTipsBitmap->Detach();
		delete m_pSeatTipsBitmap;
		m_pSeatTipsBitmap = NULL;
	}*/
	
	if (m_pSeatList)
	{
		delete m_pSeatList;
		m_pSeatList = NULL;
	}
}

BEGIN_MESSAGE_MAP(CSeatLayoutView, CScrollView)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_KILLFOCUS()

	ON_COMMAND(ID_MENU_SCROLLZOOM, OnMenuScrollzoom)
	ON_UPDATE_COMMAND_UI(ID_MENU_SCROLLZOOM, OnUpdateMenuScrollzoom)
	ON_UPDATE_COMMAND_UI(ID_MENU_FULLZOOM, OnUpdateMenuFullzoom)
	ON_UPDATE_COMMAND_UI(ID_SEAT_ADDSEAT, OnUpdateSeatAddseat)
	ON_UPDATE_COMMAND_UI(ID_SEAT_LOADSEAT, OnUpdateSeatLoadseat)
	ON_UPDATE_COMMAND_UI(IDM_SEAT_DEL, OnUpdateSeatDel)
	ON_UPDATE_COMMAND_UI(IDM_SEAT_REFRESH, OnUpdateSeatRefresh)
	ON_UPDATE_COMMAND_UI(ID_SEAT_ADDALL, OnUpdateSeatAddAll)
	ON_UPDATE_COMMAND_UI(ID_SEAT_DELALL, OnUpdateSeatDelAll)
	ON_UPDATE_COMMAND_UI(ID_SEAT_COMPUTER_MENU, OnUpdateSeatComputerMenu)
	ON_COMMAND(ID_MENU_FULLZOOM, OnMenuFullzoom)
	ON_UPDATE_COMMAND_UI(IDM_CLOCKWISE, OnUpdateClockwise)
	
	ON_COMMAND(ID_SEAT_ADDSEAT, OnSeatAddseat)
	ON_COMMAND(ID_SEAT_LOADSEAT, OnSeatLoadseat)
	ON_COMMAND(IDM_SEAT_DEL, OnSeatDel)
	ON_COMMAND(IDM_SEAT_REFRESH, OnSeatRefresh)
	ON_COMMAND(IDM_CLOCKWISE, OnClockwise)
	ON_COMMAND(ID_SEAT_COMPUTER_MENU, OnSeatComputerMenu)
	ON_COMMAND(ID_SEAT_ADDALL, OnSeatAddAll)
	ON_COMMAND(ID_SEAT_DELALL, OnSeatDelAll)

	ON_WM_TIMER()
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_UPDATESEAT, OnUpdateSeat)
	ON_COMMAND(IDM_ADD_CASHIER, OnAddCashier)
	ON_COMMAND(IDM_ADD_TOILET, OnAddToilet)
	ON_COMMAND(IDM_ADD_REFRIGRAETOR, OnAddRefrigraetor)
	ON_COMMAND(IDM_ADD_ENTRANCE, OnAddEntrance)
	ON_COMMAND(IDM_MYREFRESH, OnMyrefresh)
	ON_UPDATE_COMMAND_UI(IDM_MYREFRESH, OnUpdateMyRefresh)
	ON_UPDATE_COMMAND_UI(IDM_NEWUSER, OnUpdateNewuser)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CSeatLayoutView 绘图

void CSeatLayoutView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	LoadSeatBitmap(m_pSeatBitmap);

	m_ToolTip.Create(this);
	m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	m_ToolTip.SetEffectBk(12);
	m_ToolTip.SetBorder(RGB(200, 200, 200));

	// 2014-1-15 - qsc 刷新一次状态
	SetTimer(1,5 * 1000, NULL);
}

// 把整体显示模式下的坐标转换成滚动模式下的坐标
BOOL CSeatLayoutView::ToScrollRect(CRect& srcFullRect)
{
	if (!m_pSeatBitmap)
	{
		return FALSE;
	}

	/*BITMAP bmInfo;
	m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);*/
	CRect WinRect;
	GetWindowRect(WinRect);
	double Xradios = (m_pSeatBitmap->GetWidth()) * 1.0 / WinRect.Width();
	double Yradios = (m_pSeatBitmap->GetHeight()) * 1.0 / WinRect.Height();

	srcFullRect.top *= Yradios;
	srcFullRect.bottom *= Yradios;
	srcFullRect.left *= Xradios;
	srcFullRect.right *= Xradios;

	return TRUE;
}

// 转换成正常模式下的点坐标
BOOL CSeatLayoutView::ToScrollPoint(CPoint& srcFullPoint)
{
	if (!m_pSeatBitmap)
	{
		return FALSE;
	}
	//////////////////////////先进行平移操作，在进行叉乘///////////////////////
	//srcFullPoint += GetScrollPosition();//无论何种显示模式均有可能出现滚动效果
	/*BITMAP bmInfo;
	m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);*/
	CRect WinRect;
	GetWindowRect(WinRect);

	double Xradios = m_pSeatBitmap->GetWidth() * 1.000 / WinRect.Width();
	double Yradios = m_pSeatBitmap->GetHeight() * 1.000 / WinRect.Height();

	srcFullPoint.y *= Yradios;
	srcFullPoint.x *= Xradios;

	return TRUE;
}

BOOL CSeatLayoutView::ToTotalRect(CRect& srcTotalRect)
{
	if (!m_pSeatBitmap)
	{
		return FALSE;
	}
	
	//BITMAP bmInfo;
	//m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);
	CRect WinRect;
	GetWindowRect(WinRect);
	double Xradios = WinRect.Width() * 1.0 / m_pSeatBitmap->GetWidth();
	double Yradios = WinRect.Height() * 1.0 / m_pSeatBitmap->GetHeight();

	srcTotalRect.top *= Yradios;
	srcTotalRect.bottom *= Yradios;
	srcTotalRect.left *= Xradios;
	srcTotalRect.right *= Xradios;

	return TRUE;
}

// 转换成正常模式下的点坐标
BOOL CSeatLayoutView::ToTotalPoint(CPoint& srcTotalPoint)
{
	if (!m_pSeatBitmap)
	{
		return FALSE;
	}

	//BITMAP bmInfo;
	//m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);
	CRect WinRect;
	GetWindowRect(WinRect);

	double Xradios = WinRect.Width() * 1.0 / m_pSeatBitmap->GetWidth();
	double Yradios = WinRect.Height() * 1.0 / m_pSeatBitmap->GetHeight();

	srcTotalPoint.y *= Yradios;
	srcTotalPoint.x *= Xradios;

	return TRUE;
}

void CSeatLayoutView::OnDraw(CDC* pDC)
{
	//图片载入有误
	if ( !m_pSeatBitmap || !m_pSeatBitmap->IsLoadSuccess() )
	{
		return;
	}
	//内存DC绘图
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);

	CRect rc;
	GetClientRect(&rc);
	//HBITMAP hBitNew = CreateCompatibleBitmap(pDC->m_hDC, m_pSeatBitmap->GetWidth(),
	HBITMAP hBitNew = CreateCompatibleBitmap(pDC->m_hDC, GetMaxRight(),GetMaxBottom());
	HGDIOBJ oldBitmap = dcMemory.SelectObject(hBitNew);
	Gdiplus::Graphics graphics(dcMemory.m_hDC);

	Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 255, 255, 255));
	graphics.FillRectangle(&blackBrush, rc.left, rc.top, GetMaxRight(), GetMaxBottom());

	Gdiplus::RectF grect; grect.X=0, grect.Y=0; grect.Width = m_pSeatBitmap->GetWidth(); grect.Height = m_pSeatBitmap->GetHeight();
	graphics.DrawImage(m_pSeatBitmap->m_pBitmap, grect, 0, 0, m_pSeatBitmap->GetWidth(), m_pSeatBitmap->GetHeight(), Gdiplus::UnitPixel);
	
	if (m_bTotalView)//整体伸缩处理
	{
		CRect rect;
		GetClientRect(rect);

		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMemory, 0, 0, 
			m_pSeatBitmap->GetWidth(), 
			m_pSeatBitmap->GetHeight(), SRCCOPY);

		/*dcMemory.SelectObject(m_pSeatTipsBitmap);
		BITMAP bmInfo;
		m_pSeatTipsBitmap->GetObject(sizeof(bmInfo), &bmInfo);
		pDC->BitBlt(0, 0, bmInfo.bmWidth, 
			bmInfo.bmHeight, &dcMemory, 0, 0, SRCCOPY);*/

		//*******分开处理，调试中发现兼容DC无法刷除正方形，原因未明********/
		//绘制正方形
		double Xratio = rect.Width() / (double)(m_pSeatBitmap->GetWidth());
		double Yratio = rect.Height() / (double)(m_pSeatBitmap->GetHeight());
		DrawSeat(pDC, Xratio, Yratio);
	} 
	else//原样滚动处理
	{
		/*pDC->BitBlt(0, 0, m_pSeatBitmap->GetWidth(), 
			m_pSeatBitmap->GetHeight(), &dcMemory, 0, 0, SRCCOPY);*/

		
		pDC->BitBlt(0, 0,GetMaxRight(), 
			GetMaxBottom(), &dcMemory, 0, 0, 
			SRCCOPY);
		/*dcMemory.SelectObject(m_pSeatTipsBitmap);
		BITMAP bmInfo;
		m_pSeatTipsBitmap->GetObject(sizeof(bmInfo), &bmInfo);
		pDC->BitBlt(0, 0, bmInfo.bmWidth, 
			bmInfo.bmHeight, &dcMemory, 0, 0, SRCCOPY);*/
		//*******分开处理，调试中发现兼容DC无法刷除正方形，原因未明********/
		//绘制正方形
		DrawSeat(pDC);
		
	}
	dcMemory.SelectObject(oldBitmap);
	DeleteObject(hBitNew);
}

// 载入座位图，用成员变量接收对象
void CSeatLayoutView::LoadSeatBitmap(CGdiPlusBitmap * pSeatBitmap)
{
	CString strPath;
	strPath = CIBAConfig::GetInstance()->GetSeatBmpPath();

	//if(strPath.IsEmpty())
	if(strPath == _T(""))
	{
		strPath = theApp.GetWorkPath() + _T("\\IBAConfig\\PLAN.bmp");	// 默认图片路径
	}
	
	//捆绑图像
	//HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//if (hBitmap != NULL)


	if(m_pSeatBitmap->Load(strPath))
	{
		//m_pSeatBitmap->Detach();
		//m_pSeatBitmap->Attach(hBitmap);

		SetViewSize(m_bTotalView);
	}
	else
	{
		//m_pSeatBitmap->Detach();
		SetViewSize(m_bTotalView, TRUE);
	}


	/* 2014-3-31 - qsc
	strPath = theApp.GetWorkPath() + _T("\\IBAConfig\\STATUSTIPS.bmp");
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap != NULL)
	{
		m_pSeatTipsBitmap->Detach();
		m_pSeatTipsBitmap->Attach(hBitmap);

		SetViewSize(m_bTotalView);
	}
	else
	{
		m_pSeatTipsBitmap->Detach();
		SetViewSize(m_bTotalView, TRUE);
	}*/
}

void CSeatLayoutView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	switch (m_CommandMode)
	{
	//case M_SEAT_COMPUTER_MENU:
	//	{
	//		CMenu menu;
	//		menu.LoadMenu(IDR_POPUP_SEATZOOM);

	//		CMenu * pPopupMenu = menu.GetSubMenu(0);
	//		//必须使得弹出菜单属于this当前类才能响应对应的消息
	//		theApp.GetContextMenuManager()->ShowPopupMenu(pPopupMenu->Detach(), point.x, point.y, this, TRUE);
	//		break;
	//	}
	case M_SEAT_ADD:
		{
			CPoint pt = point;
			ScreenToClient(&pt);
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}
			m_nIndex = m_pSeatList->InRect(pt);
			m_nIndex2 = m_pSeatList2->InRect(pt);
			CMenu menu;
			menu.LoadMenu(IDR_POPUP_SEATEDIT);
			CMenu * pPopupMenu = menu.GetSubMenu(0);
			//必须使得弹出菜单属于this当前类才能响应对应的消息
			theApp.GetContextMenuManager()->ShowPopupMenu(pPopupMenu->Detach(), point.x, point.y, this, TRUE);
					
			break;
		}
	case M_SEAT_SAVE:
		{
			break;
		}
	case M_SEAT_LOAD:
		{
			break;
		}
	case M_SEAT_COMPUTER_MENU:
		{
			CPoint pt = point;
			ScreenToClient(&pt);
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}
			
			// 2014-3-13 - qsc不是在终端位置上右键，不弹出菜单
			if(m_pSeatList->InRect(pt) < 0)
				break ;

			// 2014-1-23 - qsc 可以直接右键编辑(非终端)
			m_nIndex2 = m_pSeatList2->InRect(pt);
			if(m_nIndex2 >=0 )
			{
				CMenu menu;
				menu.LoadMenu(IDR_POPUP_SEATEDIT);
				CMenu * pPopupMenu = menu.GetSubMenu(0);
				//必须使得弹出菜单属于this当前类才能响应对应的消息
				theApp.GetContextMenuManager()->ShowPopupMenu(pPopupMenu->Detach(), point.x, point.y, this, TRUE);
				return ;
			}
			

			m_nIndex = m_pSeatList->InRect(pt);

			if (m_nIndex >= 0)
			{//进入元素区域，即移动操作
				CSeat * tSeat = m_pSeatList->GetAt(m_nIndex);
				CComputerInfo info = tSeat->GetComputerInfo();

				CString strTermID = tSeat->GetTerminalID();
				CComputerListView* pComputerView = CLocalServer::GetInstance()->GetComputerListView();
				if(!pComputerView->FindComputer(0,strTermID))
				{
					pComputerView->SetCurSel(-1);
				}
				pComputerView->SetNewUserDefaultTerm(strTermID);
			} 
			else	// 如果没有选中任意一个，则设置为不选中
			{
				CComputerListView* pComputerView = CLocalServer::GetInstance()->GetComputerListView();
				pComputerView->SetCurSel(-1);
			}

			CMenu menu;
			menu.LoadMenu(IDR_COMPUTER_MENU);
			CMenu* pPopupMenu = menu.GetSubMenu(0);

#ifndef __USING_INTER_RESOURCE
			HMENU hTmp = pPopupMenu->GetSubMenu(7)->GetSafeHmenu();
			pPopupMenu->ModifyMenu((UINT_PTR)hTmp,MF_POPUP,(UINT_PTR)hTmp,LOAD_STRING(IDS_REMOTE_OPERATION));

			HMENU hTmp2 = ::GetSubMenu(hTmp, 3);
			CMenu* pPopup2 = ::CMenu::FromHandle(hTmp);
			if (pPopup2)
			{
				pPopup2->ModifyMenu((UINT_PTR)hTmp2,MF_POPUP,(UINT_PTR)hTmp2,LOAD_STRING(IDS_VOLUME_CONTROL));
			}

#endif

			theApp.GetContextMenuManager()->ShowPopupMenu(
				pPopupMenu->Detach(),
				point.x, 
				point.y, 
				CLocalServer::GetInstance()->GetComputerListView(), 
				TRUE);

		}
		break;
	}
}

// 显示区域
void CSeatLayoutView::SetViewSize(BOOL bViewType, BOOL bDefault)
{
	if (!bDefault)
	{
		if (bViewType)//整体显示模式
		{
			CRect rect;
			GetClientRect(rect);
			//滚动区域限制
			m_sizeTotal.cx = rect.Width();
			m_sizeTotal.cy = rect.Height();
		} 
		else//滚动显示模式
		{
			//滚动区域限制
			//BITMAP bmInfo;
			//m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);
			SetMaxRightCmp(m_pSeatBitmap->GetWidth());
			SetMaxBottomCmp(m_pSeatBitmap->GetHeight());
			m_sizeTotal.cx = GetMaxRight();
			m_sizeTotal.cy = GetMaxBottom();
		}
	} 
	else
	{//默认处理
		m_sizeTotal.cx = DefaultBitmapSize;
		m_sizeTotal.cy = DefaultBitmapSize;
	}
	//显示区域限制
	SetScrollSizes(MM_TEXT, m_sizeTotal);
}

// 绘画座位正方形
void CSeatLayoutView::DrawSeat(CDC * pDC, double XRatio, double YRatio)
{
	BOOL bIsCreateDC = FALSE;
	if (!pDC)
	{
		pDC = GetDC();
		bIsCreateDC = TRUE;
	}
	/////////////////////////////////////////
	if (!m_pSeatList)
	{
		if (bIsCreateDC)
		{
			pDC->DeleteDC();
		}
		return;
	}

	m_pSeatList->DrawAllRect(pDC, XRatio, YRatio);
	m_pSeatList2->DrawAllRect(pDC, XRatio, YRatio);
	///////////////////////////////////////////
	if (bIsCreateDC)
	{
		pDC->DeleteDC();
	}
}

void CSeatLayoutView::OnLButtonDown(UINT nFlags, CPoint point)
{
	switch (m_CommandMode)
	{
	case M_SEAT_COMPUTER_MENU:
		{
			// 2014-1-23 - qsc 终端之外的其他设备可以随意移动
			/*m_bMoveRect2 = FALSE;

			CPoint pt = point;
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}
			m_nIndex2 = m_pSeatList2->InRect(pt);
			if(m_nIndex2 >= 0)
			{
				m_ptPrePoint2 = m_pSeatList2->GetAt(m_nIndex2)->GetCenterPos();
			}*/
			break;
		}
	case M_SEAT_ADD:
		{
			//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
			m_bMoveRect = FALSE;
			m_bMoveRect2 = FALSE;

			CPoint pt = point;
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}
			m_nIndex = m_pSeatList->InRect(pt);
			m_nIndex2 = m_pSeatList2->InRect(pt);
			if (m_nIndex >= 0)
			{//进入元素区域，即移动操作
				//SetCapture();
				m_ptPrePoint = m_pSeatList->GetAt(m_nIndex)->GetPosition();
			}
			else
			{
				m_nIndex2 = m_pSeatList2->InRect(pt);
				if(m_nIndex2 >= 0)
				{
					m_ptPrePoint2 = m_pSeatList2->GetAt(m_nIndex2)->GetCenterPos();
				}
			}

			break;
		}
	case M_SEAT_SAVE:
		{
			break;
		}
	case M_SEAT_LOAD:
		{
			break;
		}
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CSeatLayoutView::OnLButtonUp(UINT nFlags, CPoint point)
{
	switch (m_CommandMode)
	{
	case M_SEAT_COMPUTER_MENU:
		{
			//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
			CPoint pt = point;
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}
			m_nIndex = m_pSeatList->InRect(pt);
			if (m_nIndex >= 0)
			{//进入元素区域，即移动操作
				CSeat * tSeat = m_pSeatList->GetAt(m_nIndex);
				CComputerInfo info = tSeat->GetComputerInfo();

				// 剩余上网时长，含包时情况
				CString strRemainTime;
				{
					// 取0点，区域1的扣率，因为一天的扣率都是相等的，区域只有一个
					//if(info.HasUser())	// 2014-3-26 - qsc 离线用户也要判断
					{
						int nCostRate = 0;
						//CTime time = CTime::GetCurrentTime();
						CIBADAL::GetInstance()->GetCostRate(1, 0, info.GetUserClassId(), 
							1, nCostRate);
						LONG lAvaBalance = 0 , lTotalMin = 0,  lHour = 0, lMin = 0;
						lAvaBalance = info.GetAvailavleBalance();
						if(0 != nCostRate)
						{
							lTotalMin = (lAvaBalance * 60) / nCostRate;
						}

						if(0 != lTotalMin)
						{
							lHour = lTotalMin / 60;
							lMin = lTotalMin % 60;
						}

						if (info.GetPayType() != NS_IBACommDef::EPayType_General)
						{
							CActiveMember ActiveMember;
							UINT nMemberId = info.GetMemberID();
							CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);

							UINT nNextCheckinTime = ActiveMember.GetNextCheckinTime();
							CTime currentTime = CTime::GetCurrentTime();
							CTime endBundleTime((time_t)nNextCheckinTime);
							CTimeSpan leftBundleTime(endBundleTime - currentTime);
							LONG lLeftTotalMin = leftBundleTime.GetTotalMinutes();

							lTotalMin += lLeftTotalMin;
							lHour = lTotalMin / 60;
							lMin = lTotalMin % 60;
							strRemainTime.Format(_T("%02ld:%02ld"), lHour, lMin);
						}
						else
						{
							strRemainTime.Format(_T("%02ld:%02ld"), lHour, lMin);
						}
					}		
				}

				CIBAString strToolTip, strTmp, strTmp2;
				if(Euc_PostPaid == info.GetUserClassId())
				{
					strTmp.LoadString(ID_SEAT_TERMINALINFO2);		// 删除剩余上网时长
					strTmp2.Format(_T("<font size=16>%s</font>"), strTmp);		// 加入设置字体大小
					strToolTip.Format(strTmp2, tSeat->GetTerminalID(), \
						tSeat->GetTerminalIP(),tSeat->GetTerminalIP(), \
						info.GetUserName(), info.GetSexAsString(), \
						info.GetMemberType(), info.GetCheckInTimeAsString(),\
						info.GetUseTimeAsString(), info.GetStatusAsString());
				}
				else
				{
					strTmp.LoadString(ID_SEAT_TERMINALINFO);
					strTmp2.Format(_T("<font size=16>%s</font>"), strTmp);		// 加入设置字体大小
					strToolTip.Format(strTmp2, tSeat->GetTerminalID(), \
						tSeat->GetTerminalIP(),tSeat->GetTerminalIP(), \
						info.GetUserName(), info.GetSexAsString(), \
						info.GetMemberType(), info.GetCheckInTimeAsString(),\
						info.GetUseTimeAsString(), info.GetStatusAsString(),
						info.GetAmountAsString(), info.GetAvailavleBalanceAsString(),
						strRemainTime);
				}
				
				
				
				CPoint pt = point;
				ClientToScreen(&pt);
				m_ToolTip.ShowHelpTooltip(&pt, strToolTip);
			} 
			//{
			//	// 2014-1-23 - qsc
			//	if (!m_pSeatList2)
			//	{
			//		return;
			//	} 

			//	CPoint pt = point;
			//	pt = pt + GetScrollPosition();
			//	if (m_bTotalView)
			//	{
			//		ToScrollPoint(pt);
			//	}

			//	if(m_nIndex2 >= 0)
			//	{
			//		if (m_bMoveRect2)
			//		{
			//			m_pSeatList2->GetAt(m_nIndex2)->SetCenterPos(point + GetScrollPosition());
			//			RedrawWindow();
			//			m_bMoveRect2 = FALSE;
			//			m_nIndex2 = -1;//注意此处，由于双击时会发送一次按下，两次弹起消息，故如此
			//			SaveSeat();
			//		}
			//	}
			//}
			break;
		}
	case M_SEAT_ADD:
		{
			if (!m_pSeatList)
			{
				break;
			} 
			
			CPoint pt = point;
			pt = pt + GetScrollPosition();
			if (m_bTotalView)
			{
				ToScrollPoint(pt);
			}

			if (m_nIndex >=0)
			{	
				if (m_bMoveRect)
				{
					//释放焦点
// 					if (GetCapture() == this)
// 					{
// 						ReleaseCapture();
// 					}
					CSeat * tSeat = m_pSeatList->GetAt(m_nIndex);
					CString strTerminalID = tSeat->GetTerminalID();
					CString strTerminalIP = tSeat->GetTerminalIP();
					INT_PTR nDirection = tSeat->GetDirection();

					m_pSeatList->DeleteAt(m_nIndex);
					AddSeat(point, strTerminalID, strTerminalIP, nDirection);

					RedrawWindow();
					m_bMoveRect = FALSE;
					m_nIndex = -1;//注意此处，由于双击时会发送一次按下，两次弹起消息，故如此
					SaveSeat();
				}
			}
			else if(m_nIndex2 >= 0)
			{
				if (m_bMoveRect2)
				{
					m_pSeatList2->GetAt(m_nIndex2)->SetCenterPos(point + GetScrollPosition());
					RedrawWindow();
					m_bMoveRect2 = FALSE;
					m_nIndex2 = -1;//注意此处，由于双击时会发送一次按下，两次弹起消息，故如此
					SaveSeat();
				}
			}
			else
			{
				if(!m_bMoveRect)
				{
					if (m_pSeatList->GetSize() < CLocalServer::GetInstance()->ComputerList.GetCount())
					{
						CDlgAddTerm addDlg;
						addDlg.SetExistSeatList(m_pSeatList);
						if (addDlg.DoModal() == IDOK)
						{
							CString strTerminalID, strTerminalIP;
							strTerminalID = addDlg.GetTerminalID();
							strTerminalIP = addDlg.GetTerminalIP();
							AddSeat(point, strTerminalID, strTerminalIP);
							SaveSeat();
						}
					} 
					else
					{
						CIBAString str;
						str.LoadString(IDS_NOTERMTOADD);
						MessageBox(str, _T("EzLink"));
					}
				}
				m_nIndex = -1;//注意此处，由于双击时会发送一次按下，两次弹起消息，故如此
				m_nIndex2 = -1;
			}

			m_ptPrePoint = CPoint(0, 0);//清理数据，为移动铺基础
			m_ptPrePoint2 = CPoint(0, 0);//清理数据，为移动铺基础
			
			break;
		}
	case M_SEAT_SAVE:
		{
			break;
		}
	case M_SEAT_LOAD:
		{
			break;
		}
	}
//////////////////////////////
	CScrollView::OnLButtonUp(nFlags, point);
}

void CSeatLayoutView::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_CommandMode)
	{
	case M_SEAT_COMPUTER_MENU:
		{
		// 2014-1-23 - qsc
			//if (nFlags != MK_LBUTTON)
			//{
			//	break;
			//}
			//m_bMoveRect2 = TRUE;
			//CDC * pDC = GetDC();
			//if (!pDC)
			//{
			//	break;
			//}
			//else if(m_nIndex2 >=0 )
			//{
			//	if (m_ptPrePoint2 == CPoint(0, 0))
			//	{
			//		break;
			//	}

			//	if (m_pSeatList2->GetSize() <= 0)
			//	{
			//		break;
			//	}

			//	//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
			//	CPoint Curpt = point;
			//	Curpt = Curpt + GetScrollPosition();

			//	INT OldMode = pDC->SetROP2(R2_NOTXORPEN);

			//	CSeat2 * tSeat = m_pSeatList2->GetAt(m_nIndex2);
			//	if (tSeat)
			//	{
			//		if (m_bTotalView)
			//		{
			//			ToScrollPoint(Curpt);

			//			CRect rect(0, 0, 0, 0);
			//			GetWindowRect(rect);

			//			BITMAP bmInfo;
			//			m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);

			//			double Xratio = rect.Width() / (double)(bmInfo.bmWidth);
			//			double Yratio = rect.Height() / (double)(bmInfo.bmHeight);

			//			tSeat->DrawRect(pDC, m_ptPrePoint2 - GetScrollPosition(), Xratio, Yratio);
			//			tSeat->DrawRect(pDC, Curpt - GetScrollPosition(), Xratio, Yratio);
			//		}
			//		else
			//		{
			//			tSeat->DrawRect(pDC, m_ptPrePoint2 - GetScrollPosition());
			//			tSeat->DrawRect(pDC, Curpt - GetScrollPosition());
			//		}
			//	}

			//	pDC->SetROP2(OldMode);

			//	m_ptPrePoint2 = Curpt;
			//}
			//ReleaseDC(pDC);
			break;
		}
	case M_SEAT_ADD:
		{
			if (nFlags != MK_LBUTTON)
			{
				break;
			}

			m_bMoveRect = TRUE;
			m_bMoveRect2 = TRUE;
			CDC * pDC = GetDC();
			if (!pDC)
			{
				break;
			}

			if (m_nIndex >=0 )
			{
				if (m_ptPrePoint == CPoint(0, 0))
				{
					break;
				}

				if (m_pSeatList->GetSize() <= 0)
				{
					break;
				}

				//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
				CPoint Curpt = point;
				Curpt = Curpt + GetScrollPosition();

				INT OldMode = pDC->SetROP2(R2_NOTXORPEN);

				CSeat * tSeat = m_pSeatList->GetAt(m_nIndex);
				if (tSeat)
				{
					if (m_bTotalView)
					{
						ToScrollPoint(Curpt);

						CRect rect(0, 0, 0, 0);
						GetWindowRect(rect);

						/*BITMAP bmInfo;
						m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);*/

						double Xratio = rect.Width() / (double)(m_pSeatBitmap->GetWidth());
						double Yratio = rect.Height() / (double)(m_pSeatBitmap->GetHeight());

						tSeat->DrawRect(pDC, m_ptPrePoint - GetScrollPosition(), Xratio, Yratio, FALSE);
						tSeat->DrawRect(pDC, Curpt - GetScrollPosition(), Xratio, Yratio, FALSE);
					}
					else
					{
						tSeat->DrawRect(pDC, m_ptPrePoint - GetScrollPosition(), 1.0, 1.0, FALSE);
						tSeat->DrawRect(pDC, Curpt - GetScrollPosition(),1.0, 1.0, FALSE);
					}
				}
				
				pDC->SetROP2(OldMode);

				m_ptPrePoint = Curpt;
			}
			else if(m_nIndex2 >=0 )
			{
				if (m_ptPrePoint2 == CPoint(0, 0))
				{
					break;
				}

				if (m_pSeatList2->GetSize() <= 0)
				{
					break;
				}

				//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
				CPoint Curpt = point;
				Curpt = Curpt + GetScrollPosition();

				INT OldMode = pDC->SetROP2(R2_NOTXORPEN);

				CSeat2 * tSeat = m_pSeatList2->GetAt(m_nIndex2);
				if (tSeat)
				{
					if (m_bTotalView)
					{
						ToScrollPoint(Curpt);

						CRect rect(0, 0, 0, 0);
						GetWindowRect(rect);

						/*BITMAP bmInfo;
						m_pSeatBitmap->GetObject(sizeof(bmInfo), &bmInfo);*/

						double Xratio = rect.Width() / (double)(m_pSeatBitmap->GetWidth());
						double Yratio = rect.Height() / (double)(m_pSeatBitmap->GetHeight());

						tSeat->DrawRect(pDC, m_ptPrePoint2 - GetScrollPosition(), Xratio, Yratio);
						tSeat->DrawRect(pDC, Curpt - GetScrollPosition(), Xratio, Yratio);
					}
					else
					{
						tSeat->DrawRect(pDC, m_ptPrePoint2 - GetScrollPosition());
						tSeat->DrawRect(pDC, Curpt - GetScrollPosition());
					}
				}

				pDC->SetROP2(OldMode);

				m_ptPrePoint2 = Curpt;
			}

			ReleaseDC(pDC);
			break;
		}
	case M_SEAT_SAVE:
		{
			break;
		}
	case M_SEAT_LOAD:
		{
			break;
		}
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CSeatLayoutView::OnSeatAddseat()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() == IDOK)
	{
		m_CommandMode = M_SEAT_ADD;
	}
}

void CSeatLayoutView::OnUpdateSeatAddseat(CCmdUI *pCmdUI)
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->SetCheck(m_CommandMode == M_SEAT_ADD);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

//yajun comment:从菜单更新座位图
void CSeatLayoutView::OnSeatLoadseat()  
{
	Loadseat();
	RedrawWindow();
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
	//m_CommandMode = M_SEAT_COMPUTER_MENU;
}

void CSeatLayoutView::Loadseat()
{
	if(m_pSeatList2)
	{
		// 2014-11-6 - qsc 不添加冰柜收银台等图标
		m_pSeatList2->ReadList();
		// 为空才去添加，排除一些异常情况
		if(0 == m_pSeatList2->GetSize())
		{
			CString szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\SeatConfig.ini");
			BOOL bFirstRun = GetPrivateProfileInt(_T("Seat2ConfigFlag"), _T("FirstRun"), 1, szPath);		// 是第一次启动了
			if(bFirstRun)
			{
				for(int i = 0; i < Seat2Count; i ++)
				{
					AddSeat2((NS_IBACommDef::Seat2Type)i);
				}
			}
		}
	}
	if (m_pSeatList)
	{
		m_pSeatList->ReadList();
	}

	// 2014-8-14 - qsc 读取后重新设置下大小
	SetViewSize(m_bTotalView);
}

void CSeatLayoutView::SaveSeat()
{
	if(m_pSeatList)
	{
		m_pSeatList->SaveList();
	}
	if (m_pSeatList2)
	{
		m_pSeatList2->SaveList();
	}
}
void CSeatLayoutView::OnUpdateSeatLoadseat(CCmdUI *pCmdUI)
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->SetCheck(m_CommandMode == M_SEAT_LOAD);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CSeatLayoutView::OnMenuScrollzoom()
{
	m_bTotalView = FALSE;
	SetViewSize(m_bTotalView);

	RedrawWindow();
}

void CSeatLayoutView::OnUpdateMenuScrollzoom(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(!m_bTotalView);
}

void CSeatLayoutView::OnMenuFullzoom()
{
	m_bTotalView = TRUE;
	SetViewSize(m_bTotalView);

	RedrawWindow();
}

void CSeatLayoutView::OnUpdateMenuFullzoom(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bTotalView);
}
// 往数据链添加座位数据
void CSeatLayoutView::AddSeat(CPoint ptCurpoint, CString strTerminalID, CString strTerminalIP, INT_PTR nDirection, BOOL bRedraw, bool bAddScrollPos)
{
	//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩
	CPoint pt = ptCurpoint;
	if(bAddScrollPos)
	{
		pt = pt + GetScrollPosition();
	}
	
	//调整不同模式下的坐标，全部统一成正常模式下坐标
	if (m_bTotalView)
	{
		ToScrollPoint(pt);
	}

	CSeat * tSeat = new CSeat;
	if (tSeat)
	{
		tSeat->SetPosition(pt);
		tSeat->SetRadius(DefaultRadius);//可以设置的，这里暂时的值
		tSeat->SetTerminalID(strTerminalID);
		tSeat->SetTerminalIP(strTerminalIP);
		tSeat->SetDirection(nDirection);//默认情况
		m_pSeatList->Add(tSeat);

	}
	
	if(bRedraw)
	{
		// 2014-1-16 - qsc
		CRect rc;
		tSeat->GetRect(rc);
		rc.bottom += 20;	// 加上终端名高度
		RedrawWindow(&rc);
	}
}

// 往数据链添加座位数据
void CSeatLayoutView::AddSeat2(Seat2Type type)
{
	CPoint point;
	point.x = 10 + DefaultRadius;		// 设定一个起始值
	//point.y = 130;	// 提示栏的高度
	point.y = 10;	// 提示栏的高度


	// 找出没有座位的坐标
	while(-1 != m_pSeatList->InRect(point) ||
		-1 != m_pSeatList2->InRect(point))
	{
		if(0 != m_pSeatList2->GetSize())
		{
			point.x += m_pSeatList2->GetAt(0)->GetRadius() * 3;
			if(point.x > 1100)		// 一行大约1100像素就够了
			{
				point.x = 30;
				point.y += m_pSeatList2->GetAt(0)->GetRadius() * 3;
			}
		}
		else
		{
			point.x = 30;
			point.y += DefaultRadius * 3;
		}
	}

	//调整移动的视窗坐标,必须先做，因为如果压缩，则包括视图移动也压缩

	CSeat2 * tSeat = new CSeat2(type);
	if (tSeat)
	{
		tSeat->SetPosition(point);
		tSeat->SetRadius(DefaultRadius);//可以设置的，这里暂时的值	
		m_pSeatList2->Add(tSeat);

	}
}

BOOL CSeatLayoutView::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CScrollView::PreTranslateMessage(pMsg);
}

void CSeatLayoutView::OnSeatDel()
{
	if (m_nIndex >= 0)
	{
		m_pSeatList->DeleteAt(m_nIndex);
		m_nIndex = -1;
		RedrawWindow();
		SaveSeat();
	}
	if(m_nIndex2 >= 0)
	{
		CDlgBossPwd PwdDlg;
		if (PwdDlg.DoModal() != IDOK)
		{
			return;
		}
		m_pSeatList2->DeleteAt(m_nIndex2);
		m_nIndex2 = -1;
		RedrawWindow();
		SaveSeat();
	}
}

void CSeatLayoutView::OnSeatRefresh()
{
	RedrawWindow();
}

void CSeatLayoutView::OnUpdateSeatDel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nIndex >=0 || m_nIndex2 >= 0);
}

void CSeatLayoutView::OnUpdateSeatRefresh(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void CSeatLayoutView::OnClockwise()
{
	if (m_nIndex >=0)
	{
		CSeat * tSeat = m_pSeatList->GetAt(m_nIndex);
		if (!tSeat)
		{
			return;
		}

		if (tSeat->GetDirection() >= 7)
		{
			tSeat->SetDirection(0);
		}
		else
		{
			tSeat->SetDirection(tSeat->GetDirection() + 1);
		}
		m_nIndex = -1;
		m_nIndex2 = -1;
		RedrawWindow();
	}
}

void CSeatLayoutView::OnUpdateClockwise(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nIndex >=0);
}

void CSeatLayoutView::OnUpdateSeatAddAll(CCmdUI *pCmdUI)
{
	if (m_pSeatList->GetSize() < CLocalServer::GetInstance()->ComputerList.GetCount())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CSeatLayoutView::OnUpdateSeatDelAll(CCmdUI *pCmdUI)
{
	if (0 == m_pSeatList->GetSize())
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}

}

void CSeatLayoutView::OnUpdateSeatComputerMenu(CCmdUI *pCmdUI)
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->SetCheck(m_CommandMode == M_SEAT_COMPUTER_MENU);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

}


void CSeatLayoutView::OnClose()
{
	//m_bTotalView = FALSE;
	//m_ptPrePoint = CPoint(0, 0);
	//m_bMoveRect = FALSE;
	//m_nIndex = -1;
	
	CIBAConfig::GetInstance()->SetDisplayType(0);
	theApp.GetIBAView()->PostMessage(WM_CHILDCLOSE, GetDlgCtrlID(), (LPARAM)GetSafeHwnd());

	//CScrollView::OnClose();
}

void CSeatLayoutView::OnKillFocus(CWnd* pNewWnd)
{
	m_ToolTip.HideTooltip();

	CScrollView::OnKillFocus(pNewWnd);
}

void CSeatLayoutView::OnSeatComputerMenu()
{
	m_CommandMode = M_SEAT_COMPUTER_MENU;
	SaveSeat();
}

void CSeatLayoutView::OnSeatDelAll()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() == IDOK)
	{
		m_pSeatList->DeleteAll();
		RedrawWindow();
		SaveSeat();
	}

	// 2014-8-20 - qsc 删除所有终端后重新设置下范围
	SetViewSize(m_bTotalView);
}

void CSeatLayoutView::OnSeatAddAll()
{
	//m_CommandMode = M_SEAT_COMPUTER_MENU;
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() != IDOK)
	{
		return;
	}
	AutoAddAllSeatAfterSync();

	// 2014-8-20 - qsc 添加所有终端后重新设置下范围
	SetViewSize(m_bTotalView);
}

void CSeatLayoutView::AutoAddAllSeatAfterSync()
{
	CPoint point;
	point.x = 10 + DefaultRadius;		// 设定一个起始值
	point.y = DefaultRadius + 111;	// 根据主收银台的的高度再调整，测出来的,111是第一行图片的高度

	std::vector<TermInfo> vecTermInfo;
	TermInfo termInfoTemp;
	for (INT_PTR ii = 0; ii < CLocalServer::GetInstance()->ComputerList.GetCount(); ii++)
	{
		CComputerInfo info = CLocalServer::GetInstance()->ComputerList.GetComputer(ii);

		CString str  = info.GetTerminalID();
		if (str.IsEmpty())
		{
			continue;
		}

		INT_PTR jj = 0;
		for (jj = 0; jj < m_pSeatList->GetSize(); jj++)
		{
			CSeat * tSeat = m_pSeatList->GetAt(jj);
			if (str == tSeat->GetTerminalID())
			{
				break;
			}
		}

		if(jj >= m_pSeatList->GetSize())
		{
			termInfoTemp.strTermID = info.GetTerminalID();
			termInfoTemp.strIP = info.GetComputerIP();
			vecTermInfo.push_back(termInfoTemp);
			//AddSeat(point, info.GetTerminalID(), info.GetComputerIP());
		}
	}

	// 从小到大排序，要按照终端排序，更改TermInfo的比较操作符函数就行
	// 要改成从大到小排序，reverse一下就行
	std::sort(vecTermInfo.begin(), vecTermInfo.end());	
	for(std::vector<TermInfo>::iterator iter = vecTermInfo.begin();
		iter != vecTermInfo.end(); iter++)
	{
		// 找出没有座位的坐标
		while(-1 != m_pSeatList->InRect(point) ||
			-1 != m_pSeatList2->InRect(point))
		{
			if(0 != m_pSeatList->GetSize())
			{
				point.x += m_pSeatList->GetAt(0)->GetRadius() * 3;
				if(point.x > 1100)		// 一行大约1100像素就够了
				{
					point.x = 30;
					point.y += m_pSeatList->GetAt(0)->GetRadius() * 3;
				}
			}
			else
			{
				point.x = 30;
				point.y += DefaultRadius * 3;
				//TRACE("\n%d,%d", point.x, point.y);
			}
		}
		AddSeat(point, iter->strTermID, iter->strIP, 0, FALSE, false);
	}

	RedrawWindow();		// 全部添加完再刷新
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
}

//2014-7-24 ym	后台更改终端信息并同步后，更新场景视图
void CSeatLayoutView::UpdateSeatAfterSync()
{
	INT_PTR nComputerListCount, nSeatListCount;

	nComputerListCount = CLocalServer::GetInstance()->ComputerList.GetCount();
	nSeatListCount = m_pSeatList->GetSize();

	if (nComputerListCount > nSeatListCount)
	{
		AutoAddAllSeatAfterSync();
		// 2014-8-20 - qsc 重新设置下范围
		SetViewSize(m_bTotalView);
		return;
	}
	else if (nComputerListCount < nSeatListCount)
	{
		for (INT_PTR ii = 0; ii < nSeatListCount; ii++)
		{
			CComputerInfo info;
			CString strComputer;
			CSeat *tSeat = m_pSeatList->GetAt(ii);
			CString strSeat = tSeat->GetTerminalID();

			for (INT_PTR jj = 0; jj < nComputerListCount; jj++)
			{
				info = CLocalServer::GetInstance()->ComputerList.GetComputer(jj);
				strComputer = info.GetTerminalID();
				if (strSeat == strComputer)
				{
					break;
				}
			}

			if (jj >= nComputerListCount)
			{
				m_pSeatList->DeleteAt(ii);
			}
		}
		// 2014-8-20 - qsc 重新设置下范围
		SetViewSize(m_bTotalView);
	}
}

void CSeatLayoutView::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 1:
		//PostMessage(WM_UPDATESEAT, 0, 0);
		Invalidate();
		KillTimer(nIDEvent);
		break;
	}
	CScrollView::OnTimer(nIDEvent);
}

BOOL CSeatLayoutView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CScrollView::OnEraseBkgnd(pDC);
}


LRESULT CSeatLayoutView::OnUpdateSeat(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		if(IsWindowVisible())
		{
			if(m_pSeatList)
			{
				for(int i = 0; i < m_pSeatList->GetSize(); i++)
				{
					m_pSeatList->GetAt(i)->InvalidateIfStatusChange(GetSafeHwnd());
				}
			}

		}
	}
	return 0;
}

void CSeatLayoutView::OnAddCashier()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() != IDOK)
	{
		return;
	}
	AddSeat2(MainIBA);

	RedrawWindow();		// 全部添加完再刷新
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
}

void CSeatLayoutView::OnAddToilet()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() != IDOK)
	{
		return;
	}
	AddSeat2(WC);

	RedrawWindow();		// 全部添加完再刷新
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
}

void CSeatLayoutView::OnAddRefrigraetor()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() != IDOK)
	{
		return;
	}
	AddSeat2(Refrigerator);

	RedrawWindow();		// 全部添加完再刷新
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
}

void CSeatLayoutView::OnAddEntrance()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() != IDOK)
	{
		return;
	}

	AddSeat2(Entrance);

	RedrawWindow();		// 全部添加完再刷新
	// 2014-1-14 -qsc
	OnSeatComputerMenu();
}

void CSeatLayoutView::OnMyrefresh()
{
	// TODO: 在此添加命令处理程序代码
	RedrawWindow();
}

void CSeatLayoutView::OnUpdateMyRefresh(CCmdUI *pCmdUI)
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CSeatLayoutView::OnUpdateNewuser(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL bRet = theApp.GetCurCashier()->IsOnDuty();
	if (bRet)
	{
		//pCmdUI->Enable(-1 == m_nIndex);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CSeatLayoutView::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 自定义的时候不要调用CScrollView::OnPaint();
	CScrollView::OnPaint();
}


void CSeatLayoutView::SetMaxRightCmp( int nVal )
{
	if( nVal > m_nMaxRight)
	{
		SetMaxRight(nVal);
		TRACE("X = %d\n", nVal);
	}

}

void CSeatLayoutView::SetMaxBottomCmp( int nVal )
{
	if( nVal > m_nMaxBottom) 
	{
		SetMaxBottom(nVal);
		TRACE("y = %d\n", nVal);
	}
}

