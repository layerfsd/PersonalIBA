#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCardReplaceMethod : public CServletMethod
{
public :
	CCardReplaceMethod(void);
	~CCardReplaceMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual CString GetMethodName(); //!<返回当前函数名称

public :
	
	/**
	*
	*/
	void SetCardId(UINT newVal);

	/**
	*证件号码
	*/
	void SetPersonalId(CString newVal);

	/**
	*开户网吧
	*/
	void SetOpenNetbarId(UINT newVal);

	/**
	*
	*/
	void SetMemberName(CString newVal);

	/**
	*新卡PSK加密（UNDES）
	*/
	void SetPassword(CString newVal);

	/**
	*
	*/
	void SetLastpay(UINT newVal);

	/**
	*
	*/
	void SetLastRefNo(UINT newVal);

	/**
	*
	*/
	void SetEndUsingTime(CString newVal);

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


public :

	/**
	*
	*/
	UINT GetMemberId();

	/**
	*系统参考号,用于开卡表
	*/
	UINT GetRefNo1();

	/**
	*系统参考号,用于退卡表
	*/
	UINT GetRefNo2();

	/**
	*卡序列号
	*/
	CString GetSerialNum();

	/**
	*中心交易时间
	*/
	CString GetTranTime();


};

}