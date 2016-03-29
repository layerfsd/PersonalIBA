// NDCheckInDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Encrypt.h"
#include "IBAHelpper.h"
#include ".\Socket\LocalServer.h"
#include ".\dal\IBADAL.h"
#include ".\ndcheckindlg.h"
#include "ComputerListView.h"


// CNDCheckInDlg 对话框

IMPLEMENT_DYNAMIC(CNDCheckInDlg, CNDDialog)
CNDCheckInDlg::CNDCheckInDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDCheckInDlg::IDD, pParent)
, m_bPowerOn(TRUE), m_bCheckIn(TRUE)
{
}

CNDCheckInDlg::~CNDCheckInDlg()
{
}

void CNDCheckInDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_POWERON, m_bPowerOn);
}


BEGIN_MESSAGE_MAP(CNDCheckInDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CNDCheckInDlg 消息处理程序

void CNDCheckInDlg::InitGridTree()
{
	CRect rectGrid;
	GetDlgItem(IDC_GIRD_RECT)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_GIRD_RECT)->MapWindowPoints(this, &rectGrid);
	GetDlgItem(IDC_GIRD_RECT)->ShowWindow(SW_HIDE);

	m_wndGridTree.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	m_wndGridTree.EnableHeader(TRUE, BCGP_GRID_HEADER_SORT);

	CIBAString strHeader;
	strHeader.LoadString(IDS_CHECKIN_HEAD);

	//strHeader = L"电脑名称|状态|区域";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {100, 100, 115};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_wndGridTree.InsertColumn(i, strArray.GetAt(i), Width[i]);
		m_wndGridTree.SetColumnAlign(i, HDF_LEFT);
	}
}

