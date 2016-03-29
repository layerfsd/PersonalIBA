#pragma once

#include <list>
#include "CommandProcessor.h"

namespace NS_ISERVER
{

//////////////////////////////////////////////////////////////////////////


class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket(IBufferProcess *pBufferProcess);

	virtual ~CClientSocket();

	BOOL IsConnected();

	void Close();

	void SendBuffer(const byte* pBuffer, size_t len);
		
private:

	virtual void OnConnect(int nErrorCode);

	virtual void OnReceive(int nErrorCode);

	virtual void OnSend(int nErrorCode);

	virtual void OnClose(int nErrorCode);
	
	void ClearSendQueue();

private:

	CByteArray m_recvBuf;

	typedef std::list<CByteArray*> SendBufferQueue;

	SendBufferQueue m_sendQueue;

	int m_cbRecv;

	BOOL m_isConnected;

	IBufferProcess * const m_pBufferProcess;

private:
	
	CClientSocket& operator=(const CClientSocket& rhs);
	CClientSocket(const CClientSocket& rhs);
};

//////////////////////////////////////////////////////////////////////////


}


