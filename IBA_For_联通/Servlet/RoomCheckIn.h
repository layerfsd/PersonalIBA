#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CRoomCheckIn : public CServletMethod
{
public :
	CRoomCheckIn(void);
	~CRoomCheckIn(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户ID，采用32位整数表示
	*/
	void SetMemberId(UINT newVal);

	// 包房ID，即机器区域ID
	void SetPCClassID(UINT newVal);

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


public :
	CString GetTranTime();
};

}