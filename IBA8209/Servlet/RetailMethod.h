#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CSellRecord
{
public:

	CSellRecord();

	/**
	*商品编码
	*/
	void SetCommodityCode(CString newVal);
	CString GetCommodityCode() const { return m_strCommodityCode; }

	/**
	*商品条码
	*/
	void SetCommodityBarCode(CString newVal);
	CString GetCommodityBarCode() const { return m_strCommodityBarCode; }

	/**
	*商品名称
	*/
	void SetCommodityName(CString newVal);
	CString GetCommodityName() const { return m_strCommodityName; }

	/**
	*数量，计量单位*100
	*/
	void SetQuantity(UINT newVal);
	UINT GetQuantity() const { return m_nQuantity; }

	/**
	*单价，单位：分
	*/
	void SetPrice(UINT newVal);
	UINT GetPrice() const { return m_nPrice; }

	/**
	*金额，单位：分
	*/
	void SetAmount(UINT newVal);
	UINT GetAmount() const { return m_nAmount; }
	/**
	*商品所需积分
	*/
	void SetPoint(UINT newVal);
	UINT GetPoint() const { return m_nPoint; }

private:

	CString m_strCommodityCode;
	CString m_strCommodityBarCode;
	CString m_strCommodityName;

	UINT m_nQuantity;
	UINT m_nPoint;
	UINT m_nPrice;
	UINT m_nAmount;
};

typedef CArray<CSellRecord, CSellRecord&> CSellRecordArray;

class CRetailMethod : public CServletMethod
{
public :
	CRetailMethod(void);
	~CRetailMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	CString GetMethodName();

public :
	
	/**
	*用户号，用卡购买时填写
	*/
	void SetMemberId(UINT newVal);

	/**
	*用户卡号，卡购物时填写
	*/
	void SetCardId(UINT newVal);

	/**
	*网吧号
	*/
	void SetNetBarId(UINT newVal);

	/**
	*密文(UNDES)
	*/
	void SetPassword(CString newVal);

	/**
	*仓库ID
	*/
	void SetStoreId(UINT newVal);

	/**
	*1: 现金，2:卡
	*/
	void SetPayType(UINT newVal);

	/**
	*1: 零售 2:退货 3: 赠送
	*/
	void SetRetailType(UINT newVal);

	/**
	*原订单号，退货时填写
	*/
	void SetVoucherNo(CString newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);

	/**
	*商品种类数
	*/
	void SetCount(UINT newVal);


	void SetSellRecords(CSellRecordArray& SellRecords);


	/**
	*当前系统时间
	*/
	void SetTermTime(CString newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);


public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*零售单据编号
	*/
	CString GetVoucherNo();

	/**
	*扩展错语描述,可变长度
	*/
	CString GetExtInfo();

	/**
	*中心交易时间
	*/
	CString GetTranTime();

};

///////////////////////////////在线购物/////////////////////////
class CRetailOnlineMethod : public CServletMethod
{
public :
	CRetailOnlineMethod(void);
	~CRetailOnlineMethod(void);

protected :

	void AddParams();
	void AddRespFields();

public :

	/**
	*用户号，用卡购买时填写
	*/
	void SetMemberId(UINT newVal);

	/**
	*网吧号
	*/
	void SetNetBarId(UINT newVal);

	/**
	*仓库ID
	*/
	void SetStoreId(UINT newVal);

	/**
	*1: 现金，2:卡
	*/
	void SetPayType(UINT newVal);

	/**
	*1: 零售 2:退货 3: 赠送
	*/
	void SetRetailType(UINT newVal);

	/**
	*原订单号，退货时填写
	*/
	void SetVoucherNo(CString newVal);

	/**
	*操作员姓名
	*/
	void SetOperator(CString newVal);

	/**
	*商品种类数
	*/
	void SetCount(UINT newVal);

	//设置销售商品的信息
	void SetSellRecords(CSellRecordArray& SellRecords);

	/**
	*当前系统时间
	*/
	void SetTermTime(CString newVal);

	/**
	*收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*班次ID
	*/
	void SetDutyId(UINT newVal);

public :

	/**
	*系统参考号
	*/
	UINT GetRefNo();

	/**
	*零售单据编号
	*/
	CString GetVoucherNo();

	/**
	*扩展错语描述,可变长度
	*/
	CString GetExtInfo();

	/**
	*中心交易时间
	*/
	CString GetTranTime();

};

}