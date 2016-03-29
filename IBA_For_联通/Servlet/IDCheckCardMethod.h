#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CIDCheckCardMethod : public CServletMethod
{
public :
	CIDCheckCardMethod(void);
	~CIDCheckCardMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	virtual BOOL	VerifyResp(CString strResp);		//!< 校验返回加密数据，可重写

public :
	
	/**
	*卡印刷流水号
	*/
	void SetSerialNum(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


	/**
	*0:手动输入印刷号 1:实名传入
	*/
	void SetSerialNumType(UINT newVal);

public :

	/**
	*错误码，与状态码意义相同
	*/
	UINT GetErrCode();

	/**
	*用于卡修复
	*/
	UINT GetMemberId();

	CString GetMemberIdAsString();

	/**
	*用于卡修复
	*/
	UINT GetCardId();
	
	CString GetCardIdAsString();

};

}