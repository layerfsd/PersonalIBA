#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCheckout2Method : public CServletMethod
{
public :
	CCheckout2Method(void);
	~CCheckout2Method(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual UINT	MakeLocalSerialNo();				//!< 获取本地流水号

	UINT m_nMemberId;
	
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
	*消费额（分）
	*/
	void SetCost(UINT newVal);

	/**
	*20040408200012
	*/
	void SetEndUsingTime(CString newVal);

	/**
	*对于ID卡、牛盾、IC卡无卡上机，大于0扣费到当前时间；否则按中心原即定规则扣费。
	*/
	void SetSafeLogout(UINT newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);


public :


	UINT GetMemberId() const { return m_nMemberId; }
};

}