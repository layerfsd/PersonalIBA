#pragma once

#include "Cashier.h"

class CServletHellper
{
public :

	static UINT GetMemberId(LPCTSTR lpszNetId);

	static UINT GetMemberId(LPCTSTR lpszNetId, UINT& nCardId);

	static BOOL ActivationMember(UINT nMemberID, UINT nActivationType=0);

	static BOOL Synchronize();

private:
	//向实名上报用户信息，如RegisterDlg::SendRegDataToProxy
	static void SendRegDataToProxy(UINT nMemberId);
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

	static BOOL AllowAutoRefundInLocalMode(CActiveMember& ActiveMember, int nParaAllowReturn, COleDateTime checkouttime);

	static BOOL DeregisterUser(UINT nMemberId,CString& ErrMsg);  // 自动退款用
	 
};