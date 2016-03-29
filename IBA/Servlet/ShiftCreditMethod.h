#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCreditInfo
{
public:
	
	CCreditInfo();

	/**
	*充值时间，yyyymmddhhmmss
	*/
	CString GetCreditTime();

	/**
	*用户号
	*/
	UINT GetMemberId();

	/**
	*参考号
	*/
	UINT GetRefNo();

	/**
	*充值金额
	*/
	INT GetCreditAmount();

	/**
	*充值赠送
	*/
	INT GetCreditPresent();

	/**
	*充值积分
	*/
	UINT GetPoint();

protected :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CShiftCreditMethod;

};

typedef CArray<CCreditInfo, CCreditInfo> CCreditInfoArray;

class CShiftCreditMethod : public CServletMethod
{
public :
	CShiftCreditMethod(void);
	~CShiftCreditMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写
	virtual BOOL	OnBegin();							//!< 执行请求前
	CCreditInfoArray m_CreditInfos;

public :
	
	/**
	*用户号大于0，查询指定用户
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*记录数
	*/
	UINT GetRecordCount();

	/**
	*充值时间，yyyymmddhhmmss
	*/
	CString GetCreditTime(INT nIndex);

	/**
	*用户号
	*/
	UINT GetMemberId(INT nIndex);

	/**
	*参考号
	*/
	UINT GetRefNo(INT nIndex);

	/**
	*充值金额
	*/
	INT GetCreditAmount(INT nIndex);

	/**
	*充值赠送
	*/
	INT GetCreditPresent(INT nIndex);

	/**
	*充值积分
	*/
	UINT GetPoint(INT nIndex);


};

}