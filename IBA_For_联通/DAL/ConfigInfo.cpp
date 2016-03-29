#include "stdafx.h"
#include "configinfo.h"

using namespace NS_DAL;

CConfigInfo::CConfigInfo(void)
: m_KeyID(0), m_KeyValue1(0), m_KeyValue2(0)
{
}

CConfigInfo::~CConfigInfo(void)
{
}

UINT CConfigInfo::GetKeyID() const
{
	return m_KeyID;
}

void CConfigInfo::SetKeyID(UINT newVal)
{
	m_KeyID = newVal;
}

CString CConfigInfo::GetKeyType() const
{
	return m_KeyType;
}

void CConfigInfo::SetKeyType(CString newVal)
{
	m_KeyType = newVal;
}

CString CConfigInfo::GetKeyName() const
{
	return m_KeyName;
}

void CConfigInfo::SetKeyName(CString newVal)
{
	m_KeyName = newVal;
}

CString CConfigInfo::GetKeyString() const
{
	return m_KeyString;
}

void CConfigInfo::SetKeyString(CString newVal)
{
	m_KeyString = newVal;
}

UINT CConfigInfo::GetKeyValue1() const
{
	return m_KeyValue1;
}

void CConfigInfo::SetKeyValue1(UINT newVal)
{
	m_KeyValue1 = newVal;
}

UINT CConfigInfo::GetKeyValue2() const
{
	return m_KeyValue2;
}

void CConfigInfo::SetKeyValue2(UINT newVal)
{
	m_KeyValue2 = newVal;
}

CString CConfigInfo::GetKeyValue3() const
{
	return m_KeyValue3;
}

void CConfigInfo::SetKeyValue3(CString newVal)
{
	m_KeyValue3 = newVal;
}

CString CConfigInfo::GetDescription() const
{
	return m_Description;
}

void CConfigInfo::SetDescription(CString newVal)
{
	m_Description = newVal;
}

void CConfigInfo::Clear()
{
	m_Description.Empty();
	m_KeyName.Empty();
	m_KeyString.Empty();
	m_KeyType.Empty();
	m_KeyID = 0;
	m_KeyValue1 = 0;
	m_KeyValue2 = 0;
	m_KeyValue3.Empty();

}