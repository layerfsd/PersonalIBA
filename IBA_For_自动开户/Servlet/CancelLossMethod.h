#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCancelLossMethod : public CServletMethod
{
public :
	CCancelLossMethod(void);
	~CCancelLossMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual CString GetMethodName();					//!<返回当前函数名称;

public :
	
	/**
	*
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*证件号码
	*/
	void SetPersonalId(CString newVal);

	/**
	*密文（PSK）（DES）
	*/
	void SetPassword(CString newVal);

	/**
	*操作员姓名
	*/
	//void SetOperator(CString newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);


public :


};

}