#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "dal\IBADAL.h"
#include "Socket\LocalServer.h"
#include "resource.h"
#include "ServletHellper.h"
#include ".\activememberlist.h"
#include "ComputerListView.h"
#include "Encrypt.h"
#include "CashierHelper.h"
#include "MainFrm.h"
#include "RealName\IBARealName.h"
#include "LocalCenter\G2XCommonTool.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include "..\Lib\ScanRecoInc.h"
#include "LocalMember.h"
using namespace NS_DAL;

CActiveMemberList::CActiveMemberList(void)
{
	m_hWndGrid = NULL;
	m_CurPos = NULL;

	m_ActiveMemberMap.InitHashTable(257);
}

CActiveMemberList::~CActiveMemberList(void)
{
}

void CActiveMemberList::RefreshView()
{
	PostMessage(m_hWndGrid, WM_TIMER, 1001, NULL); //刷新用户视图
}

BOOL CActiveMemberList::LoadFromDB()
{
	Clear();

	BOOL bRet = FALSE;

	CString strSQL;

	strSQL = _T("select activemember.*, member.classId as userClassId from activemember");

	strSQL.Append(_T(" left outer join member on activemember.memberId = member.memberId where activemember.memberId <> 0"));

	IBA_TRACE(strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL)) 
	{
		bRet = TRUE;
	}
	else
	{
		pDb->Release();
		return FALSE;
	}

	int nCount = Rs.GetRecordCount();

	for (UINT i = 0; i < Rs.GetRecordCount(); i++)
	{
		CActiveMember ActiveMember;

		CString strTmp; INT nTmp = 0;

		if (Rs.GetFieldValue(_T("memberId"), nTmp))
		{
			ActiveMember.SetMemberID(nTmp);
		}

		if (Rs.GetFieldValue(_T("netId"), strTmp))
		{
			ActiveMember.SetNetId(strTmp);
		}

		if (Rs.GetFieldValue(_T("userClassId"), nTmp))
		{
			ActiveMember.SetClassId(nTmp);
		}

		if (Rs.GetFieldValue(_T("name"), strTmp))
		{
			ActiveMember.SetUserName(strTmp);
		}

		if (Rs.GetFieldValue(_T("sex"), nTmp))
		{
			ActiveMember.SetSex(nTmp);
		}

		if (Rs.GetFieldValue(_T("idNumber"), strTmp))
		{
			ActiveMember.SetPersonalID(strTmp);
		}

		if (Rs.GetFieldValue(_T("idType"), nTmp))
		{
			ActiveMember.SetIdType(nTmp);
		}

		if (Rs.GetFieldValue(_T("lastRemainAmount"), nTmp))
		{
			ActiveMember.SetAvailavleBalance(nTmp);
		}

		if (Rs.GetFieldValue(_T("amount"), nTmp))
		{
			ActiveMember.SetAmount(nTmp);
		}

		if (Rs.GetFieldValue(_T("refNo"), nTmp))
		{
			//ActiveMember.SetIsOnLine( nTmp > 0 );

			ActiveMember.SetRefNo(nTmp);
		}

		if (Rs.GetFieldValue(_T("status"), nTmp))
		{
			ActiveMember.SetStatus(nTmp);
		}

		Rs.GetFieldValue(_T("activationTime"), ActiveMember.ActivationTime);

		Rs.GetFieldValue(_T("updateTime"), ActiveMember.UpdateDataTime);

		Rs.GetFieldValue(_T("checkinTime"), ActiveMember.CheckInTime);

		// 2011/08/10-8201-gxx: 
		Rs.GetFieldValue(_T("nextCheckinTime"), ActiveMember.NextCheckinTime);

		if (CIBAHelpper::IsValidTime(ActiveMember.CheckInTime))
		{
			ActiveMember.SetIsOnLine(TRUE);
		}

		

		if (Rs.GetFieldValue(_T("termId"), strTmp))
		{
			ActiveMember.SetTerminalID(strTmp);
		}

		if (Rs.GetFieldValue(_T("pcClass"), nTmp))
		{
			ActiveMember.SetPCClassID(nTmp);
		}

		if (Rs.GetFieldValue(_T("isActive"), nTmp))
		{
			ActiveMember.SetActive(nTmp);
		}

		if (Rs.GetFieldValue(_T("payType"), nTmp))
		{
			ActiveMember.SetPayType(nTmp);
		}

		if (Rs.GetFieldValue(_T("advancePayment"), nTmp))
		{
			ActiveMember.SetAdvancePayment(nTmp);
		}

		if (Rs.GetFieldValue(_T("creditProcess"), strTmp))
		{
			ActiveMember.SetCreditProcess(strTmp);
		}

		// 2011-3-29-gxx-添加收银台ID的读取
		if (Rs.GetFieldValue(_T("lastNetBarId"), nTmp))
		{
			ActiveMember.SetCasherID(nTmp);
		}

		// 2011/05/13-gxx: 添加备注
		if (Rs.GetFieldValue(_T("Remark"), strTmp))
		{
			ActiveMember.SetRemark(strTmp);
		}

		// 2011/05/19-gxx: 开户是否为包时
		if (Rs.GetFieldValue(_T("registerBT"), nTmp))
		{
			ActiveMember.SetIsRegisterBT(nTmp);
		}

		// 2011/07/08-8201-gxx: 累计封顶额
		if (Rs.GetFieldValue(_T("dynamicMaxAmount"), nTmp))
		{
			ActiveMember.SetDynamicMaxAmount(nTmp);
		}

		// 2011/07/08-8201-gxx: 是否本地用户
		if (Rs.GetFieldValue(_T("isLocal"), nTmp))
		{
			ActiveMember.SetIsLocalUser(nTmp);
		}

		// 2011/07/08-8201-gxx: 上机密码, MD5值
		if (Rs.GetFieldValue(_T("password"), strTmp))
		{
			ActiveMember.SetPassword(strTmp);
		}

		// 2011/07/18-8201-gxx: 扣率
		if (Rs.GetFieldValue(_T("PolicyInfo"), strTmp))
		{
			ActiveMember.SetPolicyInfo(strTmp);
		}

		// 2011/07/18-8201-gxx: 设置本地用户的级别ID， 确认是否为恢复状态
		//if (ActiveMember.GetIsLocalUser())
		//{
		//	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
		//	{
		//		// 转换成为恢复状态
		//		CBrokenNetwork::SettingNetworkStatus(CIBAGlobal::emNetworkRestore);		
		//	}

		//	if (Rs.GetFieldValue(_T("classId"), nTmp))
		//	{
		//		ActiveMember.SetClassId(nTmp);
		//	}
		//}

		// 2011/10/25-8230-gxx: 
		if (Rs.GetFieldValue(_T("roomMemberId"), nTmp))
		{
			ActiveMember.SetRoomMemberId(nTmp);
		}

		UpdateActiveMember(ActiveMember);

		Rs.MoveNext();
	}

	IBA_TRACE("用户总数为:%d",Rs.GetRecordCount());

	Rs.Close();
	
	pDb->Release();

	return bRet;
}

void CActiveMemberList::AddActiveMember(NS_DAL::CLocalRegisterInfo& RegisterInfo)
{
	CActiveMember ActiveMember;

	ActiveMember.SetNetbarID(RegisterInfo.GetNetbarId());
	ActiveMember.SetMemberID(RegisterInfo.GetMemberId());
	ActiveMember.SetClassId(RegisterInfo.GetUserClass());
	ActiveMember.SetUserName(RegisterInfo.GetUserName());
	ActiveMember.SetNetId(RegisterInfo.GetNetId());
	ActiveMember.SetIdType(RegisterInfo.GetIdType());
	ActiveMember.SetPersonalID(RegisterInfo.GetPersonalId());
	ActiveMember.SetSex(RegisterInfo.GetSex());
	ActiveMember.SetAvailavleBalance(RegisterInfo.GetMoney());

	ActiveMember.SetAmount(0);
	ActiveMember.ActivationTime = COleDateTime::GetCurrentTime();
	ActiveMember.SetActive(TRUE);
	ActiveMember.SetStatus(CActiveMember::EStatus_Waiting);

	ActiveMember.SetAdvancePayment(ActiveMember.GetAvailavleBalance());

	ActiveMember.SetCreditProcess(ActiveMember.GetAdvancePaymentAsString());
	ActiveMember.SetCasherID(CIBAConfig::GetInstance()->GetCashRegisterID());

	ActiveMember.SetIsLocalUser(TRUE);  //只有本地注册的才是为本地用户
	
	CString strPwd = CIBAHelpper::EncodePassword(RegisterInfo.GetPassword());
	ActiveMember.SetPassword(strPwd);

	CString strNetId = ActiveMember.GetNetId();
	strNetId = strNetId.Right(8);
	strNetId.TrimLeft(_T('0')); // 过滤掉左边 的0
	ActiveMember.SetNetIdAlias(strNetId);

	RemoveActiveMember(ActiveMember.GetMemberID());

	UpdateActiveMember(ActiveMember);

	AddActiveMemberDB(ActiveMember);

	CIBADAL::GetInstance()->AddMemberInfo(RegisterInfo);

	//
	CLocalServer::GetInstance()->GetActiveMemberView()->GetActiveMemberGrid()->AddNewRow(ActiveMember, TRUE);

	int nAt = CLocalServer::GetInstance()->GetActiveMemberView()->FindUserAt(1, ActiveMember.GetNetId());
	//
	CLocalServer::GetInstance()->GetActiveMemberView()->SetSelUser(nAt, TRUE);
}

