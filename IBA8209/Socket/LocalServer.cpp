#include "stdafx.h"
#include "localserver.h"
#include "..\IBA.h"
#include "..\Dal\IBADAL.h"
#include "..\RealName\IZd.h"


SINGLETON_IMPLEMENT(CLocalServer)

CLocalServer::CLocalServer()
: m_lpCommandProcessor(NULL), m_lpBufferProcessor(NULL), m_lpSocket(NULL)
, m_CheckInStatus(0)
{
	InitLocalServer();
}

CLocalServer::~CLocalServer(void)
{
	L2_SAFE_DELETE(m_lpSocket)
	L2_SAFE_DELETE(m_lpBufferProcessor)
	L2_SAFE_DELETE(m_lpCommandProcessor)
}

void CLocalServer::SetComputerListView(CComputerListView* newVal)
{
	ASSERT(newVal);
	m_lpComputerListView = newVal;
}

void CLocalServer::SetActiveMemberView(CActiveMemberView* newVal)
{
	m_lpActiveMemberView = newVal;
}

void CLocalServer::SetSeatLayoutView(CSeatLayoutView* newVal)
{
	m_lpSeatLayoutView = newVal;
}

CComputerListView* CLocalServer::GetComputerListView() const
{
	ASSERT(m_lpComputerListView);
	return m_lpComputerListView;
}

void CLocalServer::InitLocalServer()
{
	m_lpCommandProcessor = new CCommandProcessor(this);
	m_lpBufferProcessor = new CBufferProcess(BODY_BUFFER_LEN, m_lpCommandProcessor);
	m_lpSocket = new CClientSocket(m_lpBufferProcessor);

	ASSERT(m_lpCommandProcessor);
	ASSERT(m_lpBufferProcessor);
	ASSERT(m_lpSocket);
}

BOOL CLocalServer::ConnectServer(void)
{
	ASSERT(m_lpComputerListView);

	if (NULL == m_lpComputerListView)
	{
		return FALSE;
	}
	static const UINT ServerPort = 7879;
	IBA_TRACE(_T("连接服务器-CLocalServer::ConnectServer(void), ServerIP:%s"),CNetBarConfig::GetInstance()->GetLocalServerIP(), ServerPort);
	
	

	m_lpSocket->Close();
	m_lpSocket->Create();

	CIBALog::GetInstance()->WriteFormat(_T("ServerIP:%s, ServerPort:%d"), 
		CNetBarConfig::GetInstance()->GetLocalServerIP(), ServerPort);

	BOOL bRet = m_lpSocket->Connect(CNetBarConfig::GetInstance()->GetLocalServerIP(), ServerPort);

	if (m_lpSocket->GetLastError() != WSAEWOULDBLOCK)
	{
		CIBALog::GetInstance()->WriteFormat(_T("Connect failed. GetLastError:%d"), m_lpSocket->GetLastError());
	}

	return bRet;
}

BOOL CLocalServer::IsConnected()
{
	BOOL bRet = FALSE;

	if (m_lpSocket != NULL)
	{
		bRet = m_lpSocket->IsConnected();
	}

	return bRet;
}

void CLocalServer::SendBuffer(const NET_HEAD_MAN* pHead, const void* pBuf, BOOL bCheckStatus)
{
	if (bCheckStatus && m_CheckInStatus != 1)
	{
		return;
	}

	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD_MAN + pHead->Length);

	memcpy(buffer.GetData(), pHead, sizeof NET_HEAD_MAN);

	if (pBuf != NULL)
	{
		memcpy(buffer.GetData() + sizeof NET_HEAD_MAN, pBuf, pHead->Length);
	}

	m_lpSocket->SendBuffer(buffer.GetData(), buffer.GetSize());
	buffer.RemoveAll();
}

void CLocalServer::LocalLogin()
{
	CLIENT_LOGIN loginData = {0};
	NET_HEAD_MAN head = {0};
	
	head.Version = COM_VER;
	head.Length = sizeof(loginData);
	head.Cmd = C_MANAGER_LOGIN;

	memcpy(loginData.ClientName, "IBA", LEN_USERNAME);
	memcpy(loginData.cComputerName, "ZZS", MAX_COMPUTERNAME_LENGTH);

	SendBuffer(&head, &loginData);
}

