#pragma once
#include "afxsock.h"

class CG2XSocketClient;

class CG2XClientSocketDelegate
{
	friend class CG2XSocketClient;
protected:
	// 已和服务端建立连接
	virtual void didServerConnected(CG2XSocketClient* pSender) {}

	virtual void didSend(CG2XSocketClient* pSender) {}

	virtual void didReceiveData(CG2XSocketClient* pSender, void* buf, int bufLen) {}

	// 已和服务端失去连接, 返回是否自动重连
	virtual bool didServerDisconnected(CG2XSocketClient* pSender) { return true; } 
};

class CG2XSocketClient : public CAsyncSocket
{
public:
	CG2XSocketClient(CG2XClientSocketDelegate* pDelegate=NULL);
	virtual ~CG2XSocketClient(void);

	static const int MAX_BUF_LEN = 8192;

	// 连接服务器
	BOOL ConnectServer(LPCTSTR lpServerIp, unsigned int port);

	// 断开和服务器的连接
	BOOL DisconnectServer();

	BOOL IsConnected();

	BOOL SendData(const void* data, int len);

	// 是否允许客户端断开连接后自动重连
	void EnableAutoConnect(BOOL bEnable);

protected:
	void OnConnect(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnReceive(int nErrorCode);
	void OnClose(int nErrorCode);

private:
	// default: FALSE
	BOOL m_bEnableAutoConnect;
	unsigned int m_port;
	CString m_strServerIp;
	CG2XClientSocketDelegate* m_pDelegate;
};


//------------------------------------------------------------------------------------
//
// CG2XSocketClientSYNC: 短连接同步请求
// 服务器必须返回数据，否则认为是无效的请求
//
//------------------------------------------------------------------------------------
class CG2XSocketClientSYNC : public CDialog,public CG2XClientSocketDelegate
{
	DECLARE_MESSAGE_MAP()
protected:
	CG2XSocketClientSYNC();
	~CG2XSocketClientSYNC();

	enum
	{
		// 应该是一个有效的对话框模板，此处定义的是关于对话框的模板ID
		IDD = 100
	};
public:
	static CG2XSocketClientSYNC* SendData(LPCTSTR lpServerIp, unsigned int port ,const void* data, int len, unsigned int nTimeoutMs=20000);

	void Close();

protected:
	// 已和服务端建立连接
	virtual void didServerConnected(CG2XSocketClient* pSender);

	virtual void didSend(CG2XSocketClient* pSender);

	virtual void didReceiveData(CG2XSocketClient* pSender, void* buf, int bufLen);

	// 已和服务端失去连接, 返回是否自动重连
	virtual bool didServerDisconnected(CG2XSocketClient* pSender); 


	BOOL OnInitDialog();
	void OnTimer(UINT_PTR uEvent);

	CG2XSocketClient client;

	void* sendedData;
	int sendLenght;
	CString m_strIp;
	unsigned int m_uPort;
	unsigned int m_nTimeoutMs;

public:
	/* errorCode 0: 发送成功
	   errorCode 1: 无法连接
	   errorCode 2: 连接超时
	*/
	int errorCode;

	// 响应返回的数据
	BYTE* responseData;

	// 返回的数据的长度
	int dataLenght;
};
