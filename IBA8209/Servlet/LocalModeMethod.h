#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CLocalModeMethod : public CServletMethod
{
public :
	CLocalModeMethod(void);
	~CLocalModeMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*1:启用断网 2:停用断网
	*/
	void SetOperationType(UINT newVal);

	/**
	*上次操作时间
	*/
	void SetLastOperationTime(CString newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :


};

}