void CLocalServer::LocalCheckin()
{
	MANAGER_CHECKIN checkin = {0};

	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof(checkin);
	head.Cmd = C_MANAGER_CHECKIN;

	memcpy(checkin.password, "202CB962AC59075B964B07152D234B70", LEN_MONITORPASSWORD);

	CStringA strVer = CW2A(CIBALog::GetInstance()->GetAppVer());

	memcpy(checkin.version, (LPCSTR)strVer, strVer.GetLength());

	SendBuffer(&head, &checkin, FALSE);
}

BOOL CLocalServer::LocalPing()
{
	BOOL bRet = TRUE;

	if (m_lpSocket->IsConnected())
	{
		NET_HEAD_MAN head = {0};
		head.Cmd = C_PING;
		head.Version = COM_VER;
		head.Length = 0;

		SendBuffer(&head, NULL, FALSE);
	}
	else
	{
		if (m_CheckInStatus != -1)
		{
			bRet = ConnectServer();
		}
	}

	return bRet;
}

void CLocalServer::LocalGetComputerList()
{
	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = 0;
	head.Cmd = C_MANAGER_GETCOMPUTLIST;
	SendBuffer(&head);
}

void CLocalServer::LocalGetComputerCount()
{
	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = 0;
	head.Cmd = C_MANAGER_GET_COMPUTER_COUNT;
	SendBuffer(&head);
}

void CLocalServer::LocalGetMemberClass()
{
	NET_HEAD_MAN head = {0};

	head.Version = COM_VER;
	head.Length = 0;
	head.Cmd = C_MANAGER_GET_MEMBER_CLASSES;

	SendBuffer(&head);
}

void CLocalServer::LocalGetIP2TermID()
{
	NET_HEAD_MAN head ={0};
	head.Cmd = C_MANAGER_GET_IP2TERMID;
	head.Version = COM_VER;
	head.Length = 0;

	SendBuffer(&head);
}

void CLocalServer::LocalGetPCClassName()
{
	NET_HEAD_MAN head = {0};
	head.Cmd = C_MANAGER_GET_PCCLASSNAME;
	head.Version = COM_VER;
	head.Length = 0;
	SendBuffer(&head);
}

void CLocalServer::LocalGetPCClassMap()
{
	NET_HEAD_MAN head = {0};
	head.Cmd = C_MANAGER_GET_PCCLASSMAP;
	head.Version = COM_VER;
	head.Length = 0;
	SendBuffer(&head);
}


void CLocalServer::LocalPingClient(INT_PTR nIndex)
{
	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	CStringA strTmp = CW2A(computer.GetTerminalID());

	NET_HEAD_MAN head = {0};

	head.Version = COM_VER;
	head.Length = strTmp.GetLength();
	head.Cmd = C_MANAGER_PING_CLIENT;

	SendBuffer(&head, strTmp.GetBuffer());
}

void CLocalServer::LocalCaptureScreen(INT_PTR nIndex)
{
	NET_HEAD_MAN head = {0};
	SCREEN_TERMINFO snapComputer = {0};

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	head.Version = COM_VER;
	head.Length = sizeof(snapComputer);
	head.Cmd = C_CAPTURE_SCREEN;

	m_lpCommandProcessor->m_bCaptureScreen = TRUE;
	StartTimer(2, 10 * 1000, TRUE);

	SendBuffer(&head, &snapComputer);

}

void CLocalServer::LocalGetLog(INT_PTR nIndex)
{
	NET_HEAD_MAN head = {0};
	SCREEN_TERMINFO snapComputer = {0};

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	head.Version = COM_VER;
	head.Length = sizeof(snapComputer);
	head.Cmd = C_GET_LOG;

	m_lpCommandProcessor->m_bGetLog = TRUE;
	StartTimer(1, 10 * 1000, TRUE);

	SendBuffer(&head, &snapComputer);
}

void CLocalServer::LocalShutdownComputer(INT_PTR nIndex, BOOL bReboot)
{
	NET_HEAD_MAN head = {0};
	SCREEN_TERMINFO snapComputer = {0};

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	head.Version = COM_VER;
	head.Length = sizeof(snapComputer);
	head.Cmd = C_SHUTDOWNCOMPUTER;

	if (bReboot)
	{
		head.Sequence = 99; //表示重启
	}

	SendBuffer(&head, &snapComputer);
}

