#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CRoomCheckOut : public CServletMethod
{
public :
	CRoomCheckOut(void);
	~CRoomCheckOut(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户ID，采用32位整数表示
	*/
	void SetMemberId(UINT newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);


	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);

	// 包房ID,即包房的区域ID
	void SetPCClass(UINT newVal);

public :
	CString GetTranTime();
};

}