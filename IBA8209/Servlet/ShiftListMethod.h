#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CShiftRecord
{
public:

	CShiftRecord();

	/**
	*上班时间
	*/
	CString GetOnDutyTime();

	/**
	*下班时间
	*/
	CString GetOffDutyTime();

	/**
	*班次类型: 早、中、晚
	*/
	INT GetShiftType();

	/**
	*收银员
	*/
	CString GetOperator();

	/**
	*上班备用金
	*/
	INT GetReserveAmount();

	/**
	*押金
	*/
	INT GetDeposit();

	/**
	*工本费
	*/
	INT GetCostExpense();

	/**
	*充值金额
	*/
	INT GetCreditAmount();

	/**
	*退款金额
	*/
	INT GetReturnAmount();

	/**
	*现金购物
	*/
	INT GetCashPay();

	/**
	*商品采购支出
	*/
	INT GetStoreInAmount();

	/**
	*其他支出
	*/
	INT GetPayout();

	/**
	*留给下一班
	*/
	INT GetRemainAmount();

	/**
	*上交金额
	*/
	INT GetFetchAmount();

	/**
	*账户购物
	*/
	INT GetNetBarPay();

	/**
	*充值卡充值
	*/
	INT GetCardCredit();

	/**
	*备注
	*/
	CString GetDescription();

	// 2011/08/03-8201-gxx: 点卡销售
	INT GetDiankaSold();

protected :

	friend class CShiftListMethod;

	CRespFieldList  m_RespFields;	//!< 返回值列表
};

class CShiftListMethod : public CServletMethod
{
public :
	CShiftListMethod(void);
	~CShiftListMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

	CArray<CShiftRecord, CShiftRecord> m_ShiftList;

public :
	
	/**
	*操作员
	*/
	void SetOperator(CString newVal);


public :

	/**
	*中心时间
	*/
	CString GetServerTime();

	/**
	*最近三天的交班记录数
	*/
	INT GetRecordCount();

	/**
	*上班时间
	*/
	CString GetOnDutyTime(INT nIndex);

	/**
	*下班时间
	*/
	CString GetOffDutyTime(INT nIndex);

	/**
	*班次类型: 早、中、晚
	*/
	INT GetShiftType(INT nIndex);

	/**
	*收银员
	*/
	CString GetOperator(INT nIndex);

	/**
	*上班备用金
	*/
	INT GetReserveAmount(INT nIndex);

	/**
	*押金
	*/
	INT GetDeposit(INT nIndex);

	/**
	*工本费
	*/
	INT GetCostExpense(INT nIndex);

	/**
	*充值金额
	*/
	INT GetCreditAmount(INT nIndex);

	/**
	*退款金额
	*/
	INT GetReturnAmount(INT nIndex);

	/**
	*现金购物
	*/
	INT GetCashPay(INT nIndex);

	/**
	*商品采购支出
	*/
	INT GetStoreInAmount(INT nIndex);

	/**
	*其他支出
	*/
	INT GetPayout(INT nIndex);

	/**
	*留给下一班
	*/
	INT GetRemainAmount(INT nIndex);

	/**
	*上交金额
	*/
	INT GetFetchAmount(INT nIndex);

	/**
	*账户购物
	*/
	INT GetNetBarPay(INT nIndex);

	/**
	*充值卡充值
	*/
	INT GetCardCredit(INT nIndex);

	/**
	*备注
	*/
	CString GetDescription(INT nIndex);

	// 2011/08/03-8201-gxx: 点卡销售
	INT GetDiankaSold(INT i);

};

}