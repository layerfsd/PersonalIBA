#include "StdAfx.h"
#include "IBA.h"
#include "IControl.h"
#include ".\realname.h"

int IDUser(char *pszIDNo, char *pszUserNo, char *pszPassword, BYTE IDCardType, BOOL bValidate, const REQ_REGISTER_INFO& RegInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GXX_LOG_HIT("IDUser(...)");

	if (IsIBARunning())
	{
		return IBA_IDUser(pszIDNo, pszUserNo, pszPassword, IDCardType, bValidate, RegInfo);
	}

	return ICC_IDUser(pszIDNo, pszUserNo, pszPassword, IDCardType, bValidate, RegInfo);
}

int AddToMemberRZX(char *pszIDNo, char *pszUserNo, char *pszPassword, BYTE IDCardType, const REQ_REGISTER_INFO& RegInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GXX_LOG_HIT("AddToMemberRZX(...)");

	if (IsIBARunning())
	{
		return IBA_AddToMemberRZX(pszIDNo, pszUserNo, pszPassword, IDCardType, RegInfo);
	}

	return ICC_AddToMemberRZX(pszIDNo, pszUserNo, pszPassword, IDCardType, RegInfo);
}

int RzxNotiFy(int nNotifyType, const char * lpNotifyMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IsIBARunning())
	{
		return IBA_RzxNotiFy(nNotifyType, lpNotifyMsg);
	}

	return ICC_RzxNotiFy(nNotifyType, lpNotifyMsg);
}