#include "stdafx.h"
#include ".\dutymethod.h"
#include "..\IBAHelpper.h"

using namespace  NS_SERVLET;

COnDutyMethod::COnDutyMethod(void)
: CServletMethod(_T("OnDuty"))
{
	AddParams();
	AddRespFields();
}

COnDutyMethod::~COnDutyMethod(void)
{
}

void COnDutyMethod::AddParams()
{
	CServletMethod::AddParams();

	m_Params.AddParam(_T("shiftName"), EPT_NUMBER, 1);	
	m_Params.AddParam(_T("reserveAmount"), EPT_NUMBER, 12);	
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);	
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
}


void COnDutyMethod::SetShiftType(UINT newVal)
{
	m_Params.SetValue(_T("shiftName"), newVal);
	m_nShiftType = newVal;
}

void COnDutyMethod::SetReserveAmount(UINT newVal)
{
	m_Params.SetValue(_T("reserveAmount"), newVal);
}

void COnDutyMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void COnDutyMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void COnDutyMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("dutyId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("onDutyTime"), EPT_STRING, 14);
}

UINT COnDutyMethod::GetDutyId()
{
	return m_RespFields.GetValueAsUINT(_T("dutyId"));
}

CString COnDutyMethod::GetOnDutyTime()
{
	return m_RespFields.GetValueAsString(_T("onDutyTime"));
}

///////////////////////////////////////////////////////////////////////////////////

COffDutyMethod::COffDutyMethod(void)
: CServletMethod(_T("OffDuty"))
{
	AddParams();
	AddRespFields();
}

COffDutyMethod::~COffDutyMethod(void)
{
}

void COffDutyMethod::AddParams()
{
	CServletMethod::AddParams();

	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("deposit"), EPT_STRING, 12);
	m_Params.AddParam(_T("costExpense"), EPT_STRING, 12);
	m_Params.AddParam(_T("creditAmount"), EPT_STRING, 12);
	m_Params.AddParam(_T("returnedAmount"), EPT_STRING, 12);
	m_Params.AddParam(_T("cashPay"), EPT_STRING, 12);
	m_Params.AddParam(_T("payout"), EPT_STRING, 12); //可能是负数，所以要用字符串
	m_Params.AddParam(_T("fetchAmount"), EPT_STRING, 12);
	m_Params.AddParam(_T("remainAmount"), EPT_STRING, 12);
	m_Params.AddParam(_T("creditCard"), EPT_STRING, 12);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);

	m_Params.AddParam(_T("recordCount"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("classInfoRecords"), EPT_STRING, 0, EPLT_OWN); //实际长度

	//m_Params.AddParam(_T("classId"), EPT_NUMBER, 2);
	//m_Params.AddParam(_T("openQuantity"), EPT_NUMBER, 6);
	//m_Params.AddParam(_T("returnQuantity"), EPT_NUMBER, 6);

	m_Params.AddParam(_T("description"), EPT_STRING, 3, EPLT_VARIABLE);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("netBarPay"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("storeInAmount"), EPT_NUMBER, 12);
}

void COffDutyMethod::SetNetBarPay(UINT newVal)
{
	m_Params.SetValue(_T("netBarPay"), newVal);
}

void COffDutyMethod::SetStoreInAmount(INT newVal)
{
	m_Params.SetValue(_T("storeInAmount"), newVal);
}

void COffDutyMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void COffDutyMethod::SetDeposit(INT newVal)
{
	m_Params.SetValue(_T("deposit"), newVal);
}

void COffDutyMethod::SetCostExpense(INT newVal)
{
	m_Params.SetValue(_T("costExpense"), newVal);
}

void COffDutyMethod::SetCreditAmount(INT newVal)
{
	m_Params.SetValue(_T("creditAmount"), newVal);
}

void COffDutyMethod::SetReturnedAmount(INT newVal)
{
	m_Params.SetValue(_T("returnedAmount"), newVal);
}

void COffDutyMethod::SetCashPay(INT newVal)
{
	m_Params.SetValue(_T("cashPay"), newVal);
}

void COffDutyMethod::SetPayout(INT newVal)
{
	m_Params.SetValue(_T("payout"), newVal);
}

void COffDutyMethod::SetFetchAmount(INT newVal)
{
	m_Params.SetValue(_T("fetchAmount"), newVal);
}

void COffDutyMethod::SetRemainAmount(INT newVal)
{
	m_Params.SetValue(_T("remainAmount"), newVal);
}

void COffDutyMethod::SetCreditCard(INT newVal)
{
	m_Params.SetValue(_T("creditCard"), newVal);
}

void COffDutyMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void COffDutyMethod::SetRecordCount(UINT newVal)
{
	m_Params.SetValue(_T("recordCount"), newVal);
}

void COffDutyMethod::SetClassInfoRecords(CString newVal)
{
	m_Params.SetValue(_T("classInfoRecords"), newVal);
}

void COffDutyMethod::SetDescription(CString newVal)
{
	m_Params.SetValue(_T("description"), newVal);
}

void COffDutyMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void COffDutyMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("offDutyTime"), EPT_STRING, 14);
}

CString COffDutyMethod::GetOffDutyTime()
{
	return m_RespFields.GetValueAsString(_T("offDutyTime"));
}

