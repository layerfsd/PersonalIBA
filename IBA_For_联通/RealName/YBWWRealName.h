#pragma once

#include "realnamebase.h"

namespace NS_REALNAME
{

class CYBWWRealName : public CRealNameBase, protected CThread
{
public:
	CYBWWRealName(void);
	virtual ~CYBWWRealName(void);

	//!<实名初始化
	virtual BOOL InitRealName();

	//!<实名清理
	virtual BOOL UnInitRealName();

	virtual INT OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

private:
	
	virtual INT Run();

	BOOL ReadUserInfo(CNetBarUser & NetBarUser);

	BOOL ExecYB2IBA();

	CString m_strYB2IBA;
};

}