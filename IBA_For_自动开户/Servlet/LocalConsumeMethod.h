#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CLocalConsumeMethod : public CServletMethod
{
public :
	CLocalConsumeMethod(void);
	~CLocalConsumeMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*当前用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*用户级别
	*/
	void SetClassId(UINT newVal);

	/**
	*账号(一般与证件号相同)
	*/
	void SetSerialNum(CString newVal);

	/**
	*姓名
	*/
	void SetUserName(CString newVal);

	/**
	*证件号码
	*/
	void SetIdNumber(CString newVal);

	/**
	*充值金额
	*/
	void SetCreditAmount(UINT newVal);

	/**
	*上网金额
	*/
	void SetConsumeAmount(UINT newVal);

	/**
	*退款金额
	*/
	void SetReturnAmount(UINT newVal);

	/**
	*登入时间yyyymmddhhmmss
	*/
	void SetCheckinTime(CString newVal);

	/**
	*登出时间yyyymmddhhmmss
	*/
	void SetCheckoutTime(CString newVal);

	/**
	*终端号
	*/
	void SetTermId(CString newVal);

	/**
	*机器区域
	*/
	void SetPcClass(UINT newVal);

	/**
	*0: 正常消费 2:足额预扣包时
	*/
	void SetPayType(UINT newVal);

	/**
	*0：数据正常 1:数据有串改
	*/
	void SetDataStatus(UINT newVal);

	/**
	*操作员
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


public :

	/**
	*系统参考号
	*/
	UINT GetRefNO();

	/**
	*中心交易时间
	*/
	CString GetTranTime();
	
	/**
	*
	*/
	UINT GetMemberID();

};

}