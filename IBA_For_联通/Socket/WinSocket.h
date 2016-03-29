
#pragma once

#include <winsock.h>

class CWinSocket
{
public:
	CWinSocket();
	~CWinSocket();

	BOOL Create(void);
	
	BOOL Connect(LPCTSTR lpszIP, USHORT nPort);
	BOOL Connect(LPCTSTR lpszIP, USHORT nPort, LONG nTimeout);
	BOOL Close(void);

	BOOL Send(LPVOID lpData, INT nLen);	
	BOOL Receive(LPVOID lpData, INT& nLen);
								
	BOOL SetSendTimeout(UINT nMilliseconds);	
	BOOL SetRecvTimeout(UINT nMilliseconds);
	
	SOCKET GetSafeSocket() const { return m_hSocket; }

protected:
	
	SOCKET m_hSocket;

	struct sockaddr_in m_sockaddr;

};

