#pragma once

#include "RealNameBase.h"
#include "..\Servlet\IDCheckCardMethod.h"

namespace NS_REALNAME
{

class CJQRealName : public CRealNameBase
{
public:

	CJQRealName(void);

	virtual ~CJQRealName(void);

	//!<实名初始化
	virtual BOOL InitRealName();

	virtual INT OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<用于二次执行刷卡消息
	virtual INT  OnExecuteMsg(UINT nExecuteType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<是否允许实名菜单
	virtual BOOL IsMenuEnabled();

protected:

	virtual BOOL ReadUserInfo(CNetBarUser & NetBarUser);
private:
	
	//检查中心和实名的密码是否相同
	BOOL AutoChangeRealNamePwd(CNetBarUser& NetBarUser);

	INT ChangeCertificateType(INT nTmp);

	INT OnCloseUser(CNetBarUser & NetBarUser);

	INT OnOpenUser(CNetBarUser & NetBarUser);
};

}