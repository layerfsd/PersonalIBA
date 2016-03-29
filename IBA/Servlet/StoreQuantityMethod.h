#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CStoreQuantityMethod : public CServletMethod
{
public :
	CStoreQuantityMethod(void);
	~CStoreQuantityMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	/**
	*循环体
	*/
	CString GetCommodityInfo();

public :
	
	/**
	*仓库ID
	*/
	void SetStoreId(UINT newVal);

	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*记录数量
	*/
	UINT GetRecordCount();

	/**
	*商品编码，循环体
	*/
	CString GetCommodityCode(UINT nIndex);
	
	/**
	*库存数量，循环体
	*/
	UINT GetRemainQuantity(UINT nIndex);


};

}