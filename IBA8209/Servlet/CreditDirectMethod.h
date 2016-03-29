#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditDirectMethod : public CServletMethod
{
public :
	CCreditDirectMethod(void);
	~CCreditDirectMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*网吧编号
	*/
	void SetNetBarId(UINT newVal);

	/**
	*账户类型 1:无送 2:赠送 3:有送
	*/
	void SetAccountType(UINT newVal);

	/**
	*0: 加钱 1:减钱
	*/
	void SetCreditMode(INT newVal);

	/**
	*充值金额，可以是负数
	*/
	void SetAmount(INT newVal);

	/**
	*收银台当前时间，用于时间校验
	*/
	void SetTermTime(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);


public :

	/**
	*账户总余额
	*/
	INT GetAvailableBalance();

	/**
	*中心交易时间
	*/
	CString GetTranTime();


};

}