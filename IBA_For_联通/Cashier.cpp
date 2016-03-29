
#include "stdafx.h"
#include ".\cashier.h"
#include "IBA.h"
#include "MainFrm.h"
#include "IBAHelpper.h"
#include "dal\IBADAL.h"
#include "ActiveMemberView.h"
#include "RealName\IBARealName.h"
#include ".\Socket\LocalServer.h"
#include "Encrypt.h"
#include "Dal\RightsManagement.h"
#include "CashierHelper.h"


CCashier::CCashier(void)
{
	m_bIsBoss = FALSE;
	m_bLogin = FALSE; 
	m_bOnDuty = FALSE;
	m_nDutyID = 0;
	m_nShiftType = 0;
	m_bIsLocalModeLogin = FALSE;

}

CCashier::~CCashier(void)
{
}

//检查最后在线操作者是否不正常
BOOL CCashier::CheckLastDuty(CString& strLastOperator)
{
	CString strTmp;
	strTmp.Format(_T("LastDutyInfo-%d"), CIBAConfig::GetInstance()->GetCashRegisterID());

	CConfigInfo ConfigInfo;
	ConfigInfo.SetKeyString(strTmp);
	
	if (CIBADAL::GetInstance()->GetSystemConfig(ConfigInfo))
	{
		strLastOperator = ConfigInfo.GetKeyValue3();

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCashier::WriteDutyInfo()
{
	CString strTmp;
	strTmp.Format(_T("LastDutyInfo-%d"), CIBAConfig::GetInstance()->GetCashRegisterID());

	CConfigInfo ConfigInfo;
	
	ConfigInfo.SetKeyString(strTmp);
	ConfigInfo.SetKeyName(strTmp);
	ConfigInfo.SetKeyValue1(GetDutyID());
	ConfigInfo.SetKeyValue2(m_nShiftType);
	ConfigInfo.SetDescription(GetOnDutyTime());
	ConfigInfo.SetKeyType(_T("xxxx"));
	if (m_bOnDuty)
	{
		ConfigInfo.SetKeyValue3(GetName());
	}
	else
	{
		ConfigInfo.SetKeyValue3(_T(""));
	}

	CIBADAL::GetInstance()->UpdateSystemConfig(ConfigInfo, 9);

}

BOOL CCashier::HasPopedomOf(LPCTSTR lpszModuleName)
{
	//if (IsBoss()) //老板拥有所有的权限
	//{
	//	return TRUE;
	//}

	//CString strTmp(lpszModuleName);
	//strTmp.Append(_T(","));

	//return (m_strEnableModuleList.Find(strTmp) >= 0);

	return CRightsManagement::GetInstance()->HasRight(lpszModuleName);
}

BOOL CCashier::IsOnDuty()
{
	BOOL bRet = FALSE;

	bRet = m_bLogin && m_bOnDuty;

	return bRet;
}

CString CCashier::GetOnDutyTime() const
{
	if (!m_OnDutyTime.IsEmpty())
	{
		return CIBAHelpper::FormatCenterTime(m_OnDutyTime);
	}

	return m_OnDutyTime;
}

BOOL CCashier::CheckIn(CCashRegisterLoginMethod& CashRegisterLogin, CString strOperator)
{
	BOOL bRet = FALSE;

	CashRegisterLogin.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CashRegisterLogin.SetCashRegisterVersion(CIBALog::GetInstance()->GetAppVer());
	CashRegisterLogin.SetCashRegisterType(1);//普通模式，需要验证，无本地数据库
	CashRegisterLogin.SetUserId(m_strName);
	CashRegisterLogin.SetPassword(m_strPassword);

	time_t tm = time(NULL);
	CString strOrg;
	strOrg.Format(_T("%ld"), tm);

	CIBALog::GetInstance()->WriteFormat(_T("时间戳:%d"), tm);
	CIBALog::GetInstance()->WriteFormat(_T("%s"), m_strPassword);
	
	CIBALog::GetInstance()->WriteFormat(_T("fileHash:%s"), CIBALog::GetInstance()->GetFileHash());

	CString strTmp;
	CEncrypt::CalcMD5(m_strPassword, strTmp);

	CIBALog::GetInstance()->WriteFormat(_T("%s"), strTmp);

	strOrg += CIBALog::GetInstance()->GetFileHash();
	strOrg += strTmp.Left(16);
	strOrg += strTmp.Left(16);
	CString strRes;
	CEncrypt::CalcMD5(strOrg, strRes);
	
	CashRegisterLogin.SetTimeChuo(tm);
	CashRegisterLogin.SetCheckCode(strRes);



	bRet = CashRegisterLogin.Execute();

	if (!bRet) //连接中心失败
	{
		return bRet;
	}

	if (CashRegisterLogin.GetStatusCode() == 0) //执行成功
	{
		bRet = TRUE;

		m_bLogin = TRUE;

		m_bIsBoss = FALSE;
		//
		if (_ttoi(m_strName) == CNetBarConfig::GetInstance()->GetNetBarId())
		{	
			//网吧ID账户登录
			m_bIsBoss = TRUE;
		}

		m_nDutyID = CashRegisterLogin.GetDutyId();

		m_bOnDuty = (m_nDutyID > 0);

		if (!m_bOnDuty)
		{
			m_nDutyID = 1; //如果没有上班，就强制为班次1,从而强制中心交易时检验班次
		}
		
		m_OnDutyTime = CashRegisterLogin.GetOndutyTime();
		m_nShiftType = CashRegisterLogin.GetShiftType();     // 保存班次类型

		CashRegisterLogin.GetUserClassInfoArray(CNetBarConfig::GetInstance()->m_UserClassArray);
		
		CString strSQL;

		for (INT i = 0; i < CNetBarConfig::GetInstance()->m_UserClassArray.GetCount(); i++)
		{
			strSQL.Format(_T("update memberclass set className='%s', isMember=%d where classId=%d"),
				CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassName(),
				CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetIsMember(),
				CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassID());

			CIBADAL::GetInstance()->ExecuteSQL(strSQL);
		}

		CNetBarConfig::GetInstance()->SetSoftwareType(CashRegisterLogin.GetSoftwareType());
		CNetBarConfig::GetInstance()->SetDomainId(CashRegisterLogin.GetDomainId());
		CNetBarConfig::GetInstance()->SetMainHost(CashRegisterLogin.GetMainHost());
		CNetBarConfig::GetInstance()->SetBackUpHost(CashRegisterLogin.GetBackupHost());
		CNetBarConfig::GetInstance()->SetAuditType(CashRegisterLogin.GetAuditType());
		CNetBarConfig::GetInstance()->SetAllowNoCardLogin(CashRegisterLogin.GetAllowNoCardLogin());
		CNetBarConfig::GetInstance()->SetDodoNewHost(CashRegisterLogin.GetDodoNewHost());
		CNetBarConfig::GetInstance()->SetDodonewResource(CashRegisterLogin.GetDodonewResource());
		CNetBarConfig::GetInstance()->SetForeRealName(CashRegisterLogin.GetForeRealName());
		CNetBarConfig::GetInstance()->SetIssueNetBarId(CashRegisterLogin.GetIssueNetBarId());
		CNetBarConfig::GetInstance()->SetNoIDCardIn(CashRegisterLogin.GetNoIDCardIn());
		CNetBarConfig::GetInstance()->SetQueryHost(CashRegisterLogin.GetQueryHost());
		CNetBarConfig::GetInstance()->SetQueryHost2(CashRegisterLogin.GetQueryHost2());
		CNetBarConfig::GetInstance()->SetRealNameFrom(CashRegisterLogin.GetRealNameFrom());
		CNetBarConfig::GetInstance()->SetReturnOnlyClassId(CashRegisterLogin.GetReturnOnlyClassId());
		CNetBarConfig::GetInstance()->SetUpdateInterval(CashRegisterLogin.GetUpdateInterval());
		CNetBarConfig::GetInstance()->SetStandbyHost(CashRegisterLogin.GetStandbyHost());//异地服务器
		CNetBarConfig::GetInstance()->SetBundleTimeWithCash(CashRegisterLogin.GetBundleTimeUseBalance() == 0);
		CNetBarConfig::GetInstance()->SetMustSynchronize(CashRegisterLogin.GetMustSynchronize());
		CNetBarConfig::GetInstance()->SetCreditDirect(CashRegisterLogin.GetAllowCreditNegative() == 1);

		//载入中心配置的控制参数字串
		CNetBarConfig::GetInstance()->PraseControlOption(CashRegisterLogin.GetControlOption());

		//代理商信息
		CNetBarConfig::GetInstance()->SetAgentName(CashRegisterLogin.GetAgentName());
		CNetBarConfig::GetInstance()->SetAgentMobilePhone(CashRegisterLogin.GetAgentMobilePhone());
		CNetBarConfig::GetInstance()->SetAgentregularTel(CashRegisterLogin.GetAgentRegularTel());


		//权限列表
		m_strEnableModuleList = CashRegisterLogin.GetUserPrivileges().MakeUpper();
		m_strEnableModuleList.Append(_T(","));

		NS_DAL::CRightsManagement::GetInstance()->CreateManagement(m_strEnableModuleList, m_bIsBoss);
		
		//从中心载入包时数据
		CNetBarConfig::GetInstance()->m_NetbarBundleTime.Start();

		//清除整个注册表LastTran项，以防造成的干扰
		theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\NetBarInfo\\LastTran"));
	
		WriteLoginInfo(strOperator);//添加用户

		//检验是否需要提交
		if (IsOnDuty())
		{
			if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
			{
				CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
				CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
			}

			CBrokenNetwork::GetInstance()->m_threadRestore->Start();
		}

		// 2011/06/14-gxx: 
		((CMainFrame*)theApp.GetMainWnd())->m_wndOutput.RefreshAllInfoList();

		//{ 2011/07/13-8201-gxx: 备份数据到本地数据库
		LocalInfo.SetCasherAccess(m_strEnableModuleList);
		LocalInfo.SetControlParam(CashRegisterLogin.GetControlOption());
		CBrokenNetwork::GetInstance()->SaveLocalStatusInfo(LocalInfo);

		m_bIsLocalModeLogin = FALSE;
		//}

		// 2011/11/18-8210-gxx: 
		if (IsOnDuty())
		{
			CCashierHelper::InitDiankaPlatform();
		}

		CIBADAL::GetInstance()->AddCashierToDB();
		
	}

	return bRet;
}

BOOL CCashier::DoOnDuty(COnDutyMethod &OnDuty)
{
	BOOL bRet = FALSE;

	OnDuty.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	OnDuty.SetOperator(m_strName);

	bRet = OnDuty.Execute();

	if (OnDuty.GetStatusCode() == 0)
	{
		m_bOnDuty = TRUE;
		m_nDutyID = OnDuty.GetDutyId();
		m_OnDutyTime = OnDuty.GetOnDutyTime();
		m_nShiftType = OnDuty.GetShiftType();
		
		//写注册表，最后在线的收银员，最后在线收银员ID，最后在线收银员序号（以operators保存顺序），最后在线时间
		WriteDutyInfo();

		// 2011/06/03-gxx: 
		if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
		{
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
		}

		CBrokenNetwork::GetInstance()->m_threadRestore->Start();

		// 2011/06/14-gxx: 刷新操作日志
		((CMainFrame*)theApp.GetMainWnd())->m_wndOutput.RefreshAllInfoList();

		CIBADAL::GetInstance()->AddCashierToDB();

		// 2011/11/18-8210-gxx: 
		CCashierHelper::InitDiankaPlatform();
	}

	return bRet;
}

BOOL CCashier::DoOffDuty(COffDutyMethod &OffDuty)
{
	OffDuty.SetDutyId(m_nDutyID);
	OffDuty.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	OffDuty.SetOperator(m_strName);

	BOOL bRet = OffDuty.Execute();

	if (OffDuty.GetStatusCode() == 0)
	{
		m_bLogin = FALSE;
		m_nDutyID = 1;
		m_bOnDuty = FALSE;
		m_nShiftType = 0;
		m_OnDutyTime.Empty();

		WriteDutyInfo();

		((CMainFrame*)theApp.GetMainWnd())->m_wndOutput.DeleteDutyFile();

		// 2011/11/18-8210-gxx: 
		CCashierHelper::UninitDiankaPlatform();
	}

	return bRet;
}

BOOL CCashier::DoRegister(CRegisterMethod &Register)
{
	Register.SetDutyId(m_nDutyID);
	Register.SetOperator(m_strName);
	Register.SetAccountNetbarId(CNetBarConfig::GetInstance()->GetIssueNetBarId());
	Register.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	Register.SetAuditType(CNetBarConfig::GetInstance()->GetAuditType());

	BOOL bRet = Register.Execute();

	Register.SaveTranInfo();

	if (Register.GetStatusCode() == 0)
	{
		// 2011/09/02-8201-gxx: 
		if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
		{
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
		}  // 执行恢复, 补传中心用户在结账后的信息
	}

	return bRet;
}

BOOL CCashier::DoCredit(CCreditMethod & Credit)
{
	Credit.SetDutyId(m_nDutyID);
	Credit.SetOperator(m_strName);
	Credit.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());

	BOOL bRet = Credit.Execute();

	if (!bRet)
	{
		return FALSE;
	}

	if (Credit.GetStatusCode() == 0)
	{
		Credit.SaveTranInfo();

		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
			Credit.GetMemberId(), Credit.GetAmount(), Credit.GetCreditPresent());

		// 2011/09/02-8201-gxx: 
		if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
		{
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
		}  // 执行恢复, 补传中心用户在结账后的信息
	}

	return bRet;
}

