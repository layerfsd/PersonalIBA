#include "stdafx.h"

#include "gxx_base.h"

//---------CMyClcok()--------------------------------
NS_GXX::CMyClock::CMyClock()
{
	m_dwTickCount = GetTickCount();
}
void NS_GXX::CMyClock::Start()
{
	m_dwTickCount = GetTickCount();
}

DWORD NS_GXX::CMyClock::End()
{
	return GetTickCount() - m_dwTickCount;
}
