#include "stdafx.h"
#include "ShiftListMethod.h"

using namespace NS_SERVLET;

CShiftListMethod::CShiftListMethod(void)
: CServletMethod(_T("ShiftList")) //中心方法名称
{
	m_bUseQueryHost = TRUE; //使用查询机
	m_nRevTimeout = 120;	//超时2分钟

	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CShiftListMethod::~CShiftListMethod(void)
{
}

void CShiftListMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("queryFlag"), EPT_NUMBER, 1);

	m_Params.SetValue(_T("queryFlag"), 1); // 兼容此版本收银台，必须使用1
}

void CShiftListMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CShiftListMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("serverTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 3);

}

BOOL CShiftListMethod::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	INT nCount = GetRecordCount();

	m_ShiftList.RemoveAll();

	if (nCount > 0)
	{
		m_ShiftList.SetSize(nCount);

		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_ShiftList.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_ShiftList.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_ShiftList.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

CString CShiftListMethod::GetServerTime()
{
	return m_RespFields.GetValueAsString(_T("serverTime"));
}

INT CShiftListMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsINT(_T("recordCount"));
}

CString CShiftListMethod::GetOnDutyTime(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetOnDutyTime();
}

CString CShiftListMethod::GetOffDutyTime(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetOffDutyTime();
}

INT CShiftListMethod::GetShiftType(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetShiftType();
}

CString CShiftListMethod::GetOperator(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetOperator();
}

INT CShiftListMethod::GetReserveAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetReserveAmount();
}

INT CShiftListMethod::GetDeposit(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetDeposit();
}

INT CShiftListMethod::GetCostExpense(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetCostExpense();
}

INT CShiftListMethod::GetCreditAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetCreditAmount();
}

INT CShiftListMethod::GetReturnAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetReturnAmount();
}

INT CShiftListMethod::GetCashPay(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetCashPay();
}

INT CShiftListMethod::GetStoreInAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetStoreInAmount();
}

INT CShiftListMethod::GetPayout(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetPayout();
}

INT CShiftListMethod::GetRemainAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetRemainAmount();
}

INT CShiftListMethod::GetFetchAmount(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetFetchAmount();
}

INT CShiftListMethod::GetNetBarPay(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetNetBarPay();
}

INT CShiftListMethod::GetCardCredit(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetCardCredit();
}

CString CShiftListMethod::GetDescription(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetDescription();
}

INT NS_SERVLET::CShiftListMethod::GetDiankaSold(INT nIndex)
{
	return m_ShiftList.GetAt(nIndex).GetDiankaSold();
}


//******************************************************************************

CShiftRecord::CShiftRecord()
{
	m_RespFields.AddRespField(_T("onDutyTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("offDutyTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("shiftType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("operator"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("reserveAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("deposit"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("costExpense"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("creditAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("returnAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("cashPay"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("storeInAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("payout"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("remainAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("fetchAmount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("netBarPay"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("cardCredit"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("thirdPartCash"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("description"), EPT_STRING, 3, FALSE);	
}


CString CShiftRecord::GetOnDutyTime()
{
	return m_RespFields.GetValueAsString(_T("onDutyTime"));
}

CString CShiftRecord::GetOffDutyTime()
{
	return m_RespFields.GetValueAsString(_T("offDutyTime"));
}

INT CShiftRecord::GetShiftType()
{
	return m_RespFields.GetValueAsINT(_T("shiftType"));
}

CString CShiftRecord::GetOperator()
{
	return m_RespFields.GetValueAsString(_T("operator"));
}

INT CShiftRecord::GetReserveAmount()
{
	return m_RespFields.GetValueAsINT(_T("reserveAmount"));
}

INT CShiftRecord::GetDeposit()
{
	return m_RespFields.GetValueAsINT(_T("deposit"));
}

INT CShiftRecord::GetCostExpense()
{
	return m_RespFields.GetValueAsINT(_T("costExpense"));
}

INT CShiftRecord::GetCreditAmount()
{
	return m_RespFields.GetValueAsINT(_T("creditAmount"));
}

INT CShiftRecord::GetReturnAmount()
{
	return m_RespFields.GetValueAsINT(_T("returnAmount"));
}

INT CShiftRecord::GetCashPay()
{
	return m_RespFields.GetValueAsINT(_T("cashPay"));
}

INT CShiftRecord::GetStoreInAmount()
{
	return m_RespFields.GetValueAsINT(_T("storeInAmount"));
}

INT CShiftRecord::GetPayout()
{
	return m_RespFields.GetValueAsINT(_T("payout"));
}

INT CShiftRecord::GetRemainAmount()
{
	return m_RespFields.GetValueAsINT(_T("remainAmount"));
}

INT CShiftRecord::GetFetchAmount()
{
	return m_RespFields.GetValueAsINT(_T("fetchAmount"));
}

INT CShiftRecord::GetNetBarPay()
{
	return m_RespFields.GetValueAsINT(_T("netBarPay"));
}

INT CShiftRecord::GetCardCredit()
{
	return m_RespFields.GetValueAsINT(_T("cardCredit"));
}

CString CShiftRecord::GetDescription()
{
	return m_RespFields.GetValueAsString(_T("description"));
}


INT NS_SERVLET::CShiftRecord::GetDiankaSold()
{
	INT nTmp = m_RespFields.GetValueAsINT(_T("thirdPartCash"));
	if (nTmp < 0)
	{
		return 0;
	}
	return nTmp;
}

