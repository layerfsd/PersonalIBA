#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{
//
// 添加激活的方法
//
class CActivationMethod : public CServletMethod
{
public :
	CActivationMethod(void);
	~CActivationMethod(void);

	enum ACTIVATION_MODE{

		ACTIVATION_YES = 1,  // 激活
		ACTIVATION_NO = 2    // 不激活

	};

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
	* 设置激活模式 ACTIVATION_YES/NO
	*/
	void SetMode(UINT newVal);

	/**
	* 设置扣点类型
	*/
	void SetDotType(UINT newVal);

public :


};

}