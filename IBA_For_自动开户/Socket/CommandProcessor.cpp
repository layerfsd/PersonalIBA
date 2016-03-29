
#include "StdAfx.h"
#include "CommandProcessor.h"
#include "LocalServer.h"
#include "..\IBA.h"
#include "..\ComputerListView.h"
#include "..\ActiveMemberView.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"
#include "..\tts\IBATTS.h"
#include "..\dal\IBADAL.h"
#include "..\MainFrm.h"
#include "..\OperationInfo.h"
#include "..\ActiveMember.h"

//////////////////////////////////////////////////////////////////////////
//CCommandProcessor
//////////////////////////////////////////////////////////////////////////

CCommandProcessor::CCommandProcessor(CLocalServer* lpLocalServer)
: m_lpLocalServer(lpLocalServer)
, m_bGetLog(FALSE), m_bCaptureScreen(FALSE)
{
	AddHandles();
}

CCommandProcessor::~CCommandProcessor()
{
}

void CCommandProcessor::AddHandle(unsigned char command, pfnCommandHandle commandHandle)
{
	handleList_.insert(std::make_pair(command, commandHandle));
}

void CCommandProcessor::AddHandles()
{
	AddHandle(S_RESPOND_COMPUT_LIST,        &CCommandProcessor::OnGetComputerList);
	AddHandle(S_RESPOND_LOGIN_SUCCEED,      &CCommandProcessor::OnLogin);
	AddHandle(S_RESPOND_CLIENT_LOGIN,       &CCommandProcessor::OnClientLogin);
	AddHandle(S_RESPOND_CLIENT_DISCONNECT,  &CCommandProcessor::OnClientDisconnect);
	AddHandle(S_RESPOND_CLIENT_UPDATEDATA,  &CCommandProcessor::OnClientUpdateData);
	AddHandle(S_RESPOND_CLIENT_CHECKOUT,    &CCommandProcessor::OnClientCheckout);
	AddHandle(S_RESPOND_MAN_CHECKIN,        &CCommandProcessor::OnCheckinResp);
	AddHandle(S_RESPOND_QUESTION_COMPUTER,  &CCommandProcessor::OnQuestionComputer);
	AddHandle(C_MANAGER_GET_COMPUTER_COUNT, &CCommandProcessor::OnGetComputerCount);
	AddHandle(C_MANAGER_GET_PCCLASSMAP,     &CCommandProcessor::OnTermIDPCClassMap);
	AddHandle(C_MANGER_MANUALUNLOCAL,       &CCommandProcessor::OnTermUnLock);
	AddHandle(S_RESPOND_CLIENT_SHORTMESSAGE,&CCommandProcessor::OnClientShortMessage);
	AddHandle(S_RESPOND_MAN_SHORTMSG,       &CCommandProcessor::OnSendShortMsgResp);
	AddHandle(S_RESPOND_CAPTURE_SCREEN,     &CCommandProcessor::OnRespondCaptureScreen);
	AddHandle(S_RESPOND_GET_LOG,            &CCommandProcessor::OnRespondGetLog);
	AddHandle(S_RESPOND_PING_CLIENT,        &CCommandProcessor::OnPingClientResp);
	AddHandle(S_NEWNOTICE_MSG,       &CCommandProcessor::OnGetNewNotice);
	//AddHandle(S_RESPOND_PONG, &CCommandProcessor::OnPing);
	//AddHandle(C_MANAGER_GET_PCCLASSNAME, &CCommandProcessor::OnGetPCClassName);
	//AddHandle(S_RESPOND_CHECK_MANAGER_PASSWORD, &CCommandProcessor::OnCheckManagerPwdResp);
	//AddHandle(S_RESPOND_SET_MANAGER_PASSWORD, &CCommandProcessor::OnSetManagerPwdResp);
	//AddHandle(S_RESPOND_GET_MEMBER_CLASSES, &CCommandProcessor::OnGetMemberClassesResp);
	
	//AddHandle(C_MANAGER_QUESTIIOIN_AMOUNT, &CCommandProcessor::OnQuestionAmount);
	//AddHandle(C_MANAGER_GET_IP2TERMID, &CCommandProcessor::OnGetIP2TermID);
	AddHandle(S_AUTO_RETURN_MSG,            &CCommandProcessor::OnAutoReturnResp);
}

