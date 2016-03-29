#pragma once
#include "..\StdAfx.h"

namespace NS_GXX
{

class CCriticalSection
{
public:
	CCriticalSection();
	~CCriticalSection();

	void Lock();
	void Unlock();

private:
	CRITICAL_SECTION m_cs;
};

class CAutoLock
{
public:
	CAutoLock( CCriticalSection& cs );
	~CAutoLock();
private:
	CCriticalSection& m_cs;
};

}