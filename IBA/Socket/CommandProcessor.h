#pragma once

#include "bufferprocess.h"

namespace NS_ISERVER
{

class CLocalServer;

class CCommandProcessor : public ICommandProcessor
{
public:
	
	friend class CLocalServer;

	CCommandProcessor(CLocalServer* lpLocalServer);

	~CCommandProcessor();

	virtual bool Process(BYTE *pBuffer, size_t bufferLen);

	/**
	*连接确认事件
	*/
	virtual void OnConnectionEstablished();

	virtual void OnClose();

private:

	CCommandProcessor(const CCommandProcessor& rhs);
	CCommandProcessor& operator=(const CCommandProcessor& rhs);

private:

	typedef void (CCommandProcessor::*pfnCommandHandle)(BYTE*, size_t);
	
	typedef std::map<unsigned char, pfnCommandHandle> CommandHandleList;

	CommandHandleList handleList_;

	void AddHandles();

	void AddHandle(unsigned char command, pfnCommandHandle commandHandle);

	void DefaultProcess(BYTE *pBuffer, size_t bufferLen);

	unsigned char GetCommand(BYTE *pBuffer, size_t bufferLen);

	void GetHead(BYTE *pBuffer, NET_HEAD_MAN *pHead);

	void GetBody(BYTE *pBuffer, void *pBody, size_t bodyLen);

	CLocalServer* const m_lpLocalServer;
	
	std::map<CString, UINT> m_PCClassMap;   //终端机器与区域ID的集合

	CString m_strPCClassMap;
	
	CStringArray m_QuestionComputerList;

	void DecodeQuestionComputer();

	void SaveFile(CString strFile, CByteArray &buf);

	BOOL m_bGetLog;
	BOOL m_bCaptureScreen;

private:
	
	void OnCheckinResp(BYTE *pBuffer, size_t bufLen);
	void OnLogin(BYTE *pBuffer, size_t bufLen);
	void OnClientLogin(BYTE *pBuffer, size_t bufLen);
	void OnGetComputerList(BYTE *pBuffer, size_t bufLen);
	void OnClientCheckout(BYTE *pBuffer, size_t bufLen);
	void OnClientDisconnect(BYTE *pBuffer, size_t bufLen);
	void OnClientUpdateData(BYTE *pBuffer, size_t bufLen);
	void OnClientShortMessage(BYTE *pBuffer, size_t bufLen);
	void OnQuestionComputer(BYTE *pBuffer, size_t bufLen);
	void OnGetComputerCount(BYTE *pBuffer, size_t bufLen);
	void OnTermUnLock(BYTE *pBuffer, size_t bufLen);
	void OnSendShortMsgResp(BYTE *pBuffer, size_t bufLen);
	void OnRespondCaptureScreen(BYTE *pBuffer, size_t bufLen);
	void OnRespondGetLog(BYTE *pBuffer, size_t bufLen);
	void OnTermIDPCClassMap(BYTE *pBuffer, size_t bufLen);
	void OnPingClientResp(BYTE *pBuffer, size_t bufLen);
	//void OnQuestionAmount(BYTE *pBuffer, size_t bufLen);

	// 2011/08/04-8201-gxx: 
	void OnAutoReturnResp(BYTE *pBuffer, size_t bufLen);

	// 2011/09/08-8201-gxx: 
	void OnGetNewNotice(BYTE *pBuffer, size_t bufLen);
	
	// 2014-9-28 - qsc
	void OnLoingZd(BYTE *pBuffer, size_t bufLen);
};

}