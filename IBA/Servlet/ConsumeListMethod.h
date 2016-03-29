#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CConsumeInfo
{
public :
	
	CConsumeInfo();

	/**
	*消费网吧
	*/
	UINT GetNetBarId();

	/**
	*消费类型
	*/
	UINT GetConsumeType();

	/**
	*开始时间,循环体
	*/
	CString GetBeginTime();

	/**
	*结束时间,循环体
	*/
	CString GetEndTime();

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

private :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CConsumeListMethod;

};

typedef CArray<CConsumeInfo, CConsumeInfo&> CConsumeInfoArray;

class CConsumeListMethod : public CServletMethod
{
public :
	CConsumeListMethod(void);
	~CConsumeListMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	BOOL DecodeResp(CString strResp);

	CConsumeInfoArray m_ConsumeInfoList;

public :
	
	/**
	*用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*消费类型：0: 全部 1:上网 2:充值 3:卡购物 4：嘟嘟牛消费 5:卡购物(旧) 6:退款 7：充值卡，8：嘟嘟牛充值
	*/
	void SetConsumeType(UINT newVal);

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

	/**
	*1: 需要返回详细业务名称 0:不需要 2:同时需要备注
	*/
	void SetNeedBusinessName(UINT newVal);


public :

	/**
	*记录数量, 目前最大100条
	*/
	UINT GetRecordCount();

	/**
	*消费网吧
	*/
	UINT GetNetBarId(INT nIndex);

	/**
	*消费类型
	*/
	UINT GetConsumeType(INT nIndex);

	/**
	*开始时间,循环体
	*/
	CString GetBeginTime(INT nIndex);

	/**
	*结束时间,循环体
	*/
	CString GetEndTime(INT nIndex);

	/**
	*真实货币消费金额,循环体
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