BOOL CCashier::DoMemberInfo(CMemberInfoMethod & MemberInfo)
{
	BOOL bRet = MemberInfo.Execute();

	return bRet;
}

BOOL CCashier::DoDeregister(CDeregisterMethod & Deregister)
{
	Deregister.SetDutyId(m_nDutyID);
	Deregister.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	Deregister.SetOperator(m_strName);

	BOOL bRet = Deregister.Execute();

	Deregister.SaveTranInfo();

	if (Deregister.GetStatusCode() == 0 && Deregister.GetReturnMode() != 2)
	{
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Deregister.GetMemberId());
	}

	// 2011/10/27-8210-gxx: 实名下机
	if (Deregister.GetStatusCode() == 0)
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(Deregister.GetMemberId());
		if (nIndex >= 0)
		{
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			theApp.GetRealName()->AuditUserCheckOut(
				computer.GetTerminalID(), computer.GetComputerIP(), Deregister.GetMemberId());
		}	
	}

	return bRet;
}

BOOL CCashier::DoBalance(CBalanceMethod & Balance)
{
	//2011-03-22-gxx-添加是否实际在线的设置
	CActiveMember ActiveMember;

	if( CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
		Balance.GetMemberId(), ActiveMember) )
	{
		// 2011/06/14-gxx: 本地在线，且上报时间在10分钟之内，则认为该用户实际在线
		BOOL bOnLineInFact = FALSE;
		if (ActiveMember.GetStatus() == CActiveMember::EStatus_LocalOnline)		
		{
			COleDateTime dtTmp = ActiveMember.GetUpdateDataTime() + COleDateTimeSpan(0, 0, 10, 0);
			bOnLineInFact =( dtTmp >= COleDateTime::GetCurrentTime() );
		}
		
		// 2011/08/03-8201-gxx: 临时用户，是否实际在线一直传1
		if (ActiveMember.IsMember() == FALSE)
		{
			Balance.SetOnlineInFact(1);
		}
		Balance.SetOnlineInFact(bOnLineInFact);
	}
	else
	{
		IBA_TRACE("获取会员失败");
		Balance.SetOnlineInFact(FALSE);
	}
	//

	BOOL bRet = Balance.Execute();


	if (Balance.GetStatusCode() == 0)
	{
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
			Balance.GetMemberId(), ActiveMember))
		{
			ActiveMember.SetClassId(Balance.GetUserClass());
			ActiveMember.SetAvailavleBalance(Balance.GetTotalBalacne());
			
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
		}

		CIBADAL::GetInstance()->UpdateMemberInfo(Balance);
	}

	return bRet;
}

