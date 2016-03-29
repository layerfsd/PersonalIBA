// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"

#include "ClientSocket.h"

using namespace NS_ISERVER;

// CClientSocket

CClientSocket::CClientSocket(IBufferProcess* pBufferProcess)
: m_cbRecv(0), m_isConnected(FALSE), m_pBufferProcess(pBufferProcess)
{
	m_recvBuf.SetSize(m_pBufferProcess->GetBufferSize());
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 成员函数

void CClientSocket::OnConnect(int nErrorCode)
{
	ClearSendQueue();

	if (nErrorCode)
	{
		//Output("Connect Failed - " + ToString(nErrorCode));
		
	}
	else 
	{
		m_isConnected = TRUE;
		m_pBufferProcess->OnConnectionEstablished();
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::ClearSendQueue()
{
	for(SendBufferQueue::iterator pos = m_sendQueue.begin();
		pos != m_sendQueue.end(); ++pos)
	{
		delete *pos;
	}
	m_sendQueue.clear();
}

void CClientSocket::OnReceive(int nErrorCode)
{
	int nBytes = Receive(m_recvBuf.GetData() + m_cbRecv, (INT)m_recvBuf.GetSize() - m_cbRecv);

	if (nBytes == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			//Output(_T("OnReceive Error - ") + ToString(WSAGetLastError()));
			Close();
		}
	}
	else if (nBytes == 0)
	{
		//Output(_T("OnReceive - Server Disconnected."));
		Close();
	}
	else
	{
		m_cbRecv += nBytes;
		if ((size_t)m_cbRecv >= m_pBufferProcess->GetMinMessageSize())
		{
			if (!m_pBufferProcess->ProcessBuffer(m_recvBuf.GetData(), (size_t*)&m_cbRecv))
			{
				Close();
			}
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CClientSocket::IsConnected()
{
	return m_isConnected;
}


void CClientSocket::OnSend(int nErrorCode)
{
	//Output("CClientSocket::OnSend\n");

	while(!m_sendQueue.empty())
	{
		CByteArray *pSendBuffer = m_sendQueue.front();

		int nByte = Send(pSendBuffer->GetData(), (INT)pSendBuffer->GetSize());
		if (nByte == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				return;
			}
			else
			{
				//Output("Send Failed. Error - \n" + ToString(WSAGetLastError()));
				Close();
				return;
			}
		}
		else
		{
			if (nByte < pSendBuffer->GetSize())
			{
				pSendBuffer->RemoveAt(0, nByte);
			}
			else
			{
				m_sendQueue.pop_front();
				delete pSendBuffer;
			}
		}
	}

	CAsyncSocket::OnSend(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode) 
{
	CAsyncSocket::OnClose(nErrorCode);

    m_isConnected = FALSE;

	m_pBufferProcess->OnClose();
}

void CClientSocket::Close()
{
	m_isConnected = FALSE;

	CAsyncSocket::Close();
}

void CClientSocket::SendBuffer(const byte* pBuffer, size_t len)
{
	CByteArray *pSendBuffer = new CByteArray;
	
	pSendBuffer->SetSize(len);
	memcpy(pSendBuffer->GetData(), pBuffer, len);

	m_sendQueue.push_back(pSendBuffer);

	while (!m_sendQueue.empty())
	{
		pSendBuffer = m_sendQueue.front();

		int nByte = Send(pSendBuffer->GetData(), (int)pSendBuffer->GetSize());
		if (nByte == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				break;
			}
			else
			{
				//Output(_T("Send Failed. Error - ") + ToString(WSAGetLastError()));
				Close();
				break;
			}
		}
		else
		{
			if (nByte < pSendBuffer->GetSize())
			{
				pSendBuffer->RemoveAt(0, nByte);
			}
			else
			{
				m_sendQueue.pop_front();
				pSendBuffer->RemoveAll();
				delete pSendBuffer;
			}
		}
	}
}
