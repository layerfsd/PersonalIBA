#include "stdafx.h"
#include ".\userclassinfo.h"

CUserClassInfo::CUserClassInfo(void)
{
	m_ClassID = 0;
	m_CostExpense = 0;
	m_Deposit = 0;
	m_bIsMember = FALSE;
}

CUserClassInfo::~CUserClassInfo(void)
{
}

UINT CUserClassInfo::GetClassID()
{
	return m_ClassID;
}

void CUserClassInfo::SetClassID(UINT newVal)
{
	m_ClassID = newVal;
}

CString CUserClassInfo::GetClassName()
{
	return m_strClassName;
}

void CUserClassInfo::SetClassName(CString newVal)
{
	m_strClassName = newVal;
}

UINT CUserClassInfo::GetCostExpense()
{
	return m_CostExpense;
}

void CUserClassInfo::SetCostExpense(UINT newVal)
{
	m_CostExpense = newVal;
}

UINT CUserClassInfo::GetDeposit()
{
	return m_Deposit;
}

void CUserClassInfo::SetDeposit(UINT newVal)
{
	m_Deposit = newVal;
}

BOOL CUserClassInfo::GetIsMember()
{
	return m_bIsMember;
}

void CUserClassInfo::SetIsMember(BOOL newVal)
{
	m_bIsMember = newVal;
}
