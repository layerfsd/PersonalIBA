
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
#include "..\SeatLayoutView.h"
#include "..\RealName\IZd.h"
#include "..\RealName\RealNameBase.h"
#include "..\RealName\IBARealName.h"

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
	AddHandle(S_NEWNOTICE_MSG,				&CCommandProcessor::OnGetNewNotice);
	//AddHandle(S_RESPOND_PONG, &CCommandProcessor::OnPing);
	//AddHandle(C_MANAGER_GET_PCCLASSNAME, &CCommandProcessor::OnGetPCClassName);
	//AddHandle(S_RESPOND_CHECK_MANAGER_PASSWORD, &CCommandProcessor::OnCheckManagerPwdResp);
	//AddHandle(S_RESPOND_SET_MANAGER_PASSWORD, &CCommandProcessor::OnSetManagerPwdResp);
	//AddHandle(S_RESPOND_GET_MEMBER_CLASSES, &CCommandProcessor::OnGetMemberClassesResp);
	
	//AddHandle(C_MANAGER_QUESTIIOIN_AMOUNT, &CCommandProcessor::OnQuestionAmount);
	//AddHandle(C_MANAGER_GET_IP2TERMID, &CCommandProcessor::OnGetIP2TermID);
	AddHandle(S_AUTO_RETURN_MSG,            &CCommandProcessor::OnAutoReturnResp);
	AddHandle(C_LOGIN_ZD,					&CCommandProcessor::OnLoingZd);
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
			
			// 2014-10-10 - qsc
			if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
			{
				NS_REALNAME::CheckoutInfo coi;
				ZeroMemory(&coi, sizeof(coi));
				_tcsncpy(coi.szName, ActiveMember.GetUserName(), 100);
				_tcsncpy(coi.szNetId, ActiveMember.GetNetId(), 100);
				_tcsncpy(coi.szTermId, strTermID, 50);
				_tcsncpy(coi.szIP, strIP, 50);
				coi.bClientCheckOut = true;

				coi.nCheckoutType = mcd.nCheckoutType;

				theApp.GetRealName()->CheckOut(&coi);
			}

			//CLocalServer::GetInstance()->GetActiveMemberView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
		}
		else
		{
			// 2014-10-29 - qsc 客户端直接在线换机（也即A机器不做操作，直接用A机器的账号在B机器上机）
			// 的时候会查找不到信息
			if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
			{
				CString strSN, strName;
				CIBADAL::GetInstance()->GetZdRnInfoByTermId(strTermID, strSN, strName);
				NS_REALNAME::CheckoutInfo coi;
				ZeroMemory(&coi, sizeof(coi));
				_tcsncpy(coi.szName, strName, 100);
				_tcsncpy(coi.szNetId, strSN, 100);
				_tcsncpy(coi.szTermId, strTermID, 50);
				_tcsncpy(coi.szIP, strIP, 50);
				coi.bClientCheckOut = true;

				coi.nCheckoutType = mcd.nCheckoutType;

				theApp.GetRealName()->CheckOut(&coi);
			}
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
					// 2014-3-28 - qsc QSCTAG 暂时注释
					// 2014-2-25 - qsc
					//m_lpLocalServer->ComputerList.ClearComputerUserInfo(computer.GetMemberID());
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
		// 2014-3-28 - qsc QSCTAG 暂时注释
		//else		// 2014-1-17 - qsc 解决包时问题		
		//{
		//	CActiveMember ActiveMember;
		//	if (!m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(),
		//		ActiveMember, TRUE))
		//	{
		//		if (computer.GetAvailavleBalance() == 0)
		//		{
		//			computer.ClearUserInfo();
		//			computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
		//		}
		//	}
		//}


		m_lpLocalServer->ComputerList.AddComputer(computer);
	}
	
	m_lpLocalServer->ComputerList.MakeComputerKeyMap();

	DecodeQuestionComputer();
	try{
		m_lpLocalServer->GetComputerListView()->Refresh(TRUE);
	}catch (...)
	{}
	// 2014-1-16-qsc 终端信息读取完了才添加以排除一些不存在的电脑
	m_lpLocalServer->GetSeatLayoutView()->Loadseat();
	// 2014-1-15-qsc 添加完数据（终端信息）重绘
	m_lpLocalServer->GetSeatLayoutView()->RedrawWindow();
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