void CActiveMemberList::AddActiveMember(CBalanceMethod & Balance, BOOL bRegisterBT, // 是否开户包时
										UINT nPayout, int nBundleTimeType, int nScanType)
{
	CActiveMember ActiveMember;

	ActiveMember.SetNetbarID(Balance.GetOpenNetbarId());
	ActiveMember.SetMemberID(Balance.GetMemberId());
	ActiveMember.SetClassId(Balance.GetUserClass());
	ActiveMember.SetUserName(Balance.GetUserName());
	ActiveMember.SetNetId(Balance.GetSerailNum());
	ActiveMember.SetIdType(Balance.GetIdType());
	ActiveMember.SetPersonalID(Balance.GetPersonalId());
	ActiveMember.SetSex(Balance.GetSex());
	ActiveMember.SetAvailavleBalance(Balance.GetTotalBalacne());
	ActiveMember.SetScanType(nScanType);

	//{ 2011/05/19-gxx: 判断是否开户包时
	if (bRegisterBT )
	{
		if (nBundleTimeType == CActiveMember::EPayType_Withholding)
		{
			IBA_CHECK_UINT(nPayout);
			ActiveMember.SetAmount(nPayout);
		}
		
		ActiveMember.SetIsRegisterBT(TRUE);
	}
	else
	{
		ActiveMember.SetAmount(0);
	}
	//}
	
	ActiveMember.ActivationTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetServerTime());
	ActiveMember.SetActive(TRUE);
	ActiveMember.SetStatus(CActiveMember::EStatus_Waiting);

	// 2011/04/25-gxx: =总余额 (更改预付款，会员为余额，临时用户为可退金额) + 费用
	ActiveMember.SetAdvancePayment(Balance.GetTotalBalacne()+ActiveMember.GetAmount());
	
	ActiveMember.SetCreditProcess(ActiveMember.GetAdvancePaymentAsString());
	ActiveMember.SetCasherID(CIBAConfig::GetInstance()->GetCashRegisterID());

	CString strNetId = ActiveMember.GetNetId();
	strNetId = strNetId.Right(8);
	strNetId.TrimLeft(_T('0')); // 过滤掉左边 的0
	ActiveMember.SetNetIdAlias(strNetId);

	CActiveMember tmAct;
	if (GetActiveMemberNetID(ActiveMember.GetNetId(),tmAct, TRUE))
	{
		RemoveActiveMember(tmAct.GetMemberID());
	}
	else
	{
		RemoveActiveMember(ActiveMember.GetMemberID());
	}
	
	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to UpdateActiveMember(ActiveMember)"));
	UpdateActiveMember(ActiveMember);

	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to AddActiveMemberDB(ActiveMember)"));
	AddActiveMemberDB(ActiveMember);

	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to AddNewRow"));
	CLocalServer::GetInstance()->GetActiveMemberView()->GetActiveMemberGrid()->AddNewRow(ActiveMember, TRUE);

}

//激活时调用RemoveActiveMemberPostActivation
void CActiveMemberList::AddActiveMemberPostActivation(CBalanceMethod & Balance, BOOL bRegisterBT, // 是否开户包时
										UINT nPayout, int nBundleTimeType, int nScanType)
{
	CActiveMember ActiveMember;

	ActiveMember.SetNetbarID(Balance.GetOpenNetbarId());
	ActiveMember.SetMemberID(Balance.GetMemberId());
	ActiveMember.SetClassId(Balance.GetUserClass());
	ActiveMember.SetUserName(Balance.GetUserName());
	ActiveMember.SetNetId(Balance.GetSerailNum());
	ActiveMember.SetIdType(Balance.GetIdType());
	ActiveMember.SetPersonalID(Balance.GetPersonalId());
	ActiveMember.SetSex(Balance.GetSex());
	ActiveMember.SetAvailavleBalance(Balance.GetTotalBalacne());
	ActiveMember.SetScanType(nScanType);

	//{ 2011/05/19-gxx: 判断是否开户包时
	if (bRegisterBT )
	{
		if (nBundleTimeType == CActiveMember::EPayType_Withholding)
		{
			IBA_CHECK_UINT(nPayout);
			ActiveMember.SetAmount(nPayout);
		}

		ActiveMember.SetIsRegisterBT(TRUE);
	}
	else
	{
		ActiveMember.SetAmount(0);
	}
	//}

	ActiveMember.ActivationTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetServerTime());
	ActiveMember.SetActive(TRUE);
	ActiveMember.SetStatus(CActiveMember::EStatus_Waiting);

	// 2011/04/25-gxx: =总余额 (更改预付款，会员为余额，临时用户为可退金额) + 费用
	ActiveMember.SetAdvancePayment(Balance.GetTotalBalacne()+ActiveMember.GetAmount());

	ActiveMember.SetCreditProcess(ActiveMember.GetAdvancePaymentAsString());
	ActiveMember.SetCasherID(CIBAConfig::GetInstance()->GetCashRegisterID());

	CString strNetId = ActiveMember.GetNetId();
	strNetId = strNetId.Right(8);
	strNetId.TrimLeft(_T('0')); // 过滤掉左边 的0
	ActiveMember.SetNetIdAlias(strNetId);

	CActiveMember tmAct;
	if (GetActiveMemberNetID(ActiveMember.GetNetId(),tmAct, TRUE))
	{
		RemoveActiveMemberDBPostActivation(tmAct.GetMemberID());
	}
	else
	{
		RemoveActiveMemberDBPostActivation(ActiveMember.GetMemberID());
	}

	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to UpdateActiveMember(ActiveMember)"));
	UpdateActiveMember(ActiveMember);

	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to AddActiveMemberDB(ActiveMember)"));
	AddActiveMemberDB(ActiveMember);

	//IBA_LOG(_T("Temp.CActiveMemberList:AddActiveMember: After RemoveActiveMember and LocalAuditUserQuit, Start to AddNewRow"));
	CLocalServer::GetInstance()->GetActiveMemberView()->GetActiveMemberGrid()->AddNewRow(ActiveMember, TRUE);

}

void CActiveMemberList::AddActiveMember(CActiveMember& ActiveMember)
{
	UpdateActiveMember(ActiveMember);
	AddActiveMemberDB(ActiveMember);
	CLocalServer::GetInstance()->GetActiveMemberView()->GetActiveMemberGrid()->AddNewRow(ActiveMember, TRUE);
}



void CActiveMemberList::UpdateActiveMember(CActiveMember& ActiveMember, BOOL bUpdateDb /*= FALSE*/)
{
	//IBA_LOG(_T("Temp.CActiveMemberList::UpdateActiveMember - Start"));
	//上机时间正常则表示用户在线
	if (CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()) &&
		ActiveMember.GetCheckInTime() > COleDateTime(2011,1,1,0,0,0))
	{
		ActiveMember.SetIsOnLine( TRUE );
	}

	CSingleLock lock(&ActiveMemberListLock, TRUE);

	m_UpdateMemberIdArray.Add(ActiveMember.GetMemberID());
	m_ActiveMemberMap.SetAt(ActiveMember.GetMemberID(), ActiveMember);

	lock.Unlock();

	if (bUpdateDb)
	{
		UpdateActiveMemberDB(ActiveMember);
	}
	//IBA_LOG(_T("Temp.CActiveMemberList::UpdateActiveMember - End"));
}

void CActiveMemberList::UpdateActiveMember(UINT nMemberId, UINT nCreditMoney, UINT nCreditPresent, UINT nPayout /*= 0*/)
{
	CActiveMember ActiveMember;

	if (GetActiveMember(nMemberId, ActiveMember))
	{
		// 2011/05/17-gxx: 
		//ActiveMember.SetAdvancePayment(ActiveMember.GetAdvancePayment() + nCreditMoney + nCreditPresent);
        ActiveMember.SetAdvancePayment(ActiveMember.GetAdvancePayment() + nCreditMoney + nCreditPresent-nPayout);

		CString strCreditProcess(ActiveMember.GetCreditProcess());

		if (nCreditMoney > 0)
		{
			if (nCreditMoney % 100 == 0)
			{
				strCreditProcess.AppendFormat(_T("+%d"), nCreditMoney / 100);
			}
			else
			{
				strCreditProcess.AppendFormat(_T("+%.1f"), nCreditMoney / 100.0);
			}
		}

		if (nCreditPresent > 0)
		{
			if (nCreditPresent % 100 == 0)
			{
				strCreditProcess.AppendFormat(_T("送%d"), nCreditPresent / 100);
			}
			else
			{
				strCreditProcess.AppendFormat(_T("送%.1f"), nCreditPresent / 100.0);
			}
		}

		if (nPayout > 0)
		{
			ActiveMember.SetAvailavleBalance(ActiveMember.GetAvailavleBalance() - nPayout);

			if (nPayout % 100 == 0)
			{
				strCreditProcess.AppendFormat(_T("-%d"), nPayout / 100);
			}
			else
			{
				strCreditProcess.AppendFormat(_T("-%.1f"), nPayout / 100.0);
			}
		}

		ActiveMember.SetAvailavleBalance(ActiveMember.GetAvailavleBalance() + nCreditMoney);
		
		ActiveMember.SetCreditProcess(strCreditProcess);

		UpdateActiveMember(ActiveMember, TRUE);

		RefreshView();
	}
}

void CActiveMemberList::UpdateActiveMember( UINT nMemberId, UINT nNewBlance )
{
	CActiveMember ActiveMember;

	if (GetActiveMember(nMemberId, ActiveMember)) 
	{
		ActiveMember.SetAvailavleBalance(nNewBlance);

		UpdateActiveMember(ActiveMember, TRUE);
	}
}


void CActiveMemberList::Clear()
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	m_ActiveMemberMap.RemoveAll();
}

INT_PTR CActiveMemberList::GetCount()
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	return m_ActiveMemberMap.GetCount();
}

void CActiveMemberList::StartEnumActiveMember()
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	m_CurPos = m_ActiveMemberMap.GetStartPosition();
}

BOOL CActiveMemberList::GetEachActiveMember(CActiveMember& ActiveMember)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	if (NULL == m_CurPos)
	{
		return FALSE;
	}

	UINT nMemberId = 0;

	m_ActiveMemberMap.GetNextAssoc(m_CurPos, nMemberId, ActiveMember);

	return TRUE;
}

//使用MemberId查找用户
BOOL CActiveMemberList::GetActiveMember(UINT nMemberId, CActiveMember& ActiveMember)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	return m_ActiveMemberMap.Lookup(nMemberId, ActiveMember);
}

