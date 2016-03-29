// LogListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include "MainFrm.h"
#include "ShortMsgDlg.h"
#include "Socket\LocalServer.h"
#include "ServletHellper.h"
#include ".\loglistctrl.h"

// COutputListCtrl

IMPLEMENT_DYNAMIC(COutputListCtrl, CBCGPListCtrl)
COutputListCtrl::COutputListCtrl()
{
}

COutputListCtrl::~COutputListCtrl()
{
}


BEGIN_MESSAGE_MAP(COutputListCtrl, CBCGPListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// COutputListCtrl 消息处理程序

void COutputListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	int TabIndex = ((CMainFrame *)AfxGetMainWnd())->m_wndOutput.m_wndTabs.GetActiveTab();
	switch (TabIndex)
	{
	case 0://操作
		{
			INT nIdx = GetSelectionMark();
			if (nIdx >= 0 && CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
			{//退款时弹出消费查询
				if (CServletHellper::GetMemberId(GetItemText(nIdx, 3)) > 0)
				{
					CNDCreditDlg dlg;
					dlg.SetNetId(GetItemText(nIdx, 3));
					dlg.DoModal();
				} 
				else
				{
					theApp.GetIBAView()->QueryUserComsumeList(GetItemText(nIdx, 3), _ttoi(GetItemText(nIdx, 5)));
				}
			}
			break;
		}
	case 1://激活
		{
			INT index = GetSelectionMark();
			if (index >= 0 && CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
			{
				if (CServletHellper::GetMemberId(GetItemText(index, 2)) > 0)
				{
					CNDCreditDlg dlg;
					dlg.SetNetId(GetItemText(index, 2));
					dlg.DoModal();
				} 
				else
				{
					theApp.GetIBAView()->QueryUserComsumeList(GetItemText(index, 2));
				}
			}
			break;
		}
	case 3://客户端消息
		{
			INT nIdx = GetSelectionMark();

			if (nIdx >= 0)
			{
				CString strTermId;
				strTermId = GetItemText(nIdx, 1);

				//UINT rrr = (UINT)CLocalServer::GetInstance()->ComputerList.LookFor(0, strTermId);
				CShortMsgDlg MsgDlg;

				MsgDlg.AddReceiver((UINT)CLocalServer::GetInstance()->ComputerList.LookFor(0, strTermId));
				MsgDlg.DoModal();
			}
			break;
		}
	case 2://消费记录
		{
			//INT nIdx = GetSelectionMark();
			//if (nIdx >= 0)
			//{//退款时弹出消费查询
			//	theApp.GetIBAView()->QueryUserComsumeList(GetItemText(nIdx, 4), 0);
			//}
			break;
		}
	}

	*pResult = 0;
}

int COutputListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return 0;
}


void COutputListCtrl::OnTimer(UINT nIDEvent)
{
	static int iCount = 0;
	if (nIDEvent==200)
	{
		iCount++;
		if (iCount%4==0)
		{
			UpdateWindow();
			Invalidate();
		}
		else
		{
			CRect rc;
			GetClientRect(rc);

			rc.DeflateRect(2,2,2,2);
			CClientDC dc(this);
			CBrush br;
			br.CreateSolidBrush(RGB(255,0,0));
			dc.FrameRect(rc,&br);
			rc.DeflateRect(1,1,1,1);
			dc.FrameRect(rc,&br);
		}
		if (iCount/4 == 2)
		{
			KillTimer(200);
			iCount = 0;
		}
	}

	//CBCGPListCtrl::OnTimer(nIDEvent);
}

void COutputListCtrl::ActiveRemind()
{
	SetTimer(200,200,0);
}