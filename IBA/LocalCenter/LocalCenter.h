#pragma once

#include "G2XSocketServer.h"
#include "..\Singleton.h"
#include "..\common\ThreadTemplate.h"
#include "gxx_base.h"
#include <map>
#include "LocalDefine.h"
#include "..\Excel\CriticalSection.h"

// 无错
#define ERROR_CODE_NONE 0
// 非法请求
#define ERROR_CODE_ILLEGAL 1 
// 校验失败
#define ERROR_CODE_CHECK 2 
// 缺少参数
#define ERROR_CODE_PARAM 3
// 请求过期
#define ERROR_CODE_TIMEOUT 4
// 数据太长
#define ERROR_CODE_DATA_TOO_LARGE 5


//////////////////////////////////////////////////////////////////////////
// 2015/5/15 tmelody
// 注册成功记录到本地需要的信息结构

typedef struct tagUserInfo
{
	CString _strAccount;		//账号
	CString _strMemberID;		//会员号
	CString _strPwd;			// 
	CString _strClassID;		//会员级别
	BOOL	_bLocalRegister;	//是否本地注册

	tagUserInfo()
		: _strAccount(_T(""))
		, _strMemberID(_T(""))
		, _strPwd(_T(""))
		, _strClassID(_T(""))
		, _bLocalRegister(FALSE){
		}
}USERINFO, *PUSERINFO, *LPUSERINFO;


//////////////////////////////////////////////////////////////////////////
// 

class CLocalCenter : public CG2XServerSocketDelegate
{
	SINGLETON_DECLARE(CLocalCenter);
public:

	static CLocalCenter* GetInstance();

	BOOL InitLocalCenter();

	// 2015/5/15 tmelody
	// 记录开户用户
	BOOL RecordRegisteredUser(const USERINFO& userInfo);

	/**************************************************************************
	*
	* CG2XServerSocketDelegate
	*
	***************************************************************************/
private:
	virtual bool didClientConnected(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, const char* pData, const int nBufLen);
	virtual void didDumpText(const char* pText, bool isErrorDump/* =false */);
	virtual bool didHandleError(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, ERROR_CODE code, DWORD dwLastErrorCode);

private:
	void ThreadEntry();
	void ThreadEntryTestNetworking();

	
	GxxValuePtr ToUtf8Value(LPCWSTR lpText);
	
	std::wstring GetDataCodeErrorMessage(long nDataCodeError);
	void PutDataCode(GxxDictionaryPtr& outputInfo, long nDataCodeError);
	BOOL IsBundleTimeEnd(GxxDictionaryPtr& memberPtr);

private:
	typedef void (CLocalCenter::*_HandleFunc)(GxxDictionaryPtr, CG2XSocketServer* , PER_SOCKET_CONTEXT*) ;


	struct _HandleInfo {
		_HandleFunc handle;
	};
	// 对网络请求做解密 
	int _Decode(const char* data, int nLen, GxxDictionaryPtr& reqData);

	void _handleCommand(const char* command, GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);
	void _addHandle(const char* command, CLocalCenter::_HandleFunc memberFunc);

private:
	/** 
	 * 处理客户端请求查询本地模式
	*/
	void HandleQueryLocalMode(GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);
	/**
	 * 处理客户端请求本地登录
	*/
	void HandleLocalLogon(GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);
	/**
	 * 处理客户端请求查询余额
	*/
	void HandleQueryBalance(GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);
	/**
	 * 处理上报登录状态
	*/
	void HandleReportLogon(GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);

private:
	/**
	 * sql语句查询数据
	*/
	void HandleSqlQuery(GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient);

public:
	void OutputErrorInfo(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, int nErrorCode);
	void OutputInfo(CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, GxxDictionaryPtr outputInfo);

private:
	NS_COMMON::CThreadTemplate<CLocalCenter> m_thread;
	NS_COMMON::CThreadTemplate<CLocalCenter> m_threadTestNetwork;
	std::map<std::string, _HandleInfo> m_mapHandles;

	static const UINT SERVER_COUNT = 2;
	UINT m_nPortArray[SERVER_COUNT];
	CG2XSocketServer *m_serverArray[SERVER_COUNT];

	// 记录初始启动时间
	DWORD m_dwStartUpTick;

};
