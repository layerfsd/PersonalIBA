#include "stdafx.h"
#include "StoreQuantityMethod.h"

using namespace NS_SERVLET;

CStoreQuantityMethod::CStoreQuantityMethod(void)
: CServletMethod(_T("StoreQuantity")) //中心方法名称
{
	m_bUseQueryHost = TRUE;
	m_nRevTimeout = 120;	//超时2分钟

	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CStoreQuantityMethod::~CStoreQuantityMethod(void)
{
}

void CStoreQuantityMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("storeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
}

void CStoreQuantityMethod::SetStoreId(UINT newVal)
{
	m_Params.SetValue(_T("storeId"), newVal);
}

void CStoreQuantityMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CStoreQuantityMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 4, TRUE, TRUE);
	m_RespFields.AddRespField(_T("commodityInfo"), EPT_STRING, 22);

}

UINT CStoreQuantityMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

CString CStoreQuantityMethod::GetCommodityInfo()
{
	return m_RespFields.GetValueAsString(_T("commodityInfo"));
}

CString CStoreQuantityMethod::GetCommodityCode(UINT nIndex)
{
	nIndex = 22 * nIndex;

	CString strTmp = GetCommodityInfo().Mid(nIndex, 22);

	return strTmp.Left(16).Trim();
}

UINT CStoreQuantityMethod::GetRemainQuantity(UINT nIndex)
{
	nIndex = 22 * nIndex;

	CString strTmp = GetCommodityInfo().Mid(nIndex, 22);

	return _ttoi(strTmp.Mid(16).Trim());
}