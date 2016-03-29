#pragma once

#include <list>
#include <afxsock.h>
//////////////////////////////////////////////////////////////////////////
class CClientSocket;
class CDelegateBufferProcess;
class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();

	virtual ~CClientSocket();

	BOOL IsConnected();

	virtual void Close();

	void SendBuffer(const char* pBuffer, size_t len);
	
	void SetDelegate(CDelegateBufferProcess *pDelegate) { m_pDelegate = pDelegate; }
protected:

	virtual void OnConnect(int nErrorCode);

	virtual void OnReceive(int nErrorCode);

	virtual void OnSend(int nErrorCode);

	virtual void OnClose(int nErrorCode);

private:
	BOOL m_isConnected;
	CDelegateBufferProcess *m_pDelegate;
private:

	CClientSocket& operator=(const CClientSocket& rhs);
	CClientSocket(const CClientSocket& rhs);
};



//////////////////////////////////////////////////////////////////////////

