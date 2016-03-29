#include "stdafx.h"
#include "CashRegisterLoginMethod.h"
#include "..\IBAHelpper.h"
#include "..\Encrypt.h"
#include "..\NetBarConfig.h"
#include "..\MsgBoxDlg.h"
#include "..\IBA.h"
#include "..\IBA_DEBUG.h"
#include "..\brokennetwork.h"

using namespace NS_SERVLET;

CCashRegisterLoginMethod::CCashRegisterLoginMethod(void)
: CServletMethod(_T("CashRegisterLogin")) //中心方法名称
{
	AddParams();		//初始化参数
	AddRespFields();	//初始化返回字段

	CString strIP;

	CIBAHelpper::GetIPAndMAC(strIP, m_strPSK);
	
	m_strPSK.Replace(_T("-"), _T(""));
	m_strPSK.Append(_T("0000")); //补齐16位
}

CCashRegisterLoginMethod::~CCashRegisterLoginMethod(void)
{
}


void CCashRegisterLoginMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("cashRegisterType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("userId"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("cashRegisterVersion"), EPT_STRING, 16);
	m_Params.AddParam(_T("timeChuo"), EPT_NUMBER, 11);
	m_Params.AddParam(_T("HashCode"), EPT_STRING, 32);
	//m_Params.AddParam(_T("pcName"), EPT_STRING, 15);		// 2014-8-5 - qsc 
}

void CCashRegisterLoginMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCashRegisterLoginMethod::SetCashRegisterType(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterType"), newVal);
}

void CCashRegisterLoginMethod::SetUserId(CString newVal)
{
	m_Params.SetValue(_T("userId"), newVal);
}

void CCashRegisterLoginMethod::SetPassword(CString newVal)
{
	CString strTmp;

	CEncrypt::CalcMD5(newVal, strTmp);
	m_Params.SetValue(_T("password"), strTmp.Left(16));
}

void CCashRegisterLoginMethod::SetCashRegisterVersion(CString newVal)
{
	m_Params.SetValue(_T("cashRegisterVersion"), newVal);
}

void NS_SERVLET::CCashRegisterLoginMethod::SetTimeChuo(UINT nTime)
{
	m_Params.SetValue(_T("timeChuo"), nTime);
}

void NS_SERVLET::CCashRegisterLoginMethod::SetCheckCode(CString strCheckCode)
{
	m_Params.SetValue(_T("HashCode"), strCheckCode);
}

void NS_SERVLET::CCashRegisterLoginMethod::SetPCName(CString strCheckCode)
{
	//m_Params.SetValue(_T("pcName"), strCheckCode);
}

void CCashRegisterLoginMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("domainId"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("MainHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("BackupHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("classCount"), EPT_NUMBER, 2, TRUE, TRUE); //表示下个字段是个循环体

	m_RespFields.AddRespField(_T("classInfoBody"), EPT_STRING, 25); //每个classInfo的长度为25

	m_RespFields.AddRespField(_T("returnOnlyClassId"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("dodoNewHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("dodonewResource"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("updateInterval"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("foreRealName"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("dutyId"), EPT_NUMBER, 10);
	m_RespFields.AddRespField(_T("ondutyTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("queryHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("queryHost2"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("auditType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("popupURL"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("noIDCardIn"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("issueNetBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("serverTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("allowNoCardLogin"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("realNameFrom"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("controlOption"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("noticeInfo"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("cardRealName"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("userPrivileges"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("shiftType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("agentName"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("agentRegularTel"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("AgentMobilePhone"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("SoftwareType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("standbyHost"), EPT_STRING, 22); 
	m_RespFields.AddRespField(_T("bundleTimeUseBalance"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("mustSynchronize"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("allowCreditNegative"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("nologinPrice"),EPT_NUMBER,6);
	m_RespFields.AddRespField(_T("allowStandalone"),EPT_NUMBER,1);
	//m_RespFields.AddRespField(_T("taxRate"),EPT_NUMBER,6);
	m_RespFields.AddRespField(_T("IsInternetCafe"),EPT_NUMBER,2);
	m_RespFields.AddRespField(_T("CafeAddress"),EPT_STRING,4,FALSE);
}

UINT CCashRegisterLoginMethod::GetDomainId()
{
	return m_RespFields.GetValueAsUINT(_T("domainId"));
}

CString CCashRegisterLoginMethod::GetMainHost()
{
	return m_RespFields.GetValueAsString(_T("MainHost"));
}

CString CCashRegisterLoginMethod::GetBackupHost()
{
	return m_RespFields.GetValueAsString(_T("BackupHost"));
}

UINT CCashRegisterLoginMethod::GetClassCount()
{
	return m_RespFields.GetValueAsUINT(_T("classCount"));
}

CString CCashRegisterLoginMethod::GetClassInfo()
{
	return m_RespFields.GetValueAsString(_T("classInfoBody"));
}

UINT CCashRegisterLoginMethod::GetReturnOnlyClassId()
{
	return m_RespFields.GetValueAsUINT(_T("returnOnlyClassId")); //仅退款用户ID
}

CString CCashRegisterLoginMethod::GetDodoNewHost()
{
	return m_RespFields.GetValueAsString(_T("dodoNewHost"));
}

CString CCashRegisterLoginMethod::GetDodonewResource()
{
	return m_RespFields.GetValueAsString(_T("dodonewResource"));
}

UINT CCashRegisterLoginMethod::GetUpdateInterval()
{
	return m_RespFields.GetValueAsUINT(_T("updateInterval"));
}

UINT CCashRegisterLoginMethod::GetForeRealName()
{
	return m_RespFields.GetValueAsUINT(_T("foreRealName"));
}

UINT CCashRegisterLoginMethod::GetDutyId()
{
	return m_RespFields.GetValueAsUINT(_T("dutyId"));
}

CString CCashRegisterLoginMethod::GetOndutyTime()
{
	return m_RespFields.GetValueAsString(_T("ondutyTime"));
}

CString CCashRegisterLoginMethod::GetQueryHost()
{
	return m_RespFields.GetValueAsString(_T("queryHost"));
}

CString CCashRegisterLoginMethod::GetQueryHost2()
{
	return m_RespFields.GetValueAsString(_T("queryHost2"));
}

UINT CCashRegisterLoginMethod::GetAuditType()
{
	return m_RespFields.GetValueAsUINT(_T("auditType"));
}

CString CCashRegisterLoginMethod::GetPopupURL()
{
	return m_RespFields.GetValueAsString(_T("popupURL"));
}

CString CCashRegisterLoginMethod::GetAgentMobilePhone()
{
	return m_RespFields.GetValueAsString(_T("AgentMobilePhone"));
}

CString CCashRegisterLoginMethod::GetAgentRegularTel()
{
	return m_RespFields.GetValueAsString(_T("agentRegularTel"));
}

CString CCashRegisterLoginMethod::GetAgentName()
{
	return m_RespFields.GetValueAsString(_T("agentName"));
}

UINT CCashRegisterLoginMethod::GetSoftwareType()
{
	return m_RespFields.GetValueAsUINT(_T("SoftwareType"));
}

UINT CCashRegisterLoginMethod::GetNoIDCardIn()
{
	return m_RespFields.GetValueAsUINT(_T("noIDCardIn"));
}

UINT CCashRegisterLoginMethod::GetIssueNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("issueNetBarId"));
}

CString CCashRegisterLoginMethod::GetServerTime()
{
	return m_RespFields.GetValueAsString(_T("serverTime"));
}

UINT CCashRegisterLoginMethod::GetAllowNoCardLogin()
{
	return m_RespFields.GetValueAsUINT(_T("allowNoCardLogin"));
}

UINT CCashRegisterLoginMethod::GetRealNameFrom()
{
	return m_RespFields.GetValueAsUINT(_T("realNameFrom"));
}

CString CCashRegisterLoginMethod::GetControlOption()
{
	return m_RespFields.GetValueAsString(_T("controlOption"));
}

//取得通报内容
CString CCashRegisterLoginMethod::GetNoticeInfo()
{
	return m_RespFields.GetValueAsString(_T("noticeInfo"));
}

UINT CCashRegisterLoginMethod::GetCardRealName()
{
	return m_RespFields.GetValueAsUINT(_T("cardRealName"));
}

CString CCashRegisterLoginMethod::GetUserPrivileges()
{
	return m_RespFields.GetValueAsString(_T("userPrivileges"));
}

UINT CCashRegisterLoginMethod::GetShiftType()
{
	CNetBarConfig::GetInstance()->SetShiftType(m_RespFields.GetValueAsUINT(_T("shiftType")));
	return m_RespFields.GetValueAsUINT(_T("shiftType"));
}

BOOL CCashRegisterLoginMethod::DecodeResp(CString strResp)
{
	BOOL bRet = FALSE;

	bRet = CServletMethod::DecodeResp(strResp);
	
	m_UserClassInfoArray.RemoveAll();
	
	CString strClassInfo(GetClassInfo());
	CString strTmp;
	
	INT nIndex = 0;

	if (bRet)
	{
		for (UINT i = 0; i < GetClassCount(); i++)
		{
			//m_RespFields.AddRespField(_T("classId"), EPT_NUMBER, 2);
			//m_RespFields.AddRespField(_T("className"), EPT_STRING, 10);
			//m_RespFields.AddRespField(_T("costExpense"), EPT_NUMBER, 6);
			//m_RespFields.AddRespField(_T("deposit"), EPT_NUMBER, 6);
			//m_RespFields.AddRespField(_T("isMember"), EPT_NUMBER, 1);

			CUserClassInfo uci;

			strTmp = strClassInfo.Mid(nIndex, 2);
			nIndex+= 2;
			uci.SetClassID(_ttoi(strTmp));

			strTmp = strClassInfo.Mid(nIndex, 10);
			nIndex+= 10;
			uci.SetClassName(strTmp.Trim());

			strTmp = strClassInfo.Mid(nIndex, 6);
			nIndex+= 6;
			uci.SetCostExpense(_ttoi(strTmp));

			strTmp = strClassInfo.Mid(nIndex, 6);
			nIndex+= 6;
			uci.SetDeposit(_ttoi(strTmp));

			strTmp = strClassInfo.Mid(nIndex, 1);
			nIndex+= 1;
			uci.SetIsMember(_ttoi(strTmp));

			m_UserClassInfoArray.Add(uci);
		}
	}

	return bRet;
}

void CCashRegisterLoginMethod::GetUserClassInfoArray(CUserClassInfoArray & UserClassInfoArray)
{
	UserClassInfoArray.RemoveAll();
	UserClassInfoArray.Copy(m_UserClassInfoArray);
}


CString CCashRegisterLoginMethod::GetStandbyHost()
{
	return m_RespFields.GetValueAsString(_T("standbyHost"));
}

UINT CCashRegisterLoginMethod::GetBundleTimeUseBalance()
{
	return m_RespFields.GetValueAsUINT(_T("bundleTimeUseBalance"));
}

UINT CCashRegisterLoginMethod::GetMustSynchronize()
{
	return m_RespFields.GetValueAsUINT(_T("mustSynchronize"));
}

UINT NS_SERVLET::CCashRegisterLoginMethod::GetAllowCreditNegative()
{
	return m_RespFields.GetValueAsUINT(_T("allowCreditNegative"));
}	

UINT NS_SERVLET::CCashRegisterLoginMethod::GetIsInternetCafe()
{
	return m_RespFields.GetValueAsUINT(_T("IsInternetCafe"));
}

CString NS_SERVLET::CCashRegisterLoginMethod::GetCafeAddress()
{
	return m_RespFields.GetValueAsString(_T("CafeAddress"));
}

/*******************************************************************************************************/
CCashRegisterLoginMethod_atLogin::CCashRegisterLoginMethod_atLogin(void)
: CCashRegisterLoginMethod() //中心方法名称
{
	//CServletMethod::m_strMethodName = _T("CashRegisterLogin_atLogin");
}

CCashRegisterLoginMethod_atLogin::~CCashRegisterLoginMethod_atLogin(void)
{
}

//改写Execute，实现特定功能
BOOL CCashRegisterLoginMethod_atLogin::ExecuteCheckin()
{
	if (!OnBegin())//开始
	{
		return FALSE;
	}

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("CServletMethod::Execute - %s"), GetMethodName());
	}	
	//重置
	m_RespFields.ResetAllValue();
	m_RespFields.SetValue(_T("Status"), _T("-1")); //重置为-1

	BOOL bRet = FALSE;
	
	CString strParams(MakeParamsString());//组合成参数字串
	
	CString strResp;  
	
	//获取中心数据
	bRet = GetDecodeVerify(strParams,strResp);
	
	//若成功，如果当前状态为Restore,则改为Normal
	if(bRet)
	{
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
		{
			CBrokenNetwork::GetInstance()->EnableNetwork();
		}
	}
	else //若失败.提示重连信息，仍失败则弹出提示框并进入本地模式
	{
		AfxMessageBox(_T("网络故障，收银台重试连接中心，请点击确定后稍等片刻..."),MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);

		IBA_LOG(_T("Checkin failed.Will try again."));

		bRet = GetDecodeVerify(strParams,strResp);
		/*if(FALSE == bRet)//20160121liyajun
		{
			bRet = GetDecodeVerify(strParams,strResp);
		}*/
		if(FALSE == bRet)
		{
			IBA_LOG(_T("Checkin failed while try again. Enter Broken mode"));
			CIBAString strCaption;
			strCaption.LoadString(IDS_LOCALMODE_CAPTION);
			CIBAString strTmp;
			strTmp.LoadString(IDS_LOCALMODE_INFO);

			CMsgBoxDlg dlg(theApp.m_pMainWnd);
			//收银台与计费中心的网络已中断，将进入本地模式。请注意以下几点：\n\n1.收银台只能使用激活，充值退款，结账功能，不允许新开用户\n2.新激活用户原来账户中的金额将不可见，只能在本地模式下充值使用，已经激活的用户不受影响\n3.客户端只能查看余额，扣率，请勿随意上下机，不允许更换机器\n4.网络恢复后，请稍等几分钟，待完成补传本地模式下数据后，再使用\n
			dlg.SetText(strTmp,FALSE);
			dlg.SetType(MB_OK|MB_ICONQUESTION|MB_SYSTEMMODAL);			
			dlg.SetCaption(strCaption);			

			dlg.DoModal();


			CBrokenNetwork::GetInstance()->DisableNetwork(); 
			IBA_LOG(L"m_threadTestResume => Start");
			CBrokenNetwork::GetInstance()->m_threadTestResume->Start();//开启线程。网络恢复后，为不在线用户补传本地记录线程函数，网络确定恢复以后启动。只有在网络状态为正常后才启动。恢复完后从localMember删除记录。

			CBrokenNetwork::GetInstance()->bTestBrokenStarted = FALSE;
		}
		
	}

	OnEnd(bRet);

	return bRet;
}