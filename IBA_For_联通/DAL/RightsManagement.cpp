#include "StdAfx.h"
#include "RightsManagement.h"
#include "..\IBA.h"
#include "..\Cashier.h"
#include "..\BossPwdDlg.h"


void NS_DAL::CRightsManagement::CreateManagement(LPCTSTR lpRightsList, BOOL bBoss)
{
	m_bIsBoss = bBoss;

	ASSERT(lpRightsList);
	m_strRightsList = lpRightsList;
	m_strRightsList.AppendChar(_T(','));
}

BOOL NS_DAL::CRightsManagement::HasRight(LPCTSTR lpRightName)
{
	if (m_bIsBoss)
	{
		// 老板具有所有权限
		return TRUE;
	}
	CString strTmp(lpRightName);
	strTmp.Append(_T(","));

	return (m_strRightsList.Find(strTmp) >= 0);
}

BOOL NS_DAL::CRightsManagement::TakeRight(LPCTSTR lpRightName, BOOL bShouldOnDuty/*=TRUE*/)
{
	if (bShouldOnDuty)
	{
		if (!theApp.GetCurCashier()->IsOnDuty())
		{
			return FALSE;
		}
	}
	if (HasRight(lpRightName))
	{
		return TRUE;
	}
	else
	{
		CDlgBossPwd dlg;
		return (dlg.DoModal() == IDOK);
	}
}

void NS_DAL::CRightsManagement::SetCmdUI(CCmdUI *pCmdUI, LPCTSTR lpszName, BOOL bShouldOnDuty/*=TRUE*/)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken) //本地模式，采用原来的设置
	{
		BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 

		if (bRet)
		{
			bRet = theApp.GetCurCashier()->HasPopedomOf(lpszName);
		}

		pCmdUI->Enable(bRet);
	}
	else
	{
		if (bShouldOnDuty) //需要上班
		{
			pCmdUI->Enable(theApp.GetCurCashier()->IsOnDuty());
		}
		else //只需要判断权限
		{
			pCmdUI->Enable(TRUE);
		}
	}
}