#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{
class CGetSystemConfigMethod : public CServletMethod
{
public :
	CGetSystemConfigMethod(void);
	virtual ~CGetSystemConfigMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public:
	void SetStringField(CString newVal);
	void SetNetbarId(int nNetbarId);
	CString GetStringFieldValue();
	UINT GetValue1();
	UINT GetValue2();
};

}