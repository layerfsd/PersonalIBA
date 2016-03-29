// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "ClientSocket.h"
#include "ConnNotice.h"
// CClientSocket

CClientSocket::CClientSocket() 
: m_isConnected(FALSE)
{
}

CClientSocket::~CClientSocket()
{
}

// CClientSocket 成员函数

void CClientSocket::OnConnect(int nErrorCode)
{
	if (nErrorCode != 0)
	{
		OutputEx(_T("CClientSocket::Connect Failed -") + ToString(nErrorCode));

		Close();
		
		m_pDelegate->OnConnectionFailed();
	}
	else
	{

		OutputEx(_T("CClientSocket::Connect Success."));

		m_isConnected = TRUE;

		if(m_pDelegate)
		{
			m_pDelegate->OnConnectSucceed();
		}
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	TRACE("CClientSocket::OnReceive\n");

	BYTE cbDataBuffer[SOCKET_TCP_BUFFER] = {0};
	int nBytes = Receive(cbDataBuffer, CountArray(cbDataBuffer) );

	if (nBytes == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			OutputEx(_T("CClientSocket::OnReceive Error - ") + ToString(WSAGetLastError()));
			Close();
		}
	}
	else if (nBytes == 0)
	{
		OutputEx(_T("CClientSocket::OnReceive - Server Disconnected."));
		Close();
	}
	else
	{
		if(m_pDelegate)
		{
			m_pDelegate->OnBufferProcess(cbDataBuffer, nBytes);
		}
		// DO sone thing Close();
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CClientSocket::IsConnected()
{
	return m_isConnected;
}


void CClientSocket::OnSend(int nErrorCode)
{
	OutputEx(_T("Send Data Success"));
	CAsyncSocket::OnSend(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	OutputEx(_T("CClientSocket::OnClose"));

	CAsyncSocket::OnClose(nErrorCode);

	m_isConnected = FALSE;

	m_pDelegate->OnClose(nErrorCode);
}

void CClientSocket::Close()
{
	m_isConnected = FALSE;
	
	CAsyncSocket::Close();
	
	OutputEx(_T("CClientSocket::Close"));
}

void CClientSocket::SendBuffer(const char* pBuffer, size_t len)
{
	int nByte = Send(pBuffer, len);
	if (nByte == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			OutputEx(_T("CClientSocket::Send Failed. Error - %d") + ToString(WSAGetLastError()));
			Close();
		}
	}
}

/////////////////////////////////////////////////////////////////