void CLocalServer::LocalShutdownAllComputer(BOOL bReboot)
{
	NET_HEAD_MAN head = {0};
	
	head.Version = COM_VER;
	head.Length = sizeof(SCREEN_TERMINFO);
	head.Cmd = C_SHUTDOWNCOMPUTER;

	if (bReboot)
	{
		head.Sequence = 99; //表示重启
	}

	for (INT i = 0; i < ComputerList.GetCount(); i++)
	{
		CComputerInfo& computer = ComputerList.GetComputer(i);

		SCREEN_TERMINFO snapComputer = {0};

		_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
		_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));
		
		SendBuffer(&head, &snapComputer);
	}
}

void CLocalServer::LocalStartupComputer(INT_PTR nIndex)
{
	NET_HEAD_MAN head = {0};
	SCREEN_TERMINFO snapComputer = {0};

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	head.Version = COM_VER;
	head.Length = sizeof(snapComputer);
	head.Cmd = C_STARTUPCOMPUTER;

	IBA_LOG(_T("ID= %s, IP=%s, MemberID=%d, 开机"), 
		computer.GetTerminalID(), computer.GetComputerIP(), computer.GetMemberID());

	SendBuffer(&head, &snapComputer);

}

void CLocalServer::LocalForceUpdateData(INT_PTR nIndex)
{
	NET_HEAD_MAN head = {0};
	SCREEN_TERMINFO snapComputer = {0};

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	head.Version = COM_VER;
	head.Length = sizeof(snapComputer);
	head.Cmd = C_MANGER_FORCEUPDATEDATA;

	SendBuffer(&head, &snapComputer);

}

void CLocalServer::LocalSendGroupShortMessage(INT nPCClass, CString& strMsg)
{
	GROUP_SHORTMESSAGE groupShortMessage = {0};

	groupShortMessage.shortMsg.ID = InvalidateSMID;
	groupShortMessage.shortMsg.counterID = CIBAConfig::GetInstance()->GetCashRegisterID();

	strMsg.Trim();
	strMsg.Replace(_T("'"), _T("''"));

	_snprintf(groupShortMessage.shortMsg.message, LEN_SHORTMESSAGE, CW2A(strMsg));

	groupShortMessage.shortMsg.side = 1;
	groupShortMessage.shortMsg.status = 0;
	groupShortMessage.shortMsg.messageType = 1;
	groupShortMessage.pcClassID = nPCClass;

	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	CStringA strTime;
	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d", st.wYear,
		st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	_snprintf(groupShortMessage.shortMsg.sendTime, LEN_DATETIME, strTime);

	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof(groupShortMessage);
	head.Cmd = C_MANAGER_GROUPSHORTMESSAGE;

	SendBuffer(&head, &groupShortMessage);
}

