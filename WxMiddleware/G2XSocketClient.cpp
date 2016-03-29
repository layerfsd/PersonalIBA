#include "StdAfx.h"
#include ".\g2xsocketclient.h"

CG2XSocketClient::CG2XSocketClient( CG2XClientSocketDelegate* pDelegate/*=NULL*/ )
{
	m_pDelegate = pDelegate;
	m_bEnableAutoConnect = FALSE;

	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// 错误(一般都不可能出现)
	if (NO_ERROR != nResult)
	{
		//this->_ShowMessage(_T("初始化WinSock 2.2失败！\n"));
	}
}


CG2XSocketClient::~CG2XSocketClient(void)
{
	WSACleanup();
}

BOOL CG2XSocketClient::ConnectServer( LPCTSTR lpServerIp, unsigned int port )
{
	if (m_hSocket == INVALID_SOCKET){
		CAsyncSocket::Create();
		if (m_hSocket == INVALID_SOCKET){
			// 创建socket失败
			return FALSE;
		}
	}
	else {
		// socket连接已是连接的
		return TRUE;
	}
	m_strServerIp = lpServerIp;
	m_port = port;
	return CAsyncSocket::Connect(lpServerIp, port);
}

BOOL CG2XSocketClient::DisconnectServer()
{
	return CAsyncSocket::ShutDown();
}

BOOL CG2XSocketClient::IsConnected()
{
	return m_hSocket != INVALID_SOCKET;
}

void CG2XSocketClient::EnableAutoConnect( BOOL bEnable )
{
	m_bEnableAutoConnect = bEnable;
}

BOOL CG2XSocketClient::SendData( const void* data, int len )
{
	return CAsyncSocket::Send(data, len) == len;
}

void CG2XSocketClient::OnConnect( int nErrorCode )
{
	CAsyncSocket::OnConnect(nErrorCode);

	if (nErrorCode == NO_ERROR) {
		if (m_pDelegate){
			m_pDelegate->didServerConnected(this);
		}
	}
}

void CG2XSocketClient::OnReceive( int nErrorCode )
{
	char * pBuf = new char[MAX_BUF_LEN];
	memset(pBuf, MAX_BUF_LEN, 0);

	int nBytes = Receive(pBuf, MAX_BUF_LEN);

	if (nBytes == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			// 获取信息出错
			DisconnectServer();
		}
	}
	else if (nBytes == 0)
	{
		// 数据长度为0
		DisconnectServer();
	}
	else
	{
		if (m_pDelegate) {
			m_pDelegate->didReceiveData(this, pBuf, nBytes);
		}
	}
	delete pBuf;

}

void CG2XSocketClient::OnSend( int nErrorCode )
{
	if (nErrorCode == NO_ERROR) { 
		if (m_pDelegate) {
			m_pDelegate->didSend(this);
		}
	}
}

void CG2XSocketClient::OnClose( int nErrorCode )
{
	CAsyncSocket::OnClose(nErrorCode);
	CAsyncSocket::Close();

	if (m_pDelegate) {
		if (m_pDelegate->didServerDisconnected(this)){
			if (m_bEnableAutoConnect){
				// 自动重连
				this->ConnectServer(m_strServerIp, m_port);
			}
		}
	}
}








//-------------------------------------------------------------------------------------


CG2XSocketClientSYNC* CG2XSocketClientSYNC::SendData( LPCTSTR lpServerIp, unsigned int port ,const void* data, int len, unsigned int nTimeoutMs )
{
	CG2XSocketClientSYNC* syncClient = new CG2XSocketClientSYNC();

	syncClient->m_strIp = lpServerIp;
	syncClient->m_uPort = port;
	syncClient->m_nTimeoutMs = nTimeoutMs;

	syncClient->sendedData = new BYTE[len];
	memcpy(syncClient->sendedData, data, len);
	syncClient->sendLenght = len;

	syncClient->errorCode = syncClient->DoModal();

	return syncClient;
}

CG2XSocketClientSYNC::CG2XSocketClientSYNC() :CDialog(IDD), client(this)
{
	sendedData = NULL;
	sendLenght = 0;
	errorCode = 1;
	responseData = NULL;
	dataLenght = 0;
	m_uPort = 0;
	m_nTimeoutMs = 20000;
}

CG2XSocketClientSYNC::~CG2XSocketClientSYNC()
{

}

BEGIN_MESSAGE_MAP(CG2XSocketClientSYNC,CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CG2XSocketClientSYNC::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(WS_VISIBLE, 0, 0);
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_HIDEWINDOW);

	try
	{
		client.ConnectServer(m_strIp, m_uPort);
		if (!client.IsConnected()) {
			CDialog::EndDialog(1);
		}
	}
	catch (...)
	{
	}
	
	SetTimer(1, m_nTimeoutMs, NULL);

	return FALSE;
}



void CG2XSocketClientSYNC::Close()
{
	if (sendedData) {
		delete []sendedData;
		sendedData = NULL;
	}
	if (responseData) {
		delete []responseData;
		responseData = NULL;
	}

	delete this;
}


void CG2XSocketClientSYNC::didServerConnected( CG2XSocketClient* pSender )
{
	
}

void CG2XSocketClientSYNC::didSend( CG2XSocketClient* pSender )
{
	if (!client.SendData(sendedData, sendLenght)){
		EndDialog(1);
	}
	
}

void CG2XSocketClientSYNC::didReceiveData( CG2XSocketClient* pSender, void* buf, int bufLen )
{
	responseData = new BYTE[bufLen+1];
	responseData[bufLen] = 0;
	dataLenght = bufLen;
	memcpy(responseData, buf, bufLen);

	EndDialog(0);
}

bool CG2XSocketClientSYNC::didServerDisconnected( CG2XSocketClient* pSender )
{
	return false;
}

void CG2XSocketClientSYNC::OnTimer( UINT_PTR uEvent )
{
	if (uEvent == 1) {
		EndDialog(2);
	}
}


