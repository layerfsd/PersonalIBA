#pragma once
#include "common\ThreadTemplate.h"
#include "Servlet\CashRegisterLoginMethod.h"
#include "IBA.h"
#include "BrokenNetworkResume.h"

// 2011/07/08-8201-gxx: 
// 断网控制，网络正常的情况下保存数据，断网情况下加载数据
//
class CBrokenNetwork
{
	CBrokenNetwork(void);

public:

	~CBrokenNetwork(void);

	static CBrokenNetwork* GetInstance();

public:
	// 保存最近一次正常登陆时的信息，使得进行本地登录时能正常使用
	BOOL SaveLocalStatusInfo(CLocalStatusInfo& Info);

	BOOL LoadLocalStatusInfo();

	// 从数据库读取本地模式状态
	static CIBAGlobal::emNetworkStatus LoadLocalModeStatus();

	// 注册并返回一个MemberID值
	static UINT RegisterMemberID();

	static void DisableNetwork();
	static void EnableNetwork();

	static void SettingNetworkStatus(CIBAGlobal::emNetworkStatus status);

	BOOL TestingNetwork();

private:
	void TestingResume(); // 测试网络是否从断网中得到恢复
	void TestingUploadLocalConsume(); // 定时上传本地用户信息

private:
	CCriticalSection m_TestUploadLock;

public:
	NS_COMMON::CThreadTemplate<CBrokenNetwork> *m_threadTestResume; // 测试网络是否恢复的线程
	NS_COMMON::CThreadTemplate<CBrokenNetwork> *m_threadRestore; 
	BOOL m_bHasFirstTrying;
	CBrokenNetworkResume m_centerUserUpLoad;
};