BOOL CCashier::DoCheckCard(CIDCheckCardMethod& CheckCard)
{
	CheckCard.SetOperator(m_strName);

	BOOL bRet = CheckCard.Execute();

	return bRet;
}

BOOL CCashier::DoResetPwd(CResetPwdMethod & ResetPwd)
{
	ResetPwd.SetOperator(m_strName);

	BOOL bRet = ResetPwd.Execute();

	return bRet;
}

BOOL CCashier::DoActivation(CActivationMethod & Activation)
{
	BOOL bRet = FALSE;

	bRet = Activation.Execute();

	return bRet;
}

BOOL CCashier::DoStoreIn(CStoreInMethod & StoreIn)
{
	StoreIn.SetDutyId(m_nDutyID);
	StoreIn.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	StoreIn.SetOperator(m_strName);

	BOOL bRet = StoreIn.Execute();

	return bRet;
}

BOOL CCashier::DoShiftTotal(CShiftTotalMethod & ShiftTotal)
{//换班方法
	ShiftTotal.SetDutyId(m_nDutyID);
	ShiftTotal.SetOperator(m_strName);
	ShiftTotal.SetNeedBalance(CIBAConfig::GetInstance()->GetDynamicReserve());

	BOOL bRet = ShiftTotal.Execute();

	return bRet;
}

