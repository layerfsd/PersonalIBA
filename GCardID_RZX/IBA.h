
#pragma once

#include "RealName.h"

BOOL IsIBARunning();

int IBA_IDUser(char *pszIDNo, char *pszUserNo, char *pszPassword,
		   BYTE IDCardType, BOOL bValidate, const REQ_REGISTER_INFO& RegInfo);

int IBA_AddToMemberRZX(char *pszIDNo, char *pszUserNo, 
				   char *pszPassword,BYTE IDCardType, const REQ_REGISTER_INFO& RegInfo);

int IBA_RzxNotiFy(int nNotifyType, const char * lpNotifyMsg);
