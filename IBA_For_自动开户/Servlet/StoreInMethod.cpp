
#include "stdafx.h"
#include ".\storeinmethod.h"


using namespace NS_SERVLET;

CStoreInMethod::CStoreInMethod(void)
: CServletMethod(_T("StoreIn"))
{
	AddParams();
	AddRespFields();
}

CStoreInMethod::~CStoreInMethod(void)
{
}

void CStoreInMethod::AddParams()
{
	CServletMethod::AddParams();
	m_Params.AddParam(_T("storeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("count"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("storeInRecords"), EPT_STRING, 0, EPLT_OWN);
	m_Params.AddParam(_T("payType"), EPT_NUMBER, 1);

}

void CStoreInMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CStoreInMethod::SetStoreId(UINT newVal)
{
	m_Params.SetValue(_T("storeId"), newVal);
}

void CStoreInMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CStoreInMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CStoreInMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CStoreInMethod::SetCount(UINT newVal)
{
	m_Params.SetValue(_T("count"), newVal);
}

void CStoreInMethod::SetStoreInRecords(CStoreInRecordArray& StoreInRecords)
{
	SetCount(StoreInRecords.GetCount());

	CInputParamList aParams;
	aParams.AddParam(_T("commodityCode"), EPT_STRING, 16);
	aParams.AddParam(_T("quantity"), EPT_NUMBER, 6);
	aParams.AddParam(_T("price"), EPT_NUMBER, 6);
	aParams.AddParam(_T("amount"), EPT_NUMBER, 6);

	CString strTmp;

	for (INT i = 0; i < StoreInRecords.GetCount(); i++)
	{
		aParams.SetValue(_T("commodityCode"), StoreInRecords.GetAt(i).GetCommodityCode());
		aParams.SetValue(_T("quantity"), StoreInRecords.GetAt(i).GetQuantity());
		aParams.SetValue(_T("price"), StoreInRecords.GetAt(i).GetPrice());
		aParams.SetValue(_T("amount"), StoreInRecords.GetAt(i).GetAmount());

		strTmp.Append(aParams.GetAllValue());
	}

	m_Params.SetValue(_T("storeInRecords"), strTmp);
}

void CStoreInMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("voucherNo"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("extInfo"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("Trantime"), EPT_STRING, 14);
}
void NS_SERVLET::CStoreInMethod::SetPayType(BOOL bCash)
{
	m_Params.SetValue(_T("payType"), bCash);
}

UINT CStoreInMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CStoreInMethod::GetVoucherNo()
{
	return m_RespFields.GetValueAsUINT(_T("voucherNo"));
}

CString CStoreInMethod::GetExtInfo()
{
	return m_RespFields.GetValueAsString(_T("extInfo"));
}

CString CStoreInMethod::GetTrantime()
{
	return m_RespFields.GetValueAsString(_T("Trantime"));
}

