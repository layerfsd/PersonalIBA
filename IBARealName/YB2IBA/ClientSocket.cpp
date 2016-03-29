// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "YB2IBA.h"
#include ".\clientsocket.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 成员函数

void CClientSocket::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	AfxGetMainWnd()->SendMessage(WM_SOCKETNOTIFY, 2);

	CAsyncSocket::OnReceive(nErrorCode);
}
