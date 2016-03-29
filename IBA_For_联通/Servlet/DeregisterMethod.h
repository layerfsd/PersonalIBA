#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CDeregisterMethod : public CServletMethod
{
public:
	CDeregisterMethod(void);
	virtual ~CDeregisterMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	
	CString GetMethodName();

public :

	void SaveTranInfo();

	void SetMemberId(UINT newVal);
	UINT GetMemberId();

	void SetCardId(UINT newVal);
	void SetNetBarId(UINT newVal);
	void SetPersonalId(CString newVal);
	void SetOpenNetbarId(UINT newVal);
	void SetMemberName(CString newVal);
	void SetPassword(CString newVal);
	void SetLastTimeId(UINT newVal);
	void SetLastpay(UINT newVal);
	void SetLastRefNo(UINT newVal);
	void SetEndUsingTime(CString newVal);
	void SetOperator(CString newVal);
	//	0:正常退款 	//	1:退款不销户 	//	2:退包时后余额

	void SetReturnMode(UINT newVal);
	UINT GetReturnMode();

	void SetCashRegisterId(UINT newVal);
	void SetDutyId(UINT newVal);

public :

	UINT GetGuarantyMoney();
	INT GetBalance();
	UINT GetRefNo();
	CString GetTranTime();

};

}