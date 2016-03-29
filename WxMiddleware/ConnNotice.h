#pragma once

#include <string>
#include "G2XSocketClient.h"

#include "..\\IBA\\WxMiddlewareDelegate.h"


class CDelegateBufferProcess
{
public:
	virtual void OnConnectSucceed(){}
	virtual void OnConnectionFailed(){}
	virtual void OnBufferProcess(PVOID pBuf, int nLen){}
	virtual void OnClose(int nErrorCode){}
};

static const UINT SOCKT_MIN_SIZE = 8;
static const UINT SOCKET_TCP_BUFFER	= 16384;	//网络缓冲
static const UINT TAG_LOGIN = 0x1;
static const UINT TAG_HEART = 0x9;
static const UINT TAG_CLIENT_LOGIN = 0x10;
static const UINT TAG_CLIENT_LOGOUT = 0x11;
static const UINT TAG_MESSAGE = 0x8;
static const UINT TAG_TAKESEAT = 0x14;
static const UINT TAG_FREESEAT = 0x15;

#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

class CClientSocket;
class CUdoConnList;





class CConnNotice : public CG2XClientSocketDelegate
{
public:
	static CConnNotice* GetInstance();

protected:
	CConnNotice(); 
	virtual ~CConnNotice(void);

	
	
	static CConnNotice* m_Instance;

	CG2XSocketClient* m_lpSocket;	
	
public:
	void SetWxMiddlewareDelegate(CWxMiddlewareDelete* pDelegate);
	CWxMiddlewareDelete* GetWxMiddlewareDelegate();
	
private:
	CWxMiddlewareDelete* m_pWxDelegate;
	

	// 2014-10-23 - qsc
	bool m_bAutoClose;
protected:
	virtual void didServerConnected(CG2XSocketClient* pSender);
	virtual void didReceiveData(CG2XSocketClient* pSender, void* buf, int bufLen);
	virtual bool didServerDisconnected(CG2XSocketClient* pSender);
private:
	int m_nTimerIDHeart;
	int m_nTimerIDReConn;		// 重新连接定时器
	static VOID CALLBACK TimerProc(
		HWND hwnd,         // handle to window
		UINT uMsg,         // WM_TIMER message
		UINT_PTR idEvent,  // timer identifier
		DWORD dwTime       // current system time
		);

	void SendHeart();

public:
	BOOL IsSocketValid(void);
	BOOL ConnectNotice(void);
	void SendData(int nTag, const wchar_t *pStr, int nSize);
	void StopConn();
	void SendLogin();
	void SendClientLoginRet(LPCWSTR szTermID, int nRet);
	void SendClientLogoutRet(LPCWSTR szTermID, int nRet);
	void SendClientMessageRet(int nRet);
	static int ConvertRetToCenter(int nConvert);
	
};