void CCommandProcessor::DefaultProcess(BYTE* /*pBuffer*/, size_t /*bufferLen*/)
{
	//
}

unsigned char CCommandProcessor::GetCommand(BYTE *pBuffer, size_t /*bufferLen*/)
{
	NET_HEAD_MAN *headp_ = reinterpret_cast<NET_HEAD_MAN*>(pBuffer);

	return headp_->Cmd;
}

bool CCommandProcessor::Process(BYTE *pBuffer, size_t bufferLen)
{
	if (!pBuffer || bufferLen <= 0)
		return false;

	try
	{
		unsigned char command = GetCommand(pBuffer, bufferLen);

		if (handleList_.count(command) > 0) 
		{
			pfnCommandHandle pfnHandle = handleList_[command];
			(this->*pfnHandle)(pBuffer, bufferLen);
			return true;
		}
		else
		{
			DefaultProcess(pBuffer, bufferLen);		
			return false;
		}
	}
	catch (...) 
	{
		CIBALog::GetInstance()->Write(_T("CommandProcessor::Process - Unknown Exception"));
	}

	return false;
}

void CCommandProcessor::GetHead(BYTE *pBuffer, NET_HEAD_MAN *pHead)
{
	memcpy(pHead, pBuffer, sizeof(NET_HEAD_MAN));
}

void CCommandProcessor::GetBody(BYTE *pBuffer, void *pBody, size_t bodyLen)
{
	memcpy(pBody, pBuffer + sizeof(NET_HEAD_MAN), bodyLen);
}

void CCommandProcessor::OnConnectionEstablished()
{
	m_QuestionComputerList.RemoveAll();
	m_lpLocalServer->LocalCheckin();
}

void CCommandProcessor::OnClose()
{
	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
}

void CCommandProcessor::OnCheckinResp(BYTE *pBuffer, size_t bufLen)
{
	//NET_HEAD_MAN head = {0};
	MANAGER_CHECKIN_RESP resp = {0};

	//GetHead(pBuffer, &head);
	GetBody(pBuffer, (BYTE*)&resp, sizeof(resp));
	
	if (resp.isOk)
	{
		m_lpLocalServer->SetCheckInStatus(1);
		m_lpLocalServer->LocalLogin();

		m_lpLocalServer->LocalAskNotice();
	}
	else
	{
		m_lpLocalServer->SetCheckInStatus(-1);
	}
}

void CCommandProcessor::OnLogin(BYTE* pBuffer, size_t bufLen)
{
	const CString strMinVer(_T("3.1.1.4"));

	NET_HEAD_MAN head = {0};
	GetHead(pBuffer, &head);

	if (head.Length > 0)
	{
		MANAGER_LOGIN_RESP Login_Resp = {0};
		GetBody(pBuffer, &Login_Resp, sizeof Login_Resp);

		CString strVer = CA2W(Login_Resp.szServerVer);

		if (strVer >= strMinVer)
		{
			m_lpLocalServer->LocalGetPCClassMap();
			m_lpLocalServer->LocalGetComputerList();
			return;
		}
	}

	CString strTmp;

	strTmp.Format(IDS_ISERVERLOWVER, strMinVer);

	AfxMessageBox(strTmp, MB_ICONINFORMATION);
	
	TerminateProcess(GetCurrentProcess(), 0);
}


