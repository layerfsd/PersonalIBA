#include "stdafx.h"
#include "CheckInMethod.h"
#include "..\IBAHelpper.h"
#include "..\Encrypt.h"

using namespace NS_SERVLET;

CCheckInMethod::CCheckInMethod(void)
: CServletMethod(_T("Checkin")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCheckInMethod::~CCheckInMethod(void)
{
}

void CCheckInMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("netbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("timeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("termId"), EPT_STRING, 8);
	m_Params.AddParam(_T("encryptMode"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("cardSerialNum"), EPT_STRING, 20);
	m_Params.AddParam(_T("readerSerialNum"), EPT_STRING, 12);
	m_Params.AddParam(_T("macAddress"), EPT_STRING, 20);
	m_Params.AddParam(_T("ipAddress"), EPT_STRING, 16);
	m_Params.AddParam(_T("clientVersion"), EPT_STRING, 20);
	m_Params.AddParam(_T("autoLogin"), EPT_NUMBER, 1);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCheckInMethod::SetNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("netbarId"), newVal);
}

void CCheckInMethod::SetTimeId(UINT newVal)
{
	m_Params.SetValue(_T("timeId"), newVal);
}

void CCheckInMethod::SetTermId(CString newVal)
{
	m_Params.SetValue(_T("termId"), newVal);
}

void CCheckInMethod::SetEncryptMode(UINT newVal)
{
	m_Params.SetValue(_T("encryptMode"), newVal);
}

void CCheckInMethod::SetCardSerialNum(CString newVal)
{
	m_Params.SetValue(_T("cardSerialNum"), newVal);
}

void CCheckInMethod::SetReaderSerialNum(CString newVal)
{
	m_Params.SetValue(_T("readerSerialNum"), newVal);
}

void CCheckInMethod::SetMacAddress(CString newVal)
{
	m_Params.SetValue(_T("macAddress"), newVal);
}

void CCheckInMethod::SetIpAddress(CString newVal)
{
	m_Params.SetValue(_T("ipAddress"), newVal);
}

void CCheckInMethod::SetClientVersion(CString newVal)
{
	m_Params.SetValue(_T("clientVersion"), newVal);
}

void CCheckInMethod::SetAutoLogin(UINT newVal)
{
	m_Params.SetValue(_T("autoLogin"), newVal);
}

void CCheckInMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("freezeMoney"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("unitTime"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("nextCheckinTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("dynamicMaxAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("epdk"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("availableBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("policyInfo"), EPT_NUMBER, 96);
	m_RespFields.AddRespField(_T("username"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("sex"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("personalId"), EPT_STRING, 18);
	m_RespFields.AddRespField(_T("idType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("idDepart"), EPT_STRING, 30);
	m_RespFields.AddRespField(_T("country"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("payType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("auditType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("serverTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("currentPoint"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("classId"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("isMember"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("domainId"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("mainHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("backupHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("dodonewHost"), EPT_STRING, 22);
	m_RespFields.AddRespField(_T("cardId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("epsk"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("bundleType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("beginBundleTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("endBundleTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("serialNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("forceAudit"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("nation"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("tel"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("eReaderPsk"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("eDodonewKey"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("pcClass"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("disallowLogout"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("payMode"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("lastRemainAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("unitAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("minConsume"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("beginFreeTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("nextPayFreeTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("firstCheckinTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("consumeAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("currCheckinTime"), EPT_STRING, 14);
}

UINT CCheckInMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CCheckInMethod::GetFreezeMoney()
{
	return m_RespFields.GetValueAsUINT(_T("freezeMoney"));
}

UINT CCheckInMethod::GetUnitTime()
{
	return m_RespFields.GetValueAsUINT(_T("unitTime"));
}

UINT CCheckInMethod::GetNextCheckinTime()
{
	return m_RespFields.GetValueAsUINT(_T("nextCheckinTime"));
}

UINT CCheckInMethod::GetDynamicMaxAmount()
{
	return m_RespFields.GetValueAsUINT(_T("dynamicMaxAmount"));
}

CString CCheckInMethod::GetEpdk()
{
	return m_RespFields.GetValueAsString(_T("epdk"));
}

UINT CCheckInMethod::GetAvailableBalance()
{
	return m_RespFields.GetValueAsUINT(_T("availableBalance"));
}

CString CCheckInMethod::GetPolicyInfo()
{
	return m_RespFields.GetValueAsString(_T("policyInfo"));
}

CString CCheckInMethod::GetUsername()
{
	return m_RespFields.GetValueAsString(_T("username"));
}

UINT CCheckInMethod::GetSex()
{
	return m_RespFields.GetValueAsUINT(_T("sex"));
}

CString CCheckInMethod::GetPersonalId()
{
	return m_RespFields.GetValueAsString(_T("personalId"));
}

UINT CCheckInMethod::GetIdType()
{
	return m_RespFields.GetValueAsUINT(_T("idType"));
}

CString CCheckInMethod::GetIdDepart()
{
	return m_RespFields.GetValueAsString(_T("idDepart"));
}

UINT CCheckInMethod::GetCountry()
{
	return m_RespFields.GetValueAsUINT(_T("country"));
}

UINT CCheckInMethod::GetPayType()
{
	return m_RespFields.GetValueAsUINT(_T("payType"));
}

UINT CCheckInMethod::GetAuditType()
{
	return m_RespFields.GetValueAsUINT(_T("auditType"));
}

CString CCheckInMethod::GetServerTime()
{
	return m_RespFields.GetValueAsString(_T("serverTime"));
}

UINT CCheckInMethod::GetCurrentPoint()
{
	return m_RespFields.GetValueAsUINT(_T("currentPoint"));
}

UINT CCheckInMethod::GetClassId()
{
	return m_RespFields.GetValueAsUINT(_T("classId"));
}

UINT CCheckInMethod::GetIsMember()
{
	return m_RespFields.GetValueAsUINT(_T("isMember"));
}

UINT CCheckInMethod::GetDomainId()
{
	return m_RespFields.GetValueAsUINT(_T("domainId"));
}

CString CCheckInMethod::GetMainHost()
{
	return m_RespFields.GetValueAsString(_T("mainHost"));
}

CString CCheckInMethod::GetBackupHost()
{
	return m_RespFields.GetValueAsString(_T("backupHost"));
}

CString CCheckInMethod::GetDodonewHost()
{
	return m_RespFields.GetValueAsString(_T("dodonewHost"));
}

UINT CCheckInMethod::GetCardId()
{
	return m_RespFields.GetValueAsUINT(_T("cardId"));
}

CString CCheckInMethod::GetEpsk()
{
	return m_RespFields.GetValueAsString(_T("epsk"));
}

UINT CCheckInMethod::GetBundleType()
{
	return m_RespFields.GetValueAsUINT(_T("bundleType"));
}

UINT CCheckInMethod::GetBeginBundleTime()
{
	return m_RespFields.GetValueAsUINT(_T("beginBundleTime"));
}

UINT CCheckInMethod::GetEndBundleTime()
{
	return m_RespFields.GetValueAsUINT(_T("endBundleTime"));
}

UINT CCheckInMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

UINT CCheckInMethod::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

CString CCheckInMethod::GetSerialNum()
{
	return m_RespFields.GetValueAsString(_T("serialNum"));
}

UINT CCheckInMethod::GetBalance()
{
	return m_RespFields.GetValueAsUINT(_T("balance"));
}

UINT CCheckInMethod::GetForceAudit()
{
	return m_RespFields.GetValueAsUINT(_T("forceAudit"));
}

CString CCheckInMethod::GetNation()
{
	return m_RespFields.GetValueAsString(_T("nation"));
}

CString CCheckInMethod::GetTel()
{
	return m_RespFields.GetValueAsString(_T("tel"));
}

CString CCheckInMethod::GetEReaderPsk()
{
	return m_RespFields.GetValueAsString(_T("eReaderPsk"));
}

CString CCheckInMethod::GetEDodonewKey()
{
	return m_RespFields.GetValueAsString(_T("eDodonewKey"));
}

UINT CCheckInMethod::GetPcClass()
{
	return m_RespFields.GetValueAsUINT(_T("pcClass"));
}

UINT CCheckInMethod::GetDisallowLogout()
{
	return m_RespFields.GetValueAsUINT(_T("disallowLogout"));
}

UINT CCheckInMethod::GetPayMode()
{
	return m_RespFields.GetValueAsUINT(_T("payMode"));
}

UINT CCheckInMethod::GetLastRemainAmount()
{
	return m_RespFields.GetValueAsUINT(_T("lastRemainAmount"));
}

UINT CCheckInMethod::GetUnitAmount()
{
	return m_RespFields.GetValueAsUINT(_T("unitAmount"));
}

UINT CCheckInMethod::GetMinConsume()
{
	return m_RespFields.GetValueAsUINT(_T("minConsume"));
}

UINT CCheckInMethod::GetBeginFreeTime()
{
	return m_RespFields.GetValueAsUINT(_T("beginFreeTime"));
}

UINT CCheckInMethod::GetNextPayFreeTime()
{
	return m_RespFields.GetValueAsUINT(_T("nextPayFreeTime"));
}

CString CCheckInMethod::GetFirstCheckinTime()
{
	return m_RespFields.GetValueAsString(_T("firstCheckinTime"));
}

UINT CCheckInMethod::GetConsumeAmount()
{
	return m_RespFields.GetValueAsUINT(_T("consumeAmount"));
}

CString CCheckInMethod::GetCurrCheckinTime()
{
	return m_RespFields.GetValueAsString(_T("currCheckinTime"));
}


BOOL CCheckInMethod::OnBegin()
{
	//这里计算密钥

	CString strTmp, strMD5;

	strTmp.Append(m_Params.GetValue(_T("cardSerialNum")).Trim());
	strTmp.Append(m_strPwd);
	strTmp.Append(m_Params.GetValue(_T("termId")).Trim());

	CEncrypt::CalcMD5(strTmp, strMD5);

	CStringA strMD5A = CW2A(strMD5);

	UCHAR cBuf[8] = {0};

	CopyMemory(cBuf, strMD5A.GetBuffer(), 8);

	for (INT i = 1; i < 4; i++)
	{
		for (INT j = 0; j < 8; j++)
		{
			cBuf[j] = cBuf[j] ^ strMD5A.GetAt(i * 8 + j);
		}	
	}

	strMD5A.ReleaseBuffer();

	m_strPSK.Empty();

	for (UINT i = 0; i < 8; i++)
	{
		m_strPSK.AppendFormat(_T("%02X"), cBuf[i]);
	}

	return TRUE;
}

void CCheckInMethod::SetUserPwd(CString newVal)
{
	m_strPwd = newVal;

	CIBAHelpper::FillString(m_strPwd, 8, '0');
}