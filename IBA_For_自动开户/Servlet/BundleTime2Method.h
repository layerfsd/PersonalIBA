#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBundleTime2Method : public CServletMethod
{
public :
	CBundleTime2Method(void);
	~CBundleTime2Method(void);

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
	*足额预扣包时ID
	*/
	void SetTimeId(UINT newVal);

	/**
	*收银台ID，限制最长为8个字节
	*/
	void SetTermId(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*包时金额
	*/
	UINT GetAmount();


};

}