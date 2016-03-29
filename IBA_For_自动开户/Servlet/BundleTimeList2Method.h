#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{
class CBundleTimeInfo2
{
public :

	CBundleTimeInfo2();

	/**
	*
	*/
	UINT GetBundleId();

	/**
	*包时开始时间
	*/
	CString GetStartTime();

	/**
	*包时结束时间
	*/
	CString GetEndTime();

	/**
	*1:包月 2:包周 3:自定义天数
	*/
	UINT GetMode();

	/**
	*自定义天数，mode=1或2无效
	*/
	UINT GetDays();

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

	friend class CBundleTimeList2Method;

	CString FormatCenterTime(CString strServerTime);
};


class CBundleTimeList2Method : public CServletMethod
{
public :
	CBundleTimeList2Method(void);
	~CBundleTimeList2Method(void);

protected :

	void AddParams();
	void AddRespFields();

	CArray<CBundleTimeInfo2, CBundleTimeInfo2> m_BundleTimeList;

	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

public :
	
	/**
	*
	*/
	void SetNetBarId(UINT newVal);

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
	*包时开始时间
	*/
	CString GetStartTime(INT nIndex);

	/**
	*包时结束时间
	*/
	CString GetEndTime(INT nIndex);

	/**
	*1:包月 2:包周 3:自定义天数
	*/
	UINT GetMode(INT nIndex);

	/**
	*自定义天数，mode=1或2无效
	*/
	UINT GetDays(INT nIndex);

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