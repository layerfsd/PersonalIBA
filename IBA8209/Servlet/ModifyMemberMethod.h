#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CModifyMemberMethod : public CServletMethod
{
public :
	CModifyMemberMethod(void);
	~CModifyMemberMethod(void);

protected :

	virtual CString GetMethodName();

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*用户登录名
	*/
	void SetMemberName(CString newVal);

	/**
	*用UNDES生
	*/
	void SetPassword(CString newVal);

	/**
	*原证件号码
	*/
	void SetOldPersonalId(CString newVal);

	/**
	*用户真实姓名
	*/
	void SetUsername(CString newVal);

	/**
	*0:男 1:女
	*/
	void SetSex(UINT newVal);

	/**
	*证件类型
	*/
	void SetIdType(UINT newVal);

	/**
	*证件号码
	*/
	void SetPersonalId(CString newVal);

	/**
	*发证机关
	*/
	void SetIdDepart(CString newVal);

	/**
	*国家代码
	*/
	void SetCountryId(CString newVal);

	/**
	*住址
	*/
	void SetAddress(CString newVal);

	/**
	*Email地址
	*/
	void SetEmail(CString newVal);

	/**
	*电话
	*/
	void SetTel(CString newVal);

	/**
	*工作
	*/
	void SetJob(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);

	/**
	*
	*/
	void SetOpenNetBarId(UINT newVal);


public :


};

}