void CLocalServer::LocalSendShortMessage(INT_PTR nIndex, CString& strMsg)
{
	if (ComputerList.IsInvaildIndex(nIndex))
	{
		return;
	}

	CIBALog::GetInstance()->Write(strMsg);
	
	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	if (!computer.HasUser())
	{
		return;
	}

	SHORT_MESSAGE shortMsg = {0};

	shortMsg.ID = 1;
	shortMsg.orgID = InvalidateSMID;

	_snprintf(shortMsg.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(shortMsg.computerIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	shortMsg.counterID = CIBAConfig::GetInstance()->GetCashRegisterID(); 
	shortMsg.memberID = computer.GetMemberID();
	shortMsg.side = 1;
	shortMsg.status = 0;
	shortMsg.messageType = 0;
	
	strMsg.Trim();
	strMsg.Replace(_T("'"), _T("''"));
	
	_snprintf(shortMsg.message, LEN_SHORTMESSAGE, CW2AEX<256>(strMsg));
	
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	CStringA strTime;
	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d", st.wYear,
		st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	_snprintf(shortMsg.sendTime, LEN_DATETIME, strTime);

	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof(shortMsg);
	head.Cmd = C_MANAGER_SHORTMESSAGE;

	SendBuffer(&head, &shortMsg);
}


void CLocalServer::LocalForceCheckOut(INT_PTR nIndex, CString strMsg)
{
	if (ComputerList.IsInvaildIndex(nIndex))
	{
		return;
	}

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	if (!computer.HasUser())
	{
		return;
	}

	SHORT_MESSAGE shortMsg = {0};

	shortMsg.ID = 1;
	shortMsg.orgID = InvalidateSMID;

	_snprintf(shortMsg.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(shortMsg.computerIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	shortMsg.counterID = CIBAConfig::GetInstance()->GetCashRegisterID(); 
	shortMsg.memberID = computer.GetMemberID();
	shortMsg.side = 1;
	shortMsg.status = 0;
	shortMsg.messageType = 0;

	strMsg.Trim();
	strMsg.Replace(_T("'"), _T("''"));

	_snprintf(shortMsg.message, LEN_SHORTMESSAGE, CW2AEX<256>(strMsg));

	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	CStringA strTime;
	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d", st.wYear,
		st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	_snprintf(shortMsg.sendTime, LEN_DATETIME, strTime);

	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof(shortMsg);
	head.Cmd = C_MANAGER_SHORTMESSAGE;
	head.Sequence = 999;
	SendBuffer(&head, &shortMsg);
}


void CLocalServer::LocalSendShortMessageByMemberID(UINT nMemberID, CString& strMsg)
{
	INT_PTR nIndex = ComputerList.Find(nMemberID);

	if (nIndex >= 0)
	{
		LocalSendShortMessage(nIndex, strMsg);
	}
}

void CLocalServer::LocalCheckOutUser(INT_PTR nIndex, UINT nMemberID, UINT nType)
{
	NET_HEAD_MAN head = {0};//下机数据

	head.Version = COM_VER;
	head.Length = sizeof(tag_refund_logout);
	head.Cmd = C_MANGER_REFUND_LOGOUT;

	tag_refund_logout req = {0};//登出标志

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	_snprintf(req.TermID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(req.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));
	
	//nShutDown = 1是换机，0是普通
	req.nShutDown = nType;
	req.nMemberID = nMemberID;

	IBA_LOG(_T("ID= %s, IP=%s, MemberID=%d, 动作:%s"), 
		computer.GetTerminalID(), computer.GetComputerIP(), computer.GetMemberID(),
		(nType == 1) ? _T("换机") : _T("普通"));

	SendBuffer(&head, &req);//报文发送
}

void CLocalServer::LocalCheckOutAllUser()
{
	NET_HEAD_MAN head = {0};

	head.Version = COM_VER;
	head.Length = sizeof(tag_refund_logout);
	head.Cmd = C_MANGER_REFUND_LOGOUT;

	for (INT i = 0; i < ComputerList.GetCount(); i++)
	{
		CComputerInfo& computer = ComputerList.GetComputer(i);

		tag_refund_logout req = {0};//登出标志

		req.nShutDown = 0;
		req.nMemberID = computer.GetMemberID();

		_snprintf(req.TermID, LEN_TERMID, CW2A(computer.GetTerminalID()));
		_snprintf(req.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

		SendBuffer(&head, &req);
	}

}

void CLocalServer::LocalBundleTime(UINT nMemberID, UINT nBundleTimeId)
{
	INT_PTR nIndex = ComputerList.Find(nMemberID);

	if (nIndex >= 0)
	{
		SCREEN_TERMINFO snapComputer = {0};

		CComputerInfo& computer = ComputerList.GetComputer(nIndex);

		_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
		_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

		NET_HEAD_MAN head = {0};

		head.Version = COM_VER;
		head.Length = sizeof SCREEN_TERMINFO;
		head.Cmd = C_MANAGER_BUNDLETIME;
		head.Sequence = nBundleTimeId;

		SendBuffer(&head, &snapComputer);//报文发送
	}
}

void CLocalServer::LocalRestartServer()
{
	NET_HEAD_MAN head = {0};
	head.Cmd = C_MANAGER_RESTART_REQUEST;
	head.Version = COM_VER;
	head.Length = 0;
	SendBuffer(&head);
}

void CLocalServer::OnTimer(const UINT uTimerID)
{
	if (uTimerID == 1)
	{
		m_lpCommandProcessor->m_bGetLog = FALSE;
	}
	else if (uTimerID == 2)
	{
		m_lpCommandProcessor->m_bCaptureScreen = FALSE;
	}

	CIBALog::GetInstance()->Write(_T("CLocalServer::OnTimer"));
}


void CLocalServer::LocalAutoCheckIn(INT_PTR nIndex)
{
	if (ComputerList.IsInvaildIndex(nIndex))
	{
		return;
	}

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);
	
	SCREEN_TERMINFO snapComputer = {0};

	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	NET_HEAD_MAN head = {0};

	head.Version = COM_VER;
	head.Length = sizeof SCREEN_TERMINFO;
	head.Cmd = C_MANGER_AUTOCHECKIN;

	SendBuffer(&head, &snapComputer);//报文发送
}

void CLocalServer::LocalUnlockScreen(INT_PTR nIndex, CString strUnlockPwd)
{
	if (m_CheckInStatus != 1)
	{
		return;
	}

	if (ComputerList.IsInvaildIndex(nIndex))
	{
		return;
	}

	CComputerInfo& computer = ComputerList.GetComputer(nIndex);

	SCREEN_TERMINFO snapComputer = {0};
	_snprintf(snapComputer.termID, LEN_TERMID, CW2A(computer.GetTerminalID()));
	_snprintf(snapComputer.termIP, LEN_IPADDR, CW2A(computer.GetComputerIP()));

	NET_HEAD_MAN head = {0};

	head.Version = COM_VER;
	head.Length = sizeof SCREEN_TERMINFO + sizeof CHECK_PASSWORD;
	head.Cmd = C_MANGER_UNLOCKSCREEN;

	CHECK_PASSWORD pwd = {0};

	_snprintf(pwd.cPassword, LEN_CLIPASSWORD, CW2A(strUnlockPwd));
	
	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD_MAN + head.Length);

	memcpy(buffer.GetData(), &head, sizeof NET_HEAD_MAN);
	memcpy(buffer.GetData() + sizeof NET_HEAD_MAN, &snapComputer, sizeof SCREEN_TERMINFO);
	memcpy(buffer.GetData() + sizeof NET_HEAD_MAN + sizeof SCREEN_TERMINFO, &pwd, sizeof CHECK_PASSWORD);

	m_lpSocket->SendBuffer(buffer.GetData(), buffer.GetSize());

	buffer.RemoveAll();
}

void NS_ISERVER::CLocalServer::LocalAskNotice(BOOL bSigned)
{
	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof tag_ask_read_notice;
	head.Cmd = C_MANGER_ASKREADNOTICE;

	tag_ask_read_notice notice = {0};
	notice.nAsk = (UINT)bSigned;

	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD_MAN + head.Length);

	memcpy(buffer.GetData(), &head, sizeof NET_HEAD_MAN);
	memcpy(buffer.GetData() + sizeof NET_HEAD_MAN, &notice, sizeof tag_ask_read_notice);

	m_lpSocket->SendBuffer(buffer.GetData(), buffer.GetSize());

	buffer.RemoveAll();

	if (bSigned)
	{
		CIBALog::GetInstance()->Write(_T("发送通告签收成功"));
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("发送获取通告的请求"));
	}
	
}

void NS_ISERVER::CLocalServer::LocalAuditUserQuit(CString strTermID, CString strIP,UINT nMemberID)
{
	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof TIBAAuditUserQuit;
	head.Cmd = C_MANGER_AUDITUSERQUIT;

	TIBAAuditUserQuit info = {0};

	_snprintf(info.termID, LEN_TERMID, CT2A(strTermID));
	_snprintf(info.termIP, LEN_IPADDR, CT2A(strIP));
	info.memberID=nMemberID;

	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD_MAN + head.Length);

	memcpy(buffer.GetData(), &head, sizeof NET_HEAD_MAN);
	memcpy(buffer.GetData() + sizeof NET_HEAD_MAN, &info, sizeof TIBAAuditUserQuit);

	m_lpSocket->SendBuffer(buffer.GetData(), buffer.GetSize());

	buffer.RemoveAll();

	IBA_LOG(_T("发送实名下机信息,TermID=%s,IP=%s,MemberID=%d"),strTermID,strIP,nMemberID);
}

void NS_ISERVER::CLocalServer::LocalLoginZdReturn(ID_LOGINRETURN_ZD *pLogin_zd )
{
	NET_HEAD_MAN head = {0};
	head.Version = COM_VER;
	head.Length = sizeof ID_LOGINRETURN_ZD;
	head.Cmd = C_LOGIN_ZD;

	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD_MAN + head.Length);

	memcpy(buffer.GetData(), &head, sizeof NET_HEAD_MAN);
	memcpy(buffer.GetData() + sizeof NET_HEAD_MAN, pLogin_zd, sizeof ID_LOGINRETURN_ZD);

	m_lpSocket->SendBuffer(buffer.GetData(), buffer.GetSize());

	buffer.RemoveAll();

	IBA_LOG0(_T("发送智多实名登录"));
}
