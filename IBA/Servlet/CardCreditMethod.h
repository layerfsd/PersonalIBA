#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCardCreditMethod : public CServletMethod
{
public :
	CCardCreditMethod(void);
	~CCardCreditMethod(void);

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
	*卡类别,ZS:现金卡，XN:虚拟卡
	*/
	void SetCategory(CString newVal);

	/**
	*卡序号
	*/
	void SetCardSerial(CString newVal);

	/**
	*卡密码，用网吧PSK分两段加密(UNDES)
	*/
	void SetCardPwd(CString newVal);

	/**
	*充值金额（分）//本字段作废
	*/
	void SetAmount(UINT newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);

	/**
	*业务类型ID，收银台填0
	*/
	void SetBusinessType(UINT newVal);

	/**
	*姓名，收银台填空
	*/
	void SetName(CString newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);


public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*充值账户余额
	*/
	UINT GetBalance();

	/**
	*本次充值赠送
	*/
	UINT GetCreditPresent();

	/**
	*当前网吧可用余额
	*/
	UINT GetAvailableBalance();

	/**
	*本次充值金额
	*/
	UINT GetAmount();

	/**
	*中心交易时间
	*/
	CString GetTranTime();


};

}