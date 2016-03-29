// DlgIBAFastlane.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgIBAFastlane.h"
#include ".\dlgibafastlane.h"
#include "MainFrm.h"
#include ".\RealName\IBARealName.h"
#include "ServletHellper.h"
#include "MemberInfoDlg.h"
#include "IBAView.h"
#include "Socket\LocalServer.h"
#include "NDReturnDlg.h"
#include "IBAHelpper.h"

static CIBAString StringZengshoubao ;

// CDlgIBAFastlane 对话框

IMPLEMENT_DYNAMIC(CDlgIBAFastlane, CBCGPDialog)
CDlgIBAFastlane::CDlgIBAFastlane(CWnd* pParent /*=NULL*/)
	: CStringLoaderDlg<CBCGPDialog>(CDlgIBAFastlane::IDD, pParent)
{
	m_bBugleActive = FALSE; 
	m_nTurnCount = 0;
	m_nIBAStatus = 0;
}

CDlgIBAFastlane::~CDlgIBAFastlane()
{
}

void CDlgIBAFastlane::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONTENT, m_stcContent);
	DDX_Control(pDX, IDC_COMBO_FAST, m_combFast);
	DDX_Control(pDX, IDC_STATIC_1, m_static1);
	DDX_Control(pDX, IDC_STATIC_2, m_static2);
	DDX_Control(pDX, IDC_PIC_STATUS, m_stcPicStatus);
}


BEGIN_MESSAGE_MAP(CDlgIBAFastlane, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_CBN_SELENDOK(IDC_COMBO_FAST, OnCbnSelendokComboFast)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_FAST, OnCbnSelchangeComboFast)
	ON_CBN_SELENDCANCEL(IDC_COMBO_FAST, OnCbnSelendcancelComboFast)
	ON_MESSAGE(WM_UPDATEROLLTEXT, OnUpdateRollText)
	ON_MESSAGE(WM_UPDATE_IBA_STATUS, OnUpdateIBAStatus)
	ON_STN_CLICKED(IDC_PIC_STATUS, OnStnClickedPicStatus)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CDlgIBAFastlane 消息处理程序

BOOL CDlgIBAFastlane::OnInitDialog()
{
	//SetStringID(IDC_STATIC_2, IDS_ZSB_GUANGBO);
	//SetStringID(IDC_STATIC_1, IDS_BJTD);
	CStringLoaderDlg<CBCGPDialog>::OnInitDialog();
	StringZengshoubao.LoadString(IDS_ZSB_GOOD);

	GetDlgItem(IDC_PIC_BUGLE)->ShowWindow(SW_HIDE);

	m_static1.SetFont(&globalData.fontRegular);
	m_static2.SetFont(&globalData.fontRegular);

	m_stcContent.SetText(StringZengshoubao,_T('|'));
	m_stcContent.SetFont(&globalData.fontRegular);
	m_stcContent.StartRolling(5000);
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_1));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_2));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_3));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_4));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_5));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_6));
	m_combFast.AddString(LOAD_STRING(IDS_BJTD_7));

	/*m_combFast.AddString(L"上网账号(长度大于等于6)              (进入充值退款界面)");
	m_combFast.AddString(L"上网账号(长度等于4或5)                (选择所要账号后进入充值退款界面)");
	m_combFast.AddString(L"终端号(长度小于4或以字母开始)  (在机器列表中选中所查找的终端)");
	m_combFast.AddString(L"\\终端号                                             (在机器列表中选中所查找的终端)");
	m_combFast.AddString(L"\\ \\IP                                                    (在机器列表中选中根据IP所查找到的终端)");
	m_combFast.AddString(L"?上网账号                                         (在机器列表中选中根据上网账号所查找到的终端)");
	m_combFast.AddString(L"+上网账号 金额                               (给指定的上网账号充值，金额单位为元)");*/
	StartBugleTurn(TRUE);
	
	m_nIBAStatus = CIBAGlobal::NetworkStatus;

	m_imgStatus.LoadFromOtherResource(AfxGetInstanceHandle(),IDR_PNG_STATE,_T("PNG"));

	m_ToolTip.Create(this);
	m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	m_ToolTip.SetEffectBk(12);
	m_ToolTip.SetBorder(RGB(200, 200, 200));

	return TRUE;  
}

