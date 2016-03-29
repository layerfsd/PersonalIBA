#pragma once

#include "RealName.h"

int ICC_IDUser(char * pszIDNo,
	char * pszUserNo,
	char * pszPassword,
	BYTE IDCardType,
	BOOL bValidate,
	const REQ_REGISTER_INFO & RegInfo);

int ICC_AddToMemberRZX(char * pszIDNo,
	char*pszUserNo,
	char*pszPassword,
	BYTE IDCardType,
	const REQ_REGISTER_INFO & RegInfo);

int ICC_RzxNotiFy(int nNotifyType, const char * lpNotifyMsg);

BOOL ReadIDCardInfo(char * pszIDNo,
	char * pszUserNo,
	char * pszPassword,
	char * pszIDCardType,
	char * pszValidate,
	char * pszName,
	char * pszCertificateType,
	char * pszCertificateID,
	char * pszCompany,
	char * pszCountry,
	char * pszDetail);


