#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBundleTimeMethod : public CServletMethod
{
public :
	CBundleTimeMethod(void);
	~CBundleTimeMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户ID，采用32位整数表示
	*/
	void SetMemberId(UINT newVal);

	/**
	*网吧ID，采用16位整数表示
	*/
	void SetNetbarId(UINT newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*足额预扣包时ID
	*/
	void SetTimeId(UINT newVal);

	/**
	*机器区域
	*/
	void SetPcClass(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);

	/**
	*0:普通包时 1:预包时 2:转包时 3自定义包时
	*/
	void SetReserve(UINT newVal);

	/**
	*包时账户
	*/
	void SetAccountType(UINT newVal);

	//开始时间
	void SetBeginTime(CString newVal);

	//时长
	void SetTimePass(UINT newVal);
	
	//包时金额
	void SetBundleAmount(UINT newVal);

	//充值金额
	void SetCreditAmount(UINT newVal);

public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*包时开始时间
	*/
	CString GetStartTime();

	/**
	*包时结束时间
	*/
	CString GetEndTime();

	/**
	*包时金额
	*/
	UINT GetAmount();

	/**
	*包时ID
	*/
	UINT GetTimeId();

	/**
	*	客户端是否需要重新登入
		1：需要 0:不需要
		注：状态码不为零也需要根据此值判断是否需要重新登入
	*/
	BOOL GetNeedRelogin();
};

}