BOOL CDlgIBAFastlane::OnEraseBkgnd(CDC* pDC)
{
	CRect rcRect;
	GetClientRect( rcRect );

	CDC memDC;  // Paint的内存DC
	CBitmap bmp;

	bmp.CreateCompatibleBitmap( pDC,rcRect.Width(),rcRect.Height() );
	memDC.CreateCompatibleDC(NULL);
	memDC.SelectObject(&bmp);

	CBCGPToolBar* pParent = (CBCGPToolBar*)GetParent();
	if( pParent )
	{
		
		WINDOWPLACEMENT wp;
		this->GetWindowPlacement(&wp);

		CRect rcParent;
		pParent->GetClientRect(rcParent);

		// 绘制背景
		CDC memParentDC;
		CBitmap bmpParent;
		bmpParent.CreateCompatibleBitmap(pDC,rcParent.Width(),rcParent.Height());
		memParentDC.CreateCompatibleDC(NULL);
		memParentDC.SelectObject(&bmpParent);

		CDC* pParentDC = pParent->GetDC();
		CBCGPVisualManager::GetInstance ()->OnFillBarBackground (&memParentDC, pParent,
			rcParent,rcParent);
		pParent->ReleaseDC(pParentDC);

		CRect rt = wp.rcNormalPosition;

		memDC.BitBlt(0,0,rcRect.Width(),rcRect.Height(),&memParentDC,wp.rcNormalPosition.left,wp.rcNormalPosition.top,
			SRCCOPY);

		pDC->BitBlt(0,0,rt.Width(),rt.Height(),&memDC,0,0,SRCCOPY);
	}

	CBitmap bmpImg;
	BOOL bRes = 0;
	if (m_bBugleActive)
	{
		bRes = bmpImg.LoadBitmap(IDB_BUGLE2);
	}
	else
	{
		bRes = bmpImg.LoadBitmap(IDB_BUGLE1);
	}
	if (bRes)
	{
		BITMAP bt;
		bmpImg.GetBitmap(&bt);

		CStatic *pStc = (CStatic*)GetDlgItem(IDC_PIC_BUGLE);

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		memDC.SelectObject(&bmpImg);

		CRect rc;
		pStc->GetClientRect(rc);
		WINDOWPLACEMENT wp;
		pStc->GetWindowPlacement(&wp);

		CRect rcClient;
		GetClientRect(rcClient);

		rc.left = wp.rcNormalPosition.left;
		rc.top = rcClient.top + (rcClient.Height()-bt.bmHeight)/2;
		rc.right = rc.left + bt.bmWidth;
		rc.bottom = rc.top + bt.bmHeight;

		pDC->TransparentBlt(rc.left,rc.top,rc.Width(),rc.Height(),&memDC,0,0,rc.Width(),rc.Height(),RGB(255,255,255));
	}
	else
	{
		IBA_ASSERT2(0,"喇叭image 问题");
	}

	{
		WINDOWPLACEMENT pm;
		m_stcPicStatus.GetWindowPlacement(&pm);

		int nWidth = 85;
		int nHeight = 21;
		int nStatus = m_nIBAStatus;

		int nX = pm.rcNormalPosition.left;
		int nY = pm.rcNormalPosition.top;
		int nImgX = nStatus * nWidth;
		int nImgY = 0;
		
		BOOL bRes = m_imgStatus.Draw(pDC->GetSafeHdc(), nX, nY, nWidth, nHeight, nImgX, nImgY, nWidth, nHeight);
		pDC->SetBkMode(TRANSPARENT);
		CFont* pOldFont = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);
		CStringArray strArr;
		CIBAHelpper::SplitLine(LOAD_STRING(IDS_IBA_NET_STATUS),strArr);
		CRect rcTest(0,0,0,0);
		CString strText = strArr.GetAt(nStatus);
		pDC->DrawText(strText,rcTest,DT_CALCRECT);
		CRect rcTxt;
		rcTxt.left = nX;
		rcTxt.right = nX + nWidth;
		rcTxt.top = nY + (nHeight - rcTest.Height())/2;
		rcTxt.bottom = rcTxt.top + rcTest.Height();
		pDC->DrawText(strText,rcTxt,DT_CENTER);
		bRes = bRes;
		pDC->SelectObject(pOldFont);
	}
	//return CBCGPDialog::OnEraseBkgnd(pDC);
	return TRUE;
}

