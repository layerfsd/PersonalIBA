#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CMergeMemberMethod : public CServletMethod
{
public :
	CMergeMemberMethod(void);
	~CMergeMemberMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*当前用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*当前网吧ID
	*/
	void SetNetBarId(UINT newVal);

	/**
	*目标用户号
	*/
	void SetDestMemberId(UINT newVal);

	/**
	*当前用户密码,换户接口密码传"PASSWORDPASSWORD"时，中心不验密码。
	*/
	void SetPassword(CString newVal);

	/**
	*操作员，收银台查询填写
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


};

}