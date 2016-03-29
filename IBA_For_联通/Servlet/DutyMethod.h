#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class COnDutyMethod : public CServletMethod
{
public:
	COnDutyMethod(void);
	~COnDutyMethod(void);

	void SetShiftType(UINT newVal);
	void SetReserveAmount(UINT newVal);
	void SetOperator(CString newVal);
	void SetCashRegisterId(UINT newVal);

public :

	UINT GetDutyId();
	CString GetOnDutyTime();
	UINT GetShiftType() const { return m_nShiftType; }

protected :

	UINT m_nShiftType;
	
	void AddParams();
	void AddRespFields();

};



class COffDutyMethod : public CServletMethod
{
public:
	COffDutyMethod(void);
	~COffDutyMethod(void);

	void SetDutyId(UINT newVal);
	void SetDeposit(INT newVal);
	void SetCostExpense(INT newVal);
	void SetCreditAmount(INT newVal);
	void SetReturnedAmount(INT newVal);
	void SetCashPay(INT newVal);
	void SetPayout(INT newVal);
	void SetFetchAmount(INT newVal);
	void SetRemainAmount(INT newVal);
	void SetCreditCard(INT newVal);
	void SetOperator(CString newVal);
	
	void SetRecordCount(UINT newVal);
	void SetClassInfoRecords(CString newVal);

	void SetDescription(CString newVal);
	void SetCashRegisterId(UINT newVal);

	void SetNetBarPay(UINT newVal);
	void SetStoreInAmount(INT newVal);

public :

	CString GetOffDutyTime();

protected :

	void AddParams();
	void AddRespFields();

};

}