HBRUSH CDlgIBAFastlane::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CDlgIBAFastlane::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if ( IsWindow(m_stcContent.m_hWnd ) )
	{
		CRect rc;
		GetClientRect(rc);
		
		WINDOWPLACEMENT wp;
		m_stcPicStatus.GetWindowPlacement(&wp);
		
		CRect rcOld = wp.rcNormalPosition;
		CRect rcNew = rcOld;
		rcNew.right = rc.right - 3;
		rcNew.left = rcNew.right - rcOld.Width();
		m_stcPicStatus.MoveWindow(rcNew);
		
		GetDlgItem(IDC_PIC_BUGLE)->GetWindowPlacement(&wp);
		rcNew.right = rcNew.left - 5;
		rcNew.left = wp.rcNormalPosition.right + 3;
		
		m_stcContent.GetWindowPlacement(&wp);
		rcNew.top = wp.rcNormalPosition.top;
		rcNew.bottom = wp.rcNormalPosition.bottom;

		m_stcContent.MoveWindow(rcNew,TRUE);
	}
	
}

BOOL CDlgIBAFastlane::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN )
		{
			CWnd *pWnd = GetFocus();
			if( pWnd && pWnd->GetParent()->GetSafeHwnd()== m_combFast.GetSafeHwnd())
			{
				OnCbnSelendokComboFast();
				return TRUE;
			}
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgIBAFastlane::OnCbnSelendokComboFast()
{
	CString strTmp;
	m_combFast.GetWindowText(strTmp);// 取通道中的子串
	m_combFast.SetWindowText(_T(""));

	strTmp.Trim();

	if (strTmp.GetLength() < 2) //至少两个字符
	{
		return;
	}

	// 非数字和字母结尾的输入，都认为是无效的
	CHAR tcLast = strTmp.GetAt(strTmp.GetLength()-1);
	
	if ( !( (_T('0')<=tcLast&&tcLast<=_T('9')) || 
		(_T('a')<=tcLast&&tcLast<=_T('z')) || 
		(_T('A')<=tcLast&&tcLast<=_T('Z')) ) 
		)
	{
		return;
	}

	BOOL bComputerView = theApp.GetIBAView()->IsCurrentComputerView();

	switch (strTmp.GetAt(0))
	{
	case '\\'://查找终端
		{
			if (strTmp.GetAt(1) == '\\')// 根据IP查找
			{
				//theApp.GetIBAView()->FindComputer(2, strTmp.Mid(2).Trim());
				theApp.GetIBAView()->GotoComputerByIP(strTmp.Mid(2).Trim(), bComputerView);
			}
			else //查终端
			{
				theApp.GetIBAView()->GotoActiveMemberEx(strTmp.Mid(1).Trim());
				theApp.GetIBAView()->GotoComputer(strTmp.Mid(1).Trim(), bComputerView);
			}
		}

		break;

	case '?'://根据账号查找终端
		{
			theApp.GetIBAView()->GotoComputerByNetID(strTmp.Mid(1).Trim());
			CDlgMemberInfo MemberInfoDlg;
			MemberInfoDlg.SetQueryStyle(1);
			MemberInfoDlg.SetNetId(strTmp.Mid(1).Trim());

			if (MemberInfoDlg.DoModal() == IDOK)
			{
				m_combFast.SetFocus(); //这个是必须的，否则可能会出问题

				CString strNetID = MemberInfoDlg.GetNetId();

				if (!strNetID.IsEmpty())
				{
					theApp.GetIBAView()->GotoActiveMember(strNetID,!bComputerView);
				}
			}
		}
		
		break;

	case '+'://根据账号 金额充值
		{
			CCreditHelpper CreditHelpper;
			CreditHelpper.Execute(strTmp.Mid(1));
			m_combFast.SetFocus();
		}
		break;

	default:
		{
			int nInputLen = strTmp.GetLength();

			if ( nInputLen >=6 ) // 认为输入的用户账号
			{
				//CDlgMemberInfo MemberInfoDlg;
				//MemberInfoDlg.SetQueryStyle(1);
				//MemberInfoDlg.SetNetId(strTmp);

				//if (MemberInfoDlg.DoModal() == IDOK)
				//{
				//	m_combFast.SetFocus(); //这个是必须的，否则可能会出问题

				//	strTmp = MemberInfoDlg.GetNetId();

					if (CNetBarConfig::GetInstance()->GetInputActivation()) //允许输入激活
					{
						CActiveMemberView *pMemberView = CLocalServer::GetInstance()->GetActiveMemberView();
						pMemberView->SetSelUser(pMemberView->FindUserAt(0, strTmp), TRUE);

						theApp.GetRealName()->OnRealNameMsg(99, &strTmp, strTmp.GetLength());
						return;
					}
					// 如果是机器视图，就不用激活用户视图
					theApp.GetIBAView()->GotoActiveMember(strTmp, !bComputerView);
				//}
			}
			else if ( nInputLen <=3 ) // 按终端号来查找
			{
				CString strTermID = strTmp;
				theApp.GetIBAView()->GotoActiveMemberEx(strTmp, &strTermID);
				theApp.GetIBAView()->GotoComputer(strTermID, bComputerView);
			}
			else
			{
				// 输入的是账号

				BOOL bNext = TRUE;

				//{ 2011/08/01-8201-gxx: 应急模式，本地用户的判断

				if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
				{
					CActiveMember ActiveMember;
					if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strTmp,ActiveMember,FALSE))
					{
						CNDReturnDlg dlgReturn;
						dlgReturn.SetNetId(ActiveMember.GetNetId());
						dlgReturn.DoModal();
						return;
					}
				}
				else
				{
					CActiveMember ActiveMember;
					if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strTmp,ActiveMember,FALSE))
					{
						if (ActiveMember.GetIsLocalUser())
						{
							CNDReturnDlg dlgReturn;
							dlgReturn.SetNetId(ActiveMember.GetNetId());
							dlgReturn.DoModal();
							return;
						}
					}
				}
				
				//}

				CDlgMemberInfo MemberInfoDlg;
				MemberInfoDlg.SetQueryStyle(1);
				MemberInfoDlg.SetNetId(strTmp);

				if (MemberInfoDlg.DoModal() == IDOK)
				{
					m_combFast.SetFocus(); //这个是必须的，否则可能会出问题

					CString strNetID = MemberInfoDlg.GetNetId();

					if (!strNetID.IsEmpty())
					{
						bNext = !theApp.GetIBAView()->GotoActiveMember(strNetID,!bComputerView);
						if (!bNext)
						{
							strTmp = strNetID;

						    // 机器视图也做一下定位
							theApp.GetIBAView()->GotoComputerByNetID(strTmp, bComputerView);
						}
					}
				}
				else
				{
					//如果没有则设置焦点到快捷栏，不然(如果之前有选择某个终端的话)会弹出充值对话框
				}

				m_combFast.SetFocus();

				if (bNext)
				{
					// 终端号定位上机的用户
					CString strTermID = strTmp;
					theApp.GetIBAView()->GotoActiveMemberEx(strTmp, &strTermID);	
					theApp.GetIBAView()->GotoComputer(strTmp, bComputerView);
				}
			}
			break;
		}

	}

}