//使用TermId查找用户,可使用不完全匹配,i.e.目标用户的TermId中包含参数strTermId即可.
BOOL CActiveMemberList::GetActiveMember(CString strTermId, CActiveMember& ActiveMember, BOOL bFullMatch)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	strTermId.MakeUpper();

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		// 2011/06/07-gxx: 修改为模糊查找哦
		CString strTerm = ActiveMember.GetTerminalID();

		strTerm.MakeUpper();

		if (bFullMatch)
		{
			if ( strTerm == strTermId )
			{
				return TRUE;
			}
		}
		else
		{
			if (strTerm.Find(strTermId, 0) >= 0)
			{
				return TRUE;
			}
		}
		
	}

	return FALSE;
}

/*************************************************
Function:       // GetActiveMemberBest
Description:    // 从ActiveMemberMap中根据strTermId查找最佳匹配的元素。
				// 匹配优先级：字串完全一致且UpdateDataTime最近 > 字串完全一致 > 用户在线且UpdateDataTime最近 > UpdateDataTime最近 
				// > 字串最短
				// 当正在进行比较的优先级只有1个选项时，就表示查找成功
Calls:          // None
Table Accessed: // None
Table Updated:  // None
Input:          // strTermId - 要查找的终端号
                // bFullMatch - 是否只查找完全匹配，FALSE表示只要目标中包含所查找终端号即算匹配
Output:         // ActiveMember - 查找到的最匹配的结果
Return:         // TRUE
Others:         // None
*************************************************/
BOOL CActiveMemberList::GetActiveMemberBest(CString strTermId, CActiveMember& ActiveMember, BOOL bFullMatch/*=TRUE*/)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	strTermId.MakeUpper();

	vector<CActiveMember> vecRes;
	vector<CActiveMember> vecEqual;
	vector<CActiveMember> vecSame;

	COleDateTime dt;
	CActiveMember AMRes;

	int nFound = 0;

	//遍历Map，寻找与strTermId相同的元素，推入vector
	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember AM;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, AM); //pos为Map中下一个位置,nMemberId为获取元素的Key值,AM为所获取元素

		// 2011/06/07-gxx: 修改为模糊查找
		CString strTerm = AM.GetTerminalID();

		strTerm.MakeUpper();

		BOOL bFound = FALSE;
		if (bFullMatch)
		{
			if ( strTerm == strTermId )
			{
				bFound = TRUE;		
			}
		}
		else
		{
			if (strTerm.Find(strTermId, 0) >= 0)
			{
				bFound = TRUE;
			}
		}
		if (bFound)
		{
			vecRes.push_back(AM);
			++nFound;
		}
	}

	if (nFound == 0)
	{
		return FALSE;
	}

	if (nFound == 1)        //只有1个对应的，则直接发挥
	{
		ActiveMember = vecRes[0];
		return TRUE;
	}
	
	// nFound > 1
	CString strTerm = vecRes[0].GetTerminalID();
	strTerm.MakeUpper();
	int nMin = 0;
	//遍历所有找到的元素，将与输入参数完全相同的元素放入vecEqual，将与第一个终端号完全相等的元素放到vecSame
	for(int i = 0; i < nFound; i++)
	{
		CString strTmp = vecRes[i].GetTerminalID();
		strTmp.MakeUpper();
		if (  strTmp == strTermId ) // 和输入的终端完全相同，推入vecEqual
		{
			vecEqual.push_back(vecRes[i]);
		}
		if (vecEqual.size() > 0)    // vecEqual的size不为0，就进入下一个循环，否则继续往下
		{
			continue;
		}
		if ( strTmp == strTerm ) // 和结果里的第一个终端相同，推入vecSame
		{
			vecSame.push_back(vecRes[i]);
		}
		if (strTmp < strTerm)   // 小于结果里的第一个终端，将将其赋值给第一个终端
		{
			nMin = i;
			strTerm = strTmp;
		}
	}

	//如果与输入终端号完全相等的只有1个，则设其为结果，返回
	if (vecEqual.size() == 1)
	{
		ActiveMember = vecEqual[0];
		return TRUE;
	}
	//否则返回其中更新时间最近的
	else if(vecEqual.size() > 1)
	{
		for(size_t i = 0; i < vecEqual.size(); i++)
			if (vecEqual[i].GetUpdateDataTime() > AMRes.GetUpdateDataTime())
			{
				AMRes = vecEqual[i];
			}

		ActiveMember = AMRes;
		return TRUE;
	}
	//没有完全一致的。则遍历长度最短的。
	else if(vecSame.size() > 1)
	{
		// 优先查找在线的用户，如果没有在线的用户，则返回更新时间最近的。只有一个在线的则返回那一个。否则返回更新时间最近的。否则返回长度最短的。
		vector<CActiveMember> vecOnline;
		for(size_t i = 0; i < vecSame.size(); i++)
		{
			if (vecSame[i].GetIsOnLine())
			{
				vecOnline.push_back(vecSame[i]);
			}
		}
		if (vecOnline.size() == 0)
		{
			for(size_t i = 0; i < vecSame.size(); i++)
				if (vecSame[i].GetUpdateDataTime() > AMRes.GetUpdateDataTime())
				{
					AMRes = vecSame[i];
				}

			ActiveMember = AMRes;
			return TRUE;
		}
		else if(vecOnline.size() == 1)
		{
			ActiveMember = vecOnline[0];
			return TRUE;
		}
		else
		{
			for(size_t i = 0; i < vecOnline.size(); i++)
				if (vecOnline[i].GetUpdateDataTime() > AMRes.GetUpdateDataTime())
				{
					AMRes = vecOnline[i];
				}

			ActiveMember = AMRes;
			return TRUE;
		}
	}
	else
	{
		ActiveMember = vecRes[nMin];
		return TRUE;
	}	
}

