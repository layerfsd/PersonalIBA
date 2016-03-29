#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditMethod : public CServletMethod
{
public:
	CCreditMethod(void);
	~CCreditMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual CString GetMethodName();

public:

	void SaveTranInfo();

	UINT GetMemberId();
	UINT GetAmount();

public:

	void SetMemberId(UINT newVal);
	void SetNetBarId(UINT newVal);
	void SetAmount(UINT newVal);
	void SetOperator(CString newVal);
	void SetNoPresent(UINT newVal);
	void SetTermTime(CString newVal);
	void SetCashRegisterId(UINT newVal);
	void SetDutyId(UINT newVal);
	void SetActivityId(UINT newVal);

public:

	//1	16	Mac	A	校验码，该校验码采用PSK计算
	//	17	6	serialNo	N	返回（不修改）
	//	23	2	Status	N	状态码，参见状态码表
	//	25	6	refNo	N	系统参考号
	//	31	6	balance	N	充值账户余额
	//	37	6	creditPresent	N	本次充值赠送
	//	43	6	availableBalance	N	当前网吧可用余额
	//	49	6	usePresent	N	有送充值
	//	55	14	Trantime	N	中心交易时间


	UINT GetRefNo();
	UINT GetBalance();
	UINT GetCreditPresent();
	UINT GetAvailableBalance();
	UINT GetUsePresent();
	
	CString GetTrantime();

	UINT GetIncreasedPoint(); //本次增加的积分

};


}