void CDlgIBAFastlane::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==EVENT_TIME_BUGLE)
	{
		UpdateBugle();
		m_bBugleActive = !m_bBugleActive;

		if ( (--m_nTurnCount) <= 0 )
		{
			StopBugleTurn(); // 停止喇叭交替
		}
	}

	CBCGPDialog::OnTimer(nIDEvent);
}

void CDlgIBAFastlane::UpdateBugle()
{
	CWnd *pBugle = GetDlgItem(IDC_PIC_BUGLE);

	WINDOWPLACEMENT wp;
	pBugle->GetWindowPlacement(&wp);

	InvalidateRect(&wp.rcNormalPosition);
}

void CDlgIBAFastlane::StartBugleTurn(BOOL bStartActive/*=TRUE*/)
{
	m_nTurnCount = COUNT_TURN;
	m_bBugleActive = bStartActive;
	SetTimer(EVENT_TIME_BUGLE,TIME_BUGLE,0);
}

void CDlgIBAFastlane::StopBugleTurn(BOOL bLastActive/*=FALSE*/)
{
	m_nTurnCount = 0;
	m_bBugleActive = bLastActive;
	UpdateBugle();
	KillTimer(EVENT_TIME_BUGLE);
}

void CDlgIBAFastlane::OnCbnSelchangeComboFast()
{
	m_combFast.SetWindowText(_T(""));  // 清空显示文字空
}
void CDlgIBAFastlane::OnCbnSelendcancelComboFast()
{
	m_combFast.SetWindowText(_T(""));
}