//使用Net查找用户,可使用不完全匹配,i.e.目标用户的NetId中包含参数strNetId即可.
BOOL CActiveMemberList::GetActiveMemberNetID(CString strNetId,CActiveMember& ActiveMember, BOOL bFullMatch /*=TRUE*/)
{
	IBA_ASSERT(!strNetId.IsEmpty());

	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (bFullMatch)
		{
			if (0 == ActiveMember.GetNetId().CompareNoCase(strNetId))
			{
				return TRUE;
			}
		}
		else
		{
			CString strOld = ActiveMember.GetNetId();
			strOld.MakeLower();
			strNetId.MakeLower();
			if (strOld.Find(strNetId) >= 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//从需要更新的索引列表中(MemberID)获取当前第一个索引值返回，并在索引列表中删除
BOOL CActiveMemberList::GetUpdateActiveMemberId()
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	UINT nTmp = 0;

	if (!m_UpdateMemberIdArray.IsEmpty())
	{
		nTmp = m_UpdateMemberIdArray.GetAt(0);
		m_UpdateMemberIdArray.RemoveAt(0);
	}

	return nTmp;
}

BOOL CActiveMemberList::RemoveActiveMember(UINT nMemberId)
{	
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	CActiveMember ActiveMember;
	GetActiveMember(nMemberId, ActiveMember);
	IBA_LOG(_T("RemoveActiveMember.MemberId:%d NetId:%s TermId:%s"),nMemberId,ActiveMember.GetNetId(),ActiveMember.GetTerminalID());

	BOOL bRet = m_ActiveMemberMap.RemoveKey(nMemberId);

	if (bRet)
	{
		m_UpdateMemberIdArray.Add(nMemberId);
	}
	RemoveActiveMemberDB(nMemberId);

	//IBA_LOG0(_T("in RemoveActiveMember"));	
	// 2014-3-11 - qsc
	// 深圳网监要求----------------------------------------------------------
	//CString strIniFile = theApp.GetWorkPath();
	//strIniFile.Append(_T("\\IBAConfig\\nde_server.ini")); // base, Filename
	//if (::PathFileExists(strIniFile))
	//{
	//	TCHAR tRes[512] = {0};
	//	::GetPrivateProfileString(_T("base"), _T("Filename"), NULL, tRes, 512, strIniFile);
	//	if (::PathFileExists(tRes))
	//	{
	//		/*
	//		添加用户:add|用户名|密码

	//		删除用户:del|用户名

	//		*/
	//		CString strParam;
	//		strParam.Format(_T("del|%s"), ActiveMember.GetNetId());

	//		IBA_LOG(_T("执行: %s %s"), tRes, strParam);

	//		ShellExecute(NULL, _T("open"), tRes, strParam, NULL, SW_SHOWNORMAL);
	//	}
	//	else
	//	{
	//		IBA_LOG(_T("执行文件:%s 不存在"), tRes);;
	//	}
	//}
	//else
	//{
	//	IBA_LOG(_T("配置文件:%s 不存在"), strIniFile);
	//}	

	//---------------------------------------------------------------------

	// 查询实名在线表-20151230 liyajun避免上机前就实名下机,暂时取消
	//try
	//{
	//	CString strSQL;

	//	strSQL.Format(_T("select * from auditinfo where MEMBERID=%d order by LogonTime desc"), nMemberId);

	//	IBA_TRACE(strSQL);

	//	CADODBConnInfo* pDb = NULL;

	//	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb))
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Result :GetDBConnInfo for quering in auditinfo failed."));
	//		return FALSE;
	//	}

	//	CADORecordset Rs(pDb->GetConn());
	//	if (Rs.Open(strSQL)) 
	//	{
	//		bRet = TRUE;
	//	}
	//	else
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Result :quering in auditinfo failed!"));
	//		pDb->Release();
	//		lock.Unlock();
	//		return bRet;
	//	}

	//	int nCount = Rs.GetRecordCount();
	//	if (nCount == 0)
	//	{
	//		Rs.Close();
	//		pDb->Release();
	//		lock.Unlock();
	//		return bRet;
	//	}

	//	CString strTermID, strIP;
	//	Rs.GetFieldValue(_T("MACHNAME"), strTermID);
	//	Rs.GetFieldValue(_T("IP"), strIP);
	//	Rs.Close();
	//	pDb->Release();

	//	if (strTermID.IsEmpty() || strIP.IsEmpty())
	//	{
	//		IBA_LOG0(_T("实名上机信息，终端号和IP为空"));
	//	}
	//	else
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - AuditUserCheckOut ..."));
	//		theApp.GetRealName()->AuditUserCheckOut(strTermID, strIP, nMemberId);
	//	}
	//}
	//catch (...)
	//{
	//	IBA_LOG0(_T("Check db, throw a exception..."));
	//	bRet = FALSE;
	//}

	lock.Unlock();

	return bRet;
}

//激活时删除用户，不发送下机消息
BOOL CActiveMemberList::RemoveActiveMemberDBPostActivation(UINT nMemberId)
{
	//IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Start for MemberId = %d"),nMemberId);	
	CSingleLock lock(&ActiveMemberListLock, TRUE);



	CActiveMember ActiveMember;
	GetActiveMember(nMemberId, ActiveMember);

	BOOL bRet = m_ActiveMemberMap.RemoveKey(nMemberId);

	if (bRet)
	{
		m_UpdateMemberIdArray.Add(nMemberId);
	}

	RemoveActiveMemberDB(nMemberId);


	//IBA_LOG0(_T("in RemoveActiveMember"));	
	// 2014-3-11 - qsc
	// 深圳网监要求----------------------------------------------------------
	//CString strIniFile = theApp.GetWorkPath();
	//strIniFile.Append(_T("\\IBAConfig\\nde_server.ini")); // base, Filename
	//if (::PathFileExists(strIniFile))
	//{
	//	TCHAR tRes[512] = {0};
	//	::GetPrivateProfileString(_T("base"), _T("Filename"), NULL, tRes, 512, strIniFile);
	//	if (::PathFileExists(tRes))
	//	{
	//		/*
	//		添加用户:add|用户名|密码

	//		删除用户:del|用户名

	//		*/
	//		CString strParam;
	//		strParam.Format(_T("del|%s"), ActiveMember.GetNetId());

	//		IBA_LOG(_T("执行: %s %s"), tRes, strParam);

	//		ShellExecute(NULL, _T("open"), tRes, strParam, NULL, SW_SHOWNORMAL);
	//	}
	//	else
	//	{
	//		IBA_LOG(_T("执行文件:%s 不存在"), tRes);;
	//	}
	//}
	//else
	//{
	//	IBA_LOG(_T("配置文件:%s 不存在"), strIniFile);
	//}	

	//---------------------------------------------------------------------

	// 查询实名在线表
	//激活后删除激活用户时，不发下机消息
	//try
	//{
	//	CString strSQL;

	//	strSQL.Format(_T("select * from auditinfo where MEMBERID=%d order by LogonTime desc"), nMemberId);

	//	IBA_TRACE(strSQL);
	//	IBA_LOG0(strSQL);

	//	CADODBConnInfo* pDb = NULL;

	//	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb))
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Result :GetDBConnInfo for quering in auditinfo failed."));
	//		return FALSE;
	//	}

	//	CADORecordset Rs(pDb->GetConn());
	//	if (Rs.Open(strSQL)) 
	//	{
	//		bRet = TRUE;
	//		//IBA_LOG(_T("Execute %s success."), strSQL);
	//	}
	//	else
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Result :quering in auditinfo failed!"));
	//		pDb->Release();
	//		lock.Unlock();
	//		return bRet;
	//	}

	//	//IBA_LOG0(_T("GetRecordCount ..."));
	//	int nCount = Rs.GetRecordCount();
	//	if (nCount == 0)
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - Result :quering auditinfo count = 0"));
	//		Rs.Close();
	//		pDb->Release();
	//		lock.Unlock();
	//		return bRet;
	//	}

	//	//IBA_LOG0(_T("Get record values ..."));
	//	CString strTermID, strIP;
	//	Rs.GetFieldValue(_T("MACHNAME"), strTermID);
	//	Rs.GetFieldValue(_T("IP"), strIP);
	//	Rs.Close();
	//	pDb->Release();

	//	if (strTermID.IsEmpty() || strIP.IsEmpty())
	//	{
	//		IBA_LOG0(_T("实名上机信息，终端号和IP为空"));
	//	}
	//	else
	//	{
	//		IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - AuditUserCheckOut ..."));
	//		theApp.GetRealName()->AuditUserCheckOut(strTermID, strIP, nMemberId);
	//	}
	//}
	//catch (...)
	//{
	//	IBA_LOG0(_T("Check db, throw a exception..."));
	//	bRet = FALSE;
	//}
	
	//CComputerInfo computer;
	//if (CLocalServer::GetInstance()->ComputerList.GetComputerInfo(ActiveMember.GetTerminalID(), computer))
	//{
	//	//CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
	//	theApp.GetRealName()->AuditUserCheckOut(
	//		computer.GetTerminalID(), computer.GetComputerIP(),nMemberId);
	//}
	//}

	// 2014-2-25 - qsc
	// 2014-1-13-qsc 解决包时问题
	//INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);

	//if (nIndex >= 0) //远程下机
	//{
	//	CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
	//	computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
	//	computer.ClearUserInfo();
	//	CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);

	//	//CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
	//}


	lock.Unlock();


	//IBA_LOG0(_T("CActiveMemberList::RemoveActiveMember - End"));
	return bRet;
}

void CActiveMemberList::Copy(ActiveMemberMap& OtherActiveMemberMap)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;

		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		OtherActiveMemberMap.SetAt(ActiveMember.GetMemberID(), ActiveMember);
	}
}

void CActiveMemberList::Copy(CArray<CActiveMember>& arrMembers)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;

		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		arrMembers.Add(ActiveMember);
	}
}
//******************************************************************************
/*************************************************
Function:       // Run
Description:    // 线程函数，每隔30s更新所有激活用户的数据(主要在ActiveMember中)..
Calls:          // CheckAllActiveMember
Others:         // 该线程从启动起运行，程序结束时退出
*************************************************/
INT CActiveMemberList::Run()
{
	IBA_LOG(_T("Start thread:Check offline ActiveMember"));
	CoInitialize(NULL);

	while (TRUE)
	{

#ifdef _DEBUG
		Sleep(1000 * 5);
#else
		Sleep(1000 * 30);
#endif // _DEBUG

		//try
		//{
		if (theApp.GetCurCashier()->IsOnDuty())
		{
			CheckAllActiveMember();
		}

		RefreshView();
		//}
		//catch (...)
		//{			
		//	DWORD dwErrorCode = GetLastError();
		//	char szErrorMsg[1024];
		//	GetErrorMessage((int)dwErrorCode,szErrorMsg);
		//	int i = MultiByteToWideChar(CP_UTF8, 0, szErrorMsg, -1, NULL, 0);
		//	WCHAR* strSrc;
		//	strSrc = new WCHAR[i+1];
		//	MultiByteToWideChar(CP_UTF8, 0, szErrorMsg, -1, strSrc, i);
		//	CIBALog::GetInstance()->WriteFormat(_T("CActiveMemberList::Run Error! ErrorCode:%d, ErrorMsg:%s"),(int)dwErrorCode,strSrc);
		//	delete strSrc;		
		//}
	}

	CoUninitialize();

	CIBALog::GetInstance()->Write(_T("CActiveMemberList::Run Exit!"));

	return 0;
}

/*************************************************
Function:       // CheckAllActiveMember
Description:    // 遍历并检查激活用户
Calls:          // CheckLocalUser,CheckRoomUser,CheckActiveMember,RemoveActiveMember
Table Updated: //  ActiveMember
*************************************************/
void CActiveMemberList::CheckAllActiveMember()
{
	ActiveMemberMap TempActiveMemberMap;

	Copy(TempActiveMemberMap);

	CActiveMember ActiveMember;

	POSITION pos = TempActiveMemberMap.GetStartPosition();

	while (pos != NULL)                                                  //遍历Map中每一个用户
	{
		UINT nMemberId = 0, nCardId = 0;

		CActiveMember ActiveMember;

		TempActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);


		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)         //本地模式
		{
			if(ActiveMember.IsNeedToQuery())
			{
				CheckBrokenCenterUser(ActiveMember);
			}				
		}

		else if (ActiveMember.GetMemberID() == ActiveMember.GetRoomMemberId())   //正常模式，先检查包房用户
		{
			CheckRoomUser(ActiveMember);
		}
		//其他情况往中心查询，
		else
		{
			if (ActiveMember.IsNeedToQuery())    
			{
				CIDCheckCardMethod checkCard;

				checkCard.SetSerialNum(ActiveMember.GetNetId());

				theApp.GetCurCashier()->DoCheckCard(checkCard);  //CheckCard

				if (checkCard.GetStatusCode() == 0)
				{
					nCardId = checkCard.GetCardId();
					nMemberId = checkCard.GetMemberId();
				}

				if (nMemberId > 0)
				{
					CheckActiveMember(nCardId, nMemberId, ActiveMember);
				}
				//检查未激活用户，直接Remove
				else
				{
					if (checkCard.GetStatusCode() != - 1)
					{
						RemoveActiveMember(ActiveMember.GetMemberID());
						CIBALog::GetInstance()->Write(_T("-------------------清掉用户------------------"));
						// 2014-3-28 - qsc QSCTAG 暂时注释
						// 2014-1-13-qsc 
						//CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
					}
				}

				Sleep(50);
			}				
		}	
	}
}