void CCommandProcessor::OnClientCheckout(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_CLIENT_DISCONNECT mcd = {0};

	GetBody(pBuffer, &mcd, sizeof(MANAGER_CLIENT_DISCONNECT));

	CString strTermID = CA2W(mcd.termID);
	CString strIP = CA2W(mcd.termIP);

	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);
	
	if (nIndex >= 0)
	{
		CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);
		computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
		
		//nCheckoutType：0换机，1临时下机，2会员下机，101临时虚拟冻结下机，102会员虚拟冻结下机
		CActiveMember ActiveMember; //用户没钱自动锁屏，就是离线了
		BOOL bGet = m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetMemberID(), ActiveMember);

		// 增加结账记录处理
		if(bGet)
		{
			// Checkout2执行成功，会清除用户视图的,只做刷新
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);	
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeVipReturn);
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetUserName(ActiveMember.GetUserName());
			OperationInfo.SetDescription(ActiveMember.GetRemark());

			// 2011/10/18-8208-gxx: 
			OperationInfo.SetClassId(ActiveMember.GetClassId());
			OperationInfo.SetTermId(ActiveMember.GetTerminalID());
			OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
			OperationInfo.SetIdNum(ActiveMember.GetPersonalID());


			//CLocalServer::GetInstance()->GetActiveMemberView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
		}

		if (0 == mcd.nCheckoutType)//用户换机了
		{
			if (bGet)
			{
				if (ActiveMember.GetAvailavleBalance() == 0) //没钱了
				{
					ActiveMember.SetTerminalID(_T(""));
					ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);//中心在线状态，为了加速查询
					m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);

					CClientShortMessage csm;

					CIBAString strBody;
					strBody.LoadString(IDS_NOMONEYLOCK);
					csm.SetMsgBody(strBody);
					csm.SetTermID(computer.GetTerminalID());
					csm.SetTermIP(computer.GetComputerIP());
					csm.SetRevTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
					csm.SetMemberId(computer.GetMemberID());

					CIBADAL::GetInstance()->AddClientShortMsg(csm);
				}
			}
		}
		else
		{
			// 2011/11/04-8210-gxx: 
			if (bGet)
			{
				if (ActiveMember.GetRoomMemberId() == 0)
				{
					m_lpLocalServer->ActiveMemberList.RemoveActiveMember(computer.GetMemberID());
				}
				else
				{
					// 包房用户不做清除
					ActiveMember.SetRoomMemberId(0);
					ActiveMember.SetTerminalID(_T("")); // 终端号置空
					ActiveMember.SetPCClassID(0);
					ActiveMember.SetRemark(LOAD_STRING(IDS_ROOM_HAS_REFUND));
					CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
				}
			}
			

			computer.ClearUserInfo();

			if (2 == mcd.nCheckoutType || 102 == mcd.nCheckoutType) //会员下机
			{
				CClientShortMessage csm;
	
				CIBAString strBody;
				strBody.LoadString(IDS_PAYOUTMEMBER);
				csm.SetMsgBody(strBody);
				csm.SetTermID(computer.GetTerminalID());
				csm.SetTermIP(computer.GetComputerIP());
				csm.SetRevTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
				csm.SetMemberId(computer.GetMemberID());

				CIBADAL::GetInstance()->AddClientShortMsg(csm);

				//((CMainFrame*)AfxGetMainWnd())->m_wndOutput.UpdateMsgLog();
			}
		}
		
		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);
		m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
		m_lpLocalServer->GetActiveMemberView()->Refresh(FALSE);
	}
}

