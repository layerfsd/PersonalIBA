#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBundleTimeInfo
{
public :

	CBundleTimeInfo();

	/**
	*包时ID
	*/
	UINT GetBundleId();

	/**
	*1: 累计封顶 2:足额预扣
	*/
	UINT GetBundleType();

	/**
	*包时开始时间
	*/
	CString GetStartTime();

	/**
	*包时结束时间
	*/
	CString GetEndTime();

	/**
	*时长，单位:小时
	*/
	UINT GetTimePass();

	/**
	*单位: 分
	*/
	UINT GetPrice();

	/**
	*备注
	*/
	CString GetDescription();


protected :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CBundleTimeListMethod;

	CString FormatCenterTime(CString strServerTime);
};

class CBundleTimeListMethod : public CServletMethod
{
public :
	CBundleTimeListMethod(void);
	~CBundleTimeListMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	CArray<CBundleTimeInfo, CBundleTimeInfo> m_BundleTimeList;

	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

public :
	
	void SetClassId(UINT newVal);

	/**
	*
	*/
	void SetMemberId(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*请求来源 0:收银台 1:客户端
	*/
	void SetPosition(UINT newVal);

	/**
	*操作员(收银台填写，客户端不填)
	*/
	void SetOperator(CString newVal);


public :

	/**
	*记录数
	*/
	UINT GetRecordCount();

	/**
	*
	*/
	UINT GetBundleId(INT nIndex);

	/**
	*1: 累计封顶 2:足额预扣
	*/
	UINT GetBundleType(INT nIndex);

	/**
	*包时开始时间
	*/
	CString GetStartTime(INT nIndex);

	/**
	*包时结束时间
	*/
	CString GetEndTime(INT nIndex);

	/**
	*时长，单位:小时
	*/
	UINT GetTimePass(INT nIndex);

	/**
	*单位: 分
	*/
	UINT GetPrice(INT nIndex);

	/**
	*备注
	*/
	CString GetDescription(INT nIndex);
};

}