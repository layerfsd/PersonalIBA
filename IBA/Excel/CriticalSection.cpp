#include "stdafx.h"

#include "CriticalSection.h"

namespace NS_GXX
{

CCriticalSection::CCriticalSection ()
{
	::InitializeCriticalSection ( &m_cs );
}
CCriticalSection::~CCriticalSection()
{
	::DeleteCriticalSection ( &m_cs );
}
void CCriticalSection::Lock ()
{
	::EnterCriticalSection( &m_cs );
}
void CCriticalSection::Unlock ()
{
	::LeaveCriticalSection( &m_cs );
}

CAutoLock::CAutoLock ( CCriticalSection& cs ):m_cs (cs)
{
	cs.Lock ();
}
CAutoLock::~CAutoLock ()
{
	m_cs.Unlock ();
}

}