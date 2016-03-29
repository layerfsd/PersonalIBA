#include "stdafx.h"
#include "..\Encrypt.h"
#include "BalanceMethod.h"

using namespace NS_SERVLET;

CBalanceMethod::CBalanceMethod(void)
: CServletMethod(_T("Balance")) //中心方法名称
, m_nMemberId(0)
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBalanceMethod::~CBalanceMethod(void)
{
}

CString CBalanceMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CBalanceMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("Cardid"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("Round"), EPT_NUMBER, 1);

	//2011/3/11/gxx-是否实际在线
	m_Params.AddParam(_T("onlineInFact"),EPT_NUMBER,1);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CBalanceMethod::SetMemberId(UINT newVal)
{
	m_nMemberId = newVal;

	m_Params.SetValue(_T("memberId"), newVal);
}

void CBalanceMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CBalanceMethod::SetRound(UINT newVal)
{
	m_Params.SetValue(_T("Round"), newVal);
}

void CBalanceMethod::SetCardId(UINT newVal)
{
	m_Params.SetValue(_T("Cardid"), newVal);
}

void NS_SERVLET::CBalanceMethod::SetOnlineInFact(UINT newVal)
{
	m_Params.SetValue(_T("onlineInFact"),newVal);
}

//--------------

UINT CBalanceMethod::GetCardId()
{
	return _ttoi(m_Params.GetValue(_T("Cardid")));
}

void CBalanceMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("Online"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("Balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("onlineNetBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("onlineSerialNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("onlineAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("NetBarCount"), EPT_NUMBER, 3);
	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("usePresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("userClass"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("guarantyMoney"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("accountNetbarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("memberName"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("username"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("sex"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("personalId"), EPT_STRING, 18);
	m_RespFields.AddRespField(_T("idType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("serailNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("currentPort"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("cashBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("returnAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("importantMember"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("termId"), EPT_STRING, 8);
	m_RespFields.AddRespField(_T("auditId"), EPT_STRING, 12);
	m_RespFields.AddRespField(_T("totalPoint"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("isMember"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("disallowLogin"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("logoutTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("epsk"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("payType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("startTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("endTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("price"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("openNetbarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("allowReturn"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("disallowLogout"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("lastConsumeTime"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("lastConsumeAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("epwd"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("needActivation"), EPT_STRING, 1);
	m_RespFields.AddRespField(_T("serverTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("IdDepart"), EPT_STRING, 30);
	m_RespFields.AddRespField(_T("Tel"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("Address"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("Job"), EPT_STRING, 2, FALSE);
}

UINT CBalanceMethod::GetOnline()
{
	return m_RespFields.GetValueAsUINT(_T("Online"));
}

INT CBalanceMethod::GetBalance()
{
	return m_RespFields.GetValueAsINT(_T("Balance"));
}

UINT CBalanceMethod::GetOnlineNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("onlineNetBarId"));
}

UINT CBalanceMethod::GetOnlineSerialNo()
{
	return m_RespFields.GetValueAsUINT(_T("onlineSerialNo"));
}

UINT CBalanceMethod::GetOnlineAmount()
{
	return m_RespFields.GetValueAsUINT(_T("onlineAmount"));
}

UINT CBalanceMethod::GetNetBarCount()
{
	return m_RespFields.GetValueAsUINT(_T("NetBarCount"));
}

UINT CBalanceMethod::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

INT CBalanceMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsINT(_T("creditPresent"));
}

INT CBalanceMethod::GetUsePresent()
{
	return m_RespFields.GetValueAsINT(_T("usePresent"));
}

UINT CBalanceMethod::GetUserClass()
{
	return m_RespFields.GetValueAsUINT(_T("userClass"));
}

UINT CBalanceMethod::GetGuarantyMoney()
{
	return m_RespFields.GetValueAsUINT(_T("guarantyMoney"));
}

UINT CBalanceMethod::GetAccountNetbarId()
{
	return m_RespFields.GetValueAsUINT(_T("accountNetbarId"));
}

CString CBalanceMethod::GetMemberName()
{
	return m_RespFields.GetValueAsString(_T("memberName"));
}

CString CBalanceMethod::GetUserName()
{
	return m_RespFields.GetValueAsString(_T("username"));
}

UINT CBalanceMethod::GetSex()
{
	return m_RespFields.GetValueAsUINT(_T("sex"));
}

CString CBalanceMethod::GetPersonalId()
{
	return m_RespFields.GetValueAsString(_T("personalId"));
}

UINT CBalanceMethod::GetIdType()
{
	return m_RespFields.GetValueAsUINT(_T("idType"));
}

CString CBalanceMethod::GetSerailNum()
{
	return m_RespFields.GetValueAsString(_T("serailNum"));
}

UINT CBalanceMethod::GetCurrentPoint()
{
	return m_RespFields.GetValueAsUINT(_T("currentPort"));
}

INT CBalanceMethod::GetCashBalance()
{
	return m_RespFields.GetValueAsINT(_T("cashBalance"));
}

INT CBalanceMethod::GetReturnAmount()
{
	return m_RespFields.GetValueAsINT(_T("returnAmount"));
}

UINT CBalanceMethod::GetImportantMember()
{
	return m_RespFields.GetValueAsUINT(_T("importantMember"));
}

CString CBalanceMethod::GetTermId()
{
	return m_RespFields.GetValueAsString(_T("termId"));
}

CString CBalanceMethod::GetAuditId()
{
	return m_RespFields.GetValueAsString(_T("auditId"));
}

UINT CBalanceMethod::GetTotalPoint()
{
	return m_RespFields.GetValueAsUINT(_T("totalPoint"));
}

UINT CBalanceMethod::GetIsMember()
{
	return m_RespFields.GetValueAsUINT(_T("isMember"));
}

UINT CBalanceMethod::GetDisallowLogin()
{
	return m_RespFields.GetValueAsUINT(_T("disallowLogin"));
}

CString CBalanceMethod::GetLogoutTime()
{
	return m_RespFields.GetValueAsString(_T("logoutTime"));
}

CString CBalanceMethod::GetEpsk()
{
	CString strTmp = m_RespFields.GetValueAsString(_T("epsk"));
	
	CString strPsk;

	CEncrypt::Decrypt(strTmp, m_strPSK, strPsk);

	return strPsk;
}

UINT CBalanceMethod::GetPayType()
{
	return m_RespFields.GetValueAsUINT(_T("payType"));
}

CString CBalanceMethod::GetStartTime()
{
	return m_RespFields.GetValueAsString(_T("startTime"));
}

CString CBalanceMethod::GetEndTime()
{
	return m_RespFields.GetValueAsString(_T("endTime"));
}

UINT CBalanceMethod::GetPrice()
{
	return m_RespFields.GetValueAsUINT(_T("price"));
}

UINT CBalanceMethod::GetOpenNetbarId()
{
	return m_RespFields.GetValueAsUINT(_T("openNetbarId"));
}

UINT CBalanceMethod::GetAllowReturn()
{
	return m_RespFields.GetValueAsUINT(_T("allowReturn"));
}

UINT CBalanceMethod::GetDisallowLogout()
{
	return m_RespFields.GetValueAsUINT(_T("disallowLogout"));
}

UINT CBalanceMethod::GetLastConsumeTime()
{
	return m_RespFields.GetValueAsUINT(_T("lastConsumeTime"));
}

INT CBalanceMethod::GetLastConsumeAmount()
{
	return m_RespFields.GetValueAsINT(_T("lastConsumeAmount"));
}

CString CBalanceMethod::GetPassword()
{
	CString strPwd;

	CString strTmp = m_RespFields.GetValueAsString(_T("epwd")).Trim();

	if (!strTmp.IsEmpty())
	{
		CEncrypt::Decrypt(strTmp, GetEpsk(), strPwd, FALSE);

		strPwd.TrimRight('0'); //去除后面的所有0
	}

	return strPwd;
}

UINT CBalanceMethod::GetNeedActivation()
{
	return m_RespFields.GetValueAsUINT(_T("needActivation"));
}

INT CBalanceMethod::GetTotalBalacne() //无送充值余额 + 可用“有送充值”余额 + 可用“赠送帐户”余额 + 用户预扣金额
{
	INT nAllMoney = GetCashBalance() + GetUsePresent() + GetCreditPresent() + GetOnlineAmount();

	return nAllMoney;
}

CString CBalanceMethod::GetServerTime()
{
	return m_RespFields.GetValueAsString(_T("serverTime"));
}

//如果中心返回不可退款或是VIP用户
BOOL CBalanceMethod::IsNetbarMember()
{
	if (GetAllowReturn() == 0) //不可退
	{
		return TRUE;
	}

	if (GetIsMember() == 1)//嘟嘟牛会员
	{
		return TRUE;
	}

	return FALSE;
}

CString CBalanceMethod::GetIdDepart()
{
	return m_RespFields.GetValueAsString(_T("IdDepart"));
}

CString CBalanceMethod::GetAddress()
{
	return m_RespFields.GetValueAsString(_T("Address"));
}

CString CBalanceMethod::GetTel()
{
	return m_RespFields.GetValueAsString(_T("Tel"));
}

CString CBalanceMethod::GetJob()
{
	return m_RespFields.GetValueAsString(_T("Job"));
}

BOOL CBalanceMethod::ExecuteBalance()
{
	return Execute();
}