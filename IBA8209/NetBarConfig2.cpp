#include "stdafx.h"
#include "Encrypt.h"
#include "IBAHelpper.h"
#include "IBALog.h"
#include "IBA.h"
#include "Cashier.h"
#include ".\dal\IBADAL.h"
#include ".\netbarconfig2.h"

using namespace NS_DAL;

CNetBarConfig2::CNetBarConfig2(void)
:m_bNetIdentityManage(FALSE)
,m_nShutDownMinutes(0), m_ClientLoginStyle(0), m_ShutDownFlag(0)
,m_nMaxKeepTime(0), m_nMaxKeepTimeOfUKey(0)
,m_nMemberUseChangeComputer(1), m_nTmpUseChangeComputer(1)
,m_bShowUseTime(TRUE), m_bAllowAutoCheckIn(TRUE)
,m_nAllowSuspend(TRUE), m_nAllowBundleTime(TRUE)
,m_nBundleTimeEndTemp(0), m_nBundleTimeEnd(0)
,m_nNoEnoughTime(10)
{

}

CNetBarConfig2::~CNetBarConfig2(void)
{
}

BOOL CNetBarConfig2::LoadFromDB()
{
	CString strTmp;
	CConfigInfo configInfo;

	//******************************************************************************
	configInfo.Clear();
	configInfo.SetKeyString(_T("ClientLoginStyle"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_ClientLoginStyle = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read ClientLoginStyle Error!"));
		return FALSE;
	}

	//******************************************************************************
	//客户端解屏密码
	configInfo.Clear();
	configInfo.SetKeyString(_T("clientPassword"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_strUnLockPassword = configInfo.GetKeyValue3().MakeUpper();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read UnLockPassword Error!"));
	}

	//******************************************************************************
	//开卡器版本号
	configInfo.Clear();
	configInfo.SetKeyString(_T("CardOpenerVerID"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nCardOpenerVerID = _ttoi(configInfo.GetKeyValue3());
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read CardOpenerVerID Error!"));
	}

	//******************************************************************************
	//WWM
	configInfo.Clear();
	configInfo.SetKeyString(_T("RzxPluginAddr"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_strWWMIP = configInfo.GetKeyValue3();
		m_strWWMIP.Trim();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read RzxPluginAddr Error!"));
	}

	//******************************************************************************
	//clientDelayMinutes
	configInfo.Clear();
	configInfo.SetKeyString(_T("clientDelayMinutes"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nShutDownMinutes = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read clientDelayMinutes Error!"));
	}

	//******************************************************************************

	configInfo.Clear();
	configInfo.SetKeyString(_T("ShutDownFlag"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_ShutDownFlag = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read ShutDownFlag Error!"));
	}

	//******************************************************************************
	//SuspendUnlock
	configInfo.Clear();
	configInfo.SetKeyString(_T("SuspendUnlock"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_bAllowUnlockSuspend = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read SuspendUnlock Error!"));
	}
	
	//******************************************************************************
	//maxkeeptime
	configInfo.Clear();
	configInfo.SetKeyString(_T("MaxKeepTime"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nMaxKeepTime = configInfo.GetKeyValue1();
		m_nMaxKeepTimeOfUKey = configInfo.GetKeyValue2();

	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read MaxKeepTime Error!"));
	}

	//******************************************************************************
	//客户端安装密码
	configInfo.Clear();
	configInfo.SetKeyString(_T("SETUP_PASSWORD"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_strSetupPassword = configInfo.GetKeyValue3().MakeUpper();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read SETUP_PASSWORD Error!"));
	}

	////////////////////用户级别与下机关系设置////////////////
	configInfo.Clear();
	configInfo.SetKeyString(_T("SelfChangeComputer"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nMemberUseChangeComputer = configInfo.GetKeyValue2();

		m_nTmpUseChangeComputer = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read SelfChangeComputer Error!"));
	}

	//******************************************************************************

	configInfo.Clear();
	configInfo.SetKeyString(_T("ShowUseTime"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_bShowUseTime = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read ShowUseTime Error!"));
	}

	//******************************************************************************

	configInfo.Clear();
	configInfo.SetKeyString(_T("AutoCheckIn"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_bAllowAutoCheckIn = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read AutoCheckIn Error!"));
	}

	//******************************************************************************
	//实名

	CString strSQL;
	strSQL.Append(_T("select * from cashregister"));

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
	{
		Rs.GetFieldValue(_T("NetIdentityManage"), m_bNetIdentityManage);
		Rs.Close();
	}

	pDb->Release();

	//*********************************允许客户端包时*********************************************
	configInfo.Clear();
	configInfo.SetKeyString(_T("AllowBundleTime"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nAllowBundleTime = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read AllowBundleTime Error!"));
		//return FALSE;
	}
	//*********************************允许客户端挂机*********************************************
	configInfo.Clear();
	configInfo.SetKeyString(_T("AllowSuspend"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nAllowSuspend = configInfo.GetKeyValue1();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read AllowSuspend Error!"));
		//return FALSE;
	}

	//*********************************包时结束后操作*********************************************
	configInfo.Clear();
	configInfo.SetKeyString(_T("BundleTimeEndOperation"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nBundleTimeEndTemp = configInfo.GetKeyValue1();
		m_nBundleTimeEnd = configInfo.GetKeyValue2();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Read BundleTimeEndOperation Error!"));
		//return FALSE;
	}

	//*********************************余额不多提示时间*********************************************
	configInfo.Clear();
	configInfo.SetKeyString(_T("NoEnoughTime"));

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		m_nNoEnoughTime = configInfo.GetKeyValue1();
	}
	else
	{
		m_nNoEnoughTime = 10; //默认10分钟

		CIBALog::GetInstance()->Write(_T("Read NoEnoughTime Error!"));
		//return FALSE;
	}

	return TRUE;
}

void CNetBarConfig2::SetUnLockPassword(CString newVal)
{
	CEncrypt::CalcMD5(newVal, m_strUnLockPassword);
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("clientPassword"));
	configInfo.SetKeyValue3(m_strUnLockPassword.MakeUpper());
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 3);
}

void CNetBarConfig2::SetShutDownMinutes(UINT newVal)
{
	m_nShutDownMinutes = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("clientDelayMinutes"));
	configInfo.SetKeyValue1(m_nShutDownMinutes);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);
}

void CNetBarConfig2::SetClientLoginStyle(UINT newVal)
{
	m_ClientLoginStyle = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("ClientLoginStyle"));
	configInfo.SetKeyValue1(m_ClientLoginStyle);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);
}

void CNetBarConfig2::SetShutDownFlag(UINT newVal)
{
	m_ShutDownFlag = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("ShutDownFlag"));
	configInfo.SetKeyValue1(m_ShutDownFlag);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);
}

void CNetBarConfig2::SetWWMIP(CString newVal)
{
	m_strWWMIP = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("RzxPluginAddr"));
	configInfo.SetKeyValue3(m_strWWMIP);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 3);
}


void CNetBarConfig2::SetCardOpenerVerID(UINT newVal)
{
	m_nCardOpenerVerID = newVal;
}

void CNetBarConfig2::SetAllowUnlockSuspend(BOOL newVal)
{
	m_bAllowUnlockSuspend = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("SuspendUnlock"));
	configInfo.SetKeyValue1(m_bAllowUnlockSuspend);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);
}

void CNetBarConfig2::SetNetIdentityManage(BOOL newVal)
{
	m_bNetIdentityManage = newVal;
	
	CString strSQL;
	strSQL.Append(_T("select * from cashregister"));

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			Rs.Edit();
		}
		else
		{
			CString strIP, strMAC;
			CIBAHelpper::GetIPAndMAC(strIP, strMAC);

			Rs.AddNew();
			Rs.SetFieldValue(_T("Name"), theApp.GetCurCashier()->GetName());
			Rs.SetFieldValue(_T("ID"), CIBAConfig::GetInstance()->GetCashRegisterID());
			Rs.SetFieldValue(_T("ComputerIP"), strIP);
		}

		Rs.SetFieldValue(_T("NetIdentityManage"), m_bNetIdentityManage);
		Rs.Update();
		Rs.Close();
	}

	pDb->Release();
}

