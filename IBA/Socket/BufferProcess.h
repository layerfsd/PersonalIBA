#pragma once

#include "..\..\..\NBBS\NetComm\Communication.h"

#include <map>


namespace NS_ISERVER
{
//////////////////////////////////////////////////////////////////////////


class IBufferProcess
{
public:
	virtual bool ProcessBuffer(BYTE *pBuffer, size_t* pBufLen) = 0;
	virtual size_t GetMinMessageSize() = 0;
	virtual size_t GetBufferSize() = 0;
	virtual void OnConnectionEstablished() = 0;
	virtual void OnClose() = 0;
};

class ICommandProcessor
{
public:
	virtual bool Process(BYTE *pBuffer, size_t bufferLen) = 0;
	virtual void OnConnectionEstablished() = 0;
	virtual void OnClose() = 0;
};

//////////////////////////////////////////////////////////////////////////

class CBufferProcess : public IBufferProcess
{

public:
	CBufferProcess(size_t BufferSize, ICommandProcessor *commandProcessorp);
	~CBufferProcess(void);

	virtual void OnConnectionEstablished();

	virtual void OnClose();

	virtual bool ProcessBuffer(BYTE *pBuffer, size_t* pBufLen);

	virtual size_t GetMinMessageSize(); 

	virtual size_t GetBufferSize();

private:

	ICommandProcessor * const commandProcessorp_;
	const size_t m_BufferSize;

private:
	
	CBufferProcess(const CBufferProcess& rhs);
	CBufferProcess& operator=(const CBufferProcess& rhs);
};

//////////////////////////////////////////////////////////////////////////
}

