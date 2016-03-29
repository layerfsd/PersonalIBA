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
#include "ComputerListView.h"
#include "ComputerInfo.h"
#include "ComputerList.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include "Dal\IBADAL.h"
#include "RealName\RZXRealName.h"
#include "GlobalFunctions.h"
#include "LocalCenter\G2XCommonTool.h"
#include "Tts\IBATTS.h"

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

	//20160116-liyajun
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
	//20160116-liyajun
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

/*************************************************
Function:         //OnCbnSelendokComboFast
Description:      //便捷通道输入的处理:
				  //检查输入:<2,最后一个字符非0~9，a~z，A~Z,直接返回
				  //首字符 - '*' - 用于调试,模拟刷卡情形的开关:需要输入完整账号
				  //		'\\' - 表示终端号,正常模式下,跳到终端视图,并选择到该终端.
				  //		'\' - 表示终端IP.跳到终端视图,并选择到该终端.
				  //		'?' - 为根据账号查找终端,跳到终端视图,并选择到该终端.
				  //		'+' - 根据账号进行金额充值
				  //		others - 表示账号,<=3位表示终端号,3<n<6表示部分账号需要具体匹配,>=6表示全部账号
*************************************************/
void CDlgIBAFastlane::OnCbnSelendokComboFast()
{
	CString strTmp;
	m_combFast.GetWindowText(strTmp);// 取通道中的子串
	m_combFast.SetWindowText(_T(""));

	strTmp.Trim();

	int nInputLen = strTmp.GetLength(); //字串长度

	if (strTmp.GetLength() < 2) //少于2个字符，退出
	{
		return;
	}

	CHAR tcLast = strTmp.GetAt(strTmp.GetLength()-1);   //获取最后一个字符
	
	if ( !( (_T('0')<=tcLast&&tcLast<=_T('9')) ||       //非0~9，a~z，A~Z的都认为是无效的
		(_T('a')<=tcLast&&tcLast<=_T('z')) || 
		(_T('A')<=tcLast&&tcLast<=_T('Z')) ) 
		)
	{
		return;
	}

	BOOL bComputerView = theApp.GetIBAView()->IsCurrentComputerView();   //判断目前激活的是否机器视图

	BOOL bAllowInputActivation = CNetBarConfig::GetInstance()->GetInputActivation();  //是否允许便捷通道激活

	//根据第一个字符判断
	switch (strTmp.GetAt(0))    //判断输入的第一位
	{
	//调试用代码，用来模拟刷卡
	case '*':
		{
			CString strNetId = strTmp.Mid(1);
			if(strNetId.GetLength() < 5)
			{
				return;
			}
			else if(strNetId.GetLength() < 18 )
			{
				strNetId.Format(_T("%018s"),strTmp.Mid(1));
			}
			//CString strNetId = _T("420114198607072219");
			//CString strIdNumber = _T("420114198607072219");
			CString strIdNumber = strNetId;
			CString strName = _T("拉卡泽特");

			if (bAllowInputActivation) //允许输入激活
			{
				IBA_LOG(_T("Temp.Simulate sweeping personal IDcard ..."));

				CActiveMemberView *pMemberView = CLocalServer::GetInstance()->GetActiveMemberView();
				int nIndex = pMemberView->FindUserAt(0, strTmp);
				if(0 <= nIndex)   //如果用户列表中已存在该用户信息,设置其为选中，根据需要弹窗
				{
					CActiveMember ActiveMember;
					pMemberView->SetSelUser(nIndex,TRUE);
					//如果已经激活
					if(pMemberView->GetCurActiveMember(ActiveMember))
					{
						strTmp = ActiveMember.GetNetId();
						//设置m_NebBarUser，m_localActiveMember，激活用户，根据需要弹窗
						theApp.GetRealName()->OnRealNameMsg(999, &strTmp, strTmp.GetLength()); 
						return;
					}	
				}

				//CActiveMemberView *pMemberView = CLocalServer::GetInstance()->GetActiveMemberView();
				//pMemberView->SetSelUser(pMemberView->FindUserAt(0, strTmp), TRUE);

				int		nIdType = 12;         // 二代证
				CString strPassword = "123";  // 密码

				CRegKey reg;

				if(0 == reg.Create(HKEY_CURRENT_USER, _T("SoftWare\\Sunlike\\IBA\\RealName\\Base\\NewUser")))/*Base\\NewUser"))*/
				{
					reg.SetStringValue(_T("IDNo"), strNetId);
					reg.SetStringValue(_T("CertificateID"), strIdNumber);
					reg.SetDWORDValue(_T("CertificateType"), 12);
					reg.SetStringValue(_T("Name"), strName);
					reg.SetStringValue(_T("Password"), strPassword);

					reg.Close();
				}

				theApp.GetRealName()->OnRealNameMsg(999, &strTmp, strTmp.GetLength());
				return;
			}
			// 如果是机器视图，就不用激活用户视图
			theApp.GetIBAView()->GotoActiveMember(strTmp, !bComputerView);
		}
	case '\\'://为'\\'表示查找终端
		{
			if (strTmp.GetAt(1) == '\\') // - //+xxx表示后面为IP：只查找激活机器视图，不做其他动作
			{	
				theApp.GetIBAView()->GotoComputerByIP(strTmp.Mid(2).Trim(), !bComputerView);//用IP在终端列表中查找,并选中当前终端

			}
			else // - /+xxx表示后面为终端号：使用终端号不完全匹配在激活列表中查找用户，选中该用户，在任务栏显示用户信息，弹退款窗
			{
				theApp.GetIBAView()->GotoComputer(strTmp.Mid(1).Trim(), !bComputerView);  //用终端号在终端列表中查找,并选中当前终端
			}
			
			CComputerInfo Info;

				//弹出对应窗口
			if(CLocalServer::GetInstance()->GetComputerListView()->GetCurComputerInfo(Info))
			{
				ActivateWndByComputerInfo(Info);
			}	
		}

		break;

	case '?'://为根据账号查找终端，并在终端视图中选中该终端
		{
			theApp.GetIBAView()->GotoComputerByNetID(strTmp.Mid(1).Trim());

			CComputerInfo Info;

				//弹出对应窗口
			if(CLocalServer::GetInstance()->GetComputerListView()->GetCurComputerInfo(Info))
			{
				ActivateWndByComputerInfo(Info);
			}	
		}		
		break;

	case '+'://根据账号进行金额充值
		{
			CCreditHelpper CreditHelpper;
			CreditHelpper.Execute(strTmp.Mid(1));//从便捷通道取输入，根据网络状况进行中心充值激活或本地充值激活
			m_combFast.SetFocus();
		}
		break;

	default://其他的
		{
			//如果首位为字母
			//CHAR chFirstChar = strTmp.GetAt(0);
			//if( _T('a')<=chFirstChar && chFirstChar<=_T('z') || _T('A')<=chFirstChar && chFirstChar<=_T('Z'))
			//{
			//	CString strTermID = strTmp;
			//	CActiveMember ActiveMember;
			//	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberBest(strTermID,ActiveMember, FALSE);
			//	theApp.GetIBAView()->GotoComputer(strTermID, !bComputerView);
			//	CComputerInfo Info;

			//	//弹出对应窗口
			//	if(CLocalServer::GetInstance()->GetComputerListView()->GetCurComputerInfo(Info))
			//	{
			//		ActivateWndByComputerInfo(Info);
			//	}	
			//	return;
			//}

			CActiveMemberView *pMemberView = CLocalServer::GetInstance()->GetActiveMemberView();   //用户视图

			BOOL bFoundInActivedList(FALSE);

			switch (nInputLen)
			{
			//实际1位的情况在开始已经被筛除掉,这里是为了代码阅读
			case 1:
			case 2:
			case 3:
				{
					CString strTermID = strTmp;
					CActiveMember ActiveMember;
					CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberBest(strTermID,ActiveMember, FALSE);
					theApp.GetIBAView()->GotoComputer(strTermID, !bComputerView);

					CComputerInfo Info;

						//弹出对应窗口
					if(CLocalServer::GetInstance()->GetComputerListView()->GetCurComputerInfo(Info))
					{
						ActivateWndByComputerInfo(Info);
					}	
				}
				break;
			//4位或5位,表示账号.
			case 4:
			case 5: 
				{
					//从本地库查找匹配ID,并由用户选择
					CString strSelectedID;
					UINT nMemberId;
					if(FALSE == QueryIDs_PartialMatch(strTmp,strSelectedID)){return;}
					if(FALSE == QueryIDs_PartialMatch(strTmp,nMemberId)){return;}					
					//if(FALSE == CIBADAL::GetInstance()->GetMemberId(strSelectedID,nMemberId)){return;}

					//是否激活
					CActiveMember ActiveMember;
					BOOL bActivated = CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);

					//已激活,弹窗
					if(bActivated)
					{
						PopupWindow(ActiveMember);
					}
					else//未激活.需根据后台设置是否允许激活来判断动作
					{
						if(bAllowInputActivation)//允许激活的,激活 + 弹窗
						{							
							if(Activation(strSelectedID,nMemberId))
							{
								CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
								PopupWindow(ActiveMember);
							}
						}
						else//不允许激活的,弹出充值窗口
						{
							PopupWindow_UnActiveMember(strSelectedID);
						}
					}	
				}
				break;
			//6位或以上
				//匹配已激活用户时,仍同上查询后弹窗
				//匹配未激活用户时，本地模式下到本地表中查找用户
								  //正常模式下到中心校验卡号
				//若不允许激活，则到本地库进行不完全匹配，并弹出窗口
			default:
				{
					//从本地库查找匹配ID,并由用户选择
					CString strSelectedID;
					UINT nMemberId;
					CActiveMember ActiveMember;
					if(QueryActiveUser(strTmp,nMemberId))//在激活列表中找到,则弹出对应窗口
					{						
						//CIBADAL::GetInstance()->GetMemberId(strSelectedID,nMemberId);
						CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
						PopupWindow(ActiveMember);
					}
					else//未激活,是否允许输入激活
					{
						if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)//本地模式
						{
							//到本地表中查找用户
							if(FALSE == CIBADAL::GetInstance()->GetMemberId(strTmp,nMemberId))//找不到则直接退出
							{
								return;
							}
							if(bAllowInputActivation)//允许激活的,激活 + 弹窗
							{
								if(Activation(strTmp,nMemberId))
								{
									CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
									PopupWindow(ActiveMember);
								}
							}
							else//不允许激活的，不完全匹配查找，弹窗
							{
								if(QueryIDs_PartialMatch(strTmp,strSelectedID))
								{
									PopupWindow_UnActiveMember(strSelectedID);
								}	
							}
						}
						else//正常模式或恢复模式
						{
							//校验卡号
							CIDCheckCardMethod IDCheckCard;
							IDCheckCard.SetSerialNum(strTmp);
							IDCheckCard.SetSerialNumType(1);
							theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

							if(IDCheckCard.GetMemberId() <= 0)//若没有返回用户号，直接退出
							{
								return;
							}
							//有用户号的，激活并弹窗
							if(bAllowInputActivation)//允许激活的,激活 + 弹窗
							{
								if(Activation(strTmp,nMemberId))
								{
									CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
									PopupWindow(ActiveMember);
								}
							}
							else//不允许激活的，弹窗
							{
								PopupWindow_UnActiveMember(strTmp);
							}
						}
					}
				}
				
			}	
		}
	}
	return;
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
//取消点击可以尝试网络
void CDlgIBAFastlane::OnStnClickedPicStatus()
{
	CIBAString strToolTip;
	strToolTip.LoadString(IDS_IBA_STATUS_TEXT);
	//strToolTip = _T("<h3>嘟嘟牛收银台</h3><br></br><h4>正常模式</h4><br></br><p>收银台与计费中心正常连接，所有功能都能正常使用。</p>	<br></br><h4>应急模式</h4><br></br><p>收银台与计费中心失去连接，只能开本地用户、充值退款和修改密码，不能交班。</p><br></br><h4>恢复模式</h4><br></br><p>收银台与计费中心正常连接，仍有本地用户还没有结账退款，暂时还不能交班。</p>");

	CRect rc(0,0,0,0);
	GetDlgItem(IDC_PIC_STATUS)->GetWindowRect(rc);

	//20160116-liyajun
	m_ToolTip.ShowHelpTooltip(&rc.TopLeft(), strToolTip );

	SetFocus();

	// 2015/5/18 tmelody
	// 点击如果是断网状态，那么立即尝试恢复
	//// {
	//IBA_LOG0(_T("点击按钮试图恢复网络"));
	//if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	//{
	//	CGetTimeMethod time;
	//	time.Execute();

	//	int nStatus = time.GetStatusCode();
	//	if (nStatus >= 0)
	//	{
	//		IBA_LOG0(_T("网络此时已恢复"));
	//		CBrokenNetwork::EnableNetwork();
	//		//从尝试模式恢复到正常模式时，没有本地模式数据，因此不需要开启网络恢复线程
	//		//CBrokenNetwork::GetInstance()->m_threadRestore->Start();
	//	}
	//}
	// }
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

/*************************************************
Function:       //LocalQueryActiveUser
Description:    //不完全账号匹配查找已激活用户
				//到本地ActiveMember表中查找对应用户,如果找到多个匹配用户,则弹出窗口供用户选择.根据选中的用户,弹出其对应窗口
Calls:          //GotoActiveMember
Table Accessed: //ActiveMember
Input:          //strNetId - 查找用户的参数
Return:         //没有找到用户，返回false
Others:         //与LocalQueryUser区别：只查找已激活
*************************************************/
BOOL CDlgIBAFastlane::QueryActiveUser(CString strNetId,UINT &nMemberId)
{
	//strResultNetdId.Empty();

	CDlgMemberInfo MemberInfoDlg;
	MemberInfoDlg.SetQueryStyle(5); //查询本地已激活用户-ActiveMember表
	MemberInfoDlg.SetNetId(strNetId);

	if (MemberInfoDlg.DoModal() == IDOK) 
	{
		m_combFast.SetFocus(); //这个是必须的，否则可能会出问题
		//IBA_LOG(_T("QueryActiveUser::NetId:%s"),MemberInfoDlg.GetNetId());
		//strResultNetdId = MemberInfoDlg.GetNetId(); //取得用户当前选中的账号
		//IBA_LOG(_T("QueryActiveUser::strResultNetId:%s"),strResultNetdId);
		nMemberId = MemberInfoDlg.GetMemberId();

		if(0 >= nMemberId)
		{
			return FALSE;
		}
		else
		{
			//bRet = theApp.GetIBAView()->GotoActiveMember(strNetID,false);
			return TRUE;
		}
	}
	else
	{
		//如果没有则设置焦点到快捷栏，不然(如果之前有选择某个终端的话)会弹出充值对话框
		m_combFast.SetFocus();
		//如果对话框条目数不为0，表示用户没有选择，但其实是已经查找到的
		//if(MemberInfoDlg.GetItemCount() != 0) 
		//{
		//	bRet = true;
		//}
		return FALSE;
	}	
}
/*************************************************
Function:       //ActivateWndByComputerInfo
Description:    //根据终端信息computerInfo到激活列表中找到用户，然后弹出窗口
Input:          //Info - 终端信息
*************************************************/
void CDlgIBAFastlane::ActivateWndByComputerInfo(CComputerInfo& Info)
{
	CString strNetId = Info.GetNetId();
	UINT nMemberId = Info.GetMemberID();

	CActiveMember ActiveMemberActivated;

	if(!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMemberActivated))
	{
		IBA_LOG(_T("Abnormal.CDlgIBAFastlane::OnCbnSelecdokComboFast. Couldn't find ActiveMember for specified computer!"));
		return ;
	}
	
	PopupWindow(ActiveMemberActivated);
	//if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 0) //不弹出任何对话框
	//{
	//	return ;
	//}

	//if (AfxGetMainWnd()->IsIconic())//判断当前是最小窗口
	//{
	//	AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);//最大化
	//}

	//AfxGetMainWnd()->BringWindowToTop();
	//AfxGetMainWnd()->SetForegroundWindow();

	//CCurrentNetBarUser::GetInstance()->SetNetId(strNetId);

	////弹出充值退款
	//if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 1)
	//{
	//	if (CCreditDlg::m_StaticPrehWnd != NULL) //窗口已经弹出来了
	//	{
	//		BOOL bRet = ::SendMessage(CCreditDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)(LPCTSTR)strNetId);
	//		if (!bRet) //表示正在充值
	//		{
	//			//DoNothing
	//		}
	//		else
	//		{
	//			::SetForegroundWindow(CCreditDlg::m_StaticPrehWnd);
	//			::SetFocus(CCreditDlg::m_StaticPrehWnd);
	//		}
	//	}
	//	else
	//	{
	//		CCreditDlg dlg;
	//		dlg.SetLocalActiveMember(ActiveMemberActivated);
	//		dlg.DoModal();	
	//	}
	//}
	//else if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 2) //使用新的充值/结账对话框
	//{
	//	if (AfxGetMainWnd()->IsWindowEnabled())
	//	{
	//		//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值，可以先跳过。
	//		BOOL bIsNDCredit(FALSE);
	//		LPTSTR lpszWndCaption = new TCHAR[30];
	//		memset(lpszWndCaption,0,30);
	//		HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
	//		if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
	//		{
	//			CString strWndCation(lpszWndCaption);
	//			strWndCation.Trim();
	//			bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
	//		}
	//		if(bIsNDCredit)  //充值窗口已经弹出
	//		{
	//			//DoNothing
	//		}
	//		else
	//		{
	//			CNDReturnDlg dlg;
	//			dlg.SetLocalActiveMember(ActiveMemberActivated);
	//			dlg.DoModal();
	//		}
	//	}
	//}
	//else
	//{
	//	//...不显示任何对话框则DoNothing
	//}
}

