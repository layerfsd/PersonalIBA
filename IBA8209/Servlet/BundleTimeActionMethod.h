#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBundleTimeActionMethod : public CServletMethod
{
public :
	CBundleTimeActionMethod(void);
	~CBundleTimeActionMethod(void);

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


public :

	/**
	*包时网吧ID，0：无包时
	*/
	UINT GetNetBarId();

	/**
	*包时终端流水号
	*/
	UINT GetBundleTimeSerialNo();

	/**
	*包时类型
	*/
	UINT GetPayType();

	/**
	*包时机器区域
	*/
	UINT GetPcClass();

	/**
	*包时开始时间
	*/
	CString GetStartTime();

	/**
	*包时结束时间
	*/
	CString GetEndTime();

	/**
	*包时总金额
	*/
	UINT GetAmount1();

	/**
	*已消费包时金额
	*/
	UINT GetAmount2();

	/**
	*系统参考号,0: 不在线
	*/
	UINT GetRefNo();

	/**
	*在线已冻结金额
	*/
	UINT GetFreezeMoney();

	/**
	*包时ID
	*/
	UINT GetTimeId();


};

}