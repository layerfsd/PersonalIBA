
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
#include "LocalMember.h"


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

	DWORD length = MAX_PATH;
	TCHAR szComputerName[MAX_PATH] = {0};
	::GetComputerName(szComputerName, &length);
	CString strComputerName(szComputerName);
	if(strComputerName.GetLength() > 15)
	{
		strComputerName.Left(15);
	}

	CashRegisterLogin.SetPCName(strComputerName);


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
		
		
		// 2013-12-3 15:26:04 qsc 这种方式不行，因为2591tset,2591a等转换成数字和2591相等
		// if (_ttoi(m_strName) == CNetBarConfig::GetInstance()->GetNetBarId())
		CString strNetbarId;
		strNetbarId.Format(_T("%d"), CNetBarConfig::GetInstance()->GetNetBarId());
		
		if(strNetbarId.GetLength() == m_strName.GetLength() &&
			strNetbarId == m_strName)
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
		// 2014-8-11 - qsc
		SetCenterAddr(CashRegisterLogin);

		CNetBarConfig::GetInstance()->SetSoftwareType(CashRegisterLogin.GetSoftwareType());
		CNetBarConfig::GetInstance()->SetDomainId(CashRegisterLogin.GetDomainId());
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
		CNetBarConfig::GetInstance()->SetBundleTimeWithCash(CashRegisterLogin.GetBundleTimeUseBalance());
		CNetBarConfig::GetInstance()->SetMustSynchronize(CashRegisterLogin.GetMustSynchronize());
		CNetBarConfig::GetInstance()->SetCreditDirect(CashRegisterLogin.GetAllowCreditNegative() == 1);
		CNetBarConfig::GetInstance()->SetIsInternetCafe(CashRegisterLogin.GetIsInternetCafe());
		if(CashRegisterLogin.GetIsInternetCafe() > 0)
		{
			CNetBarConfig::GetInstance()->SetAddressInternetCafe(CashRegisterLogin.GetCafeAddress());
		}
		//载入中心配置的控制参数字串
		CNetBarConfig::GetInstance()->PraseControlOption(CashRegisterLogin.GetControlOption());

		if (CNetBarConfig::GetInstance()->GetMonitorAuditProxy() == 1 && CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
		{
			theApp.m_pMainWnd->SetTimer(1005, 5000, NULL);
		}

		// QSCADD-2013-6-17 14:34:45 比较程序当前版本和中心传回来的版本
		if (CNetBarConfig::GetInstance()->GetForceQuitParam() == 1)
		{
			CString thisVer = CIBALog::GetInstance()->GetAppVer();
			thisVer.Remove(_T('.'));
			CString strVer = CNetBarConfig::GetInstance()->GetForceQuitVersion();
			if (thisVer.Compare(strVer) < 0)
			{
				CString strTip;
				strTip.Format(IDS_IBA_VERBELOW_TIP, strVer);
				// 低于中心的版本
				HWND hWndTop;
				HWND hWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);
				theApp.IBAMsgBox2(strTip, MB_OK, TRUE);
				theApp.AutoRestart();
			}
		}

		//
		MonitorAuditProxy();

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

		//保存网咖地址到本地
		CIBADAL::GetInstance()->UpdateInternetCafeInfo();
	}

	return bRet;
}

