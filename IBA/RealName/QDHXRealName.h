#pragma once

#include "RealNameBase.h"

namespace NS_REALNAME
{

class CQDHXRealName : public CRealNameBase
{
public:
	CQDHXRealName(void);
	virtual ~CQDHXRealName(void);

	virtual INT OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	virtual BOOL UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption = 0);

};

}