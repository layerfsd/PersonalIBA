#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CResetPwdMethod : public CServletMethod
{
public :
	CResetPwdMethod(void);
	~CResetPwdMethod(void);

protected :

	void AddParams();
	void AddRespFields();
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
	*由PSK加密的新密码（UNDES）
	*/
	void SetPassword(CString newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);


public :


};

}