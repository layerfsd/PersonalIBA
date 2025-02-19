#pragma once

#include "Cashier.h"

class CServletHellper
{
public :

	static UINT GetMemberId(LPCTSTR lpszNetId);

	static UINT GetMemberId(LPCTSTR lpszNetId, UINT& nCardId);

	static BOOL ActivationMember(UINT nMemberID);

	static BOOL Synchronize();
};


//******************************************************************************

class CCreditHelpper
{
public:
	CCreditHelpper(void);
	~CCreditHelpper(void);

	void SetNetId(CString newVal);
	void SetCreditMoney(INT newVal);
	
	BOOL Execute();
	BOOL Execute(CString strCmdLine);

private:

	void OnCreditOK(CCreditMethod & Credit);

	CString m_strNetId;
	
	UINT m_nMemberId;
	INT m_nCreditMoney;
};


//******************************************************************************

class CDeregisterHelpper
{
public:

	static BOOL AllowAutoRefund(CBalanceMethod & Balance);

	static BOOL DeregisterUser(CBalanceMethod & Balance);  // �Զ��˿���
	 
};