void CCommandProcessor::OnClientLogin(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_COMPUT_INFO ci = {0};
	GetBody(pBuffer, &ci, sizeof(ci));

	CComputerInfo computer(ci);

	// 2011/06/10-gxx: 
	if (computer.GetComputerStatus() != CComputerInfo::ECS_ONLINE)
	{
		CActiveMember ActiveMember;
		if (m_lpLocalServer->ActiveMemberList.GetActiveMember( computer.GetTerminalID(), ActiveMember, TRUE))
		{
			computer.AttachActiveMember(ActiveMember);
		}	
	}

	m_lpLocalServer->ComputerList.AddComputer(computer);

	IBA_TRACE(_T("OnClientLogin = %s\r\n"), computer.GetTerminalID());

	for (INT i = 0; i < m_QuestionComputerList.GetCount(); i++)
	{
		if (m_QuestionComputerList.GetAt(i).Find(computer.GetTerminalID()) >= 0)
		{
			m_QuestionComputerList.RemoveAt(i);
		}
	}

	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);

	//重启挂机的情况
	if (computer.GetComputerStatus() == CComputerInfo::ECS_SUSPEND)
	{
		CActiveMember ActiveMember;

		if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(), ActiveMember))
		{
			ActiveMember.SetActive(TRUE);
			m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
			m_lpLocalServer->GetActiveMemberView()->Refresh(FALSE);
		}
	}

}

void CCommandProcessor::OnClientDisconnect(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_CLIENT_DISCONNECT mcd = {0};

	GetBody(pBuffer, &mcd, sizeof(mcd));

	CString strTermID = CA2W(mcd.termID);
	CString strIP = CA2W(mcd.termIP);

	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);

	if (nIndex >= 0)
	{
		CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);
	
		if (computer.GetMemberID() > 0) //电脑失去连接了
		{
			CActiveMember ActiveMember;

			if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetMemberID(), ActiveMember))
			{
				ActiveMember.SetActive(FALSE);
				m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
			}

			m_lpLocalServer->GetActiveMemberView()->Refresh(FALSE);
		}
		
		computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
		computer.SetComputerStatus(CComputerInfo::ECS_OFFLINE);
		
		if (computer.GetAvailavleBalance() == 0) //余额为0则清空用户信息
		{
			//computer.ClearUserInfo();
		}

		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);
		m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
	}

	TRACE("OnClientDisconnect\r\n");
}

void CCommandProcessor::OnGetComputerList(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_COMPUT_INFO* pComputerInfo = 
		reinterpret_cast<MANAGER_COMPUT_INFO*>(pBuffer + sizeof(NET_HEAD_MAN));

	size_t nComputerCount = (bufLen - sizeof(NET_HEAD_MAN)) / sizeof(MANAGER_COMPUT_INFO);

	m_lpLocalServer->ComputerList.Clear();

	for (size_t i = 0; i < nComputerCount; i++)
	{
		CComputerInfo computer(*(pComputerInfo + i));

		std::map<CString, UINT>::const_iterator pos = m_PCClassMap.find(computer.GetTerminalID());
		
		if (pos != m_PCClassMap.end())
		{
			computer.SetPCClassID(pos->second);
		}

		CComputerInfo::ECOMPUTERSTATUS nStatus = computer.GetComputerStatus();
		if (nStatus!=CComputerInfo::ECS_ONLINE)
		{
			CActiveMember ActiveMember;
			if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(),
				ActiveMember, TRUE))
			{
				computer.AttachActiveMember(ActiveMember);
			}
	
		}
		else
		{
			CActiveMember ActiveMember;
			if (!m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(),
				ActiveMember, TRUE))
			{
				if (computer.GetAvailavleBalance() == 0)
				{
					computer.ClearUserInfo();
					computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
				}
			}
		}


		m_lpLocalServer->ComputerList.AddComputer(computer);
	}
	
	m_lpLocalServer->ComputerList.MakeComputerKeyMap();

	DecodeQuestionComputer();

	m_lpLocalServer->GetComputerListView()->Refresh(TRUE);
}