void CNetBarConfig2::SetMaxKeepTime(UINT val)
{
	m_nMaxKeepTime = val;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("MaxKeepTime"));
	configInfo.SetKeyValue1(m_nMaxKeepTime);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetMaxKeepTimeOfUKey(UINT val)
{
	m_nMaxKeepTimeOfUKey = val;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("MaxKeepTime"));
	configInfo.SetKeyValue2(m_nMaxKeepTimeOfUKey);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 2);
}

void CNetBarConfig2::SetSetupPassword(CString newVal)
{
	CEncrypt::CalcMD5(newVal, m_strSetupPassword);

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("SETUP_PASSWORD"));
	configInfo.SetKeyValue3(m_strSetupPassword.MakeUpper());
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 3);
}

void CNetBarConfig2::SetMemberUserChangeComputer(UINT newVal)
{
	m_nMemberUseChangeComputer = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("SelfChangeComputer"));
	configInfo.SetKeyValue2(m_nMemberUseChangeComputer);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 2);
}

void CNetBarConfig2::SetTmpUserChangeComputer(UINT newVal)
{
	m_nTmpUseChangeComputer = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("SelfChangeComputer"));
	configInfo.SetKeyValue1(m_nTmpUseChangeComputer);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetShowUseTime(BOOL newVal)
{
	m_bShowUseTime = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("ShowUseTime"));
	configInfo.SetKeyValue1(m_bShowUseTime);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetAllowAutoCheckIn(BOOL newVal)
{
	m_bAllowAutoCheckIn = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("AutoCheckIn"));
	configInfo.SetKeyValue1(m_bAllowAutoCheckIn);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetAllowBundleTime(UINT val)
{ 
	m_nAllowBundleTime = val;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("AllowBundleTime"));
	configInfo.SetKeyValue1(val);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetAllowSuspend(UINT val)
{ 
	m_nAllowSuspend = val;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("AllowSuspend"));
	configInfo.SetKeyValue1(val);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetBundleTimeEndTemp(UINT newVal)
{
	m_nBundleTimeEndTemp = newVal;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("BundleTimeEndOperation"));
	configInfo.SetKeyValue1(newVal);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}

void CNetBarConfig2::SetBundleTimeEnd(UINT newVal)
{
	m_nBundleTimeEnd = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("BundleTimeEndOperation"));
	configInfo.SetKeyValue2(newVal);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 2);
}

void CNetBarConfig2::SetNoEnoughTime(UINT newVal)
{
	m_nNoEnoughTime = newVal;

	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("NoEnoughTime"));
	configInfo.SetKeyValue1(newVal);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
}