/***************************************************************************************************
Function:       //PopupWindow_UnActiveMember
Description:    //为未激活用户弹出充值窗口
Input:          //strNetId - 待弹出窗口的用户账号
Others:         //因为未激活,只能进行充值操作,要隐藏其他按钮.
********************************************************************************************************/
void CDlgIBAFastlane::PopupWindow_UnActiveMember(CString strNetId)
{
	CActiveMember Am;
	if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(strNetId,Am))
	{
		return;
	}
	
	//如果当前窗口是最小窗口，在最大化
	if (AfxGetMainWnd()->IsIconic())
	{
		AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);
	}

	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetForegroundWindow();

	CCurrentNetBarUser::GetInstance()->SetNetId(strNetId);

	//弹出充值窗口
	if (AfxGetMainWnd()->IsWindowEnabled())
	{
		CNDCreditDlg dlg;
		dlg.SetNetId(strNetId);	
		dlg.DoModal();
	}
	return;
}

/***************************************************************************************************
Function:       //PopupWindow
Description:    //按照收银台设置弹出窗口
Input:          //ActiveMember - 待弹出窗口的用户对象
********************************************************************************************************/
void CDlgIBAFastlane::PopupWindow(CActiveMember& ActiveMember)
{
	CString strNetId = ActiveMember.GetNetId();
	
	//如果当前窗口是最小窗口，在最大化
	if (AfxGetMainWnd()->IsIconic())
	{
		AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);
	}

	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetForegroundWindow();

	CCurrentNetBarUser::GetInstance()->SetNetId(strNetId);

	int nPopupFlag = CIBAConfig::GetInstance()->GetOperationAfterSwipingCard();
	/* nPopupFlag == 0 不弹出任何窗口
       nPopupFlag == 1 弹出充值退款窗
	   nPopupFlag == 2 会员弹出充值窗,临时用户弹出结账窗*/  //但向中心未激活用户,只可能是会员,临时用户查不到

	switch (nPopupFlag)
	{
	case 0:
		return;
	case 1:
		if (CCreditDlg::m_StaticPrehWnd != NULL) //窗口已经弹出来了
		{
			BOOL bRet = ::SendMessage(CCreditDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)(LPCTSTR)strNetId);
			if (bRet) //表示没有在充值
			{
				::SetForegroundWindow(CCreditDlg::m_StaticPrehWnd);
				::SetFocus(CCreditDlg::m_StaticPrehWnd);
			}
		}
		else
		{
			CCreditDlg dlg;			
			dlg.SetNetId(strNetId, TRUE);			
			dlg.DoModal();	
		}
		return;
	case 2:
		if (AfxGetMainWnd()->IsWindowEnabled())
		{
			//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值，直接返回
			BOOL bIsNDCredit(FALSE);
			LPTSTR lpszWndCaption = new TCHAR[30];
			memset(lpszWndCaption,0,30);
			HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
			if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
			{
				CString strWndCation(lpszWndCaption);
				strWndCation.Trim();
				bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
			}
			if(bIsNDCredit)
			{
				return;
			}


			//会员弹出充值窗,临时用户弹出结账窗
			bool bIsMember = ActiveMember.IsMember();
			if(bIsMember)//会员
			{
				CNDCreditDlg dlg;
				dlg.SetNetId(strNetId);
				dlg.DoModal();
			}
			else//临时用户
			{
				CNDReturnDlg dlg;
				dlg.SetNetId(strNetId);
				dlg.DoModal();
			}
		}
		return;
	default:
		return;
	}
}