BOOL CCashier::CheckIn_atLogin(CCashRegisterLoginMethod_atLogin& CashRegisterLogin, CString strOperator)
{
	//输出Build时间
	CString strTime = CIBALog::GetInstance()->GetAppBuildTime();
	CIBALog::GetInstance()->WriteFormat(_T("Build Time: %s"), strTime);

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

	DWORD length = MAX_PATH;
	TCHAR szComputerName[MAX_PATH] = {0};
	::GetComputerName(szComputerName, &length);
	CString strComputerName(szComputerName);
	if(strComputerName.GetLength() > 15)
	{
		strComputerName.Left(15);
	}

	CashRegisterLogin.SetPCName(strComputerName);


	//请求登陆
	bRet = CashRegisterLogin.ExecuteCheckin();

	if (!bRet) //连接中心失败
	{
		return bRet;
	}

	if (CashRegisterLogin.GetStatusCode() == 0) //执行成功
	{
		bRet = TRUE;

		m_bLogin = TRUE;

		m_bIsBoss = FALSE;


		// 2013-12-3 15:26:04 qsc 这种方式不行，因为2591tset,2591a等转换成数字和2591相等
		// if (_ttoi(m_strName) == CNetBarConfig::GetInstance()->GetNetBarId())
		CString strNetbarId;
		strNetbarId.Format(_T("%d"), CNetBarConfig::GetInstance()->GetNetBarId());

		if(strNetbarId.GetLength() == m_strName.GetLength() &&
			strNetbarId == m_strName)
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
		// 2014-8-11 - qsc
		SetCenterAddr(CashRegisterLogin);

		CNetBarConfig::GetInstance()->SetSoftwareType(CashRegisterLogin.GetSoftwareType());
		CNetBarConfig::GetInstance()->SetDomainId(CashRegisterLogin.GetDomainId());
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
		CNetBarConfig::GetInstance()->SetBundleTimeWithCash(CashRegisterLogin.GetBundleTimeUseBalance());
		CNetBarConfig::GetInstance()->SetMustSynchronize(CashRegisterLogin.GetMustSynchronize());
		CNetBarConfig::GetInstance()->SetCreditDirect(CashRegisterLogin.GetAllowCreditNegative() == 1);
		CNetBarConfig::GetInstance()->SetIsInternetCafe(CashRegisterLogin.GetIsInternetCafe());
		if(CashRegisterLogin.GetIsInternetCafe() > 0)
		{
			CNetBarConfig::GetInstance()->SetAddressInternetCafe(CashRegisterLogin.GetCafeAddress());
		}
		//载入中心配置的控制参数字串
		CNetBarConfig::GetInstance()->PraseControlOption(CashRegisterLogin.GetControlOption());

		if (CNetBarConfig::GetInstance()->GetMonitorAuditProxy() == 1 && CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
		{
			theApp.m_pMainWnd->SetTimer(1005, 5000, NULL);
		}

		// QSCADD-2013-6-17 14:34:45 比较程序当前版本和中心传回来的版本
		if (CNetBarConfig::GetInstance()->GetForceQuitParam() == 1)
		{
			CString thisVer = CIBALog::GetInstance()->GetAppVer();
			thisVer.Remove(_T('.'));
			CString strVer = CNetBarConfig::GetInstance()->GetForceQuitVersion();
			if (thisVer.Compare(strVer) < 0)
			{
				CString strTip;
				strTip.Format(IDS_IBA_VERBELOW_TIP, strVer);
				// 低于中心的版本
				HWND hWndTop;
				HWND hWnd = CWnd::GetSafeOwner_(NULL, &hWndTop);
				theApp.IBAMsgBox2(strTip, MB_OK, TRUE);
				theApp.AutoRestart();
			}
		}

		//
		MonitorAuditProxy();

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

		//保存网咖地址到本地
		CIBADAL::GetInstance()->UpdateInternetCafeInfo();
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

	/*	CBrokenNetwork::GetInstance()->m_threadRestore->Start();*/

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

		//更新CreditProcess
		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(Credit.GetMemberId(), Credit.GetAmount(), Credit.GetCreditPresent());

		//写入数据库credited
		CActiveMember Am;
		if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(Credit.GetMemberId(),Am))
		{
			CCreditInfoDB CreditInfo;
			CreditInfo.SetCardSerial(Am.GetNetId());
			CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(Credit.GetTrantime()));
			CreditInfo.SetRefNo(Credit.GetRefNo());
			CreditInfo.SetCreditPresent(Credit.GetCreditPresent());
			CreditInfo.SetCreditAmount(Credit.GetAmount());
			CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
			CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			CreditInfo.SetMemberId(Credit.GetMemberId());
			CreditInfo.SetSerialNo(Credit.GetLocalSerialNo());
			CreditInfo.SetCassId(Am.GetClassId());
			CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);
		}

		if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
		{
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
		}  
	}

	return bRet;
}
/*************************************************
Function:       // MakeupCredit
Description:    // 补上报本地模式下充值
Input:          // Credit - 用于执行充值
Return:         // 充值成功则返回TRUE
Others:         // 与DoCredit唯一区别是，不更新ActiveMember.CreditProcess.因为已经在本地充值时进行了更新.
*************************************************/
BOOL CCashier::MakeupCredit(CCreditMethod & Credit)
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

		//CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
		//	Credit.GetMemberId(), Credit.GetAmount(), Credit.GetCreditPresent());

		//写入数据库credited
		CActiveMember Am;
		if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(Credit.GetMemberId(),Am))
		{
			CCreditInfoDB CreditInfo;
			CreditInfo.SetCardSerial(Am.GetNetId());
			CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(Credit.GetTrantime()));
			CreditInfo.SetRefNo(Credit.GetRefNo());
			CreditInfo.SetCreditPresent(Credit.GetCreditPresent());
			CreditInfo.SetCreditAmount(Credit.GetAmount());
			CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
			CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			CreditInfo.SetMemberId(Credit.GetMemberId());
			CreditInfo.SetSerialNo(Credit.GetLocalSerialNo());
			CreditInfo.SetCassId(Am.GetClassId());
			CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);
		}
	}

	return bRet;
}

