#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CNetBarConfigMethod : public CServletMethod
{
public:
	CNetBarConfigMethod(void);
	~CNetBarConfigMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public:

	void SetOperator(CString newVal);


public :

	UINT GetNetBarCount();
	UINT GetNetBarId();
	CString GetNetBarName();
	CString GetNetBarCaption();
	CString GetParentNetBar();
	UINT GetNetBarType();

};

}