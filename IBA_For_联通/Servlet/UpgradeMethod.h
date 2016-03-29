#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CUpgradeMethod : public CServletMethod
{
public :
	CUpgradeMethod(void);
	~CUpgradeMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public :
	
	/**
	*用户ID
	*/
	void SetMemberId(UINT newVal);

	/**
	*网吧ID
	*/
	void SetNetBarId(UINT newVal);

	/**
	*原级别
	*/
	void SetOldClassId(UINT newVal);

	/**
	*新级别
	*/
	void SetNewClassId(UINT newVal);

	/**
	*升级理由
	*/
	void SetReason(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*中心交易时间
	*/
	CString GetTranTime();

	UINT GetRefNo();
};

}