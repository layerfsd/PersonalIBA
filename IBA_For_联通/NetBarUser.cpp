#include "stdafx.h"
#include ".\netbaruser.h"

CNetBarUser::CNetBarUser(void)
{
	m_nMemberId = 0;
	m_Sex = 0;
	m_bCanEditUserInfo = FALSE;
	m_bIsMember = FALSE;
	m_nDataFrom = 0;

	DeleteKey = TRUE;
	ScanType = 0;
}

CNetBarUser::~CNetBarUser(void)
{
}

