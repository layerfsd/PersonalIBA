#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCashierTotalMethod : public CServletMethod
{
public :
	CCashierTotalMethod(void);
	~CCashierTotalMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*开始时间，空表示从最近注册时间开始
	*/
	void SetBeginTime(CString newVal);

	/**
	*结束时间，空表示不限制
	*/
	void SetEndTime(CString newVal);

	/**
	*收银员名称
	*/
	void SetOperator(CString newVal);


public :

	/**
	*卡数量
	*/
	UINT GetCardCount();

	/**
	*金额合计
	*/
	UINT GetAmount();


};

}