/*************************************************
Function:       //CheckRoomUser
Description:    //检查包房用户。本地掉线超过2分钟，自动结账。
Table Updated: //  ActiveMember
Input:          //ActiveMember - 待检查的包房用户
*************************************************/
void CActiveMemberList::CheckRoomUser(CActiveMember& ActiveMember)
{
	//还有钱，CheckCard，然后像其他用户一样CheckActiveMember
	if (ActiveMember.GetAvailavleBalance() > 0)
	{
		if (ActiveMember.IsNeedToQuery())//如果需要查询
		{
			CIDCheckCardMethod checkCard;

			checkCard.SetSerialNum(ActiveMember.GetNetId());

			theApp.GetCurCashier()->DoCheckCard(checkCard);

			UINT nCardId = 0;
			UINT nMemberId = 0;
			if (checkCard.GetStatusCode() == 0)
			{
				nCardId = checkCard.GetCardId();
				nMemberId = checkCard.GetMemberId();
			}

			if (nMemberId > 0)
			{
				CheckActiveMember(nCardId, nMemberId, ActiveMember);
			}
			else
			{
				if (checkCard.GetStatusCode() != - 1)
				{
					RemoveActiveMember(ActiveMember.GetMemberID());
					CIBALog::GetInstance()->Write(_T("-------------------清掉用户------------------"));
				}
			}

			Sleep(50);
		}
		return;
	}

	BOOL bCanReturn = FALSE;

	if (ActiveMember.GetStatus() == CActiveMember::EStatus_CenterOnLine ||     //中心在线
		(ActiveMember.GetStatus() == CActiveMember::EStatus_LocalOnline && ActiveMember.IsOffLinePossibility())) //本地在线但可能掉线了
	{
		BOOL bCanAuto = FALSE;
		if (ActiveMember.GetUpdateDataTime().GetStatus() != COleDateTime::valid)
		{
			bCanAuto = TRUE;
		}
		else
		{
			COleDateTimeSpan dtSpan = COleDateTime::GetCurrentTime() - ActiveMember.GetUpdateDataTime(); 

			if (dtSpan.GetTotalMinutes() >= 2.0) // 2分钟
			{
				bCanAuto = TRUE;
			}
		}

		if (bCanAuto)
		{
			ActiveMember.SetStatus(CActiveMember::EStatus_RoomAutoReturn); // 状态改为包房自动退款状态
		}
		else
		{
			return;
		}
		

		CIDCheckCardMethod checkCard;
		checkCard.SetSerialNum(ActiveMember.GetNetId());
		theApp.GetCurCashier()->DoCheckCard(checkCard);
		UINT nCardId = 0;
		UINT nMemberId = 0;
		if (checkCard.GetStatusCode() == 0)
		{
			nCardId = checkCard.GetCardId();
			nMemberId = checkCard.GetMemberId();
		}
		if (nMemberId >0 )
		{
			CBalanceMethod Balance;
			Balance.SetCardId(nCardId);
			Balance.SetMemberId(nMemberId);

			theApp.GetCurCashier()->DoBalance(Balance);
			if (Balance.GetStatusCode()!=0)
			{
				return;
			}

			if (Balance.GetOnline() == 1)
			{
				// 把更新时间改为可登出的时间，下一次包房做自动退款时判断
				ActiveMember.UpdateDataTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetLogoutTime());

				CString strTmp = CIBAHelpper::FormatTime(ActiveMember.UpdateDataTime);
				UpdateActiveMember(ActiveMember, TRUE);

				IBA_LOG(_T("中心返回包房自动退款的最终时间(MemberID=%d):%s"), ActiveMember.GetMemberID(), strTmp);
			}
			else
			{
				bCanReturn = TRUE;
			}
			
		}
		else
		{
			bCanReturn = FALSE;
		}
	}

	if (!bCanReturn && ActiveMember.GetStatus() == CActiveMember::EStatus_RoomAutoReturn)
	{
		CString strTmp1,strTmp2;
		strTmp1 = CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime());
		strTmp2 = CIBAHelpper::FormatTime(ActiveMember.GetUpdateDataTime());
		if (COleDateTime::GetCurrentTime() >= ActiveMember.GetUpdateDataTime())
		{
			bCanReturn = TRUE;
		}
	}
		
	if (bCanReturn)
	{
		// 执行包房结账
		CRoomCheckOut roomCheckOut;
		roomCheckOut.SetMemberId(ActiveMember.GetMemberID());
		roomCheckOut.SetPCClass(ActiveMember.GetPCClassID());

		theApp.GetCurCashier()->DoRoomCheckOut(roomCheckOut);

		if (roomCheckOut.GetStatusCode() == 0)
		{
			BOOL bRead = GetActiveMember(ActiveMember.GetMemberID(),ActiveMember);
			CArray<UINT> arrMembers;
			GetRoomSMembers(ActiveMember.GetMemberID(), arrMembers);

			// 输出包房结账的操作日志
			{
				COperationInfo OperationInfo;
				OperationInfo.SetOperationType(CIBAGlobal::emOpTypeRoomReturn);
				OperationInfo.SetIsBundTime(FALSE);

				if (bRead)
				{
					OperationInfo.SetUserName(ActiveMember.GetUserName());
					OperationInfo.SetNetId(ActiveMember.GetNetId());
					OperationInfo.SetMemberId(ActiveMember.GetMemberID());
					OperationInfo.SetTermId(ActiveMember.GetTerminalID());
					OperationInfo.SetIdNum(ActiveMember.GetNetId());
				}

				CString strTmp;
				strTmp += ActiveMember.GetPCClass();
				strTmp += _T(": ");
				strTmp.AppendFormat(LOAD_STRING(IDS_ROOM_BODY), arrMembers.GetCount());
				OperationInfo.SetDescription(strTmp);

				OperationInfo.SetCheckInTime(ActiveMember.GetActivationTime()); // 包房的开通时间

				OperationInfo.SetClassId(ActiveMember.GetClassId());

				::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
				
				IBA_LOG(_T("包房自动退款成功!MemberID=%d"), ActiveMember.GetMemberID());
			}

			
		}
		
		ClearRoomMembers(ActiveMember.GetRoomMemberId());
	}
}

/***************************************************************************************************************************************************
Function:       //CheckActiveMember
Description:    //为满足IsNeedQuery的用户检查和更新用户信息(正常模式下)
				//DoBalance
				//用户在线，设置为EStatus_CenterOnLine(IsNeedQuery:curtime - updatetime>0就需要查询,表示下次继续查询)
				//用户不在线，处理详见ProcessActiveMemberInAbnormalCases
				//根据Balance更新用户Amout和AvailableBalance信息..
Input:          // nCardId      -  用于向中心请求Balance
                // nMemberId    -  用于向中心请求Balance
Output:         //ActiveMember - 更新后的用户信息
Others:			//配合IsNeedQuery,只有EStatus_CenterOnLine - 换机用户/包时未上机超过90s且到时用户,(包时用户将设置updatetime为到时时间,如时间未到,将不满足IsNeedQuery
					条件,不会进入查询)以及EStatus_LocalOnLine - 正常在线用户不在线超过90s才会执行.
***************************************************************************************************************************************************/
void CActiveMemberList::CheckActiveMember(UINT nCardId, UINT nMemberId, CActiveMember &ActiveMember)
{
	//查余额
	CBalanceMethod Balance;
	Balance.SetCardId(nCardId);
	Balance.SetMemberId(nMemberId);

	theApp.GetCurCashier()->DoBalance(Balance);

	if (Balance.GetStatusCode() == 1) //无效用户
	{
		IBA_LOG(_T("Temp.CheckActiveMember:RemoveActiveMember-MemberId = %d 无效用户"),ActiveMember.GetMemberID());
		RemoveActiveMember(nMemberId);
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
		return;
	}	
	if (Balance.GetStatusCode() != 0) return; //失败则退出
	
	ActiveMember.UpdateDataTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetLogoutTime());//如果已经登出了，则要使用登出时间


	//用户在线，表示是换机，设置为中心在线
	if (Balance.GetOnline() == 1) 
	{
		ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);
	}

	else//用户不在线,确认和处理
	{
		if(ProcessActiveMemberInAbnormalCases(Balance,ActiveMember)) { return;}
	}


	//从中心获取的新的余额
	//用户不在线时,本地无法从Iserver获取账户信息.所以从中心Balance接口获取,计算(已消费金额 + 余额) - 新的账户余额 = 新的已消费金额
	INT nAllMoney = Balance.GetTotalBalacne();
	INT nTmp = ActiveMember.GetAmount() + ActiveMember.GetAvailavleBalance() - nAllMoney; 
	ActiveMember.SetAmount(nTmp > 0 ? nTmp : 0);   //设置用户消费金额
	ActiveMember.SetAvailavleBalance(nAllMoney);   //设置用户当前余额
	UpdateActiveMember(ActiveMember, TRUE);
	CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

	IBA_LOG(_T("Temp.CheckActiveMember.SetAvailableBalance = %d, for MemberId = %d"),ActiveMember.GetAvailavleBalance(),ActiveMember.GetMemberID());	
}

/***************************************************************************************************************************************************
Function:       //ProcessActiveMemberInAbnormalCases
Description:    //CheckActiveMember中，若用户换机/不在线超过一定时间(时间在IsOffLinePossibility中设置),做特定的确认和处理.
					1.会员用户/临时用户但后台设置不允许退款,直接从激活列表中移除.(i.e.说明a.会员用户再次上机时可以自动激活,b.临时用户不允许换机)
					2.其他
						a.累计封顶且余额已为0,直接退款注销
						b.累计封顶/足额预扣,更新updatetime为到期时间
						c.正常计费余额为0,符合退款条件，自动退款.否则不进行处理,一直显示在用户视图,待收银员处理.
						d.正常计费还有钱,设状态为EStatus_Waiting,不再进行检查
Input:          //Balance - 中心返回的账户信息
Return:         //移除了激活用户，或自动退款了，返回TRUE.否则返回FALSE.
***************************************************************************************************************************************************/
BOOL CActiveMemberList::ProcessActiveMemberInAbnormalCases(CBalanceMethod& Balance,CActiveMember &ActiveMember)
{
	if (Balance.IsNetbarMember()) // 会员或是不允许退款，取消激活
	{
		IBA_LOG(_T("Temp.CheckActiveMember:RemoveActiveMember-MemberId = %d 会员/不允许退款&不在线"),ActiveMember.GetMemberID());
		RemoveActiveMember(Balance.GetMemberId());
		return TRUE;
	}
	else //允许退款的临时用户
	{
		if (Balance.GetPayType() > 0) //累计封顶/足额预扣
		{
			//用户不在线，累计封顶，余额为0，自动退款
			if (Balance.GetPayType() == CIBAGlobal::emPayTypeAccumulative && Balance.GetTotalBalacne() == 0)
			{
				IBA_LOG(_T("Temp.CheckActiveMember:DeregisterUser-MemberId = %d 临时用户&累计封顶&不在线&无余额&允许退款"),ActiveMember.GetMemberID());
				CString strErrMsg;
				if(FALSE == CDeregisterHelpper::DeregisterUser(Balance.GetMemberId(),strErrMsg))
				{
					IBA_LOG(_T("自动退款失败.MemberId=%d,错误:%s"),Balance.GetMemberId(),strErrMsg);
				}
				CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
				return TRUE;
			}

			/*其他，设置为EStatus_CenterOnLine.一旦包时时间到,
				1.中心会更改其PayType=0,
				2.由于CurrentTime > Balance.EndTime(IsNeedQuery将允许进入Check函数)
			因此,将会进入正常计费的处理逻辑,因此这里只需设置updatetime = 包时结束时间,不需要更多处理.*/
			ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);

			ActiveMember.UpdateDataTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetEndTime());
		}
		else //正常计费
		{	
			//还有钱或者有押金,改为EStatus_Waiting,不再查询
			if (Balance.GetTotalBalacne() > 0 || Balance.GetGuarantyMoney() > 0)
			{
				ActiveMember.SetStatus(CActiveMember::EStatus_Waiting); 
				ActiveMember.SetTerminalID(_T(""));
				//ActiveMember.SetActive(FALSE);				
			}
			else //没有钱了，符合退款条件，自动退款.   ---注意：当没有钱了，但没有自动退款时，要检查后台设置看是否设置了相关选项导致AllowAutoRefund返回false.
			{
				if(CDeregisterHelpper::AllowAutoRefund(Balance))//其中一条,一定要已经下机的，且下机时间超过收银台设置的自动退款时间.另外，后台设置了不允许退款，这里也不会退款.
				{
					ActiveMember.SetStatus(CActiveMember::EStatus_AutoReturn); 
				}
				if (ActiveMember.GetStatus() == CActiveMember::EStatus_AutoReturn)
				{
					IBA_LOG(_T("Temp.CheckActiveMember:DeregisterUser-MemberId = %d 临时用户&不在线&无余额&允许退款&达设定时间"),ActiveMember.GetMemberID());
					CString strErrMsg;
					if(FALSE == CDeregisterHelpper::DeregisterUser(Balance.GetMemberId(),strErrMsg))
					{
						IBA_LOG(_T("自动退款失败.MemberId=%d,错误:%s"),Balance.GetMemberId(),strErrMsg);
					}
					CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
					return TRUE;
				}
				else
				{
					//没钱了,但设置了不允许自动退款,不进行动作,反复查询
				}
			}
		}
	}
	return FALSE;
}

