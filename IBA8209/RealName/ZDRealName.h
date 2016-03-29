#pragma once

#include "RealNameBase.h"
#include ".\IZd.h"

namespace NS_REALNAME
{

class CZDRealName : public CRealNameBase
{
public:
	CZDRealName(void);
	virtual ~CZDRealName(void);

	virtual BOOL InitRealName();

	virtual BOOL UnInitRealName();
	
	virtual BOOL CheckRealName();

	virtual INT  OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<是否允许开户
	virtual BOOL IsRegisterEnabled();

	//!<是否允许客户端登录
	virtual BOOL IsCheckInEnabled();

	virtual void OnCheckOut(const CheckoutInfo* pCheckoutInfo);

	BOOL OnOpenCloseUser(int nType, CIZd::T_IC_USER* pICUser);

	BOOL OnLoginOut(int nType, CIZd::T_IC_LOGIN* ptLogin);

	INT OnOpenUser(CNetBarUser & NetBarUser);

	INT OnCloseUser(CNetBarUser & NetBarUser);

	BOOL CheckTakeUp(CString strTermId);

	void CheckoutOldComputer(const CString& strTermID, int nMemberId);

	BOOL CheckIn(LPCTSTR szTermID, LPCTSTR szComputerIP, LPCTSTR szNetId, LPCTSTR szPwd, int nMemberId);

	BOOL AutoChangeRealNamePwd(CNetBarUser& NetBarUser);
private:

	//!<自动更新实名资料
	// virtual BOOL AutoModifyMemberInfo();
};

}