void CCommandProcessor::OnTermIDPCClassMap(BYTE *pBuffer, size_t bufLen)
{
	CStringA mapStr;
	mapStr = (char*)(pBuffer + sizeof(NET_HEAD_MAN));

	CString strPCClassMap = CA2T(mapStr);

	CStringArray PCClassAarry;

	CIBAHelpper::SplitLine(strPCClassMap, PCClassAarry, '\n');

	for (INT i = 0; i < PCClassAarry.GetCount(); i++)
	{
		CString strTmp = PCClassAarry.GetAt(i);
		
		INT nIdx = strTmp.Find('=');

		CString strKey = strTmp.Left(nIdx);

		UINT nValue = _ttoi(strTmp.Mid(nIdx + 1));

		m_PCClassMap.insert(std::make_pair(strKey, nValue));
	}
}

void CCommandProcessor::OnClientUpdateData(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_COMPUT_INFO ci = {0};
	GetBody(pBuffer, &ci, sizeof(ci));
	
	CComputerInfo computer(ci);
	
	//清除相同的用户信息，可能是之前换机的那台信息等
	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(computer.GetMemberID());

	if (nIndex >= 0)
	{
		CComputerInfo& computer2 = m_lpLocalServer->ComputerList.GetComputer(nIndex);
		computer2.ClearUserInfo();
		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer2);	
	}

	//先清除后增加
	m_lpLocalServer->ComputerList.AddComputer(computer);
	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);

	//更新ActiveMember

	BOOL bExists = FALSE;

	CActiveMember ActiveMember;

	if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetMemberID(), ActiveMember))
	{
		bExists = TRUE;
	}

	ActiveMember.Attach(ci);
	ActiveMember.SetPCClassID(computer.GetPCClassID());
	ActiveMember.SetActive(TRUE);

	// 2011/10/27-8210-gxx: 
	if (bExists)
	{
		if (ActiveMember.GetRoomMemberId() == 0)
		{
			UINT nRoomMemberID = 0;
			CLocalServer::GetInstance()->ActiveMemberList.IsRoomArea(computer.GetPCClassID(), nRoomMemberID);
			if (nRoomMemberID > 0)
			{
				ActiveMember.SetRoomMemberId(nRoomMemberID);
				bExists = FALSE;
			}
		}
	}
	

	//不存在要更新数据库的预付款和备注等
	if (!bExists)
	{
		CIBALog::GetInstance()->WriteFormat(_T("%d is not exists in ActiveMemberList"), ActiveMember.GetMemberID());

		ActiveMember.SetActivationTime( ActiveMember.GetCheckInTime() );
		ActiveMember.SetAdvancePayment(ActiveMember.GetAvailavleBalance());
		ActiveMember.SetCreditProcess(ActiveMember.GetAvailavleBalanceAsString());
	}

	m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);//!bExists);
	m_lpLocalServer->GetActiveMemberView()->Refresh(FALSE);
}

void CCommandProcessor::OnQuestionComputer(BYTE *pBuffer, size_t bufLen)
{
	m_QuestionComputerList.RemoveAll();

	CStringA strQuestionComputer;

	CString strTmp;

	static const size_t nPerLen = LEN_TERMID + 1 + LEN_IPADDR + 1 + MAX_COMPUTERNAME_LENGTH;

	size_t nCount = (bufLen - sizeof(NET_HEAD_MAN)) / nPerLen;

	for (size_t i = 0; i < nCount; i++)
	{
		strQuestionComputer = (char*)(pBuffer + sizeof(NET_HEAD_MAN) + nPerLen * i);

		strTmp = CA2WEX<512>(strQuestionComputer);
		
		m_QuestionComputerList.Add(strTmp);
	}

	DecodeQuestionComputer();

	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
}

void CCommandProcessor::DecodeQuestionComputer()
{
	CString strTermID, strIP, strTmp;

	for (INT i = 0; i < m_QuestionComputerList.GetCount(); i++)
	{
		strTmp = m_QuestionComputerList.GetAt(i);

		UINT nPos1 = strTmp.Find(_T('/'));
		UINT nPos2 = strTmp.Find(_T('~'));

		strTermID = strTmp.Left(nPos1);
		strIP = strTmp.Mid(nPos1 + 1, nPos2 - nPos1 - 1);

		INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);

		if (nIndex >= 0)
		{
			CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);
			computer.SetComputerStatus(CComputerInfo::ECS_THIEF);
			m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);			
		}
	}
}

