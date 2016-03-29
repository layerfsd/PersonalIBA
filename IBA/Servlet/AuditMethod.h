#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CAuditMethod : public CServletMethod
{
public :
	CAuditMethod(void);
	~CAuditMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	CString GetMethodName();

public :
	
	/**
	*用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*1:任子行 2:过滤王 3:摩根 4:其他
	*/
	void SetAuditType(UINT newVal);

	/**
	*公安接口身份ID
	*/
	void SetAuditId(CString newVal);


public :


};

}