#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CAssetStatusMethod : public CServletMethod
{
public :
	CAssetStatusMethod(void);
	~CAssetStatusMethod(void);

protected :

	void AddParams();
	void AddRespFields();
public :
	
	/**
	*终端编号
	*/
	void SetTermId(CString newVal);

	/**
	*资产ID，对于软件，是中心版本表中定义的子系统ID
	*/
	void SetAssetId(UINT newVal);

	/**
	*资产状况, 对于软件，是当前版本号
	*/
	void SetAssetStatus(CString newVal);


public :


};

}