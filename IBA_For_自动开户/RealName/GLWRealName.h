#pragma once

#include "RealNameBase.h"
#include "..\Servlet\IDCheckCardMethod.h"

namespace NS_REALNAME
{

class CGLWRealName : public CRealNameBase
{
public:

	CGLWRealName(void);

	virtual ~CGLWRealName(void);

	//!<实名初始化
	virtual BOOL InitRealName();

	//!<调用实名相关功能
	virtual INT  InvokeRealName(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	virtual INT OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<用于二次执行刷卡消息
	virtual INT  OnExecuteMsg(UINT nExecuteType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<是否允许实名菜单
	virtual BOOL IsMenuEnabled();

	//!<是否需要校验开户实名资料
	virtual BOOL NeedToCheckRegisterData();

protected:

	virtual BOOL OnCardInfo(DataX_UserInfo& dxUserInfo);

	virtual BOOL OnForceCheckOut(DataX_UserInfo& dxUserInfo);

	virtual BOOL ReadUserInfo(CNetBarUser & NetBarUser);
private:
	
	//检查中心和实名的密码是否相同
	BOOL AutoChangeRealNamePwd(CNetBarUser& NetBarUser);

	INT ChangeCertificateType(INT nTmp);

	

	INT OnCloseUser(CNetBarUser & NetBarUser);

	INT OnOpenUser(CNetBarUser & NetBarUser);

	BOOL SendMsgToGLW(UINT nMsgType);
};

}