BOOL CNDCheckInDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	OnBnClickedButtonQuery();

	InitGridTree();

	FillGridTree();

	m_wndGridTree.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNDCheckInDlg::FillGridTree(void)
{
	m_wndGridTree.RemoveAll();

	//{ 2011/07/11-8201-gxx: 是否本地用户换机或者在断网情况下中心用户不能跨区域换机
	BOOL bNormalAdd = TRUE;
	INT nPCClassID = 0;
	if (!m_strNetId.IsEmpty())
	{
		CActiveMember ActiveMember;
		if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(m_strNetId, ActiveMember))
		{
			if ( (!ActiveMember.GetTerminalID().IsEmpty()) )
			{
				if (ActiveMember.GetIsLocalUser())
				{
					bNormalAdd = FALSE;
					nPCClassID = ActiveMember.GetPCClassID();
				}
				else if (!ActiveMember.GetIsLocalUser() && CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
				{
					bNormalAdd = FALSE;
					nPCClassID = ActiveMember.GetPCClassID();
				}
			}
		}
	}
	//}

	for (INT_PTR i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	{
		CComputerInfo & computer = CLocalServer::GetInstance()->ComputerList.GetComputer(i);

		if (computer.GetComputerStatus() == CComputerInfo::ECS_OFFLINE
			|| computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
		{
			CActiveMember ActiveMember;

			BOOL bFind = CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(computer.GetTerminalID(), ActiveMember);
			
			//{ 2011/07/11-8201-gxx: 是否允许本地用户换机或者在断网情况下中心用户不能跨区域换机
			if (!bFind && !bNormalAdd)
			{
				if (nPCClassID != computer.GetPCClassID())
				{
					bFind = TRUE;
				}
			}
			//}
			
			// 确认没有，列表中就可以添加终端
			if (!bFind)
			{
				CBCGPGridRow* pRow = m_wndGridTree.CreateRow(m_wndGridTree.GetColumnCount());
				pRow->GetItem(0)->SetValue(computer.GetTerminalID().GetString(), FALSE);
				pRow->GetItem(1)->SetValue(computer.GetStatusAsString().GetString(), FALSE);
				pRow->GetItem(2)->SetValue(computer.GetPCClass().GetString(), FALSE);

				pRow->SetData(i);

				m_wndGridTree.AddRow(pRow, FALSE);
			}
		}
	}
	

	m_wndGridTree.SetSortColumn(0);
	m_wndGridTree.AdjustLayout();
}

void CNDCheckInDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CNDDialog::OnLButtonDblClk(nFlags, point);

	if (0 == point.x && 0 == point.y) //双击GRID
	{
		OnBnClickedOk();
	}
}

void CNDCheckInDlg::OnBnClickedOk()
{
	UpdateData();

	CBCGPGridRow* pRow = m_wndGridTree.GetCurSel();

	if (pRow != NULL) 
	{
		INT_PTR nIdx = pRow->GetData();

		if (CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(nIdx))
		{
			return;
		}

		CComputerInfo & computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIdx);

		if (CheckTakeUp(computer.GetTerminalID())) //占用了
		{
			MsgBox(IDS_CHANGECOMPUTERERROR);
			return;
		}
		
		//{ 2011/07/27-8201-gxx: 
		CActiveMember ActiveMember;
		if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(m_strNetId, ActiveMember))
		{
			CIBALog::GetInstance()->WriteFormat(_T("上机时，从用户视图获取账号：%s对应的用户失败"), m_strNetId);
			CBCGPDialog::OnOK();
			return;
		}

		if (!GetCheckIn()) //换机登出
		{
			
		}

		// 2011/11/03-8210-gxx: 包房主账号上机
		if (ActiveMember.GetMemberID() == ActiveMember.GetRoomMemberId())
		{
			if (ActiveMember.GetPCClassID() != computer.GetPCClassID())
			{
				theApp.IBAMsgBox(LOAD_STRING(IDS_ROOM_CHECKIN_ERROR1));
				return ;
			}
		}
		
		
		//添加到数据库
		CLastUserInfo LastUserInfo;
		LastUserInfo.SetNetId(m_strNetId);
		LastUserInfo.SetMemberId(m_nMemberId);
		LastUserInfo.SetTermId(computer.GetTerminalID());
		LastUserInfo.SetLoginStyle(1/*GetCheckIn() ? 1 : 0*/); //自动登录要求传强制登录参数
		LastUserInfo.SetUpdateTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));

		CString strTmp;
		if ( (int(m_nMemberId)) < 0 ) 
		{
			// 本地用户不再次加密
			LastUserInfo.SetPassWord(m_strPwd);
			if (!ActiveMember.GetTerminalID().IsEmpty())
			{
				// 本地用户换机
				CheckoutOldComputer(); // 换机登出
			}
			
			CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
			CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);
		}
		else
		{
			// 中心用户
			strTmp = CIBAHelpper::EncodePassword(m_strPwd);
			LastUserInfo.SetPassWord(strTmp);

			if (!GetCheckIn() && !ActiveMember.GetTerminalID().IsEmpty())
			{
				// 中心用户换机

				CLastUserInfo tmpLastInfo;
				if (CIBADAL::GetInstance()->GetLastUserInfo(tmpLastInfo, m_nMemberId))
				{
					LastUserInfo.SetPassWord( tmpLastInfo.GetPassWord() );
				}
				
				CheckoutOldComputer(); // 换机登出

				CIBADAL::GetInstance()->DeleteLastUserInfo(tmpLastInfo.GetTermId());
				CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
				CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);

				/*m_strTermId = ActiveMember.GetTerminalID();
				INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.LookFor(0, m_strTermId);

				if (nIdx >=0)
				{
					CLocalServer::GetInstance()->LocalCheckOutUser(nIdx, m_nMemberId, 0);

					Sleep(1000);

					CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);
					IBA_LOG(_T("重发换机登出消息，termID=%s"), m_strTermId);
				}	*/
			}
			else
			{
				// 中心用户上机
				
				CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
				CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);
			}
		}

		ActiveMember.SetTerminalID(computer.GetTerminalID());
		ActiveMember.SetPCClassID(computer.GetPCClassID());

		//{ 2011/10/26-8230-gxx: 
		ActiveMember.SetRoomMemberId(0);
		UINT nRoomMemberID=0;
		if ( CLocalServer::GetInstance()->ActiveMemberList.IsRoomArea(computer.GetPCClassID(), nRoomMemberID) )
		{
			ActiveMember.SetRoomMemberId(nRoomMemberID);
		}
		//}
		
		
		

		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
		CLocalServer::GetInstance()->ActiveMemberList.RefreshView();
		

		//请求登录
		
		if (computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
		{
			CLocalServer::GetInstance()->LocalAutoCheckIn(nIdx);
		}
		else
		{
			if (m_bPowerOn) //自动开机
			{
				IBA_LOG0(_T("自动开机"));
				CLocalServer::GetInstance()->LocalStartupComputer(nIdx);
			}
		}

		MsgBoxFormat(MB_OK, IDS_CHANGECOMPUTERSUCCESS, LastUserInfo.GetTermId());

		CBCGPDialog::OnOK();
	}
}