void CCommandProcessor::OnGetComputerCount(BYTE *pBuffer, size_t bufLen)
{

}

void CCommandProcessor::OnClientShortMessage(BYTE *pBuffer, size_t bufLen)
{
	if (CIBAConfig::GetInstance()->GetPauseRevMsg()) //暂停接收消息
	{
		return;
	}

	if (!theApp.GetCurCashier()->IsLogin())
	{
		return;
	}

	SHORT_MESSAGE_INFO shortMsgInfo  = {0};

	GetBody(pBuffer, &shortMsgInfo, sizeof(shortMsgInfo));	

	if (shortMsgInfo.shortMsg.side == 1 &&
		CIBAConfig::GetInstance()->GetCashRegisterID() == (UINT)shortMsgInfo.shortMsg.counterID) //自己发送的
	{
		return;
	}

	m_lpLocalServer->GetComputerListView()->OnClientShortMessage(shortMsgInfo);
}

void CCommandProcessor::OnSendShortMsgResp(BYTE* /*pBuffer*/, size_t /*bufLen*/)
{
	
}

void CCommandProcessor::OnRespondCaptureScreen(BYTE *pBuffer, size_t /*bufLen*/)
{
	if (!m_bCaptureScreen)
	{
		return;
	}

	m_bCaptureScreen = FALSE;

	try
	{
		NET_HEAD_MAN head = {0};
		SCREEN_TERMINFO stTermInfo = {0};

		GetHead(pBuffer, &head);
		GetBody(pBuffer, &stTermInfo, sizeof(stTermInfo));

		CByteArray buf;
		buf.SetSize(head.Length - sizeof(stTermInfo));
		CopyMemory(buf.GetData(), pBuffer + sizeof(head) + sizeof(stTermInfo), buf.GetSize());

		CString strPicFile(theApp.GetWorkPath() + _T("\\IBAScreen\\"));

		CIBAHelpper::MakeSurePathExists(strPicFile);

		strPicFile.Append(CA2W(stTermInfo.termID));
		strPicFile.Trim();
		strPicFile.AppendFormat(_T("[%s].jpg"), CIBAHelpper::GetCurrentTimeAsString());

		SaveFile(strPicFile, buf);
	}
	catch (CFileException* e)
	{		
		e->Delete();
		CIBALog::GetInstance()->Write(_T("Capture Screen: Operation file fails."));
	}
	catch (...)
	{
		CIBALog::GetInstance()->Write(_T("Capture Screen failed."));
	}
}

void CCommandProcessor::OnRespondGetLog(BYTE *pBuffer, size_t /*bufLen*/)
{
	if (!m_bGetLog)
	{
		return;
	}
	
	m_bGetLog = FALSE;

	try
	{
		NET_HEAD_MAN head = {0};
		SCREEN_TERMINFO stTermInfo = {0};

		GetHead(pBuffer, &head);
		GetBody(pBuffer, &stTermInfo, sizeof(stTermInfo));

		CByteArray buf;
		buf.SetSize(head.Length - sizeof(stTermInfo));
		CopyMemory(buf.GetData(), pBuffer + sizeof(head) + sizeof(stTermInfo), buf.GetSize());

		CString strPicFile(theApp.GetWorkPath() + _T("\\IBATermLog\\"));
		CIBAHelpper::MakeSurePathExists(strPicFile);

		strPicFile.Append(CA2W(stTermInfo.termID));
		strPicFile.Trim();

		if (head.Version == 1) //zip
		{
			strPicFile.AppendFormat(_T("[%s].zip"), CIBAHelpper::GetCurrentTimeAsString());
		}
		else
		{
			strPicFile.AppendFormat(_T("[%s].log"), CIBAHelpper::GetCurrentTimeAsString());
		}
		
		SaveFile(strPicFile, buf);

	}
	catch (CFileException* e)
	{		
		e->Delete();
		CIBALog::GetInstance()->Write(_T("Get Log: Operation file failed."));
	}
	catch (...)
	{
		CIBALog::GetInstance()->Write(_T("Get Log failed."));
	}
}

