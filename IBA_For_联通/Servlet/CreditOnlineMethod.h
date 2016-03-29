#pragma once

#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditOnlineMethod : public CServletMethod
{
public:
	CCreditOnlineMethod(void);
	~CCreditOnlineMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public :

	void SaveTranInfo();

	void SetMemberId(UINT newVal);
	void SetNetBarId(UINT newVal);
	void SetAmount(UINT newVal);
	void SetOperator(CString newVal);
	void SetNoPresent(UINT newVal);
	void SetTermTime(CString newVal);
	void SetCashRegisterId(UINT newVal);
	void SetDutyId(UINT newVal);
	void SetTermId(CString newVal);

public :

	UINT GetRefNo();
	UINT GetBalance();
	UINT GetCreditPresent();
	UINT GetAvailableBalance();
	UINT GetUsePresent();
	CString GetTranTime();

};

}