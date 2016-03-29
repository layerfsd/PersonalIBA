// LocalCashier.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include ".\localcashier.h"

// CLocalCashier

CLocalCashier::CLocalCashier()
{
	
}

CLocalCashier::~CLocalCashier()
{
}

BOOL CLocalCashier::CheckIn(CCashRegisterLoginMethod& CashRegisterLogin, CString strOperator)
{
	SetLocaNetbarlInfo();

	BOOL bRet = FALSE;
	
	CashRegisterLogin.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
	CashRegisterLogin.SetCashRegisterVersion(CIBALog::GetInstance()->GetAppVer());
	///////////////////////本地模式1时，登入类型是0，不验证方式//////////////////////
	CashRegisterLogin.SetCashRegisterType(0);
	CashRegisterLogin.SetUserId(m_strName);
	CashRegisterLogin.SetPassword(m_strPassword);

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
		m_nShiftType = CashRegisterLogin.GetShiftType();

		CashRegisterLogin.GetUserClassInfoArray(CNetBarConfig::GetInstance()->m_UserClassArray);
		//网吧识别
		CNetBarConfig::GetInstance()->SetIssueNetBarId(CashRegisterLogin.GetIssueNetBarId());
		
		//权限列表

		m_strEnableModuleList = L"IBAP_TERMCONSUME,IBAP_CONSUMELIST,IBAP_OPERATIONQUERY,IBAP_REGISTER,IBAP_CASHSHIFTQUERY,IBAP_MODIFYPWD,IBAP_NETBARSET,IBAP_BUNDLETIME,IBAP_CONFIG,IBAP_CREDIT";

		m_strEnableModuleList.Append(_T(","));

		//从中心载入包时数据
		CNetBarConfig::GetInstance()->m_NetbarBundleTime.Start();

		//清除整个注册表LastTran项，以防造成的干扰
		theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\NetBarInfo\\LastTran"));
	}

	return bRet;
}

// 执行向中心发送断网的通知
BOOL CLocalCashier::DoLocalMode()
{
	CLocalModeMethod LocalMode;

	LocalMode.SetOperationType(1);//

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

// 初始化时设置本地数据库
BOOL CLocalCashier::SetLocaNetbarlInfo(void)
{
	CADODatabase LocalNetbar;

	if (!LocalNetbar.Open(_T("DSN=LOCALNETBAR;"))) 
	{
		CIBALog::GetInstance()->Write(_T("LocalNetbar Connected faile!"));
		return FALSE;
	}

	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select * from netbarcashregister where CashRegisterID = %d"), CIBAConfig::GetInstance()->GetCashRegisterID());//操作


	CADORecordset Rs(&LocalNetbar);

	if (Rs.Open(strSQL))//打开
	{
		if (Rs.GetRecordCount() == 0)
		{
			Rs.AddNew();
		}
		else
		{
			Rs.Edit();
			Rs.MoveFirst();
		}

		Rs.SetFieldValue(_T("CashRegisterID"), CIBAConfig::GetInstance()->GetCashRegisterID());
		Rs.SetFieldValue(_T("NetBarID"), CNetBarConfig::GetInstance()->GetNetBarId());
		Rs.SetFieldValue(_T("CashRegisterName"), _T("IBA"));

		CString strTmp, strIP;
		CIBAHelpper::GetIPAndMAC(strIP, strTmp);
		strTmp.Replace(_T("-"), _T(""));
		Rs.SetFieldValue(_T("pcMAC"), strTmp);
		
		Rs.SetFieldValue(_T("RemoteLogin"), 1);

		bRet = Rs.Update();
		Rs.Close();
	}

	LocalNetbar.Close();

	return bRet;
}


BOOL CLocalCashier::CheckLastDuty(CString& strLastOperator)
{
	return TRUE;
}

void CLocalCashier::WriteDutyInfo()
{
	//这里什么都不做。
}