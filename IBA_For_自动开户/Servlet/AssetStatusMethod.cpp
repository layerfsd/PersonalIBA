#include "stdafx.h"
#include "AssetStatusMethod.h"

using namespace NS_SERVLET;

CAssetStatusMethod::CAssetStatusMethod(void)
: CServletMethod(_T("AssetStatus")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CAssetStatusMethod::~CAssetStatusMethod(void)
{
}

void CAssetStatusMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("termId"), EPT_STRING, 8);
	m_Params.AddParam(_T("assetId"), EPT_NUMBER, 8);
	m_Params.AddParam(_T("assetStatus"), EPT_STRING, 30);
}

void CAssetStatusMethod::SetTermId(CString newVal)
{
	if (newVal.GetLength() > 8)
	{
		newVal = newVal.Right(8);
	}

	m_Params.SetValue(_T("termId"), newVal);
}

void CAssetStatusMethod::SetAssetId(UINT newVal)
{
	m_Params.SetValue(_T("assetId"), newVal);
}

void CAssetStatusMethod::SetAssetStatus(CString newVal)
{
	m_Params.SetValue(_T("assetStatus"), newVal);
}

void CAssetStatusMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