/*************************************************
Function:       //QueryIDs_PartialMatch
Description:    //不完全账号匹配查找已注册用户,如果找到多个匹配用户,则弹出窗口供用户选择
Table Accessed: //Member
Input:          //strNetId - 查找用户的参数
Output:			//strResultNetId - 找到的用户ID,或用户选择ID
Return:         //没有找到，返回false
*************************************************/
BOOL CDlgIBAFastlane::QueryIDs_PartialMatch(CString strNetId,CString &strResultNetId)
{
	strResultNetId.Empty();

	//从本地库查找数据
	CDlgMemberInfo MemberInfoDlg;
	MemberInfoDlg.SetQueryStyle(1); //从本地查询有效用户
	MemberInfoDlg.SetNetId(strNetId);

	//选择了用户或只有一个用户,返回TRUE.否则返回FALSE.
	if (MemberInfoDlg.DoModal() == IDOK)   //从member表中取得第一个匹配的有效用户
	{
		m_combFast.SetFocus(); //这个是必须的，否则可能会出问题
		strResultNetId = MemberInfoDlg.GetNetId();

		if(strResultNetId.IsEmpty())
		{
			return FALSE;
		}
		else
		{
			return TRUE;			
		}
	}
	else//未选择用户,返回FALSE
	{
		//如果没有则设置焦点到快捷栏，不然(如果之前有选择某个终端的话)会弹出充值对话框
		m_combFast.SetFocus();
		////如果对话框条目数不为0，表示用户没有选择，但其实是已经查找到的
		//if(MemberInfoDlg.GetItemCount() != 0) 
		//{
		//	return TRUE;
		//}
		return FALSE;
	}	
}

