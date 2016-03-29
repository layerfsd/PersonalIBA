#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CDeclareLossMethod : public CServletMethod
{
public :
	CDeclareLossMethod(void);
	~CDeclareLossMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual CString GetMethodName();					//!<返回当前函数名称;

public :
	
	/**
	*证件号码
	*/
	void SetPersonalId(CString newVal);

	/**
	*开户网吧，一期该值和netBarId相同
	*/
	void SetOpenNetbarId(UINT newVal);

	/**
	*
	*/
	void SetMemberName(CString newVal);

	/**
	*用网吧的PSK加密（UNDES）
	*/
	void SetPassword(CString newVal);

	/**
	*操作员姓名
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
	*
	*/
	UINT GetMemberId();


};

}