BOOL CActiveMemberList::AddActiveMemberDB(CActiveMember &ActiveMember)
{
	CString strSQL;

	//先删除同一个账号的记录，可能是以前退款没有成功删除掉的
	strSQL.Format(_T("delete from activemember where netId = '%s'"), ActiveMember.GetNetId());
	CIBADAL::GetInstance()->ExecuteSQL(strSQL);

	// 2011/03/29-gxx: 添加收银台ID-使用lastNetBarId字段
	// 2011/05/19-gxx: 添加是否开户包时字段amount, registerBT
	// 2011/07/08-8201-gxx: 增加dynamicMaxAmount, isLocal, password,(断网模式需求)
	// 2011/07/12-8201-gxx: 增加netIdAlias,账号别名
	// 2011/10/25-8230-gxx: 增加包房房主的用户号
	strSQL = _T("insert into activemember(memberId, netId, lastRemainAmount, advancePayment, ");
	strSQL.Append(_T("creditProcess, name, sex, idType, idNumber, classId, activationTime, status,"));
	strSQL.Append(_T("lastNetBarId, netBarId, amount, registerBT, "));
	strSQL.Append(_T("dynamicMaxAmount, isLocal, password, netIdAlias, "));
	strSQL.Append(_T("roomMemberId, "));
	strSQL.Append(_T("scanType "));
	strSQL.Append(_T(") values("));

	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetMemberID());
	strSQL.AppendFormat(_T("'%s',"), ActiveMember.GetNetId());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetAvailavleBalance());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetAdvancePayment());

	strSQL.AppendFormat(_T("'%s',"), ActiveMember.GetCreditProcess());
	strSQL.AppendFormat(_T("'%s',"), ActiveMember.GetUserName());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetSex());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetIdType());

	strSQL.AppendFormat(_T("'%s',"), ActiveMember.GetPersonalID());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetClassId());
	strSQL.AppendFormat(_T("'%s',"), ActiveMember.ActivationTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetStatus()); //初始为待上机
	
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetCasherID());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetNetbarID());
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetAmount());
	strSQL.AppendFormat(_T("%d,"),  ActiveMember.GetIsRegisterBT());

	// 2011/07/08-8201-gxx: 
	strSQL.AppendFormat(_T("%d,"),  ActiveMember.GetDynamicMaxAmount());
	strSQL.AppendFormat(_T("%d,"),  ActiveMember.GetIsLocalUser());
	
	strSQL.AppendFormat(_T("'%s',"),  ActiveMember.GetPassword());

	// 2011/07/12-8201-gxx: 
	strSQL.AppendFormat(_T("'%s',"),  ActiveMember.GetNetIdAlias());

	// 2011/10/25-8230-gxx: 
	strSQL.AppendFormat(_T("%d,"), ActiveMember.GetRoomMemberId());

	strSQL.AppendFormat(_T("%d"), ActiveMember.GetScanType());

	strSQL.Append(_T(")"));

	TRACE(strSQL);

	return CIBADAL::GetInstance()->ExecuteSQL(strSQL);
}

BOOL CActiveMemberList::UpdateActiveMemberDB(CActiveMember &ActiveMember)
{
	//IBA_LOG(_T("Temp.CActiveMemberList::UpdateActiveMemberDB - Start"));

	CString strSQL;

	strSQL.AppendFormat(_T("update activemember set lastRemainAmount=%d"), ActiveMember.GetAvailavleBalance());

	strSQL.AppendFormat(_T(",amount=%d"), ActiveMember.GetAmount());

	strSQL.AppendFormat(_T(",termId='%s'"), ActiveMember.GetTerminalID());

	strSQL.AppendFormat(_T(",pcClass=%d"), ActiveMember.GetPCClassID());

	strSQL.AppendFormat(_T(",updatetime='%s'"), CIBAHelpper::FormatTime(ActiveMember.UpdateDataTime));

	strSQL.AppendFormat(_T(",isActive=%d"), ActiveMember.IsActive());

	strSQL.AppendFormat(_T(",status=%d"), ActiveMember.GetStatus());

	strSQL.AppendFormat(_T(",activationTime='%s'"), CIBAHelpper::FormatTime(ActiveMember.ActivationTime));

	strSQL.AppendFormat(_T(",checkinTime='%s'"), CIBAHelpper::FormatTime(ActiveMember.CheckInTime));

	strSQL.AppendFormat(_T(",advancePayment=%d,refNo=%d"), ActiveMember.GetAdvancePayment(),2);

	//CIBALog::GetInstance()->WriteFormat(_T(",advancePayment=%d,refNo=%d"), ActiveMember.GetAdvancePayment(),2);

	strSQL.AppendFormat(_T(",creditProcess='%s'"), ActiveMember.GetCreditProcess());

	// 2011/04/25-gxx: 更新上网账号,修改卡号后要刷新账号
	strSQL.AppendFormat(_T(",netId='%s'"), ActiveMember.GetNetId());

	// 2011/05/13-gxx: 
	strSQL.AppendFormat(_T(",remark='%s'"),ActiveMember.GetRemark());

	// 2011/07/11-8201-gxx: 
	strSQL.AppendFormat(_T(", dynamicMaxAmount=%d "),ActiveMember.GetDynamicMaxAmount());
	
	strSQL.AppendFormat(_T(", Password='%s'"),ActiveMember.GetPassword());

	// 2011/10/25-8230-gxx: 
	strSQL.AppendFormat(_T(", roomMemberId=%d"), ActiveMember.GetRoomMemberId());

	strSQL.AppendFormat(_T(" where memberid=%d"), ActiveMember.GetMemberID());

	IBA_LOG(_T("Temp.CActiveMemberList::UpdateActiveMemberDB - %s"),strSQL);

	return CIBADAL::GetInstance()->ExecuteSQL(strSQL);

	//IBA_LOG(_T("Temp.CActiveMemberList::UpdateActiveMemberDB - End"));

}

BOOL CActiveMemberList::RemoveActiveMemberDB(UINT nMemberId)
{
	//IBA_LOG0(_T("enter RemoveActiveMemberDB"));
	CString strSQL;

	strSQL.Format(_T("delete from activemember where memberId = %d or memberId = 0"), nMemberId);

	return CIBADAL::GetInstance()->ExecuteSQL(strSQL);
	//IBA_LOG0(_T("leave RemoveActiveMemberDB"));	
}


BOOL CActiveMemberList::ClearAllMembers()
{
	CString strSQL;

	strSQL = _T("delete from activemember");

	BOOL bRes = CIBADAL::GetInstance()->ExecuteSQL(strSQL);

	RefreshView();

	m_ActiveMemberMap.RemoveAll();

	CLocalServer::GetInstance()->GetActiveMemberView()->
		SendMessage(WM_COMMAND,IDM_MYREFRESH,0);

	return bRes;
}


INT CActiveMemberList::GetDynamicReserve(UINT nCasherID)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	int nSum = 0;

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (ActiveMember.IsMember() || ActiveMember.GetIsLocalUser()) continue;

		const UINT nID = ActiveMember.GetCasherID();

		if (nID == nCasherID || (nCasherID == 1 && nID == 0)) // 1表示主收银台，为0的算作为主收银台的
		{
			nSum += ActiveMember.GetAdvancePayment();
		}
	}

	nSum = nSum / 100 * 100; //取整到元

	return nSum;
}

void CActiveMemberList::AddReLoginRequest(TReLoginRequest& btt)
{
	BOOL bFo = FALSE;

	// 判断是否有重复的
	for (int i = 0; i < m_arrReLoginRequest.GetCount(); i++ )
	{
		if (m_arrReLoginRequest.GetAt(i).memberId == btt.memberId)
		{
			bFo = TRUE;
			break;
		}
	}

	if (!bFo)
	{
		m_arrReLoginRequest.Add(btt);
	}
}

