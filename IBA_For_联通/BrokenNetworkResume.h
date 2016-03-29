#pragma once
/**************************************************************************
class: CBrokenNetworkResume 断网后恢复数据
date: 2011-3-28-gxx-add

**************************************************************************/

#include "Dal\IDTypeInfo.h"

class CBrokenNetworkResume : protected CThread
{
public:
	CBrokenNetworkResume(void);
	~CBrokenNetworkResume(void);

	BOOL Init();

	// 获取是否应该恢复
	BOOL GetShouldResume();
	void SetShouldResume(BOOL bResume = TRUE);
	
	// 开始恢复
	void StartResume(INT nConnectionState);

public:

	virtual INT Run();

	BOOL CheckShouldResume();

	//执行登出操作（跟中心的）
	BOOL DeregisterUser(NS_DAL::CCheckOutinfo& CheckOutinfo);

protected:
	CCriticalSection m_TestUploadLock;
	BOOL m_bShouldResume;
};
