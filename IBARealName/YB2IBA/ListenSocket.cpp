// ListenSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "YB2IBA.h"
#include ".\listensocket.h"


// CListenSocket

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket 成员函数

void CListenSocket::OnAccept(int nErrorCode)
{
	AfxGetMainWnd()->SendMessage(WM_SOCKETNOTIFY, 1);

	CAsyncSocket::OnAccept(nErrorCode);
}
