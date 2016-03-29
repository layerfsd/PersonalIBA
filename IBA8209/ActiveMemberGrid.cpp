#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "Socket\LocalServer.h"
#include ".\activemembergrid.h"

CActiveMemberGrid::CActiveMemberGrid(void)
: m_lpActiveMemberList(&CLocalServer::GetInstance()->ActiveMemberList)
{

}

CActiveMemberGrid::~CActiveMemberGrid(void)
{
}
BEGIN_MESSAGE_MAP(CActiveMemberGrid, CIBAGrid)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

int CActiveMemberGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CIBAGrid::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_NETBARUSER);

	m_Images.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);
	m_Images.Add(&bmp, RGB(250, 50, 250));
	SetImageList(&m_Images);

	SetRowMarker(FALSE);
	EnableHeader(TRUE, BCGP_GRID_HEADER_SORT | BCGP_GRID_HEADER_MOVE_ITEMS);
	EnableMarkSortedColumn(FALSE, FALSE);

	if (CIBAConfig::GetInstance()->GetFontType() > 0)
	{
		CIBAHelpper::CreateFont(m_TextFont);
		SetFont(&m_TextFont);
	}

	InitColumnHead();

	SetTimer(TimeIdOfUpdateTime, IntervalOfUpdateTime, NULL);
	SetSortColumn(3, FALSE); //激活时间
	LoadState(_T("ActiveMemberGrid"));//从注册表载入

	AdjustLayout();

	

	m_lpActiveMemberList->SetWndGrid(GetSafeHwnd());
	m_lpActiveMemberList->Start();

	SetTimer(TimerIdOfBundleTimeTry, IntervalOfBundleTimeTry, NULL);

	return 0;
}

void CActiveMemberGrid::InitColumnHead()
{
	CIBAString strTmp;
	strTmp.LoadString(IDS_ACTIVEMEMBERLISTHEAD);
	
	//strTmp = L"上网账号|用户级别|可用金额|开始时间|电脑名称|区域|上机时间|累计时长|累计消费|消费模式|姓名|证件号码|预付款|备注";
	
	//strTmp = L"上网账号|级别|余额|开始时间|电脑|区域|上机时间|用时|费用|消费模式|预付款|备注|姓名|证件号码";

	const UINT nWidth[] = {140,60,60,60,60,60,60,50,50,60,60,120,60,140,120};

	CStringArray strArray;
	CIBAHelpper::SplitLine(strTmp, strArray);

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		InsertColumn(i, strArray.GetAt(i), nWidth[i]);
	}
}

void CActiveMemberGrid::OnDestroy()
{
	SaveState(_T("ActiveMemberGrid"));

	CIBAGrid::OnDestroy();
}