void CNDCheckInDlg::SetNetId(CString strNetId)
{
	m_strNetId = strNetId;
}

void CNDCheckInDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_nMemberId > 0)
	{
		m_Images.Draw(&dc, m_bIsMember ? 1 : 2, CPoint(30, 25), ILD_NORMAL);
	}
}

void CNDCheckInDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance);

	m_strPwd = Balance.GetPassword();

	if (!GetCheckIn()) //换机
	{
		CString strTmp(_T(" 换机"));

		if (!m_strTermId.IsEmpty())
		{
			strTmp.Append(_T(" - "));
			strTmp.Append(m_strTermId);
		}

		SetWindowText(strTmp);
	}
}

void CNDCheckInDlg::OnBnClickedButtonRefresh()
{
	FillGridTree();
}

void CNDCheckInDlg::CheckoutOldComputer()
{
	if (m_strTermId.IsEmpty())
	{
		return;
	}

	INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.LookFor(0, m_strTermId);
	
	if (nIdx < 0)
	{
		return;
	}
	
	IBA_LOG(_T("%s 换机下机"), m_strTermId);
	CLocalServer::GetInstance()->LocalCheckOutUser(nIdx, m_nMemberId, 1);

	Sleep(1000);

	CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);
}

BOOL CNDCheckInDlg::CheckTakeUp(CString strTermId)
{
	//查询该终端是否被占用
	BOOL bTakeUp = FALSE;

	CLastUserInfo LastUserInfo;

	if (CIBADAL::GetInstance()->GetLastUserInfo(LastUserInfo, 0, strTermId))
	{
		if (LastUserInfo.GetSuspend())//挂机
		{
			bTakeUp = TRUE;
		}
		else//非挂机
		{
			COleDateTime updateTime;
			updateTime.ParseDateTime(LastUserInfo.GetUpdateTime());
			COleDateTimeSpan interval = COleDateTime::GetCurrentTime() - updateTime;
			
			if (interval.GetTotalMinutes() < 10)//被占用，并且用户还有效
			{
				bTakeUp = TRUE;
			} 
		}
	}

	return bTakeUp;
}

BOOL CNDCheckInDlg::DoLocalModeQuery()
{
	BOOL bRet = CNDDialog::DoLocalModeQuery();
	if (!bRet)
	{
		return FALSE;
	}

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken &&
			m_LocalActiveMember.GetIsLocalUser() == FALSE &&
			GetCheckIn() == TRUE)
	{
		MsgBox(_T("中心用户不允许在应急模式下上机"));
		CBCGPDialog::OnOK();
		return TRUE;
	}

	m_strPwd = m_LocalActiveMember.GetPassword();

	if (!GetCheckIn()) //换机
	{
		CString strTmp(_T(" 换机"));

		if (!m_strTermId.IsEmpty())
		{
			strTmp.Append(_T(" - "));
			strTmp.Append(m_strTermId);
		}

		SetWindowText(strTmp);
	}
	return TRUE;
}