BOOL CCashier::DoMemberInfo(CMemberInfoMethod & MemberInfo)
{
	BOOL bRet = MemberInfo.Execute();

	return bRet;
}

/*************************************************
Function:       // DoDeregister
Description:    // 到中心执行注销操作，除了ReturnMode=2(仅退包时后余额的)，其余均取消本地激活并进行实名下机
Input:          // Deregister 
				// ReturnMode - 退款模式.0:正常退款 1:退款不销户 2:退包时后余额
Return:         // Deregister返回0，返回TRUE
*************************************************/
BOOL CCashier::DoDeregister(CDeregisterMethod & Deregister,UINT nReturnMode /*=0*/)
{
	//本地模式,更新或插入LocalMember记录
	BOOL bSuccessDBOperation(TRUE);
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		//查询LocalMember是否有记录
		CLocalMember lMember;
		BOOL bHasRecord = CIBADAL::GetInstance()->GetLocalMemberByMemberID(Deregister.GetMemberId(),lMember);
		if(bHasRecord)//有记录,更新退款信息
		{
			COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
			if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(Deregister.GetMemberId(),CheckoutTime,TRUE))
			{
				bSuccessDBOperation = FALSE;
				IBA_LOG(_T("Error.更新本地退款记录失败."));
			}
		}
		else//无记录,创建记录,插入记录,更新记录
		{
			GxxDictionaryPtr lmPrt = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(Deregister.GetMemberId());  //创建记录
			if(lmPrt)
			{
				if(CIBADAL::GetInstance()->InsertNewLocalMember(lmPrt))//插入记录
				{
					COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
					if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(Deregister.GetMemberId(),CheckoutTime,TRUE))//更新记录
					{
						bSuccessDBOperation = FALSE;
						IBA_LOG(_T("Error.更新本地退款记录失败."));
					}	
				}
				else
				{
					bSuccessDBOperation = FALSE;
					IBA_LOG(_T("Error.为退款插入本地记录失败."));
				}
			}
			else
			{
				bSuccessDBOperation = FALSE;
				IBA_LOG(_T("Error.为退款创建本地记录失败."));
			}
		}
	}
	else//正常模式,执行Deregister
	{
		//请求退款
		Deregister.SetDutyId(m_nDutyID);
		Deregister.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
		Deregister.SetOperator(m_strName);

		Deregister.Execute();
		Deregister.SaveTranInfo();
	}	
	//请求失败,直接退出
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		if(FALSE == bSuccessDBOperation){return FALSE;}
	}
	else
	{
		if(0 != Deregister.GetStatusCode()){return FALSE;}
	}

	//用户在线.实名下机
	CActiveMember ActiveMember;
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(Deregister.GetMemberId(),ActiveMember);
	if(ActiveMember.GetIsOnLine())
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(Deregister.GetMemberId());
		if (nIndex >= 0)
		{
			//实名下机
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			theApp.GetRealName()->AuditUserCheckOut(computer.GetTerminalID(), computer.GetComputerIP(), Deregister.GetMemberId());
			//智多实名下机
			if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
			{
				NS_REALNAME::CheckoutInfo coi;
				ZeroMemory(&coi, sizeof(coi));
				CString strName,strSex;
				strSex = ActiveMember.GetSex()?_T("女"):_T("男");
				strName.Format(_T("%s(%s)"),ActiveMember.GetUserName(),strSex);
				_tcsncpy(coi.szName, strName, 100);
				_tcsncpy(coi.szNetId, ActiveMember.GetNetId(), 100);
				_tcsncpy(coi.szTermId, ActiveMember.GetTerminalID(), 50);

				CString strIP;
				CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
				_tcsncpy(coi.szIP, strIP, 50);
				coi.nCheckoutType = CIBAGlobal::cot_TempLogout;

				theApp.GetRealName()->CheckOut(&coi);
			}
			//实名退临时卡.(如果中心未设置仅退款,则执行实名退临时卡)
			if (CNetBarConfig::GetInstance()->GetReturnOnlyClassId() != ActiveMember.GetClassId())
			{
				theApp.GetRealName()->InvokeRealName(100, (LPVOID)ActiveMember.GetNetId().GetString());
			}
		}	
	}
	//else//用户不在线,金桥实名也要进行实名下机 - 孙珂要求
	//{
	//	if(CIBAGlobal::emRealnameJQ == CNetBarConfig::GetInstance()->GetAuditType())
	//	{
	//		IBA_LOG(_T("金桥实名下机 - 用户不在线,TermId及IPAddress为空"));
	//		CString strTerminalID(_T(""));
	//		CString strComputerIP(_T(""));
	//		theApp.GetRealName()->AuditUserCheckOut(strTerminalID,strComputerIP, Deregister.GetMemberId());
	//	}
	//}

	//判断退款类型.退包时后余额的,不用从激活列表中删除用户.
	if (nReturnMode != 2 /*&& !bIsOnLine*/)  //ReturnMode - 0:正常退款 1:退款不销户 2:仅退包时后余额
	{
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Deregister.GetMemberId());
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
	}

	//非本地模式,要插入新退款记录到本地库.本地模式,将在补上报时插入.
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		CReturnedInfo ReturnedInfo;
		ReturnedInfo.SetCardId(_ttoi(ActiveMember.GetNetId()));
		ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(Deregister.GetTranTime()));
		ReturnedInfo.SetRefNo(Deregister.GetRefNo());
		ReturnedInfo.SetDeposit(Deregister.GetGuarantyMoney());
		ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());
		ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
		ReturnedInfo.SetMemberId(Deregister.GetMemberId());
		ReturnedInfo.SetSerialNo(Deregister.GetLocalSerialNo());
		ReturnedInfo.SetReturnAmount(Deregister.GetBalance() + Deregister.GetGuarantyMoney());
		ReturnedInfo.SetClassId(ActiveMember.GetClassId());

		CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);
	}

	return TRUE;
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
		IBA_TRACE("会员不在线");
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

		//保存AllowReturn信息到本地表
		CIBADAL::GetInstance()->UpdateAllowReturn(Balance.GetUserClass(),Balance.GetAllowReturn(),Balance.GetIsMember());
	}
	return bRet;
}

