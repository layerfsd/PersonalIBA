#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CUsePointMethod : public CServletMethod
{
public :
	CUsePointMethod(void);
	~CUsePointMethod(void);

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
	*使用积分数量
	*/
	void SetPoint(UINT newVal);

	/**
	*物品数量
	*/
	void SetQuantity(UINT newVal);

	/**
	*物品名称
	*/
	void SetArticle(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);

	/**
	*业务类型，用于DODONEW扣积分
	*/
	void SetBusinessType(UINT newVal);


public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*用户当前积分
	*/
	UINT GetCurrentPort();

	/**
	*中心交易时间
	*/
	CString GetTranTime();


};

}