BOOL CDlgIBAFastlane::QueryIDs_PartialMatch(CString strNetId,UINT &nMemberId)
{
	//从本地库查找数据
	CDlgMemberInfo MemberInfoDlg;
	MemberInfoDlg.SetQueryStyle(1); //从本地查询有效用户
	MemberInfoDlg.SetNetId(strNetId);

	//选择了用户或只有一个用户,返回TRUE.否则返回FALSE.
	if (MemberInfoDlg.DoModal() == IDOK)   //从member表中取得第一个匹配的有效用户
	{
		m_combFast.SetFocus(); //这个是必须的，否则可能会出问题
		nMemberId = MemberInfoDlg.GetMemberId();

		if(0 >= nMemberId)
		{
			return FALSE;
		}
		else
		{
			return TRUE;			
		}
	}
	else//未选择用户,返回FALSE
	{
		//如果没有则设置焦点到快捷栏，不然(如果之前有选择某个终端的话)会弹出充值对话框
		m_combFast.SetFocus();
		////如果对话框条目数不为0，表示用户没有选择，但其实是已经查找到的
		//if(MemberInfoDlg.GetItemCount() != 0) 
		//{
		//	return TRUE;
		//}
		return FALSE;
	}	
}
/*************************************************
Function:       //Activation
Description:    //激活用户
Table Accessed: //Member
Input:          //strNetId - 查找用户的参数
Output:			//nMemberId - 用户号
Return:         //成功返回TRUE
*************************************************/
BOOL CDlgIBAFastlane::Activation(CString strNetId, UINT &MemberID)
{
	IBA_LOG(_T("便捷通道激活..."));
	CActiveMember ActiveMember;
	INT nAllMoney = 0;

	//本地模式
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		//查找用户
		GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->GetMemberFromMember(strNetId);

		//从Member表中校验NetId,未注册账户或临时账号均不允许注册
		CString sql ;
		if(!memberPtr){return FALSE;}
		if(FALSE == CNetBarConfig :: GetInstance()-> GetIsMember ( memberPtr ->intValueForKey ("classId"))){return FALSE;}

		//任子行实名加复制照片处理
		if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
		{
			CString strPhotoPath;
			std ::wstring idNum = G2XUtility ::CG2XCommonTool :: Utf8_to_WideChar( std ::string ( memberPtr-> valueForKey ("idNumber")->stringValue ()));
			CString stridNum(idNum .c_str());   
			if(NS_REALNAME::CRZXRealName::GetPhotoPath(strPhotoPath,stridNum))
			{	
				CString strPath, strNewFile;
				strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
				CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
				strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(strNetId));
				IBA_LOG(_T("获取任子行照片路径%s, 拷贝路径%s"), strPhotoPath, strNewFile);
				if(!CopyFile(strPhotoPath, strNewFile,FALSE))
				{
					IBA_LOG(_T("拷贝任子行照片失败"));
				}
			}	
			else
			{
				IBA_LOG(_T("获取任子行照片失败"));
			}
		}

		//为会员用户创建临时激活对象，用于在本地模式下上机。初始余额设置为0.
		IBA_TRACE ("%s", memberPtr -> describe_d(0, false ).c_str ());
		ActiveMember .SetNetbarID ( memberPtr-> valueForKey ("netBarId")->intValue ());                       //NetbarID
		ActiveMember .SetMemberID ( memberPtr-> valueForKey ("memberId")->intValue ());                       //MemberID
		ActiveMember .SetClassId ( memberPtr-> valueForKey ("classId")->intValue ());                         //ClassID
	/*	std ::wstring name = G2XUtility ::CG2XCommonTool :: Utf8_to_WideChar( std ::string ( memberPtr-> valueForKey ("name")->stringValue ()));
		ActiveMember .SetUserName ( CString( name .c_str ()));        */                                        //UserName
		CString strUserName;
		if(CIBADAL::GetInstance()->GetUserNameFromMember(strNetId,strUserName))
		{
			ActiveMember.SetUserName(strUserName);
		}
		else
		{
			std ::wstring name = G2XUtility ::CG2XCommonTool :: Utf8_to_WideChar( std ::string ( memberPtr-> valueForKey ("name")->stringValue ()));
			ActiveMember .SetUserName ( CString( name .c_str ()));                             //UserName
		}
		ActiveMember .SetNetId ( CString( CA2T (memberPtr -> valueForKey("serialNum")-> stringValue ())));       //NetId
		ActiveMember .SetIdType ( memberPtr-> valueForKey ("idType")->intValue ());                              //idType
		ActiveMember .SetPersonalID ( CString( CA2T (memberPtr -> valueForKey("idNumber")-> stringValue ())));   //PersonalID
		ActiveMember .SetSex ( memberPtr-> valueForKey ("sex")->intValue ());           //Sex
		ActiveMember .SetAvailavleBalance (0);                                          //AvailavleBalance
		ActiveMember .SetAdvancePayment (0);                                            //AdvancePayment
		ActiveMember .SetAmount (0);                                                    //Amount
		ActiveMember .SetCreditProcess ( ActiveMember. GetAdvancePaymentAsString ());   //CreditProcess start with AdvancePayment
		ActiveMember .SetCasherID ( CIBAConfig:: GetInstance ()->GetCashRegisterID ()); //CashierID
		CString strNetId = ActiveMember. GetNetId ();
		strNetId = strNetId . Right(8);
		strNetId .TrimLeft ( _T('0')); // 过滤掉左边 的0
		ActiveMember .SetNetIdAlias ( strNetId);                             //AliasNetId
		ActiveMember .SetActivationTime ( COleDateTime:: GetCurrentTime ()); //ActivationTime = now
		ActiveMember .SetActive ( TRUE);                                   //Active=TRUE
		ActiveMember .SetStatus ( CActiveMember:: EStatus_Waiting );       //Status = Waiting待上机
		ActiveMember .SetIsOnLine(FALSE);                     //IsOnline
		//ActiveMember.SetRemark(_T("LocalActivated"));       //Remark:标记是本地激活的
		//ActiveMember.SetTerminalID(_T("''"));               //TerminalId是CheckoutInfo表的主键，不能为NULL(activeMember表默认为NULL，所以这里要赋值)

		//添加到激活列表
		CLocalServer::GetInstance()->ActiveMemberList.AddActiveMember(ActiveMember);
	}
	else//正常模式/恢复模式
	{
		//如果后台设置了自动获取账号，则当证件类型为11或12时(一二代身份证)，截取证件号码后12位为strNetId。中心同样可以辨认
		//if(1 == CNetBarConfig::GetInstance()->GetEnableAccount12())
		//{
		//	CString strCID = m_NetBarUser.GetCertificateID();
		//	if(12 < strCID.GetLength() && 
		//		(m_NetBarUser.GetCertificateType() == 11 ||
		//		m_NetBarUser.GetCertificateType() == 12))
		//	{
		//		m_NetBarUser.SetNetId(strCID.Right(12));
		//		strNetId = strCID.Right(12);
		//	}
		//}

		//CheckCard-校验卡号
		UINT nMemberId;
		UINT nCardId;

		CIDCheckCardMethod IDCheckCard;
		IDCheckCard.SetSerialNum(strNetId);
		IDCheckCard.SetSerialNumType(1);
		theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

		if(0 != IDCheckCard.GetStatusCode()){return FALSE;}
		nCardId = IDCheckCard.GetCardId();		
		nMemberId = IDCheckCard.GetMemberId();

		
		//QueryBalance-查询余额
		CCurrentNetBarUser::GetInstance()->Balance.SetMemberId(nMemberId);
		CCurrentNetBarUser::GetInstance()->Balance.SetCardId(nCardId);
		CCurrentNetBarUser::GetInstance()->Balance.SetRound(0);//取整
		theApp.GetCurCashier()->DoBalance(CCurrentNetBarUser::GetInstance()->Balance);

		if (0 != CCurrentNetBarUser::GetInstance()->Balance.GetStatusCode()){return FALSE;}

		// 任子行实名加复制照片处理
		if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
		{
			CString strPhotoPath;
			if(NS_REALNAME::CRZXRealName::GetPhotoPath(strPhotoPath, CCurrentNetBarUser::GetInstance()->Balance.GetPersonalId()))
			{	
				CString strPath, strNewFile;
				strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
				CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
				strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(strNetId));
				IBA_LOG(_T("获取任子行照片路径%s, 拷贝路径%s"), strPhotoPath, strNewFile);
				if(!CopyFile(strPhotoPath, strNewFile,FALSE))
				{
					IBA_LOG(_T("拷贝任子行照片失败"));
				}
			}	
			else
			{
				IBA_LOG(_T("获取任子行照片失败"));
			}
		}

		//激活
		CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
		//如管家设置了上传照片，上传照片
		if(0 != CNetBarConfig::GetInstance()->GetSoftSfreg())
		{
			CString strDest = CIBAHelpper::GetUserZjImgFilePath(Balance.GetIdType(), Balance.GetPersonalId());
			// 2014-8-13 - qsc 每次都覆盖 if(!PathFileExists(strDest))
			{
				const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
				CopyFile(strSource, strDest, FALSE);
				// 2014-7-4 - qsc 拷贝完做下备份,再删除原文件
				CString strBak = theApp.GetWorkPath() + _T("\\IBATemp\\ID_bak.jpg");
				CopyFile(strSource, strBak, FALSE);
				DeleteFile(strSource);
			}		
		}
		else if(2 == CNetBarConfig::GetInstance()->GetSoftSfreg())
		{
			CString strDest = CIBAHelpper::GetUserZcImgFilePath(Balance.GetIdType(), Balance.GetPersonalId());
			if(!PathFileExists(strDest))
			{
				if(!CIBAHelpper::TakePhoto(CIBAHelpper::GetUserZcImgPath(Balance.GetIdType(), Balance.GetPersonalId()), Balance.GetPersonalId()))
				{
					IBA_LOG(_T("CRealNameBase::ActivationMember Result is FALSE caused by taking photo failed- for MemberID = %d"),Balance.GetMemberId());
					return FALSE; 
				}
			}		
		}

		//本地模式余额为0
		nAllMoney = Balance.GetTotalBalacne();
		if (FALSE == Balance.GetNeedActivation()){return FALSE;}
		if (FALSE == CServletHellper::ActivationMember(nMemberId)) {return FALSE;}

		//先删除，并更新界面，这样可保证显示在最前面
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMemberDBPostActivation(Balance.GetMemberId());
		CLocalServer::GetInstance()->ActiveMemberList.AddActiveMemberPostActivation(Balance);

		//获取ActiveMember方便显示记录
		CLocalServer::GetInstance()->GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
	}

	//播报余额
	CIBATTS::GetInstance()->SpeakBalance(nAllMoney);
	//显示激活记录
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		::SendMessage (CMainFrame :: m_hOperationLog, WM_OPERATIONLOG , 1, (LPARAM)&ActiveMember );
	}
	else
	{
		::SendMessage (CMainFrame :: m_hOperationLog, WM_OPERATIONLOG , 1, NULL );
	}
	
	//刷新用户视图
	CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
	//设置CurrentNetBarUser
	CCurrentNetBarUser::GetInstance()->SetNetId(strNetId);

	MemberID = ActiveMember.GetMemberID();
	return TRUE;
}