void CCommandProcessor::SaveFile(CString strFile, CByteArray &buf)
{
	CFile File;

	if (File.Open(strFile, CFile::modeCreate | CFile::modeWrite))
	{
		File.Write(buf.GetData(), (UINT)buf.GetSize());
		File.Flush();
		File.Close();
		
		if (CIBAConfig::GetInstance()->GetOpenAfterSave())
		{
			ShellExecute(NULL, _T("open"), strFile, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("SaveFile: Create file failed."));
	}
}

void CCommandProcessor::OnTermUnLock(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_COMPUT_INFO ci = {0};

	GetBody(pBuffer, &ci, sizeof(ci));

	CString strTermID = CA2W(ci.termID);
	CString strIP = CA2W(ci.IPAddr);

	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);

	if (nIndex >= 0)
	{
		CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);
		computer.Attach(ci);

		if (5 == ci.dwIDOStatus) //解锁完毕
		{
			computer.ClearUserInfo();
		}

		computer.SetComputerStatus(7 == ci.dwIDOStatus ? CComputerInfo::ECS_UNLOCK : CComputerInfo::ECS_LOCKED);

		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);
		m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
	}

	if (7 == ci.dwIDOStatus) //解锁
	{
		NS_TTS::CIBATTS::GetInstance()->SpeakUnlock();
	}
}

void CCommandProcessor::OnPingClientResp(BYTE *pBuffer, size_t bufLen)
{
	PINGCLIENTRESP pcr = {0};
	GetBody(pBuffer, &pcr, sizeof PINGCLIENTRESP);

	CString strTermID = CA2W(pcr.termId);
	CString strIP = CA2W(pcr.termIp);

	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);

	if (nIndex >= 0)
	{
		CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);

		if (computer.GetComputerStatus() == CComputerInfo::ECS_THIEF)
		{
			if (0 == pcr.cCode) //不通
			{
				computer.SetComputerStatus(CComputerInfo::ECS_OFFLINE);
			}
		}
		else if (computer.GetComputerStatus() == CComputerInfo::ECS_OFFLINE)
		{
			if (1 == pcr.cCode) //通
			{
				computer.SetComputerStatus(CComputerInfo::ECS_THIEF);
			}
		}

		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);
		m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
	}
}

void CCommandProcessor::OnAutoReturnResp(BYTE *pBuffer, size_t bufLen)
{
	AUTORETURNMSG prm = {0};
	GetBody(pBuffer, &prm, sizeof AUTORETURNMSG);
	
	CIBALog::GetInstance()->Write(_T("receive IServer return information----------------------"));
	CString strCheckInTime;
	CString strCheckOutTime;

}

void CCommandProcessor::OnGetNewNotice(BYTE *pBuffer, size_t bufLen)
{
	CIBALog::GetInstance()->Write(_T("获取公告成功"));

	TNewNotice newNotice = {0};
	GetBody(pBuffer, &newNotice, sizeof TNewNotice);

	CString strTitle = CA2W(newNotice.cNoticeTitle);
	CString strInfo = CA2W(newNotice.cNoticeInfo);
	CString strLink = CA2W(newNotice.cNoticeLink);
	BOOL bShouldSign = BOOL(newNotice.nNoticeFlag);

	((CMainFrame*)theApp.GetMainWnd())->ShowNotice(strTitle,strInfo,strLink,bShouldSign);

}