void CActiveMemberGrid::UpdateRowData(CBCGPGridRow* pRow, CActiveMember& ActiveMember)
{
	//图标
	if (ActiveMember.GetPayType() == CActiveMember::EPayType_General && 
		ActiveMember.GetIsOnLine() &&
		ActiveMember.GetAvailavleBalance() < (LONG)CIBAConfig::GetInstance()->GetMinMoney())
	{
		pRow->GetItem(0)->SetImage(4, FALSE); //没钱了
	}
	else
	{
		if (ActiveMember.GetIsLocalUser())
		{
			pRow->GetItem(0)->SetImage(5,FALSE);
		}
		else
		{
			pRow->GetItem(0)->SetImage(ActiveMember.IsMember() ? 1 : 2, FALSE);
		}
	}
	
	if (!ActiveMember.IsActive()) //没有激活
	{
		pRow->GetItem(0)->SetImage(3, FALSE);
	}

	//上网账号|级别|余额|开始时间|
	pRow->GetItem(0)->SetValue(ActiveMember.GetNetId().GetString(), FALSE);
	if (ActiveMember.GetIsLocalUser())
	{
		CIBAString str;
		str.LoadString(IDS_LOCALUSER);
		pRow->GetItem(1)->SetValue(str.GetString(), FALSE);
	}
	else
	{
		pRow->GetItem(1)->SetValue(ActiveMember.GetUserClassName().GetString(), FALSE);
	}
	
	pRow->GetItem(2)->SetValue(ActiveMember.GetAvailavleBalanceAsString().GetString(), FALSE);
	pRow->GetItem(3)->SetValue(ActiveMember.GetActivationTimeAsString().GetString(), FALSE);

	//电脑|区域|上机时间|用时|
	pRow->GetItem(4)->SetValue(ActiveMember.GetTermIdInfo().GetString(), FALSE);
	pRow->GetItem(5)->SetValue(ActiveMember.GetPCClass().GetString(), FALSE);
	pRow->GetItem(6)->SetValue(ActiveMember.GetCheckInTimeAsString().GetString(), FALSE);
	pRow->GetItem(7)->SetValue(ActiveMember.GetUseTimeAsString().GetString(), FALSE);

	//费用|消费模式|预付款|明细|
	pRow->GetItem(8)->SetValue(ActiveMember.GetAmountAsString().GetString(), FALSE);
	pRow->GetItem(9)->SetValue(ActiveMember.GetPayTypeAsString().GetString(), FALSE);
	pRow->GetItem(10)->SetValue(ActiveMember.GetAdvancePaymentAsString().GetString(), FALSE);
	pRow->GetItem(11)->SetValue(ActiveMember.GetCreditProcess().GetString(), FALSE);
	
	//姓名|证件号码
	pRow->GetItem(12)->SetValue(ActiveMember.GetUserName(TRUE).GetString(), FALSE);
	pRow->GetItem(13)->SetValue(ActiveMember.GetPersonalID().GetString(), FALSE);

	// 备注
	{
		if (!ActiveMember.GetIsLocalUser())
		{
			CIBAString strRegisterBT;
			strRegisterBT.LoadString(IDS_BT_NOT_PC); // 包时未上机
			CString strRemark;
			strRemark = ActiveMember.GetRemark();

			if (ActiveMember.GetIsOnLine())
			{
				if (strRemark == strRegisterBT)
				{
					ActiveMember.SetRemark(_T(""));
				}
			}
			else
			{
				if (ActiveMember.GetIsRegisterBT()&&(strRemark==strRegisterBT||strRemark.IsEmpty()))
				{
					ActiveMember.SetRemark(strRegisterBT);
				}
			}
		}
		else
		{
			CIBAString strRemark;
			strRemark.LoadString(IDS_LOCALUSER); // 本地用户
			ActiveMember.SetRemark(strRemark);
		}
	}
	pRow->GetItem(14)->SetValue(ActiveMember.GetRemark().GetString(), FALSE);

}

void CActiveMemberGrid::RedrawAllActiveMembers()
{
	RemoveAll();

	CActiveMember ActiveMember;

	CSingleLock lock(&m_lpActiveMemberList->ActiveMemberListLock, TRUE);

	m_lpActiveMemberList->StartEnumActiveMember();

	while (m_lpActiveMemberList->GetEachActiveMember(ActiveMember))
	{
		AddNewRow(ActiveMember);
	}
}

void CActiveMemberGrid::RedrawUpdateActiveMembers()
{
	while (TRUE)
	{
		UINT nMemberId = m_lpActiveMemberList->GetUpdateActiveMemberId();

		if (nMemberId > 0)
		{
			CActiveMember ActiveMember;

			if (m_lpActiveMemberList->GetActiveMember(nMemberId, ActiveMember))
			{
				CBCGPGridRow* pRow = FindRowByData(ActiveMember.GetMemberID(), FALSE);

				if (pRow != NULL)//已经存在的
				{
					UpdateRowData(pRow, ActiveMember);
				}
				else //新增加的
				{
					AddNewRow(ActiveMember);
				}
			}
			else
			{
				//已经删除了

				CBCGPGridRow* pRow = FindRowByData(nMemberId, FALSE);
				
				if (pRow != NULL)
				{
					RemoveRow(pRow->GetRowId());
				}
			}
		}
		else
		{
			//更新完毕了
			break;
		}
	}
}

void CActiveMemberGrid::UpdateAllComputersUseTime()
{
	CActiveMember ActiveMember;

	CSingleLock lock(&m_lpActiveMemberList->ActiveMemberListLock, TRUE);

	m_lpActiveMemberList->StartEnumActiveMember();

	while (m_lpActiveMemberList->GetEachActiveMember(ActiveMember))
	{
		CBCGPGridRow* pRow = FindRowByData(ActiveMember.GetMemberID());

		if (pRow != NULL)
		{
			pRow->GetItem(7)->SetValue(ActiveMember.GetUseTimeAsString().GetString());
		}
	}
}

void CActiveMemberGrid::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfUpdateTime == nIDEvent)
	{
		UpdateAllComputersUseTime();

		Refresh(FALSE);
	}
	else if (TimeIdOfRefresh == nIDEvent)
	{
		Refresh(FALSE);
	}
	else if (TimerIdOfBundleTimeTry == nIDEvent)
	{
		m_lpActiveMemberList->TryReLogin();
	}

	CIBAGrid::OnTimer(nIDEvent);
}