BOOL CCashier::DoCreditReverse(CCreditReverseMethod & CreditReverse)
{
	CreditReverse.SetOperator(m_strName);
	CreditReverse.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CreditReverse.SetDutyId(m_nDutyID);

	BOOL bRet = CreditReverse.Execute();

	return bRet;

}

BOOL CCashier::DoCheckout2(CCheckout2Method & Checkout2)
{
	Checkout2.SetOperator(m_strName);

	BOOL bRet = Checkout2.Execute();

	if (Checkout2.GetStatusCode() == 0)
	{
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Checkout2.GetMemberId());
		CIBADAL::GetInstance()->DeleteLastUser(Checkout2.GetMemberId());

		// 2011/10/27-8210-gxx: 
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(Checkout2.GetMemberId());
		if (nIndex >= 0)
		{
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			theApp.GetRealName()->AuditUserCheckOut(
				computer.GetTerminalID(), computer.GetComputerIP(), Checkout2.GetMemberId());
		}

	}

	return bRet;
}

BOOL CCashier::DoModifyMember(CModifyMemberMethod & ModifyMember)
{
	ModifyMember.SetOperator(m_strName);

	BOOL bRet = ModifyMember.Execute();

	return bRet;
}

BOOL CCashier::DoBundleTimeList(CBundleTimeListMethod & BundleTimeList)
{
	BundleTimeList.SetOperator(m_strName);
	BundleTimeList.SetPosition(0);
	BOOL bRet = BundleTimeList.Execute();

	return bRet;
}

