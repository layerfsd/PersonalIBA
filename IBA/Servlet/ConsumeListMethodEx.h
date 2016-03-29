#pragma once
#include "servletmethod.h"

// 2011/05/09-gxx: 添加
// class: CConsumeListMethodEx 第三方消费查询接口,查询最近的消费记录
//
namespace NS_SERVLET
{

class CConsumeInfoEx
{
public :
	
	CConsumeInfoEx();

	UINT GetMemberID();
	
	CString GetCostTime();

	/**
	*真实货币消费金额,循环体
	*/
	INT GetAmount();

	/**
	*详细业务名称，可能为空
	*/
	CString GetBusinessName();

	/**
	*备注，可能为空
	*/
	CString GetDescription();

public :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CConsumeListExMethod;

};

typedef CArray<CConsumeInfoEx, CConsumeInfoEx&> CConsumeInfoExArray;

class CConsumeListMethodEx : public CServletMethod
{
public :
	CConsumeListMethodEx(void);
	~CConsumeListMethodEx(void);

protected :

	void AddParams();
	void AddRespFields();

	BOOL DecodeResp(CString strResp);

	CConsumeInfoExArray m_ConsumeInfoList;

public :
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
	*用户ID
	*/
	UINT GetMemberID(INT nIndex);

	/**
	*消费时间
	*/
	CString GetCostTime(INT nIndex);

	/**
	*消费金额
	*/
	INT GetAmount(INT nIndex);

	/**
	*详细业务名称，可能为空
	*/
	CString GetBusinessName(INT nIndex);

	/**
	*备注，可能为空
	*/
	CString GetDescription(INT nIndex);

};

}