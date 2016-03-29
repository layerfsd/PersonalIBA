#include "stdafx.h"
#include "shifttotalmethod.h"

using namespace NS_SERVLET;

CShiftTotalMethod::CShiftTotalMethod(void)
: CServletMethod(_T("ShiftTotal"))
{
	m_bUseQueryHost = TRUE; //使用查询机
	m_nRevTimeout = 120;	//超时2分钟

	AddParams();
	AddRespFields();
}

CShiftTotalMethod::~CShiftTotalMethod(void)
{
}

void CShiftTotalMethod::AddParams()
{
	CServletMethod::AddParams();

	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("needBalance"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("lastTranTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("lastTranType"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("LastRefNo"), EPT_NUMBER, 6);
}

void CShiftTotalMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CShiftTotalMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CShiftTotalMethod::SetNeedBalance(UINT newVal)
{
	m_Params.SetValue(_T("needBalance"), newVal);
}

void CShiftTotalMethod::SetLastTranTime(CString newVal)
{
	m_Params.SetValue(_T("lastTranTime"), newVal);
}

void CShiftTotalMethod::SetLastTranType(UINT newVal)
{
	m_Params.SetValue(_T("lastTranType"), newVal);
}

void CShiftTotalMethod::SetLastRefNo(UINT newVal)
{
	m_Params.SetValue(_T("LastRefNo"), newVal);
}

void CShiftTotalMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("ondutyTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("reserveAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("deposit"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("costExpense"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("creditAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("returnedAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("cashPay"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("creditCard"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("tempBalance"), EPT_NUMBER, 12);

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 2, TRUE, TRUE); //表示下个字段是个循环体
	m_RespFields.AddRespField(_T("CardInfoRecords"), EPT_STRING, 14); //每个CardInfoRecord的长度为14

	m_RespFields.AddRespField(_T("netBarPay"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("storeInAmount"), EPT_NUMBER, 12);

	//{ 2011/05/10-gxx: 增收宝( 消费，余额)
	m_RespFields.AddRespField(_T("thirdPartPay"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("ddnBalance"), EPT_NUMBER, 12);
	//}

	// 2011/07/26-8201-gxx: 点卡销售
	m_RespFields.AddRespField(_T("thirdPartCash"), EPT_NUMBER, 12);
	
	// 2014-11-3 - qsc 会员充值金额
	m_RespFields.AddRespField(_T("memberCreditAmount"), EPT_NUMBER, 12);
	
	// 2014-11-3 - qsc 微信充值金额
	m_RespFields.AddRespField(_T("wxpayAmount"), EPT_NUMBER, 12);
}

BOOL CShiftTotalMethod::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetRecordCount();

	m_CardInfoRecords.RemoveAll();

	if (nCount > 0)
	{
		m_CardInfoRecords.SetSize(nCount);
		
		CString strCardInfoRecords = m_RespFields.GetValueAsString(_T("CardInfoRecords"));
		
		INT nIndex = 0;

		for (INT i = 0; i < m_CardInfoRecords.GetCount(); i++)
		{
			CString strTmp = strCardInfoRecords.Mid(nIndex);
			m_CardInfoRecords.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_CardInfoRecords.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

CString CShiftTotalMethod::GetOndutyTime()
{
	return m_RespFields.GetValueAsString(_T("ondutyTime"));
}

INT CShiftTotalMethod::GetReserveAmount()
{
	return m_RespFields.GetValueAsUINT(_T("reserveAmount"));
}

INT CShiftTotalMethod::GetDeposit()
{
	return m_RespFields.GetValueAsINT(_T("deposit"));
}

INT CShiftTotalMethod::GetCostExpense()
{
	return m_RespFields.GetValueAsINT(_T("costExpense"));
}

INT CShiftTotalMethod::GetCreditAmount()
{
	return m_RespFields.GetValueAsINT(_T("creditAmount"));
}

INT CShiftTotalMethod::GetReturnedAmount()
{
	return m_RespFields.GetValueAsINT(_T("returnedAmount"));
}

INT CShiftTotalMethod::GetCashPay()
{
	return m_RespFields.GetValueAsINT(_T("cashPay"));
}

INT CShiftTotalMethod::GetCreditCard()
{
	return m_RespFields.GetValueAsINT(_T("creditCard"));
}

INT CShiftTotalMethod::GetTempBalance()
{
	return m_RespFields.GetValueAsINT(_T("tempBalance"));
}

UINT CShiftTotalMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

CString CShiftTotalMethod::GetCardInfoRecords()
{
	return m_RespFields.GetValueAsString(_T("CardInfoRecords"));
}

UINT CShiftTotalMethod::GetClassId(INT nIndex)
{
	return m_CardInfoRecords.GetAt(nIndex).GetClassId();
}

UINT CShiftTotalMethod::GetOpenQuantity(INT nIndex)
{
	return m_CardInfoRecords.GetAt(nIndex).GetOpenQuantity();
}

UINT CShiftTotalMethod::GetReturnedQuantity(INT nIndex)
{
	return m_CardInfoRecords.GetAt(nIndex).GetReturnedQuantity();
}

INT CShiftTotalMethod::GetNetBarPay()
{
	return m_RespFields.GetValueAsINT(_T("netBarPay"));
}

INT CShiftTotalMethod::GetStoreInAmount()
{
	return m_RespFields.GetValueAsINT(_T("storeInAmount"));
}

//******************************************************************************

CCardInfoRecord::CCardInfoRecord()
{
	m_RespFields.AddRespField(_T("classId"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("openQuantity"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("returnedQuantity"), EPT_NUMBER, 6);
}

UINT CCardInfoRecord::GetClassId()
{
	return m_RespFields.GetValueAsUINT(_T("classId"));
}

UINT CCardInfoRecord::GetOpenQuantity()
{
	return m_RespFields.GetValueAsUINT(_T("openQuantity"));
}

UINT CCardInfoRecord::GetReturnedQuantity()
{
	return m_RespFields.GetValueAsUINT(_T("returnedQuantity"));
}


INT CShiftTotalMethod::GetZSBConsume()
{
	return m_RespFields.GetValueAsINT(_T("thirdPartPay"));
}

INT CShiftTotalMethod::GetZSBBalance()
{
	return m_RespFields.GetValueAsINT(_T("ddnBalance"));
}

INT NS_SERVLET::CShiftTotalMethod::GetDiankaAmount()
{
	return m_RespFields.GetValueAsINT(_T("thirdPartCash"));
}

INT NS_SERVLET::CShiftTotalMethod::GetWxPayAmount()
{
	return m_RespFields.GetValueAsINT(_T("wxpayAmount"));
}
