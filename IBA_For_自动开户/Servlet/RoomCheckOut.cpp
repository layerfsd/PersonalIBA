#include "stdafx.h"
#include "RoomCheckOut.h"


using namespace NS_SERVLET;

CRoomCheckOut::CRoomCheckOut(void)
: CServletMethod(_T("RoomCheckout")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CRoomCheckOut::~CRoomCheckOut(void)
{
}

void CRoomCheckOut::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！

	CServletMethod::AddParams();
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("pcClass"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
}


void CRoomCheckOut::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CRoomCheckOut::SetPCClass(UINT newVal)
{
	m_Params.SetValue(_T("pcClass"), newVal);
}

void CRoomCheckOut::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CRoomCheckOut::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CRoomCheckOut::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}


void CRoomCheckOut::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("tranTime"), EPT_STRING, 14);
}


CString CRoomCheckOut::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}
