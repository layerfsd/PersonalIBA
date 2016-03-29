#pragma once

#include "servletmethod.h"

namespace NS_SERVLET
{

class CShiftStoreInMethod;

class CShiftStoreInRes
{
public :
	
	CShiftStoreInRes()
	{
		m_RespFields.AddRespField(_T("voucherNo"), EPT_NUMBER, 16);
		m_RespFields.AddRespField(_T("commodityCode"), EPT_STRING, 16);
		m_RespFields.AddRespField(_T("payType"), EPT_NUMBER, 1);
		m_RespFields.AddRespField(_T("quantity"), EPT_NUMBER, 6);
		m_RespFields.AddRespField(_T("amount"), EPT_NUMBER, 6);
	}

	CString GetVoucherNo()// 单据编号
	{
		return m_RespFields.GetValueAsString(_T("voucherNo"));
	}
	CString GetCommodityCode()// 商品代码
	{
		return m_RespFields.GetValueAsString(_T("commodityCode"));
	}
	UINT GetPayType()// 支付类型，1现金 0非现金
	{
		UINT nVal = m_RespFields.GetValueAsUINT(_T("payType"));
		IBA_CHECK_UINT(nVal);
		return nVal;
	}
	UINT GetQuantity()// 数量
	{
		UINT nVal = m_RespFields.GetValueAsUINT(_T("quantity"));
		IBA_CHECK_UINT(nVal);
		return nVal;
	}
	UINT GetAmount()// 金额
	{
		UINT nVal = m_RespFields.GetValueAsUINT(_T("amount"));
		IBA_CHECK_UINT(nVal);
		return nVal;
	}

public :

	CRespFieldList  m_RespFields;	//!< 返回值列表

	friend class CShiftStoreInMethod;

};

typedef CArray<CShiftStoreInRes, CShiftStoreInRes&> CShiftStoreInResArray;


//
//class:Method: CShiftStoreInMethod 入库记录查询接口
//
class CShiftStoreInMethod :public CServletMethod
{
public:
	CShiftStoreInMethod(void);
	~CShiftStoreInMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	
	virtual BOOL DecodeResp(CString strResp);

public:
	//班次ID
	void SetDutyId(UINT newVal);

	//收银台ID
	void SetCashRegisterId(UINT newVal);

	//操作员姓名
	void SetOperator(CString newVal);
	

	

	/////////////////////返回域///////////////////////
	CString GetOndutyTime(); //YYYYMMDDhhmmss
	UINT GetRecordCount();

	/////////////////////循环的返回值//////////////////
	
	CShiftStoreInRes& GetStoreRecord(INT nIndex);

	
private:
	CShiftStoreInResArray m_arrayRes;
};

}