void CActiveMemberList::TryReLogin()
{
	if (m_arrReLoginRequest.GetCount() == 0)
	{
		return;
	}

	for (INT i = 0; i < m_arrReLoginRequest.GetCount(); )
	{
		TReLoginRequest* pReLoginRequest = &(m_arrReLoginRequest.GetAt(i));

		CActiveMember ActiveMember;

		if (!GetActiveMember(pReLoginRequest->memberId, ActiveMember)) //用户不存在
		{
			m_arrReLoginRequest.RemoveAt(i);
			continue;
		}

		if (TReLoginRequest::TRY_MAX <= pReLoginRequest->tryCount) //一分钟后还失败
		{
			m_arrReLoginRequest.RemoveAt(i);
			continue;
		}

		if (ActiveMember.GetRefNo() != pReLoginRequest->lastRefNo) //成功了
		{
			IBA_TRACE("包时成功!");
			m_arrReLoginRequest.RemoveAt(i);
			continue;
		}

		if (GetTickCount() - pReLoginRequest->firstTryTime > 8 * 1000) //大于8S
		{
			CLocalServer::GetInstance()->LocalBundleTime(pReLoginRequest->memberId);

			pReLoginRequest->tryCount++;
		}

		i++; //下一个
	}
}

BOOL CActiveMemberList::IsAllowCredit(UINT nMemberID)
{
	CComputerInfo info;
	if ( CLocalServer::GetInstance()->ComputerList.GetComputerInfo(nMemberID, info) )
	{
		if (info.GetComputerStatus() == CComputerInfo::ECS_SUSPEND)
		{
			theApp.IBAMsgBox(_T("该用户处于挂机中，不允许充值"));
			return FALSE;
		}	
	}
	return TRUE;
}

int CActiveMemberList::GetUserStateByNetId(CString strNetId)
{
	CActiveMember ActiveMember;
	if (!GetActiveMemberNetID(strNetId, ActiveMember, FALSE))
	{
		return CIBAGlobal::emUserStateNULL;
	}
	if (ActiveMember.GetIsLocalUser())
	{
		return CIBAGlobal::emUserStateLocal;
	}
	return CIBAGlobal::emUserStateCenter;
}

BOOL CActiveMemberList::HasLocalUser()
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	while (pos != NULL)
	{
		UINT nMemberId = 0;
		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (ActiveMember.GetIsLocalUser())
		{
			return TRUE;
		}
	}

	return FALSE;
}

//void CActiveMemberList::CheckLocalUser(CActiveMember& ActiveMember)
//{
//	IBA_TRACE("检查本地用户...");
//	
//	if (!ActiveMember.GetIsLocalUser())
//	{
//		IBA_ASSERT2(0,"必须为本地用户");
//		return;
//	}
//
//	if (ActiveMember.GetAvailavleBalance() == 0)
//	{
//		if (ActiveMember.GetPolicyInfo().IsEmpty())   //可以取到扣率就取，取不到到后面再处理
//		{
//			CString strSQL;
//
//			strSQL.Format(_T("select PolicyInfo from ActiveMember where memberId=%d"),(INT)ActiveMember.GetMemberID());
//
//			IBA_TRACE(strSQL);
//
//			CADODBConnInfo* pDb = NULL;
//
//			if (CIBADAL::GetInstance()->GetDBConnInfo(pDb))
//			{
//
//				CADORecordset Rs(pDb->GetConn());
//
//				if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0) 
//				{
//					Rs.GetFieldValue(_T("PolicyInfo"), ActiveMember.SetPolicyInfo());
//					Rs.Close();
//					pDb->Release();
//				}
//				else
//				{
//					pDb->Release();
//				}
//			}
//		}
//
//		UINT nLocalExpense = ActiveMember.GetLocalExpense(FALSE);   //获取本地消费的金额，为Amount或通过扣率计算，不满当前小时剩余时间则计算为当前剩余时间。金额不进行向上补入。
//		nLocalExpense += 5;
//		if (nLocalExpense >= (UINT)ActiveMember.GetAdvancePayment())  //如果本地消费金额 <= 预付款金额 + 5分
//		{
//			CCashierHelper::ReturnLocalUser(ActiveMember);  //存入LocalConsume，发送客户端下机消息，从ActiveMemberMap中删除对应，更新用户视图
//		}
//	}
//}

/*************************************************
Function:       // CheckBrokenCenterUser
Description:    // 本地模式下配合IsNeedQuery检查符合条件的用户(超过90S未收到Iserver更新,换机,包时用户已到期到未上机)
Input:          // ActiveMember - 待检查用户
*************************************************/
void CActiveMemberList::CheckBrokenCenterUser(CActiveMember& ActiveMember)
{
	if (ActiveMember.GetIsLocalUser())
	{
		IBA_ASSERT2(0,"必须为中心用户");
		return;
	}

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		return;
	}

	int nMemberId = ActiveMember.GetMemberID();
	if(nMemberId == 0) return;

	//从localMember获取用户下机时间
	COleDateTime checkoutTime;
	COleDateTime zerotime(1970,1,1,0,0,0);
	CLocalMember localMember;
	if(CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,localMember))  //已经下机
	{
		if(CIBAHelpper::IsValidTime(localMember.checkoutTime) && localMember.checkoutTime > zerotime)
		{
			checkoutTime  = localMember.checkoutTime;
		}		
	}
	else if(CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()) && ActiveMember.GetCheckInTime() > zerotime) //在上机但没下机
	{
		checkoutTime = ActiveMember.GetCheckInTime();
	}	
	else  //新开户用户.未曾上机和下机
	{
		checkoutTime = zerotime;
	}

	//如同CheckActiveMember一样设置
	if(CIBAHelpper::IsValidTime(checkoutTime) && checkoutTime > zerotime)
	{
		ActiveMember.SetUpdateDataTime(checkoutTime);
	}
	else
	{
		ActiveMember.SetUpdateDataTime(zerotime);
	}

	//再次确定用户是否在线
	if (CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()) && ActiveMember.GetCheckInTime() > COleDateTime(2011,1,1,0,0,0))
	{
		ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);
	}
	else
	{

		//先获取后台设置是否可以退款,获取不到的直接返回
		int nParamAllowReturn;
		UINT IsMember = 0;
		int n = CIBADAL::GetInstance()->GetAllowReturn(ActiveMember.GetClassId(),&IsMember);
		if(n >= 0)
		{
			nParamAllowReturn = n;
		}
		else//查询不到是否可以退款信息,暂不处理
		{
			ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);
			return;
		}

		if (IsMember > 0 || !nParamAllowReturn == 0) // 会员或是不允许退款，取消激活
		{
			IBA_LOG(_T("Temp.CheckBrokenCenterUser:RemoveActiveMember-MemberId = %d 会员/不允许退款&不在线"),ActiveMember.GetMemberID());
			RemoveActiveMember(nMemberId);
			return;
		}
		else //允许退款的临时用户
		{
			if (ActiveMember.GetPayType() > 0) //累计封顶/足额预扣
			{
				//用户不在线，累计封顶，余额为0，自动退款
				if (ActiveMember.GetPayType() == CIBAGlobal::emPayTypeAccumulative && ActiveMember.GetAvailavleBalance() == 0)
				{
					IBA_LOG(_T("Temp.CheckBrokenCenterUser:DeregisterUser-MemberId = %d 临时用户&累计封顶&无余额&不在线&允许退款"),ActiveMember.GetMemberID());
					CString strErrMsg;
					if(FALSE == CDeregisterHelpper::DeregisterUser(nMemberId,strErrMsg))
					{
						IBA_LOG(_T("Temp.CheckBrokenCenterUser退款失败-MemberId = %d,错误:%s"),nMemberId,strErrMsg);
					}
					CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
					return;
				}

				/*其他，设置为EStatus_CenterOnLine.一旦包时时间到,
					1.中心会更改其PayType=0,
					2.由于CurrentTime > Balance.EndTime(IsNeedQuery将允许进入Check函数)
				因此,将会进入正常计费的处理逻辑,因此这里只需设置updatetime = 包时结束时间,不需要更多处理.*/
				ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);
			
				time_t time = ActiveMember.GetNextCheckinTime();
				COleDateTime nextdatetime(time);
				if(CIBAHelpper::IsValidTime(nextdatetime) && nextdatetime > zerotime)
				{
					ActiveMember.UpdateDataTime = nextdatetime;
				}
				
			}
			else //正常计费
			{	
				//还有钱或者有押金,改为EStatus_Waiting,不再查询
				if (ActiveMember.GetAvailavleBalance() > 0)
				{
					ActiveMember.SetStatus(CActiveMember::EStatus_Waiting); 
					ActiveMember.SetTerminalID(_T(""));
					//ActiveMember.SetActive(FALSE);				
				}
				else //没有钱了，符合退款条件，自动退款.   ---注意：当没有钱了，但没有自动退款时，要检查后台设置看是否设置了相关选项导致AllowAutoRefund返回false.
				{
					if(CDeregisterHelpper::AllowAutoRefundInLocalMode(ActiveMember,nParamAllowReturn,localMember.checkoutTime))//其中一条,一定要已经下机的，且下机时间超过收银台设置的自动退款时间.另外，后台设置了不允许退款，这里也不会退款.
					{
						ActiveMember.SetStatus(CActiveMember::EStatus_AutoReturn); 
					}
					if (ActiveMember.GetStatus() == CActiveMember::EStatus_AutoReturn)
					{
						IBA_LOG(_T("Temp.CheckBrokenCenterUser:ReturnLocalCenterUser-MemberId = %d 临时用户&无余额&不在线&允许退款&下机达设定时间"),ActiveMember.GetMemberID());
						CString strErrMsg;
						if(FALSE == CDeregisterHelpper::DeregisterUser(nMemberId,strErrMsg))
						{
							IBA_LOG(_T("CheckBrokenCenterUser.自动退款失败.MemberId=%d,错误:%s"),nMemberId,strErrMsg);
						}
						CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
						return ;
					}
					else
					{
						//没钱了,但设置了不允许自动退款,不进行动作,反复查询
					}
				}
			}
		}
	}
	return;
}


	///**************************************************************************************************************/
	//}
	////余额为0 - 不是包时，且没有在进行充值，结账
	////		  - 处于包时，如果包时时间到就结账
	//if (ActiveMember.GetAvailavleBalance() == 0)      //如果余额为0
	//{
	//	if (ActiveMember.GetNextCheckinTime() == 0)
	//	{
	//		//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值，可以先跳过。
	//		BOOL bIsNDCredit(FALSE);
	//		BOOL bIsCredit(FALSE);
	//		LPTSTR lpszWndCaption = new TCHAR[30];
	//		memset(lpszWndCaption,0,30);
	//		HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
	//		if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
	//		{
	//			CString strWndCation(lpszWndCaption);
	//			strWndCation.Trim();
	//			bIsCredit = !strWndCation.CompareNoCase(_T("充值退款"));
	//			bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
	//		}
	//		if(!(bIsCredit || bIsNDCredit))  //充值窗口未弹出
	//		{
	//			//UINT nLocalExpense = ActiveMember.GetLocalExpense(FALSE);
	//			//nLocalExpense += 5;
	//			//if (nLocalExpense >= (UINT)ActiveMember.GetAdvancePayment())
	//			//{
	//			IBA_LOG(_T("Temp.CActiveMemberList::CheckBrokenCenterUser.用户余额为0，结账!"));
	//			CCashierHelper::ReturnLocalCenterUser(ActiveMember);//执行本地模式的结账或下机				
	//		}
	//	}
	//	else //包时
	//	{			
	//		COleDateTime odt((__int64)ActiveMember.GetNextCheckinTime());
	//		COleDateTime curTime = COleDateTime::GetCurrentTime();
	//		if (curTime >= odt)//包时到期
	//		{
	//			IBA_LOG(_T("Temp.CActiveMemberList::CheckBrokenCenterUser.用户包时到期且余额为0，结账!"));
	//			CCashierHelper::ReturnLocalCenterUser(ActiveMember);
	//		}

	//	}
	//}
	////余额不为0 -  会员 -        上机中，但已离线超过1小时，结账
	//else     
	//{
	//	//会员
	//	if (ActiveMember.IsMember())   
	//	{
	//		if(!ActiveMember.GetTerminalID().IsEmpty())  //在上机
	//		{
	//			CComputerInfo ComInfo;
	//			if (CLocalServer::GetInstance()->ComputerList.GetComputerInfo(ActiveMember.GetMemberID(),ComInfo)) //可找到终端信息
	//			{
	//				if (ComInfo.GetComputerStatus() != CComputerInfo::ECS_ONLINE)  //处于离线
	//				{
	//					COleDateTime dtNow = COleDateTime::GetCurrentTime();
	//					COleDateTimeSpan span = dtNow - ActiveMember.GetUpdateDataTime();
	//					IBA_TRACE("会员离线，时间 差=%.2f",span.GetTotalMinutes());
	//					
	//					double dMins = span.GetTotalMinutes();
	//					if (dMins >= 60.0)                                                         //上一次更新时间距现在大于60分钟，结账
	//					{
	//					/*	CCheckOutinfo chInfo;
	//						CCashierHelper::MakeCheckOutInfo(ActiveMember,chInfo);*/
	//						//chInfo.SetCheckOutTime() = CIBAHelpper::FormatTime(ActiveMember.GetUpdateDataTime());
	//						IBA_LOG(_T("Temp.CActiveMemberList::CheckBrokenCenterUser.会员上机中，但是已离线超过1小时，结账!"));
	//						CCashierHelper::ReturnLocalCenterUser(ActiveMember);
	//					}
	//				}
	//			}
	//		}
	//	}

	//	//临时用户
	//	else    
	//	{
	//		//if (!ActiveMember.GetTerminalID().IsEmpty())     //上机中
	//		//{
	//		//	CComputerInfo ComInfo;
	//		//	if (CLocalServer::GetInstance()->ComputerList.GetComputerInfo(ActiveMember.GetMemberID(),ComInfo))  //可找到终端信息
	//		//	{
	//		//		// 临时用户上机，但是离线了
	//		//		if (ComInfo.GetComputerStatus() != CComputerInfo::ECS_ONLINE)    //离线
	//		//		{
	//		//			INT nCost = ActiveMember.GetLocalExpense();
	//		//			if (nCost >= ActiveMember.GetAdvancePayment())               //nLocalExpense >= AdvancePayment，自动结账
	//		//			{
	//		//				ActiveMember.SetAvailavleBalance(0);
	//		//				ActiveMember.SetAmount(ActiveMember.GetAdvancePayment());
	//		//				IBA_LOG(_T("Temp.CActiveMemberList::CheckBrokenCenterUser.临时用户上机中，但是离线了，nLocalExpense >= AdvancePayment，结账!"));
	//		//				CCashierHelper::ReturnLocalCenterUser(ActiveMember);	
	//		//			}
	//		//		}
	//		//	}
	//		//}
	//	}
	//}