/*************************************************
Function:       // DoCheckCard
Description:    // 校验会员号
Calls:          // CServlet::Execute
Table Accessed: // None
Table Updated: //  None
Input:          // CheckCard结构，包含SerialNum(卡印刷流水号),SerialNumType(0:手动输入印刷号 1:实名传入),Operator(收银员)
Output:         // CheckCard结构，包含memberId(会员号)，cardId(账号)
Return:         // 成功返回TRUE，失败返回FALSe
Others:         // None
*************************************************/
BOOL CCashier::DoCheckCard(CIDCheckCardMethod& CheckCard)
{
	CheckCard.SetOperator(m_strName);

	return CheckCard.Execute();
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

/*************************************************
Function:       //DoCheckout2
Description:    //人工登出会员.实名下机.清理用户
Table Updated:  //LastUser
Input:          //Checkout2 -  
				//bClearData - 是否清理用户和本地库记录 
Return:         // 成功则返回TRUE，否则返回FALSE
*************************************************/
BOOL CCashier::DoCheckout2(CCheckout2Method & Checkout2, BOOL bClearData /*=TRUE*/)
{
	//用户不在线,无需登出.将被IBA自动移除.(CheckActiveMember)
	CActiveMember ActiveMember;
	BOOL bActive = CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(Checkout2.GetMemberId(),ActiveMember);
	if(FALSE == bActive || FALSE == ActiveMember.GetIsOnLine())
	{
		//2016-0309 直接移除掉激活，不等IBA自动移除
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Checkout2.GetMemberId());  //取消本地激活
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);//更新视图
		return FALSE;
	}

	BOOL bSuccessDBOperation(TRUE);

	//本地模式
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		//查询LocalMember是否有记录
		CLocalMember lMember;
		BOOL bHasRecord = CIBADAL::GetInstance()->GetLocalMemberByMemberID(Checkout2.GetMemberId(),lMember);
		if(bHasRecord)//有记录,更新退款信息
		{
			COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
			if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(Checkout2.GetMemberId(),CheckoutTime,TRUE))
			{
				bSuccessDBOperation = FALSE;
				IBA_LOG(_T("Error.更新本地结账记录失败."));
			}
		}
		else//无记录,创建记录,插入记录,更新记录
		{
			GxxDictionaryPtr lmPrt = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(Checkout2.GetMemberId());  //创建记录
			if(lmPrt)
			{
				if(CIBADAL::GetInstance()->InsertNewLocalMember(lmPrt))//插入记录
				{
					COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
					if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(Checkout2.GetMemberId(),CheckoutTime,TRUE))//更新记录
					{
						bSuccessDBOperation = FALSE;
						IBA_LOG(_T("Error.更新本地结账记录失败."));
					}	
				}
				else
				{
					bSuccessDBOperation = FALSE;
					IBA_LOG(_T("Error.为结账插入本地记录失败."));
				}
			}
			else
			{
				bSuccessDBOperation = FALSE;
				IBA_LOG(_T("Error.为结账创建本地记录失败."));
			}
		}
	}
	else//正常模式
	{
		//人工登出
		Checkout2.SetOperator(m_strName);
		Checkout2.Execute();
	}

	//Fail.返回FALSE
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		if(FALSE == bSuccessDBOperation){return FALSE;}
	}
	else
	{
		if(0 != Checkout2.GetStatusCode()){return FALSE;}
	}

	//Success
	//实名下机
	INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(Checkout2.GetMemberId());
	if (nIndex >= 0)
	{
		CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
		theApp.GetRealName()->AuditUserCheckOut(computer.GetTerminalID(), computer.GetComputerIP(), Checkout2.GetMemberId());
		//智多实名
		if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
		{
			NS_REALNAME::CheckoutInfo coi;
			ZeroMemory(&coi, sizeof(coi));
			CString strName,strSex;
			strSex = ActiveMember.GetSex()?_T("女"):_T("男");
			strName.Format(_T("%s(%s)"),ActiveMember.GetUserName(),strSex);
			_tcsncpy(coi.szName, strName, 100);
			_tcsncpy(coi.szNetId, ActiveMember.GetNetId(), 100);
			_tcsncpy(coi.szTermId, ActiveMember.GetTerminalID(), 50);

			CString strIP;
			CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
			_tcsncpy(coi.szIP, strIP, 50);
			coi.nCheckoutType = CIBAGlobal::cot_MemLogout;

			theApp.GetRealName()->CheckOut(&coi);
		}
	}
	//激活列表中删除用户,删除本地库记录(LastUser)
	if(bClearData)
	{
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Checkout2.GetMemberId());  //取消本地激活
		CIBADAL::GetInstance()->DeleteLastUser(Checkout2.GetMemberId()); //从LastUser表删除对应记录
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);//更新视图
	}
	return TRUE;	
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
	// 2014-4-28 qsc
	//QSCT1 AssetStatus.SetAssetStatus(CIBALog::GetInstance()->GetAppVer());//收银台程序版本号
	{
		// 2014-4-28 增加SFREG版本上报 QSCT1
		// memcpy(checkin.version, (LPCSTR)strVer, strVer.GetLength());

		CString strSFREGPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\sfreg.exe");
		CString strSfregVerion;
		strSfregVerion.Empty();
		if(PathFileExists(strSFREGPath))
		{
			CFileVersionInfo fvi;
			if (!fvi.Create(strSFREGPath))
			{
				IBA_LOG0(_T("error: 读取sfreg.exe的版本号失败"));
			}
			else
			{
				IBA_LOG(_T("sfreg.exe version :%s"), fvi.GetFileVersion());
				strSfregVerion = fvi.GetFileVersion();
			}
		}
		CString strAllVer;
		if (strSfregVerion.IsEmpty())
		{
			strAllVer = CIBALog::GetInstance()->GetAppVer(); 
		}
		else
		{
			strAllVer.Format(_T("%s,%s"), CIBALog::GetInstance()->GetAppVer(), strSfregVerion);
		}
		AssetStatus.SetAssetStatus(strAllVer);
	}
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
		// 2013-12-3 15:26:04 qsc 这种方式不行，因为2591tset,2591a等转换成数字和2591相等
		// if (_ttoi(m_strName) == CNetBarConfig::GetInstance()->GetNetBarId())
		CString strNetbarId;
		strNetbarId.Format(_T("%d"), CNetBarConfig::GetInstance()->GetNetBarId());

		if(strNetbarId.GetLength() == m_strName.GetLength() &&
			strNetbarId == m_strName)
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

