#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditReverseMethod : public CServletMethod
{
public:
	CCreditReverseMethod(void);
	~CCreditReverseMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	CString GetMethodName();

public:

	void SetMemberId(UINT newVal);
	void SetNetBarId(UINT newVal);
	void SetTrantime(CString newVal);
	void SetLastRefNo(UINT newVal);
	void SetOperator(CString newVal);
	void SetDutyId(UINT newVal);
	void SetCashRegisterId(UINT newVal);

public:

	INT GetBalance();
	INT GetAvailable();
	INT GetPresentBalance();

	// 2011/08/02-8201-gxx: Ô­Ê¼ÔùËÍ¶î
	INT GetCreditPresent();

};

}