LRESULT CDlgIBAFastlane::OnUpdateRollText(WPARAM wParam, LPARAM lParam)
{
	CDlgIBAFastlane* pDlg = this;
	pDlg->m_stcContent.StopRolling();
	pDlg->m_stcContent.ClearAll();
	
	CString strs = LPCTSTR(wParam);
	if (!strs.IsEmpty())
	{
		pDlg->m_stcContent.SetText(strs,_T('|'));
	}
	else
	{
		// 如果没有获取到消费记录，就播放使用增收宝的好处
		pDlg->m_stcContent.SetText(StringZengshoubao,_T('|'));
	}

	pDlg->StartBugleTurn();
	pDlg->m_stcContent.StartRolling(5000);
	return 0;
}
void CDlgIBAFastlane::OnStnClickedPicStatus()
{
	CIBAString strToolTip;
	strToolTip.LoadString(IDS_IBA_STATUS_TEXT);
	//strToolTip = _T("<h3>嘟嘟牛收银台</h3><br></br><h4>正常模式</h4><br></br><p>收银台与计费中心正常连接，所有功能都能正常使用。</p>	<br></br><h4>应急模式</h4><br></br><p>收银台与计费中心失去连接，只能开本地用户、充值退款和修改密码，不能交班。</p><br></br><h4>恢复模式</h4><br></br><p>收银台与计费中心正常连接，仍有本地用户还没有结账退款，暂时还不能交班。</p>");

	CRect rc(0,0,0,0);
	GetDlgItem(IDC_PIC_STATUS)->GetWindowRect(rc);

	m_ToolTip.ShowHelpTooltip(&rc.TopLeft(), strToolTip );

	SetFocus();
}

LRESULT CDlgIBAFastlane::OnUpdateIBAStatus(WPARAM wParam, LPARAM lParam)
{
	m_nIBAStatus = wParam;
	Invalidate();
	return 0;
}
void CDlgIBAFastlane::OnLButtonDown(UINT nFlags, CPoint point)
{
	WINDOWPLACEMENT pm;
	m_stcPicStatus.GetWindowPlacement(&pm);
	CRect rc = pm.rcNormalPosition;
	if (rc.PtInRect(point))
	{
		OnStnClickedPicStatus();
	}

	CStringLoaderDlg<CBCGPDialog>::OnLButtonDown(nFlags, point);
}

BOOL CDlgIBAFastlane::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CStringLoaderDlg<CBCGPDialog>::OnSetCursor(pWnd, nHitTest, message);
}

void CDlgIBAFastlane::OnMouseMove(UINT nFlags, CPoint point)
{
	WINDOWPLACEMENT pm;
	m_stcPicStatus.GetWindowPlacement(&pm);
	CRect rc = pm.rcNormalPosition;
	if (rc.PtInRect(point))
	{
		SetCursor(::LoadCursor(NULL,IDC_HAND));
	}

	CStringLoaderDlg<CBCGPDialog>::OnMouseMove(nFlags, point);
}