BOOL CCashier::MonitorAuditProxy()
{
	if (CNetBarConfig::GetInstance()->GetMonitorAuditProxy() == 0)
	{
		return TRUE;
	}
	if (CIBAConfig::GetInstance()->GetCashRegisterID() != 1)
	{
		if (CNetBarConfig::GetInstance()->GetAllowOpenUser() == FALSE)
		{
			// 副收银台不允许开户
			AfxMessageBox(L"主收银台未开启或者主收银台代理程序未开启，收银台禁止开户操作，请您确认代理程序正确启动后重新启动收银台!", MB_OK|MB_ICONWARNING);
			return FALSE;
		}
	}
	m_bAuditProxyRunning = TRUE;


#ifdef DEBUG
	CString strProxy(theApp.GetWorkPath());
	strProxy.Append(_T("\\IBABin\\123.exe"));

	CL2Process SunlikeAuditProxy;
	SunlikeAuditProxy.Attach(strProxy);
	BOOL bRes = SunlikeAuditProxy.CheckRunning(NULL);
	if (SunlikeAuditProxy.IsRunning())
	{
		AfxMessageBox(L"Is Running");
	}
#else
	
	

	if (CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
	{
		// 主收银台才做这样的判断
		HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("_SUNLIKEAUDITPROXY_MUTEX_"));
		if (hMutex != NULL)
		{
			CloseHandle(hMutex);
			CString strProxy(theApp.GetWorkPath());
			strProxy.Append(_T("\\IBABin\\SunlikeAuditProxy.exe"));

			CL2Process SunlikeAuditProxy;
			SunlikeAuditProxy.Attach(strProxy);
			if (!SunlikeAuditProxy.IsRunning())
			{
				m_bAuditProxyRunning = FALSE;
			}
		}
		else
		{
			m_bAuditProxyRunning = FALSE;
		}
		if (m_bAuditProxyRunning == FALSE)
		{
			AfxMessageBox(L"代理程序没有启动或路径错误，收银台禁止开户操作，请您确认代理程序正确启动后重新登入收银台!", MB_OK|MB_ICONWARNING);
			CNetBarConfig::GetInstance()->SetAllowOpenUser(FALSE);
		}
		else
		{
			CNetBarConfig::GetInstance()->SetAllowOpenUser(TRUE);
		}
	}
	
#endif

	return m_bAuditProxyRunning;
	
}