BOOL CCashier::DoBundleTimeAction(CBundleTimeActionMethod & BundleTimeAction)
{
	BOOL bRet = BundleTimeAction.Execute();

	return bRet;
}

BOOL CCashier::DoBundleTime(CBundleTimeMethod & BundleTime)
{
	BundleTime.SetOperator(m_strName);
	BundleTime.SetDutyId(m_nDutyID);
	BundleTime.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	BundleTime.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());

	BOOL bRet = BundleTime.Execute();

	return bRet;
}

BOOL CCashier::DoCancelBundleTime(CCancelBundleTimeMethod & CancelBundleTime)
{
	CancelBundleTime.SetOperator(m_strName);
	CancelBundleTime.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());

	BOOL bRet = CancelBundleTime.Execute();

	return bRet;
}

BOOL CCashier::DoCardReplace(CCardReplaceMethod & CardReplace)
{
	CardReplace.SetOperator(m_strName);
	CardReplace.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CardReplace.SetDutyId(m_nDutyID);

	BOOL bRet = CardReplace.Execute();

	return bRet;
}

BOOL CCashier::DoMergeMember(CMergeMemberMethod & MergeMember)
{
	MergeMember.SetOperator(m_strName);
	MergeMember.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	MergeMember.SetDutyId(m_nDutyID);
	MergeMember.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());

	BOOL bRet = MergeMember.Execute();

	return bRet;
}


BOOL CCashier::DoShiftCredit(CShiftCreditMethod & ShiftCredit)
{
	ShiftCredit.SetOperator(m_strName);
	ShiftCredit.SetDutyId(m_nDutyID);

	BOOL bRet = ShiftCredit.Execute();

	return bRet;
}

BOOL CCashier::DoConsumeList(CConsumeListMethod & ConsumeList)
{
	ConsumeList.SetOperator(m_strName);
	ConsumeList.SetNeedBusinessName(2);

	BOOL bRet = ConsumeList.Execute();

	return bRet;
}

