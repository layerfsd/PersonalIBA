#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{
class CCommonParam : public CServletMethod
{
public :
	CCommonParam(void);
	virtual ~CCommonParam(void);

protected :

	void AddParams();
	void AddRespFields();

public:
	void SetParam(CString newVal);
	void SetNetbarId(int nNetbarId);
	CString GetCommonRequest();
	virtual BOOL VerifyResp(CString strResp);
};

}