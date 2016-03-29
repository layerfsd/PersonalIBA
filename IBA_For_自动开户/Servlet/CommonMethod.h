#pragma once
#include "servletmethod.h"	

namespace NS_SERVLET
{

class CStatusMessageMethod : public CServletMethod
{
public:
	CStatusMessageMethod(void);
	~CStatusMessageMethod(void);

	void SetStatusCode(INT newVal);
	CString GetMessage();

	BOOL Execute();

protected :

	void AddParams();
	void AddRespFields();

protected :

	virtual CString MakeMacString(CString strParams);
	virtual BOOL    VerifyResp(CString strResp);		//!< 解析返回值，可重写

private :
	
	INT m_nStatusCode;
};

class CGetTimeMethod : public CServletMethod
{
public:
	CGetTimeMethod(void);
	~CGetTimeMethod(void);

	CString GetServerTimeAsString();
	BOOL SyncLocalTime();

	using CServletMethod::Execute;

protected :

	void AddRespFields();
	
	virtual CString MakeMacString(CString strParams);
	virtual BOOL    VerifyResp(CString strResp);		//!< 解析返回值，可重写
};

}