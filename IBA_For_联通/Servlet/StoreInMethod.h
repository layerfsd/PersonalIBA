#pragma once

#include "servletmethod.h"

namespace NS_SERVLET
{

class CStoreInRecord
{
public:

	CStoreInRecord()
	{
		m_nAmount = 0;
		m_nPrice = 0;
		m_nQuantity = 0;
		m_nRemainQuantity = 0;
	}

	/**
	*商品编码
	*/
	void SetCommodityCode(CString newVal) { m_strCommodityCode = newVal; }
	CString GetCommodityCode() const { return m_strCommodityCode; }

	/**
	*数量，计量单位*100
	*/
	void SetQuantity(UINT newVal){ m_nQuantity = newVal; }
	UINT GetQuantity() const { return m_nQuantity; }

	/**
	*单价，单位：分
	*/
	void SetPrice(UINT newVal) { m_nPrice = newVal; }
	UINT GetPrice() const { return m_nPrice; }

	/**
	*金额，单位：分
	*/
	void SetAmount(UINT newVal) { m_nAmount = newVal; }
	UINT GetAmount() const { return m_nAmount; }
	/**
	*原来库存数量，
	*/
	UINT GetRemainQuantity() const { return m_nRemainQuantity; }
	void SetRemainQuantity(UINT val) { m_nRemainQuantity = val; }

	private:
		
	CString m_strCommodityCode;

	UINT m_nQuantity;
	UINT m_nPrice;
	UINT m_nAmount;
	UINT m_nRemainQuantity;
};

typedef CArray<CStoreInRecord, CStoreInRecord&> CStoreInRecordArray;


class CStoreInMethod :public CServletMethod
{
public:
	CStoreInMethod(void);
	~CStoreInMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public:
	////////////////////写入报文///////////////
	//仓库ID
	void SetStoreId(UINT newVal);
	//网吧号
	void SetNetBarId(UINT newVal);
	//操作员姓名
	void SetOperator(CString newVal);
	//收银台ID
	void SetCashRegisterId(UINT newVal);
	//班次ID
	void SetDutyId(UINT newVal);
	//商品种类数
	void SetCount(UINT newVal);
	//入库商品记录
	void SetStoreInRecords(CStoreInRecordArray& StoreInRecords);
	// 2011/05/27-gxx: 是否现金支付
    void SetPayType(BOOL bCash);

	/////////////////////返回值//////////////////
	//系统参考号
	UINT GetRefNo();
	//零售单据编号
	UINT GetVoucherNo();
	//扩展错语描述,可变长度
	CString GetExtInfo();
	//中心交易时间
	CString GetTrantime();
};

}
