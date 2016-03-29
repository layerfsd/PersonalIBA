#include "stdafx.h"
#include ".\registermethod.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"
#include "LastTranInfo.h"

using namespace NS_SERVLET;

CRegisterMethod::CRegisterMethod(void)
: CServletMethod(_T("Register"))
{
	AddParams();
	AddRespFields();
}

CRegisterMethod::~CRegisterMethod(void)
{
}

void CRegisterMethod::AddParams()
{
	CServletMethod::AddParams();

	m_Params.AddParam(_T("cardId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("userClass"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("costExpense"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("guarantyMoney"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("accountNetbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("userName"), EPT_STRING, 10);
	m_Params.AddParam(_T("Sex"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("PersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("idType"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("IdDepart"), EPT_STRING, 30);
	m_Params.AddParam(_T("country"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("auditType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("auditId"), EPT_STRING, 12);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);

	m_Params.AddParam(_T("address"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("email"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Tel"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Job"), EPT_STRING, 2, EPLT_VARIABLE);

	m_Params.AddParam(_T("creditAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("noPresent"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("noCard"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("activityId"), EPT_NUMBER, 12);

	/*
	4	timeId	N	包时ID(9999表示自定义包时)
		1	pcClass	N	机器区域
		14	beginTime	N	包时开始时间
		6	timePass	N	包时时长(分钟)
		6	bundleAmount	N	包时金额(分)
		1	accountType	N	包时账户

	*/
	m_Params.AddParam(_T("timeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("pcClass"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("beginTime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("timePass"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("bundleAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("accountType"), EPT_NUMBER, 1);

	// 2014-11-12
	m_Params.AddParam(_T("dataType"),	EPT_NUMBER, 1);	
	// 固定传身份证
	m_Params.SetValue(_T("dataType"), 0);		// 0 刷身份证录入、1 手动录入、2通过扫描证件录入、4 手机号码录入
}

void CRegisterMethod::SetCardId(CString newVal)
{
	m_Params.SetValue(_T("cardId"), newVal);
}

void CRegisterMethod::SetUserClass(UINT newVal)
{
	m_Params.SetValue(_T("userClass"), newVal);
}

void CRegisterMethod::SetCostExpense(UINT newVal)
{
	m_Params.SetValue(_T("costExpense"), newVal);
}

void CRegisterMethod::SetGuarantyMoney(UINT newVal)
{
	m_Params.SetValue(_T("guarantyMoney"), newVal);
}

void CRegisterMethod::SetAccountNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("accountNetbarId"), newVal);
}

void CRegisterMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CRegisterMethod::SetPassword(CString newVal)
{
	CString strTmp;

	if (newVal.IsEmpty()) // 空密码
	{
		strTmp = _T("                "); // 16个空格
	}
	else
	{
		CIBAHelpper::FillString(newVal, 8, '0');

		if (GetCardType() == ECT_ID)
		{
			CEncrypt::CalcMac(newVal, strTmp, m_strPSK, FALSE);
		}
		else if (GetCardType() == ECT_IC)
		{
			CEncrypt::CalcMac(newVal, strTmp, m_strPSK, TRUE);
		}
	}
	
	m_Params.SetValue(_T("password"), strTmp);
}

void CRegisterMethod::SetUserName(CString newVal)
{
	m_Params.SetValue(_T("userName"), newVal);
}

void CRegisterMethod::SetSex(UINT newVal)
{
	m_Params.SetValue(_T("Sex"), newVal);
}

void CRegisterMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CRegisterMethod::SetIdType(UINT newVal)
{
	m_Params.SetValue(_T("idType"), newVal);
}

void CRegisterMethod::SetIdDepart(CString newVal)
{
	m_Params.SetValue(_T("IdDepart"), newVal);
}

void CRegisterMethod::SetCountry(CString newVal)
{
	m_Params.SetValue(_T("country"), newVal);
}

void CRegisterMethod::SetAuditType(UINT newVal)
{
	m_Params.SetValue(_T("auditType"), newVal);
}

void CRegisterMethod::SetAuditId(CString newVal)
{
	m_Params.SetValue(_T("auditId"), newVal);
}

void CRegisterMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CRegisterMethod::SetAddress(CString newVal)
{
	m_Params.SetValue(_T("address"), newVal);
}

void CRegisterMethod::SetEmail(CString newVal)
{
	m_Params.SetValue(_T("email"), newVal);
}

void CRegisterMethod::SetTel(CString newVal)
{
	m_Params.SetValue(_T("Tel"), newVal);
}

void CRegisterMethod::SetJob(CString newVal)
{
	m_Params.SetValue(_T("Job"), newVal);
}

void CRegisterMethod::SetCreditAmount(UINT newVal)
{
	m_Params.SetValue(_T("creditAmount"), newVal);
}

void CRegisterMethod::SetNoPresent(UINT newVal)
{
	m_Params.SetValue(_T("noPresent"), newVal);
}

void CRegisterMethod::SetNoCard(UINT newVal)
{
	m_Params.SetValue(_T("noCard"), newVal);
}

void CRegisterMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CRegisterMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CRegisterMethod::SetActivityId(UINT newVal)
{
	m_Params.SetValue(_T("activityId"), newVal);
}

void CRegisterMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("serialNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("refNo1"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Trantime"), EPT_STRING, 14);

	/*
	89	6	amoun	N	实际充值金额
	95	6	guarantyMoney	N	实收押金
	101	6	costExpense	N	实收工本费
*/
	m_RespFields.AddRespField(_T("amoun"), EPT_STRING, 6);
	m_RespFields.AddRespField(_T("guarantyMoney"), EPT_STRING, 6);
	m_RespFields.AddRespField(_T("costExpense"), EPT_STRING, 6);
}

UINT CRegisterMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

UINT CRegisterMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

CString CRegisterMethod::GetSerialNum()
{
	return m_RespFields.GetValueAsString(_T("serialNum"));
}

UINT CRegisterMethod::GetRefNo1()
{
	return m_RespFields.GetValueAsUINT(_T("refNo1"));
}

UINT CRegisterMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsUINT(_T("creditPresent"));
}

CString CRegisterMethod::GetTrantime()
{
	return m_RespFields.GetValueAsString(_T("Trantime"));
}


CString CRegisterMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}
	
	return strTmp;
}

void CRegisterMethod::SaveTranInfo()
{
	if (GetStatusCode() == 0)
	{
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(GetRefNo());
		TranInfo.SetLastTranTime(GetTrantime());
		TranInfo.SetLastTranType(1);
		TranInfo.Save();
	}
}

void CRegisterMethod::SetActivationType( UINT nType )
{
	m_Params.SetValue(_T("dataType"), nType);		// 0 刷身份证录入、1 手动录入、2通过扫描证件录入、4 手机号码录入
}
