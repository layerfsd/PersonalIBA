#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditActivityPolicyMethod : public CServletMethod
{
public :
	CCreditActivityPolicyMethod(void);
	~CCreditActivityPolicyMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*记录数
	*/
	UINT GetRecordCount();

	/**
	*活动ID
	*/
	UINT GetActivityId();

	/**
	*活动名称
	*/
	CString GetActivityName();

	/**
	*活动类型
	*/
	UINT GetActivityType();

	/**
	*用户级别 0:不限级别
	*/
	UINT GetClassId();

	/**
	*最低充值金额
	*/
	UINT GetLimitAmount();

	/**
	*显示金额，当此值大于0时，充值金额必须等于limitAmount
	*/
	UINT GetDisplayAmount();

	/**
	*开始时间yyyymmddhhmmss
	*/
	CString GetStartTime();

	/**
	*截止时间yyyymmddhhmmss
	*/
	CString GetStopTime();


};

}