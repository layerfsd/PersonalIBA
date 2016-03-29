
#include "stdafx.h"
#include "WinSocket.h"

CWinSocket::CWinSocket() 
: m_hSocket(NULL)
{
	memset(&m_sockaddr, 0, sizeof(sockaddr_in));
}

CWinSocket::~CWinSocket()
{
}

BOOL CWinSocket::Create(void)
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_hSocket)
	{
		m_hSocket = NULL;

		return FALSE;
	}

	return TRUE;
}

BOOL CWinSocket::Close(void)
{
	if (closesocket(m_hSocket) == SOCKET_ERROR)
	{
		return FALSE;
	}

	m_hSocket = NULL;

	memset(&m_sockaddr, 0, sizeof(sockaddr_in));

	return TRUE;
}

BOOL CWinSocket::Connect(LPCTSTR lpszIP, USHORT nPort)
{
	char szIP[16] = {0};

#ifdef _UNICODE
	lstrcpyA(szIP, CW2A(lpszIP));
#else
	lstrcpyA(szIP, lpszIP);
#endif

	hostent* hostEnt = gethostbyname(szIP);

	if (hostEnt != NULL)
	{
		long lIPAddress = ((in_addr*)hostEnt->h_addr)->s_addr;
		m_sockaddr.sin_addr.s_addr = lIPAddress;
	}
	else
	{
		m_sockaddr.sin_addr.s_addr = inet_addr(szIP);
	}

	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(nPort);

	if (connect(m_hSocket, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWinSocket::Connect(LPCTSTR lpszIP, USHORT nPort, LONG nTimeout)
{
	//设置非阻塞方式连接
	unsigned long lMode = 1;
	
	if (ioctlsocket(m_hSocket, FIONBIO, &lMode) == SOCKET_ERROR)
	{
		return FALSE;
	}

	Connect(lpszIP, nPort);

	//设置连接超时
	timeval timeout = {0};
	timeout.tv_sec = nTimeout;
	timeout.tv_usec = 0;

	fd_set r = {0};
	r.fd_count = 1;
	r.fd_array[0] = m_hSocket;

	if (select(0, &r, &r, 0, &timeout) <= 0)
	{
		return FALSE;
	}

	//恢复成阻塞模式
	lMode = 0;

	if (ioctlsocket(m_hSocket, FIONBIO, &lMode) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWinSocket::Send(LPVOID lpData, INT nLen)
{
	if (send(m_hSocket, (const char*)lpData, nLen, 0) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWinSocket::Receive(LPVOID lpData, INT& nLen)
{
	INT nRet = recv(m_hSocket, (char*)lpData, nLen, 0);

	if (nRet == SOCKET_ERROR)
	{
		nLen = 0;

		return FALSE;
	}

	nLen = nRet;

	return TRUE;
}

BOOL CWinSocket::SetSendTimeout(UINT nMilliseconds)
{
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nMilliseconds, sizeof(nMilliseconds)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWinSocket::SetRecvTimeout(UINT nMilliseconds)
{
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nMilliseconds, sizeof(nMilliseconds)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}