INT CActiveMemberList::GetLocalTotalMoney(UINT nCasherID)
{
	CSingleLock lock(&ActiveMemberListLock, TRUE);

	POSITION pos = m_ActiveMemberMap.GetStartPosition();

	int nSum = 0;

	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (!ActiveMember.GetIsLocalUser()) 
			continue;

		const UINT nID = ActiveMember.GetCasherID();

		if (nID == nCasherID || (nCasherID == 1 && nID == 0)) // 1表示主收银台，为0的算作为主收银台的
		{
			nSum += ActiveMember.GetAdvancePayment();
		}
	}

	nSum = nSum / 100 * 100; //取整到元

	return nSum;
}

BOOL CActiveMemberList::FillRoomInfo(CRoomInfo& info)
{
	if (info.GetRoomID() == 0)
	{
		ASSERT(info.GetRoomID() > 0);
		return FALSE;
	}

	UINT nPCClass = info.GetRoomID();
	UINT nRoomMemberID = info.GetMemberID();
	

	POSITION pos = m_ActiveMemberMap.GetStartPosition();
	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (ActiveMember.GetPCClassID() == nPCClass)
		{
			info.SetOnlineCount() += (ActiveMember.GetTerminalID().IsEmpty()?0:1);
		}
		
		if (nRoomMemberID == nMemberId)
		{
			info.SetNetId(ActiveMember.GetNetId());
		}
	}	

	return TRUE;
}

//遍历激活列表中属于该终端区域的所有用户，如果某个用户的RoomMemberId>0，则认为该区域为包房区域。返回RoomMemberId.
BOOL CActiveMemberList::IsRoomArea(UINT pcClass, UINT& nRoomMemberID)
{
	nRoomMemberID = 0;
	POSITION pos = m_ActiveMemberMap.GetStartPosition();
	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (ActiveMember.GetPCClassID() == pcClass && ActiveMember.GetRoomMemberId() > 0)
		{
			nRoomMemberID = ActiveMember.GetRoomMemberId();
			return TRUE;
		}
	}	
	return FALSE;
}

BOOL CActiveMemberList::GetRoomSMembers(UINT nRoomMemberID,CArray<UINT>& arrMembers)
{
	if (nRoomMemberID == 0)
	{
		ASSERT(nRoomMemberID>0);
		return FALSE;
	}
	

	POSITION pos = m_ActiveMemberMap.GetStartPosition();
	while (pos != NULL)
	{
		UINT nMemberId = 0;

		CActiveMember ActiveMember;
		m_ActiveMemberMap.GetNextAssoc(pos, nMemberId, ActiveMember);

		if (ActiveMember.GetRoomMemberId() == nRoomMemberID)
		{
			arrMembers.Add(ActiveMember.GetMemberID());
		}
	}	
	return TRUE;
}

void CActiveMemberList::ClearRoomMembers(UINT nRoomMemberID)
{
	CArray<UINT> arrMembers;
	if (!GetRoomSMembers(nRoomMemberID, arrMembers))
	{
		IBA_LOG0(_T("从在线用户表中读取包房内的所有用户信息失败"));
		//return FALSE;
	}

	IBA_LOG(_T("通知包房内的所有的UDO下机,包房主MemberID=%d"), nRoomMemberID);

	for(int i=0; i < arrMembers.GetCount(); i++)
	{
		UINT nMemberId = arrMembers.GetAt(i);
		CActiveMember ActiveMember;
		GetActiveMember(nMemberId,ActiveMember);

		BOOL bClear = TRUE;
		if (ActiveMember.IsMember())
		{
			if (ActiveMember.GetPayType() > 0)
			{
				bClear = FALSE;
			}
		}
		else
		{
			// 副账号如果有包时状态，或者有余额，那么不可以清掉
			if (ActiveMember.GetPayType() > 0 || ActiveMember.GetAvailavleBalance() > 0)
			{
				bClear = FALSE;
			}
		}
		INT_PTR nIn = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);

		IBA_LOG0(_T("包房主账号结账，副账号跟着被登出和清除"));
		if ( nIn!=-1 )
		{
			// 发送给指定客户端, 登出并不连中心
			CLocalServer::GetInstance()->LocalCheckOutUser(nIn, nMemberId, 2);
			Sleep(500);

			IBA_LOG(_T("包房自动结账，通知UDO下机， TermID=%s MemberID=%d"), ActiveMember.GetTerminalID(), ActiveMember.GetMemberID());
		}

		if (bClear)
		{	
			RemoveActiveMember(nMemberId);

			if (!ActiveMember.IsMember())
			{
				CDeregisterMethod Deregister;

				Deregister.SetMemberId(nMemberId);
				Deregister.SetPersonalId(ActiveMember.GetPersonalID());
				Deregister.SetMemberName(ActiveMember.GetUserName());
				Deregister.SetCardId(0);
				Deregister.SetPassword(_T(""));
				Deregister.SetEndUsingTime(_T(""));

				theApp.GetCurCashier()->DoDeregister(Deregister);

				if (Deregister.GetStatusCode() == 0)
				{	

				}
				else
				{
					IBA_LOG(_T("包房结账时，清理包房nRoomMemberID=%d里的用户MemberID=%d失败"),nRoomMemberID, nMemberId);
				}
			}
		}
		else
		{
			/*ActiveMember.SetRoomMemberId(0);
			ActiveMember.SetRemark(LOAD_STRING(IDS_ROOM_HAS_REFUND));
			UpdateActiveMember(ActiveMember,TRUE);*/
		}
	}

	CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
}
