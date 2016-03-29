#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CDetailDataMethod : public CServletMethod
{
public :
	CDetailDataMethod(void);
	~CDetailDataMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

	CString m_strDetailInfo;

public :
	
	/**
	*要查询的页号
	*/
	void SetPageNum(UINT newVal);

	/**
	*
	*/
	void SetNetBarId(UINT newVal);

	/**
	*交易类型: 1开户 2充值 3退款 4冲正 5充值卡 6用户升级
	*/
	void SetTranType(UINT newVal);

	/**
	*用户级别，0：查询全部级别
	*/
	void SetClassId(UINT newVal);

	/**
	*查询起始时间
	*/
	void SetBeginTime(CString newVal);

	/**
	*查询截止时间
	*/
	void SetEndTime(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*中心时间
	*/
	CString GetServerTime();

	/**
	*每页记录数
	*/
	UINT GetPageSize();

	/**
	*记录数(大于等于100表示还有数据)
	*/
	UINT GetRecordCount();

	/**
	*详见备注
	*/
	CString GetDetailInfo();


};

}