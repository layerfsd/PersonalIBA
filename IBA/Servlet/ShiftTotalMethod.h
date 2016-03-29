#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCardInfoRecord
{
public :

	CCardInfoRecord();
	
	UINT GetClassId();
	UINT GetOpenQuantity();
	UINT GetReturnedQuantity();

protected :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CShiftTotalMethod;
};

class CShiftTotalMethod : public CServletMethod
{
public :
	CShiftTotalMethod(void);
	~CShiftTotalMethod(void);

public :

	void SetDutyId(UINT newVal);
	void SetOperator(CString newVal);
	void SetNeedBalance(UINT newVal);
	void SetLastTranTime(CString newVal);
	void SetLastTranType(UINT newVal);
	void SetLastRefNo(UINT newVal);

public :

	CString GetOndutyTime();
	INT GetReserveAmount();
	INT GetDeposit();
	INT GetCostExpense();
	INT GetCreditAmount();
	INT GetReturnedAmount();
	INT GetCashPay();
	INT GetCreditCard();
	INT GetTempBalance();

	INT GetNetBarPay();//帐户购物数量

	INT GetStoreInAmount(); //商品进货支出

	UINT GetRecordCount();
	CString GetCardInfoRecords();

	UINT GetClassId(INT nIndex);
	UINT GetOpenQuantity(INT nIndex);
	UINT GetReturnedQuantity(INT nIndex);

	// 2011/05/10-gxx: 增收宝( 消费，余额)
	INT GetZSBConsume();
	INT GetZSBBalance();

	// 2011/07/26-8201-gxx: 点卡销售
	INT GetDiankaAmount();

	// 2014-11-3 - qsc 微信余额
	INT GetWxPayAmount();
protected :

	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

	void AddParams();
	void AddRespFields();
	CArray<CCardInfoRecord, CCardInfoRecord> m_CardInfoRecords;

};

}