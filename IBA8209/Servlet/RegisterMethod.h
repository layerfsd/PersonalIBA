#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CRegisterMethod : public CServletMethod
{
public :
	CRegisterMethod(void);
	virtual ~CRegisterMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual CString GetMethodName(); //!<返回当前函数名称

public :

	void SaveTranInfo();

	//void SetCardId(UINT newVal);
	void SetCardId(CString newVal);
	void SetUserClass(UINT newVal);
	void SetCostExpense(UINT newVal);
	void SetGuarantyMoney(UINT newVal);
	void SetAccountNetbarId(UINT newVal);
	void SetMemberName(CString newVal);
	void SetPassword(CString newVal);
	void SetUserName(CString newVal);
	void SetSex(UINT newVal);
	void SetPersonalId(CString newVal);
	void SetIdType(UINT newVal);
	void SetIdDepart(CString newVal);
	void SetCountry(CString  newVal);
	void SetAuditType(UINT newVal);
	void SetAuditId(CString newVal);
	void SetOperator(CString newVal);
	void SetAddress(CString newVal);
	void SetEmail(CString newVal);
	void SetTel(CString newVal);
	void SetJob(CString newVal);
	void SetCreditAmount(UINT newVal);
	void SetNoPresent(UINT newVal);
	void SetNoCard(UINT newVal);
	void SetCashRegisterId(UINT newVal);
	void SetDutyId(UINT newVal);
	void SetActivityId(UINT newVal);

	void SetActivationType(UINT nType);

	void SetTimeId(UINT newVal)
	{
		m_Params.SetValue(_T("timeId"), newVal);
	}
	void SetPcClass(UINT newVal)
	{
		m_Params.SetValue(_T("pcClass"), newVal);
	}
	void SetBeginTime(CString newVal)
	{
		m_Params.SetValue(_T("beginTime"), newVal);
	}
	void SetTimePass(UINT newVal)
	{
		m_Params.SetValue(_T("timePass"), newVal);
	}
	void SetBundleAmount(UINT newVal)
	{
		m_Params.SetValue(_T("bundleAmount"), newVal);
	}	
	void SetAccountType(UINT newVal)
	{
		m_Params.SetValue(_T("accountType"), newVal);
	}

	UINT GetAmoun()
	{
		return m_RespFields.GetValueAsUINT(_T("amoun"));
	}
	UINT GetGuarantyMoney()
	{
		return m_RespFields.GetValueAsUINT(_T("guarantyMoney"));
	}
	UINT GetCostExpense()
	{
		return m_RespFields.GetValueAsUINT(_T("costExpense"));
	}
public :

	UINT GetMemberId();
	UINT GetRefNo();
	CString GetSerialNum();
	UINT GetRefNo1();
	UINT GetCreditPresent();
	CString GetTrantime();
};

}