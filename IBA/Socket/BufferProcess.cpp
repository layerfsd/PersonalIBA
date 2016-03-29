#include "StdAfx.h"
#include "bufferprocess.h"
#include "..\IBA.h"

using namespace NS_ISERVER;

//////////////////////////////////////////////////////////////////////////
//CBufferProcess
//////////////////////////////////////////////////////////////////////////

CBufferProcess::CBufferProcess(size_t BufferSize, ICommandProcessor *commandProcessorp)
: m_BufferSize(BufferSize), commandProcessorp_(commandProcessorp)
{
	
}

CBufferProcess::~CBufferProcess(void)
{
}

size_t CBufferProcess::GetMinMessageSize()
{
	return sizeof(NET_HEAD_MAN);
}

bool CBufferProcess::ProcessBuffer(BYTE *pBuffer, size_t* pBufLen)
{
	NET_HEAD_MAN *pHead = reinterpret_cast<NET_HEAD_MAN*>(pBuffer);
	UINT packLen = pHead->Length + sizeof(NET_HEAD_MAN);

	if (packLen > m_BufferSize)
	{
		return false;
	}

	//try
	//{
	while ((*pBufLen) >= packLen)
	{
		commandProcessorp_->Process(pBuffer, packLen);

		memmove(pBuffer, pBuffer + packLen, *pBufLen - packLen);
		*pBufLen -= packLen;
		if ((*pBufLen) > GetMinMessageSize())
		{
			NET_HEAD_MAN *pHead = reinterpret_cast<NET_HEAD_MAN*>(pBuffer);
			packLen = pHead->Length + sizeof(NET_HEAD_MAN);
		}
		else break;
	}
	//}
	//catch(...)
	//{
	//	CIBALog::GetInstance()->WriteFormat(_T("CBufferProcess::ProcessBuffer Error:command - %d"), pHead->Cmd);
	//}

	return true;
}

void CBufferProcess::OnConnectionEstablished()
{
	commandProcessorp_->OnConnectionEstablished();
}

void CBufferProcess::OnClose()
{
	commandProcessorp_->OnClose();
}

size_t CBufferProcess::GetBufferSize()
{
	return m_BufferSize;
}