BOOL CCashier::DoTermConsume(CTermConsumeMethod & TermConsume)
{
	TermConsume.SetOperator(m_strName);

	BOOL bRet = TermConsume.Execute();

	return bRet;
}

BOOL CCashier::DoUpgrade(CUpgradeMethod & Upgrade)
{
	Upgrade.SetOperator(m_strName);

	BOOL bRet = Upgrade.Execute();

	return bRet;
}

BOOL CCashier::DoCardCredit(CCardCreditMethod & CardCredit)
{
	CardCredit.SetOperator(m_strName);
	CardCredit.SetBusinessType(0);
	CardCredit.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CardCredit.SetDutyId(m_nDutyID);

	BOOL bRet = CardCredit.Execute();

	return bRet;
}

BOOL CCashier::DoRetail(CRetailMethod & RetailMethod)
{
	RetailMethod.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	RetailMethod.SetDutyId(m_nDutyID);
	RetailMethod.SetOperator(m_strName);
	RetailMethod.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
	RetailMethod.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());

	BOOL bRet = RetailMethod.Execute();

	return bRet;
}

BOOL CCashier::DoRetailOnline(CRetailOnlineMethod & RetailOnlineMethod)
{
	RetailOnlineMethod.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	RetailOnlineMethod.SetDutyId(m_nDutyID);
	RetailOnlineMethod.SetOperator(m_strName);
	RetailOnlineMethod.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
	RetailOnlineMethod.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());

	BOOL bRet = RetailOnlineMethod.Execute();

	return bRet;
}

BOOL CCashier::DoStoreQuantity(CStoreQuantityMethod & StoreQuantity)
{
	StoreQuantity.SetOperator(m_strName);
	StoreQuantity.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());

	BOOL bRet = StoreQuantity.Execute();

	return bRet;
}

BOOL CCashier::DoUsePoint(CUsePointMethod & UsePoint)
{
	UsePoint.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	UsePoint.SetOperator(m_strName);
	UsePoint.SetBusinessType(0);

	BOOL bRet = UsePoint.Execute();

	return bRet;
}

// 执行向中心发送断网的通知
BOOL CCashier::DoLocalMode()
{
	CLocalModeMethod LocalMode;

	LocalMode.SetOperationType(2);//关闭断网

	if (m_strName.IsEmpty())
	{
		LocalMode.SetOperator(_T("admin"));//默认的断网用户
	} 
	else
	{
		LocalMode.SetOperator(m_strName);//当前收银员
	}
	LocalMode.SetLastOperationTime(CIBAHelpper::GetCurrentTimeAsString());

	BOOL bRet = LocalMode.Execute();

	return bRet;
}

BOOL CCashier::DoAssetStatus()
{
	CAssetStatusMethod AssetStatus;

	DWORD length = MAX_PATH;
	TCHAR szComputerName[MAX_PATH] = {0};
	GetComputerName(szComputerName, &length);

	AssetStatus.SetTermId(szComputerName);//终端号
	AssetStatus.SetAssetId(CNetBarConfig::GetInstance()->GetICControlVerID());//ICC版本号
	AssetStatus.SetAssetStatus(CIBALog::GetInstance()->GetAppVer());//收银台程序版本号
	BOOL bRet = AssetStatus.Execute();

	return bRet;
}


void CCashier::WriteLoginInfo(CString strOperator)//登录后写注册表
{
	//写注册表，之前必须进行setProfilekey的写前准备
	theApp.WriteProfileString(_T("NetBarInfo"), _T("LastOperator"), strOperator);

	//查找注册表
	CString strTmp = theApp.GetProfileString(_T("NetBarInfo"), _T("Operators"));
	strTmp.Trim();
	if (strTmp.IsEmpty())
	{
		//添加所有用户到注册表
		COperatorArray OperatorArray;
		CIBADAL::GetInstance()->GetInstance()->GetOperatorRecord(OperatorArray);

		for (INT_PTR i = 0; i < OperatorArray.GetCount(); i ++)
		{
			strTmp += OperatorArray.GetAt(i) + _T("|");
		}
		theApp.WriteProfileString(_T("NetBarInfo"), _T("Operators"), strTmp);
	}
}

BOOL CCashier::DoBundleTime2(CBundleTime2Method & BundleTime2)
{	
	BundleTime2.SetOperator(m_strName);
	
	return BundleTime2.Execute();
}

