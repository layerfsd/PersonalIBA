#include "stdafx.h"
#include "MemberInfoMethod.h"

using namespace NS_SERVLET;

CMemberInfoMethod::CMemberInfoMethod(void)
: CServletMethod(_T("MemberInfo")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CMemberInfoMethod::~CMemberInfoMethod(void)
{
	m_MemberInfos.RemoveAll();
}

CString CMemberInfoMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}


BOOL CMemberInfoMethod::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetMemberCount();

	m_MemberInfos.RemoveAll();

	if (nCount > 0)
	{
		m_MemberInfos.SetSize(nCount);
		
		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_MemberInfos.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_MemberInfos.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_MemberInfos.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

void CMemberInfoMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("username"), EPT_STRING, 10);
	m_Params.AddParam(_T("PersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("SerialNum"), EPT_STRING, 20);
	m_Params.AddParam(_T("minMemberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("cardId"), EPT_STRING, 12);
	m_Params.AddParam(_T("Round"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("Available"), EPT_NUMBER, 1);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CMemberInfoMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CMemberInfoMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CMemberInfoMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CMemberInfoMethod::SetUsername(CString newVal)
{
	m_Params.SetValue(_T("username"), newVal);
}

void CMemberInfoMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CMemberInfoMethod::SetSerialNum(CString newVal)
{
	m_Params.SetValue(_T("SerialNum"), newVal);
}

void CMemberInfoMethod::SetMinMemberId(UINT newVal)
{
	m_Params.SetValue(_T("minMemberId"), newVal);
}

void CMemberInfoMethod::SetCardId(UINT newVal)
{
	m_Params.SetValue(_T("cardId"), newVal);
}

void CMemberInfoMethod::SetRound(UINT newVal)
{
	m_Params.SetValue(_T("Round"), newVal);
}

void CMemberInfoMethod::SetAvailable(UINT newVal)
{
	m_Params.SetValue(_T("Available"), newVal);
}

void CMemberInfoMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("memberCount"), EPT_NUMBER, 2);
}

UINT CMemberInfoMethod::GetMemberCount()
{
	return m_RespFields.GetValueAsUINT(_T("memberCount"));
}

UINT CMemberInfoMethod::GetNetBarId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetNetBarId();
}

UINT CMemberInfoMethod::GetMemberId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetMemberId();
}

UINT CMemberInfoMethod::GetAvailable(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetAvailable();
}

UINT CMemberInfoMethod::GetCardId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetCardId();
}

UINT CMemberInfoMethod::GetUserClass(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetUserClass();
}

UINT CMemberInfoMethod::GetGuarantyMoney(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetGuarantyMoney();
}

UINT CMemberInfoMethod::GetAccountNetbarId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetAccountNetbarId();
}

CString CMemberInfoMethod::GetMemberName(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetMemberName();
}

CString CMemberInfoMethod::GetUserName(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetUserName();
}

UINT CMemberInfoMethod::GetSex(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetSex();
}

CString CMemberInfoMethod::GetPersonalId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetPersonalId();
}

UINT CMemberInfoMethod::GetIdType(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetIdType();
}

CString CMemberInfoMethod::GetIdDepart(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetIdDepart();
}

UINT CMemberInfoMethod::GetCountry(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetCountry();
}

CString CMemberInfoMethod::GetSerailNum(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetSerailNum();
}

CString CMemberInfoMethod::GetRzxId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetRzxId();
}

CString CMemberInfoMethod::GetGlwId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetGlwId();
}

CString CMemberInfoMethod::GetMgId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetMgId();
}

CString CMemberInfoMethod::GetOtherId(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetOtherId();
}

UINT CMemberInfoMethod::GetBalance(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetBalance();
}

CString CMemberInfoMethod::GetAddress(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetAddress();
}

CString CMemberInfoMethod::GetEmail(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetEmail();
}

CString CMemberInfoMethod::GetTel(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetTel();
}

CString CMemberInfoMethod::GetJob(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetJob();
}

