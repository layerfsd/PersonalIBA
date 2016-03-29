#include "stdafx.h"
#include "LocalModeMethod.h"

using namespace NS_SERVLET;

CLocalModeMethod::CLocalModeMethod(void)
: CServletMethod(_T("LocalMode")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CLocalModeMethod::~CLocalModeMethod(void)
{
}

void CLocalModeMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("operationType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("lastOperationTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
}

void CLocalModeMethod::SetOperationType(UINT newVal)
{
	m_Params.SetValue(_T("operationType"), newVal);
}

void CLocalModeMethod::SetLastOperationTime(CString newVal)
{
	m_Params.SetValue(_T("lastOperationTime"), newVal);
}

void CLocalModeMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CLocalModeMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

