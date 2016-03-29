#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCardAssignMethod : public CServletMethod
{
public :
	CCardAssignMethod(void);
	~CCardAssignMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*
	*/
	void SetCardId(UINT newVal);

	/**
	*工本费(分)
	*/
	void SetCostExpense(UINT newVal);

	/**
	*押金（分）
	*/
	void SetGuarantyMoney(UINT newVal);

	/**
	*证件号码
	*/
	void SetPersonalId(UINT newVal);

	/**
	*开户网吧
	*/
	void SetOpenNetbarId(UINT newVal);

	/**
	*
	*/
	void SetMemberName(CString newVal);

	/**
	*用网吧的PSK加密的密文（UNDES）
	*/
	void SetPassword(CString newVal);

	/**
	*操作员姓名
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

	/**
	*1: 自带卡 0:买卡
	*/
	void SetHasCard(UINT newVal);


public :

	/**
	*
	*/
	UINT GetMemberId();

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*卡序列号
	*/
	CString GetSerialNum();

	/**
	*中心交易时间
	*/
	UINT GetTranTime();


};

}