#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCancelBundleTimeMethod : public CServletMethod
{
public :
	CCancelBundleTimeMethod(void);
	~CCancelBundleTimeMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户ID，采用32位整数表示
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*操作员或系统名称
	*/
	void SetOperator(CString newVal);


public :


};

}