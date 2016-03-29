#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "ServletHellper.h"
#include "MainFrm.h"
#include ".\currentnetbaruser.h"

SINGLETON_IMPLEMENT(CCurrentNetBarUser)

CCurrentNetBarUser::CCurrentNetBarUser(void)
{
}

CCurrentNetBarUser::~CCurrentNetBarUser(void)
{
}

CString CCurrentNetBarUser::GetNetId()
{
	CString strNetId;
	if ( m_strNetId.IsEmpty() )
	{
		strNetId = Balance.GetSerailNum();
	}
	else
	{
		strNetId = m_strNetId;
	}

	return strNetId;
}

INT CCurrentNetBarUser::GetLastStatusCode() const
{
	return m_nLastStatusCode;
}

UINT CCurrentNetBarUser::GetMemberId() const
{
	return m_nMemberId;
}

BOOL CCurrentNetBarUser::QueryBalance(LPCTSTR lpszNetId)
{
	m_strNetId = lpszNetId;

	m_nMemberId = 0;
	m_nLastStatusCode = 0;

	UINT nCardId = 0;

	CIDCheckCardMethod checkCard;
	
	checkCard.SetSerialNum(m_strNetId);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	const INT nStatus = checkCard.GetStatusCode();

	if (nStatus == 0 || nStatus == 75)
	{
		nCardId = checkCard.GetCardId();
		m_nMemberId = checkCard.GetMemberId();
	}
	else
	{
		m_nLastStatusCode = checkCard.GetStatusCode();
	}

	if (m_nMemberId > 0)
	{
		Balance.SetMemberId(m_nMemberId);
		Balance.SetCardId(nCardId);
		Balance.SetRound(0);//È¡Õû

		theApp.GetCurCashier()->DoBalance(Balance);

		if (Balance.GetStatusCode() == 0)
		{
			theApp.UpdateToolbarLastID(Balance.GetSerailNum());
			return TRUE;
		}
		else
		{
			m_nLastStatusCode = Balance.GetStatusCode();
		}
	}

	return FALSE;
}