BOOL CCashier::DoBundleTimeList2(CBundleTimeList2Method & BundleTimeList2)
{
	BundleTimeList2.SetOperator(m_strName);
	
	return BundleTimeList2.Execute();
}

BOOL CCashier::DoShiftList(CShiftListMethod & ShiftList)
{
	ShiftList.SetOperator(m_strName);

	return ShiftList.Execute();
}

BOOL CCashier::DoDetailData(CDetailDataMethod & DetailData)
{
	return DetailData.Execute();
}

BOOL CCashier::DoCheckIn(CCheckInMethod & CheckInData)
{
	CheckInData.SetClientVersion(_T("9.9.9.9"));
	CheckInData.SetEncryptMode(1);

	return CheckInData.Execute();
}

BOOL CCashier::DoCreditDirect(CCreditDirectMethod& CreditDirect)
{
	CreditDirect.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CreditDirect.SetDutyId(m_nDutyID);
	CreditDirect.SetOperator(m_strName);
	CreditDirect.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());

	return CreditDirect.Execute();
}

BOOL CCashier::DoConsumeListEx(CConsumeListMethodEx & ConsumeListEx)
{
	ConsumeListEx.SetOperator(m_strName);

	return ConsumeListEx.Execute();
}

BOOL CCashier::LocalModeLogin()
{
	CString strPwdMD5;
	CEncrypt::CalcMD5(m_strPassword, strPwdMD5);

	CString strSQL;
	strSQL.Format(_T("select * from cashregister where ID = %d"), CIBAConfig::GetInstance()->GetCashRegisterID());

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 0)
		{
			Rs.Close();
			pDb->Release();
			return FALSE;
		}

		CString strName;
		if (!Rs.GetFieldValue(_T("name"),strName))
		{
			Rs.Close();
			pDb->Release();
			return FALSE;
		}
		CString strPwd;
		if (!Rs.GetFieldValue(_T("password"),strPwd))
		{
			Rs.Close();
			pDb->Release();
			return FALSE;
		}

		if (strPwd != strPwdMD5 || strName != m_strName)
		{
			Rs.Close();
			pDb->Release();
			return FALSE;
		}

		m_bIsLocalModeLogin = TRUE;
	
		CString strID;
		Rs.GetFieldValue(_T("dutyID"),strID);
		m_nDutyID = (UINT)_ttoi(strID);

		m_bLogin = TRUE;
		m_bIsBoss = FALSE;
		if (_ttoi(m_strName) == CNetBarConfig::GetInstance()->GetNetBarId())
		{	
			//网吧ID账户登录
			m_bIsBoss = TRUE;
		}
		m_bOnDuty = TRUE;
		if (m_nDutyID==0)
		{
			m_nDutyID = 1;
		}

		CBrokenNetwork::GetInstance()->LoadLocalStatusInfo();

		// 2011/11/09-8210-gxx: 
		NS_DAL::CRightsManagement::GetInstance()->CreateManagement(m_strEnableModuleList, m_bIsBoss);

		Rs.Close();
		pDb->Release();

		// 2011/07/25-8201-gxx: 
		((CMainFrame*)theApp.GetMainWnd())->m_wndOutput.RefreshAllInfoList();

		return TRUE;
	}

	pDb->Release();
	return FALSE;
}

BOOL CCashier::DoLocalConsume(CLocalConsumeMethod& LocalConsume)
{
	return LocalConsume.Execute();
}

BOOL CCashier::DoNetbarRoom(CNetbarRoomMethod& RoomMethod)
{
	RoomMethod.SetOperator(m_strName);

	return RoomMethod.Execute();
}

BOOL CCashier::DoRoomCheckIn(CRoomCheckIn& RoomCheckInMethod)
{
	RoomCheckInMethod.SetOperator(m_strName);
	RoomCheckInMethod.SetDutyId(m_nDutyID);
	RoomCheckInMethod.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());

	return RoomCheckInMethod.Execute();
}

BOOL CCashier::DoRoomCheckOut(CRoomCheckOut& RoomCheckOutMethod)
{
	RoomCheckOutMethod.SetOperator(m_strName);
	RoomCheckOutMethod.SetDutyId(m_nDutyID);
	RoomCheckOutMethod.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());

	return RoomCheckOutMethod.Execute();
}