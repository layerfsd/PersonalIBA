#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBuckleAmountMethod : public CServletMethod
{
public :
	CBuckleAmountMethod(void);
	~CBuckleAmountMethod(void);

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

	void SetCheckinTime(CString newVal);

	void SetCheckoutTime(CString newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);


	void SetBuckleAmount(UINT newVal);

	void SetTermId(CString newVal);

	void SetPcClass(UINT newVal);

	void SetPayType(UINT newVal);


public :


	UINT GetMemberId() const { return m_nMemberId; }
};

}