void CCommandProcessor::OnLoingZd( BYTE *pBuffer, size_t bufLen )
{
	IBA_LOG0(_T("OnLoinZd"));
	// 主收银台才做这个操作
	if (CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
	{
		if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
		{
			TLOGIN_ZD login_zd = {0};
			GetBody(pBuffer, &login_zd, sizeof TLOGIN_ZD);
			CActiveMember ActiveMember;

			CString strNetId = CA2T(login_zd.szNetid);

			if(strNetId.GetLength() < 10)
			{
				strNetId.Insert(0, CString(_T("0000000000")).Left(10 - strNetId.GetLength()));
			}
			bool bChange = false;
			CString strDBTermId;
			COleDateTime oleTime;
			if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strNetId, ActiveMember))
			{
				bChange = !ActiveMember.GetTerminalID().IsEmpty();		// 不为空说明是换机

				// 数据库中没有相关记录，说明不是换机
				// 开户指定上机终端，会直接再用户视图插入数据防止这种情况出现
				CIBADAL::GetInstance()->GetZdRnInfo(strNetId, strDBTermId, oleTime);		
				if(strDBTermId.IsEmpty())
				{
					bChange = false;
				}
			}

			ID_LOGINRETURN_ZD loginReturn_zd;
			CIZd::ID_LOGINRETURN loginReturn;
			ZeroMemory(&loginReturn_zd, sizeof(loginReturn_zd));
			ZeroMemory(&loginReturn, sizeof(loginReturn));
			// 第一次登录调用OpenUserEx，换机调用LoginEx
			if(bChange)
			{
				// 同一台机器，说明不是换机，只是类似于机器重启的操作
				COleDateTimeSpan span = COleDateTime::GetCurrentTime() - oleTime;
				if(0 == strDBTermId.CompareNoCase(CA2T(login_zd.szTermId)) && 
					span.GetTotalSeconds() < 20)		// 20秒内算同台机器
				{
					loginReturn.iErrCode = 0;
				}
				else
				{
					if(CIZd::GetInstance()->IsIDPwdCorrect(login_zd.szNetid, login_zd.szPwd))
					{
						IBA_LOG(_T("%s换到%s"), strDBTermId, CA2T(login_zd.szTermId));
						// 2014-10-30 - qsc 换机，先把原来的下机
						CString strIP;
						CIBADAL::GetInstance()->GetIPFromTermId(strDBTermId, strIP);
						CIZd::GetInstance()->V2_Logout(login_zd.szNetid, login_zd.szNetid, CT2A(strIP));
						
						loginReturn = CIZd::GetInstance()->V2_LoginEx(
							login_zd.szNetid, login_zd.szNetid, login_zd.szPwd, login_zd.szIP);	

						if(0 == loginReturn.iErrCode)
						{
							CIBADAL::GetInstance()->AddZdRnInfo(strNetId, CA2T(login_zd.szTermId), CA2T(loginReturn.szName));
						}
					}
					else
					{
						loginReturn.iErrCode = -9;
						strcpy(loginReturn.szErrMsg, "密码输入错误");
					}
				}
			}
			else
			{
				loginReturn = CIZd::GetInstance()->V2_OpenUserEx(
					login_zd.szNetid, login_zd.szNetid, login_zd.szPwd, login_zd.szIP, "1");

				if(0 == loginReturn.iErrCode)
				{
					CIBADAL::GetInstance()->AddZdRnInfo(strNetId, CA2T(login_zd.szTermId), CA2T(loginReturn.szName));
				}
			}
			char szIP[20];
			char szTermId[15];

			// 下面是智多部分
			char szCardId[21];		//上网卡号
			int  iCardType;			//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
			char szName[31]; 		//姓名
			int   iCertType;		//证件类型
			char szCertNumber[19];	//证件号码
			int  iErrCode;			//错误号，详见错误定义表；0表示成功
			char szErrMsg[101];		//错误描述，如果Error不为0的话
			strncpy(loginReturn_zd.szTermId, login_zd.szTermId, 15);
			strncpy(loginReturn_zd.szIP,	 login_zd.szIP, 20);
			strncpy(loginReturn_zd.szCardId, loginReturn.szCardId, 21);
			strncpy(loginReturn_zd.szName,	loginReturn.szName, 31);
			strncpy(loginReturn_zd.szCertNumber, loginReturn.szCertNumber, 19);
			strncpy(loginReturn_zd.szErrMsg, loginReturn.szErrMsg, 101);
			loginReturn_zd.iCardType	= loginReturn.iCertType;
			loginReturn_zd.iErrCode		= loginReturn.iErrCode;

			CLocalServer::GetInstance()->LocalLoginZdReturn(&loginReturn_zd);
		}	
	}
}