void CCashier::SetCenterAddr(CCashRegisterLoginMethod& CashRegisterLogin)
{
	// 2014-8-18 - qsc 登录的时候还是老的逻辑
	CString strMain = CashRegisterLogin.GetMainHost();
	CString strBk = CashRegisterLogin.GetBackupHost();
	CString strStandby = CashRegisterLogin.GetStandbyHost();
	CNetBarConfig::GetInstance()->SetMainHost(strMain);
	CNetBarConfig::GetInstance()->SetBackUpHost(strBk);
	CNetBarConfig::GetInstance()->SetStandbyHost(strStandby);//异地服务器


	//// 若当前连通的地址和返回的中心地址不一致，则替换主中心地址为当前地址
	//// 然后匹配和当前地址相同的备中心地址或异地中心地址，替换为返回的中心地址
	//CString strRequesAddr = CashRegisterLogin.GetUrlFromIndex(CashRegisterLogin.m_nFirstCenterIndex);
	//CString strMain = CashRegisterLogin.GetMainHost();
	//CString strBk = CashRegisterLogin.GetBackupHost();
	//CString strStandby = CashRegisterLogin.GetStandbyHost();

	//IBA_LOG(_T("登录成功地址:%s,主%s,备:%s,异:%s"), strRequesAddr, strMain, strBk, strStandby);
	//if(0 != strRequesAddr.CompareNoCase(strMain))
	//{
	//	if(0 == strRequesAddr.CompareNoCase(strBk))
	//	{
	//		strBk = strMain;
	//	}
	//	else if(0 == strRequesAddr.CompareNoCase(strStandby))
	//	{
	//		strStandby = strMain;
	//	}	
	//	strMain = strRequesAddr;
	//}
	//
	//IBA_LOG(_T("主%s,备:%s,异:%s"), strMain, strBk, strStandby);
	//CNetBarConfig::GetInstance()->SetMainHost(strMain);
	//CNetBarConfig::GetInstance()->SetBackUpHost(strBk);
	//CNetBarConfig::GetInstance()->SetStandbyHost(strStandby);//异地服务器
}

BOOL CCashier::DoBuckleAmount( CBuckleAmountMethod& BuckleAmountMethod )
{
	BuckleAmountMethod.SetOperator(m_strName);
	//BuckleAmountMethod.SetDutyId(m_nDutyID);
	return BuckleAmountMethod.Execute();
}