UINT CMemberInfoMethod::GetCurrentPort(INT nIndex)
{
	return m_MemberInfos.GetAt(nIndex).GetCurrentPort();
}

//******************************************************************************

CMemberInfo::CMemberInfo()
{
	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("Available"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("cardId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("userClass"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("guarantyMoney"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("accountNetbarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("memberName"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("Username"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("Sex"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("PersonalId"), EPT_NUMBER, 18);
	m_RespFields.AddRespField(_T("idType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("IdDepart"), EPT_STRING, 30);
	m_RespFields.AddRespField(_T("Country"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("SerailNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("rzxId"), EPT_STRING, 12);
	m_RespFields.AddRespField(_T("glwId"), EPT_STRING, 12);
	m_RespFields.AddRespField(_T("MgId"), EPT_STRING, 12);
	m_RespFields.AddRespField(_T("otherId"), EPT_STRING, 12);
	m_RespFields.AddRespField(_T("Balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Address"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("Email"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("Tel"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("Job"), EPT_STRING, 2, FALSE);
	m_RespFields.AddRespField(_T("currentPort"), EPT_NUMBER, 6);
}


UINT CMemberInfo::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

UINT CMemberInfo::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

UINT CMemberInfo::GetAvailable()
{
	return m_RespFields.GetValueAsUINT(_T("Available"));
}

UINT CMemberInfo::GetCardId()
{
	return m_RespFields.GetValueAsUINT(_T("cardId"));
}

UINT CMemberInfo::GetUserClass()
{
	return m_RespFields.GetValueAsUINT(_T("userClass"));
}

UINT CMemberInfo::GetGuarantyMoney()
{
	return m_RespFields.GetValueAsUINT(_T("guarantyMoney"));
}

UINT CMemberInfo::GetAccountNetbarId()
{
	return m_RespFields.GetValueAsUINT(_T("accountNetbarId"));
}

CString CMemberInfo::GetMemberName()
{
	return m_RespFields.GetValueAsString(_T("memberName"));
}

CString CMemberInfo::GetUserName()
{
	return m_RespFields.GetValueAsString(_T("Username"));
}

UINT CMemberInfo::GetSex()
{
	return m_RespFields.GetValueAsUINT(_T("Sex"));
}

CString CMemberInfo::GetPersonalId()
{
	return m_RespFields.GetValueAsString(_T("PersonalId"));
}

UINT CMemberInfo::GetIdType()
{
	return m_RespFields.GetValueAsUINT(_T("idType"));
}

CString CMemberInfo::GetIdDepart()
{
	return m_RespFields.GetValueAsString(_T("IdDepart"));
}

UINT CMemberInfo::GetCountry()
{
	return m_RespFields.GetValueAsUINT(_T("Country"));
}

CString CMemberInfo::GetSerailNum()
{
	return m_RespFields.GetValueAsString(_T("SerailNum"));
}

CString CMemberInfo::GetRzxId()
{
	return m_RespFields.GetValueAsString(_T("rzxId"));
}

CString CMemberInfo::GetGlwId()
{
	return m_RespFields.GetValueAsString(_T("glwId"));
}

CString CMemberInfo::GetMgId()
{
	return m_RespFields.GetValueAsString(_T("MgId"));
}

CString CMemberInfo::GetOtherId()
{
	return m_RespFields.GetValueAsString(_T("otherId"));
}

UINT CMemberInfo::GetBalance()
{
	return m_RespFields.GetValueAsUINT(_T("Balance"));
}

CString CMemberInfo::GetAddress()
{
	return m_RespFields.GetValueAsString(_T("Address"));
}

CString CMemberInfo::GetEmail()
{
	return m_RespFields.GetValueAsString(_T("Email"));
}

CString CMemberInfo::GetTel()
{
	return m_RespFields.GetValueAsString(_T("Tel"));
}

CString CMemberInfo::GetJob()
{
	return m_RespFields.GetValueAsString(_T("Job"));
}

UINT CMemberInfo::GetCurrentPort()
{
	return m_RespFields.GetValueAsUINT(_T("currentPort"));
}