void CActiveMemberGrid::Refresh(BOOL bRedrawAll)
{
	if (bRedrawAll)
	{
		RedrawAllActiveMembers();
	}
	else
	{
		RedrawUpdateActiveMembers();
	}

	AdjustLayout();
}

void CActiveMemberGrid::AddNewRow(CActiveMember &ActiveMember,  BOOL bAtFront)
{
	// 2014-2-28 - qsc 仅显示在线用户
	if(0 != CNetBarConfig::GetInstance()->GetOnlyOnlineUser())
	{
		if(CActiveMember::EStatus_LocalOnline != ActiveMember.GetStatus() &&
			CActiveMember::EStatus_CenterOnLine != ActiveMember.GetStatus())
		{
			return ;
		}
	}
	
	CBCGPGridRow* pRow = CBCGPGridCtrl::CreateRow(GetColumnCount());
	pRow->SetData(ActiveMember.GetMemberID());
	UpdateRowData(pRow, ActiveMember);

	// 2011/07/08-8201-gxx: 给本地用户着色
	if (ActiveMember.GetIsLocalUser())
	{
		for(int i=0; i<GetColumnCount(); i++)
		{
			pRow->GetItem(i)->SetTextColor(RGB(38,153,3),FALSE);
		}
	}

	AddRow(pRow, bAtFront);
}	

void CActiveMemberGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CBCGPGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	if (VK_RETURN == nChar)
	{
		if (CIBAConfig::GetInstance()->GetOpenAfterEnter())
		{
			GetParent()->PostMessage(WM_LBUTTONDBLCLK);
		}
	}
}

int CActiveMemberGrid::CompareItems(const CBCGPGridItem* pItem1, const CBCGPGridItem* pItem2) const
{
	if (pItem1->GetColumnId()!=pItem2->GetColumnId())
	{
		return CIBAGrid::CompareItems(pItem1, pItem2);
	}

	UINT nMemberID1 = (UINT)pItem1->GetParentRow()->GetData();
	UINT nMemberID2 = (UINT)pItem2->GetParentRow()->GetData();

	CActiveMember activeMember1;
	CActiveMember activeMember2;
	m_lpActiveMemberList->GetActiveMember(nMemberID1, activeMember1);
	m_lpActiveMemberList->GetActiveMember(nMemberID2, activeMember2);

	BOOL bOnLine1 = activeMember1.GetIsOnLine();
	BOOL bOnLine2 = activeMember2.GetIsOnLine();

	int nSortState = m_Columns.GetColumnState(pItem1->GetColumnId());
	if (nSortState==0)
	{
		return 0;
	}
	else if (nSortState==1) //升序
	{	
		if (!bOnLine1 && bOnLine2)
		{
			return -1;  // 1未上机的始终在前面
		}
		if (bOnLine1 && !bOnLine2)
		{	
			return 1;   // 2未上机，要排在前面
		}
		if (!bOnLine1 && !bOnLine2) // 均未上机，始终激活时间排序
		{
			COleDateTime time1 = activeMember1.GetActivationTime();
			COleDateTime time2 = activeMember2.GetActivationTime();

			if (time2 > time1)
			{
				return 1;
			}
			else if (time1 == time2)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			// 全部都是上机状态
		}
	}
	else if (nSortState==-1) //降序
	{	
		if (!bOnLine1 && bOnLine2)
		{
			return 1;  // 1未上机的始终在前面
		}
		if (bOnLine1 && !bOnLine2)
		{	
			return -1;   // 2未上机，要排在前面
		}
		if (!bOnLine1 && !bOnLine2) // 均未上机，用激活时间排序
		{
			COleDateTime time1 = activeMember1.GetActivationTime();
			COleDateTime time2 = activeMember2.GetActivationTime();

			if (time1 > time2)
			{
				return 1;
			}
			else if (time1 == time2)
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			// 全部都是上机状态
		}
	}

	if (pItem1->GetColumnId()==3) // 是按激活时间
	{
		COleDateTime time1 = activeMember1.GetActivationTime();
		COleDateTime time2 = activeMember2.GetActivationTime();

		if (time1 > time2)
		{
			return 1;
		}
		else if (time1 == time2)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if(pItem1->GetColumnId() == 6)//上机时间排序
	{
		COleDateTime time1 = activeMember1.GetCheckInTime();
		COleDateTime time2 = activeMember2.GetCheckInTime();

		if (time1 > time2)
		{
			return 1;
		}
		else if (time1 == time2)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return CIBAGrid::CompareItems(pItem1, pItem2);
	}

	
}
