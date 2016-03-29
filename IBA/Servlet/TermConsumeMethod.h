#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CTermConsumeInfo
{
public :

	CTermConsumeInfo(void);
	
	/**
	*用户号
	*/
	UINT GetMemberId();

	/**
	*开始时间,循环体(不含年份)
	*/
	CString GetBeginTime();

	/**
	*结束时间,循环体(不含年份)
	*/
	CString GetEndTime();

	/**
	*金额，单位：分
	*/
	UINT GetAmount();
	
private :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CTermConsumeMethod;

};

//******************************************************************************

class CTermConsumeMethod : public CServletMethod
{
public :
	CTermConsumeMethod(void);
	~CTermConsumeMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	BOOL DecodeResp(CString strResp);

	CArray<CTermConsumeInfo, CTermConsumeInfo&>  m_TermConsumeList;

public :
	
	/**
	*终端编号
	*/
	void SetTermed(CString newVal);

	/**
	*开始时间，空表示从最近注册时间开始
	*/
	void SetBeginTime(CString newVal);

	/**
	*结束时间，空表示不限制
	*/
	void SetEndTime(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*记录数量, 目前最大100条
	*/
	UINT GetRecordCount();

	/**
	*用户号
	*/
	UINT GetMemberId(INT nIndex);

	/**
	*开始时间,循环体(不含年份)
	*/
	CString GetBeginTime(INT nIndex);

	/**
	*结束时间,循环体(不含年份)
	*/
	CString GetEndTime(INT nIndex);

	/**
	*金额，单位：分
	*/
	UINT GetAmount(INT nIndex);

};

}