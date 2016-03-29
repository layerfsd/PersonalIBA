#pragma once

#include "..\Singleton.h"
#include "ClientSocket.h"
#include "..\ComputerList.h"
#include "..\ActiveMemberList.h"

class CComputerListView;
class CActiveMemberView;

namespace NS_ISERVER
{

class CLocalServer : private CTimer
{

	SINGLETON_DECLARE(CLocalServer)

public:
	
	static CLocalServer* GetInstance();

	enum EDOING
	{	
		ED_NOTHING,
		ED_CAPTURESCREEN,
		ED_GETLOG
	};

private :
	
	CClientSocket* m_lpSocket;
	CCommandProcessor* m_lpCommandProcessor;
	CBufferProcess* m_lpBufferProcessor;
	
private :

	void InitLocalServer();
	void SendBuffer(const NET_HEAD_MAN* pHead, const void* pBuf = NULL, BOOL bCheckStatus = TRUE);

	int m_CheckInStatus; //初始未登入:0	 登入成功 1: 登入失败: -1;
	
	EDOING m_CurrentState; 

	CComputerListView* m_lpComputerListView;
	CActiveMemberView* m_lpActiveMemberView;

	virtual void OnTimer(const UINT uTimerID);

public :

	CComputerListView* GetComputerListView() const;
	void SetComputerListView(CComputerListView* newVal);

	CActiveMemberView* GetActiveMemberView() const { return m_lpActiveMemberView; }
	void SetActiveMemberView(CActiveMemberView* newVal);

	CComputerList ComputerList;
	CActiveMemberList ActiveMemberList;

	int GetCheckInStatus() const { return m_CheckInStatus; }
	void SetCheckInStatus(int newVal) { m_CheckInStatus = newVal; }

	EDOING GetCurrentState() const { return m_CurrentState; }
	void SetCurrentState(EDOING newVal) { m_CurrentState = newVal; }

public :

	BOOL ConnectServer(void);
	BOOL IsConnected();
	BOOL LocalPing();
	void LocalLogin();
	void LocalCheckin();
	void LocalGetComputerList();
	void LocalGetComputerCount();
	void LocalGetMemberClass();
	void LocalGetIP2TermID();
	void LocalGetPCClassName();
	void LocalGetPCClassMap();
	void LocalPingClient(INT_PTR nIndex);
	void LocalCaptureScreen(INT_PTR nIndex);
	void LocalGetLog(INT_PTR nIndex);
	void LocalShutdownAllComputer(BOOL bReboot);
	void LocalShutdownComputer(INT_PTR nIndex, BOOL bReboot);
	void LocalStartupComputer(INT_PTR nIndex);
	void LocalForceCheckOut(INT_PTR nIndex, CString strMsg);
	void LocalSendShortMessage(INT_PTR nIndex, CString& strMsg);
	void LocalSendShortMessageByMemberID(UINT nMemberID, CString& strMsg);
	void LocalSendGroupShortMessage(INT nPCClass, CString& strMsg);
	
	//nType = 1是换机，0是普通,2结账下机(告诉客户端只做锁屏，不连中心)，3通知客户端重新登录
	void LocalCheckOutUser(INT_PTR nIndex, UINT nMemberID, UINT nType = 0);
	void LocalCheckOutAllUser();
	
	void LocalBundleTime(UINT nMemberID, UINT nBundleTimeId = 9999);
	void LocalRestartServer();

	//要求客户端查余额并更新
	void LocalForceUpdateData(INT_PTR nIndex);

	//请求客户端自动登录
	void LocalAutoCheckIn(INT_PTR nIndex);

	//解屏
	void LocalUnlockScreen(INT_PTR nIndex, CString strUnlockPwd);

	// 2011/09/08-8201-gxx: 请求公告信息，（IServer从中心获取的公告信息）
	//   bSigned 是否公告被签收（在成功接收）
	void LocalAskNotice(BOOL bSigned=FALSE);

	// 2011/10/27-8210-gxx: 		
	void LocalAuditUserQuit(CString strTermID, CString strIP,UINT nMemberID);

	
};

}

using namespace NS_ISERVER;