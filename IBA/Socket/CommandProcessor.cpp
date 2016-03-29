
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
#include "..\CashierHelper.h"

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

	//try
	//{
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
	//}
	//catch (...) 

	//{
	//	DWORD dwErr = GetLastError();
	//	LPVOID lpMsgBuf; 
	//	FormatMessage( 
	//		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	//		FORMAT_MESSAGE_FROM_SYSTEM | 
	//		FORMAT_MESSAGE_IGNORE_INSERTS, 
	//		NULL, 
	//		dwErr, 
	//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	//		(LPTSTR) &lpMsgBuf, 
	//		0, 
	//		NULL 
	//		); 
	//	IBA_LOG(_T("CommandProcessor::Process - Unknown Exception.ErrorCode:%d,ErrMsg:%s"),dwErr,(LPCTSTR)lpMsgBuf);
	//}

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

/*************************************************
Function:       //OnClientCheckout
Description:    //响应UDO结账.可能为UDO会员结账,UDO换机,UDO用户余额不足自动下机
				//进行相应实名下机,更新终端信息和用户信息
Table Updated:  //LocalMember,LastUser,ShortMessage
Input:          //pBuffer - UDO消息,包含:终端号,终端IP,终端状态,(1是换机，0是普通,2结账下机(告诉客户端只做锁屏，不连中心)，3通知客户端重新登录)
                //bufLen - 数据长度
*************************************************/
void CCommandProcessor::OnClientCheckout(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_CLIENT_DISCONNECT mcd = {0};
	GetBody(pBuffer, &mcd, sizeof(MANAGER_CLIENT_DISCONNECT)); 

	CString strTermID = CA2W(mcd.termID); //TermID
	CString strIP = CA2W(mcd.termIP); //IP
	int nCheckoutType = mcd.nCheckoutType; //1是换机，0是普通,2结账下机(告诉客户端只做锁屏，不连中心)，3通知客户端重新登录

	//终端列表中查找终端
	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(strTermID, strIP);
	if(nIndex < 0){return;}
	CComputerInfo& computer = m_lpLocalServer->ComputerList.GetComputer(nIndex);
	UINT nMemberId = computer.GetMemberID();
	CString sIP = computer.GetComputerIP();
	CString sTermId = computer.GetTerminalID();

	//激活列表中查找终端上的用户
	CActiveMember ActiveMember;
	BOOL bUserFound = m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetMemberID(),ActiveMember);

	//更新终端信息-清除用户信息,状态改为锁屏
	computer.ClearUserInfo();
	computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
	m_lpLocalServer->ComputerList.UpdateComputer(nIndex,computer);
	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);

	//终端上无用户或用户未激活.不要更新用户信息和进行实名下机.直接返回.
	if(FALSE == bUserFound){return;}

	//实名下机
	theApp.GetRealName()->AuditUserCheckOut(sTermId, sIP, nMemberId);
	//ZD实名下机 -  客户端直接在线换机（也即A机器不做操作，直接用A机器的账号在B机器上机）的时候会查找不到信息
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

	//输出操作日志-会员结账
	COperationInfo OperationInfo;
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturnVip);
	}
	else
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeVipReturn);
	}	
	OperationInfo.SetNetId(ActiveMember.GetNetId());
	OperationInfo.SetUserName(ActiveMember.GetUserName());
	//OperationInfo.SetDescription(ActiveMember.GetRemark());
	OperationInfo.SetClassId(ActiveMember.GetClassId());
	OperationInfo.SetTermId(ActiveMember.GetTerminalID());
	OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
	OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
	CString strRemark = ActiveMember.GetRemark();

	//2016-0201
	//删除LastUser库记录 -- 仅当换机时才不删除记录 
	if(0 == nCheckoutType && 0 < ActiveMember.GetAvailavleBalance())
	{
		IBA_LOG(_T("OnClientCheckOUt - checkouttype = 0 and balance >0"));
	}	
	else
	{
		CIBADAL::GetInstance()->DeleteLastUser(nMemberId);
	}

	//下机类型=0,且余额为0,可能是换机中或者是余额不足被踢出：更新状态为中心在线,使IBA加速查询.删除终端号.无需更新其他用户信息
	if(0 == nCheckoutType)
	{
		if(0 == ActiveMember.GetAvailavleBalance())
		{
			//设置状态
			ActiveMember.SetTerminalID(_T(""));
			ActiveMember.SetStatus(CActiveMember::EStatus_CenterOnLine);
			m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
			//增加和显示客户端消息 - 没钱了，锁屏
			CClientShortMessage csm;
			CIBAString strBody;
			strBody.LoadString(IDS_NOMONEYLOCK);
			csm.SetMsgBody(strBody);
			csm.SetTermID(sTermId);
			csm.SetTermIP(sIP);
			csm.SetRevTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
			csm.SetMemberId(nMemberId);

			CIBADAL::GetInstance()->AddClientShortMsg(csm);

			//添加操作信息,说明是余额为0的情况
			CString strMsg(strRemark);
			strRemark.Format(_T("余额不足.下机锁屏"));
		}
		else
		{
			//添加操作信息,说明是换机的情况
			CString strMsg(strRemark);
			strRemark.Format(_T("换机.下机锁屏"));
		}
	}
	else if(2 == nCheckoutType || 202 == nCheckoutType)//表示会员下机
	{
		BOOL bSuccessDBOperation(TRUE);//数据库操作是否成功

		//本地模式,向本地库添加结账记录
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			//查询LocalMember是否有记录
			CLocalMember lMember;
			BOOL bHasRecord = CIBADAL::GetInstance()->GetLocalMemberByMemberID(ActiveMember.GetMemberID(),lMember);
			if(bHasRecord)//有记录,更新退款信息
			{
				COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
				if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(ActiveMember.GetMemberID(),CheckoutTime,TRUE))
				{
					bSuccessDBOperation = FALSE;
					IBA_LOG(_T("Error.更新本地结账记录失败."));
				}
			}
			else//无记录,创建记录,插入记录,更新记录
			{
				GxxDictionaryPtr lmPrt = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(ActiveMember.GetMemberID());  //创建记录
				if(lmPrt)
				{
					if(CIBADAL::GetInstance()->InsertNewLocalMember(lmPrt))//插入记录
					{
						COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
						if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(ActiveMember.GetMemberID(),CheckoutTime,TRUE))//更新记录
						{
							bSuccessDBOperation = FALSE;
							IBA_LOG(_T("Error.更新本地结账记录失败."));
						}	
					}
					else
					{
						bSuccessDBOperation = FALSE;
						IBA_LOG(_T("Error.为结账插入本地记录失败."));
					}
				}
				else
				{
					bSuccessDBOperation = FALSE;
					IBA_LOG(_T("Error.为结账创建本地记录失败."));
				}
			}
			//操作失败,添加失败信息到操作日志
			if(FALSE == bSuccessDBOperation)
			{
				CString strErrMsg(strRemark);
				strRemark.Format(_T("%s,%s"),LOAD_STRING(IDS_ERROR_DBOPERATION),strErrMsg);
			}
		}
		//增加和显示客户端消息 - 会员下机
		CClientShortMessage csm;
		CIBAString strBody;
		strBody.LoadString(IDS_PAYOUTMEMBER);
		csm.SetMsgBody(strBody);
		csm.SetTermID(sTermId);
		csm.SetTermIP(sIP);
		csm.SetRevTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
		csm.SetMemberId(nMemberId);

		CIBADAL::GetInstance()->AddClientShortMsg(csm);
		//包房用户,不从激活列表删除
		if(0 != ActiveMember.GetRoomMemberId())
		{
			ActiveMember.SetRoomMemberId(0);
			ActiveMember.SetTerminalID(_T("")); 
			ActiveMember.SetPCClassID(0);
			ActiveMember.SetRemark(LOAD_STRING(IDS_ROOM_HAS_REFUND));
			m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
		}
		else//非包房用户,从激活列表删除
		{
			m_lpLocalServer->ActiveMemberList.RemoveActiveMember(nMemberId);
		}
	}
	else//临时用户.
	{
		//正常情况不会出现临时用户
		return;
	}

	//显示操作日志
	OperationInfo.SetDescription(strRemark);
	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

	//刷新用户视图和终端视图
	m_lpLocalServer->GetActiveMemberView()->Refresh(TRUE);
	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);


			//本地模式下，更新或创建更新LocalMember表的isReturned和CheckOutTime
		//if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		//{
		//	if(!bIsRemoved)
		//	{
		//			//	余额为0,可能是UDO金额不足踢出的情况,按照目前流程,不需要做checkout或deregister动作,因此直接退出.本地也由CheckActiveMember来取消激活.
		//		if(ActiveMember.GetAvailavleBalance() == 0)
		//		{
		//			IBA_LOG(_T("Temp.CCommandProcessor::OnClientCheckOut - 余额为0,可能是UDO金额不足踢出的情况,Do Nothing!"));
		//			return;
		//		}
		//		if(!CCashierHelper::UpdateLocalMemberForLocalReturn(ActiveMember.GetMemberID()))
		//		{
		//			IBA_LOG(_T("Abnormal.CCommandProcessor::OnClientCheckOut - UpdateLocalMemberForLocalReturn failed!"));
		//			return;
		//		}
		//			//余额>0的表示正常结账的，从激活列表中删除
		//		m_lpLocalServer->ActiveMemberList.RemoveActiveMember(ActiveMember.GetMemberID());
		//	}
		//}
}

/*************************************************
Function:       //OnClientLogin
Description:    //处理Iserver消息S_RESPOND_CLIENT_LOGIN
				//ONLINE的不处理,之前没有ONLINE的,如果激活列表中找到对应终端的用户,attach到ComputerInfo,并增加到ComoputerList.
				//如果ECS_SUSPEND,且激活列表中存在用户信息,则属于重启挂机.更新其用户视图状态为Active.
Input:          //pBuffer - 数据
				//bufLen - 数据长度
*************************************************/
void CCommandProcessor::OnClientLogin(BYTE *pBuffer, size_t bufLen)
{
	MANAGER_COMPUT_INFO ci = {0};
	GetBody(pBuffer, &ci, sizeof(ci));

	CComputerInfo computer(ci);

	//状态不为ECS_ONLINE的终端,使用终端ID到激活列表中寻找用户,找到则attach
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

	//移除所有问题终端显示 - QuestionComputerList来自IServer消息S_RESPOND_QUESTION_COMPUTER
	for (INT i = 0; i < m_QuestionComputerList.GetCount(); i++)
	{
		if (m_QuestionComputerList.GetAt(i).Find(computer.GetTerminalID()) >= 0)
		{
			m_QuestionComputerList.RemoveAt(i);
		}
	}

	m_lpLocalServer->GetComputerListView()->Refresh(FALSE);

	//如果为重启挂机,更新用户视图为Active
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
		IBA_LOG(_T("CCommandProcessor::OnClientDisconnect - SetComputerStatus(CComputerInfo::ECS_OFFLINE) for TermId:%s IP:%s"),strTermID,strIP);
		
		if (computer.GetAvailavleBalance() == 0) //余额为0则清空用户信息
		{
			//computer.ClearUserInfo();
		}

		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer);
		m_lpLocalServer->GetComputerListView()->Refresh(FALSE);
	}

	TRACE("OnClientDisconnect\r\n");
}

/*************************************************
Function:       //OnGetComputerList
Description:    //取得Iserver传过来的终端信息,遍历所有终端信息,状态为ECS_ONLINE的attach对应激活用户信息到ComputerInfo
				//依次添加到ComputerList,更新ComputerListView,重绘座位图
Calls:          //ComputerListView()->Refresh(TRUE),SeatLayoutView()->RedrawWindow()
Input:          //BYTE *pBuffer  - 接收到的数据
                //size_t bufLen  - 数据长度
*************************************************/
void CCommandProcessor::OnGetComputerList(BYTE *pBuffer, size_t bufLen)
{
	//取得第一个ComputerInfo指针,计算共有多少个ComputerInfo
	MANAGER_COMPUT_INFO* pComputerInfo = 
		reinterpret_cast<MANAGER_COMPUT_INFO*>(pBuffer + sizeof(NET_HEAD_MAN));

	size_t nComputerCount = (bufLen - sizeof(NET_HEAD_MAN)) / sizeof(MANAGER_COMPUT_INFO);

	m_lpLocalServer->ComputerList.Clear();

	//遍历所有ComputerInfo
	for (size_t i = 0; i < nComputerCount; i++)
	{
		CComputerInfo computer(*(pComputerInfo + i));

		//从m_PCClassMap中取得对应区域信息(ClassID)
		std::map<CString, UINT>::const_iterator pos = m_PCClassMap.find(computer.GetTerminalID());
		
		if (pos != m_PCClassMap.end())
		{
			computer.SetPCClassID(pos->second);
		}

		//判断Iserver传过来的终端状态
		CComputerInfo::ECOMPUTERSTATUS nStatus = computer.GetComputerStatus();
			//不为ECS_ONLINE则从激活列表中取得用户信息并attach到computer.  ONLINE的Iserver会传递UpdateData在OnClientUpdateDate中进行添加或更新
		if (nStatus!=CComputerInfo::ECS_ONLINE)
		{
			CActiveMember ActiveMember;
			if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(),ActiveMember, TRUE))
			{
				computer.AttachActiveMember(ActiveMember);
			}	
		}
		//else
		//{
		//	CString strTermId = computer.GetComputerName();
		//	IBA_LOG(_T("CCommadProcessor::OnGetComputerList - offline client Id = %s, status code = %d"),strTermId,computer.GetComputerStatus());
		//}
		// 2014-3-28 - qsc QSCTAG 暂时注释
		//else		// 2014-1-17 - qsc 解决包时问题,不为在线状态,又不在激活列表中,如果余额为0,设置为锁屏	
		//{
		//	CActiveMember ActiveMember;
		//	if (!m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetTerminalID(),ActiveMember, TRUE))
		//	{
		//		if (computer.GetAvailavleBalance() == 0)
		//		{
		//			computer.ClearUserInfo();
		//			computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
		//		}
		//	}
		//}

		//将终端信息增加到终端列表
		m_lpLocalServer->ComputerList.AddComputer(computer);
	}
	//更新m_ComputerKeyMap(索引为strTermID+strComputerIP组成的)
	m_lpLocalServer->ComputerList.MakeComputerKeyMap();

	DecodeQuestionComputer();

	//2015-0919 liyajun 暂时取消上报PC信息
	// 2015-3-5-tmelody 上报PC信息
	//if (m_lpLocalServer->GetNeedReportPCInfo())
	//{
	//	m_lpLocalServer->ComputerList.StartReportPCInfo();
	//	m_lpLocalServer->SetNeedReportPCInfo(FALSE);
	//}

	//更新终端视图
	//try{
	m_lpLocalServer->GetComputerListView()->Refresh(TRUE);
	//}catch (...)
	//{}
	// 2014-1-16-qsc 终端信息读取完了才添加以排除一些不存在的电脑
	m_lpLocalServer->GetSeatLayoutView()->Loadseat();
	// 2014-1-15-qsc 添加完数据（终端信息）重绘
	m_lpLocalServer->GetSeatLayoutView()->RedrawWindow();
}

/*************************************************
Function:       //OnTermIDPCClassMap
Description:    //解析收到的终端ID列表,获取区域信息.保存到m_PCClassMap,同时保存到NetBarConfig::m_TermIDPCClassMap
Input:          //BYTE *pBuffer - 接收的数据
                //size_t bufLen - 数据长度
*************************************************/
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

		//2015-0824 liyajun 保存终端机器区域信息到NetBarConfig
		CNetBarConfig::GetInstance()->m_TermIDPCClassMap = m_PCClassMap;
	}
}

/*************************************************
Function:       // OnClientUpdateData
Description:    // 从客户端接收某一终端的更新报文，更新终端列表，更新对应的激活用户列表(从报文中获取最新的终端状态，用户信息，余额花费等等..)
Calls:          // UpdateComputer, UpdateActiveMember
Table Updated:  // ActiveMember
Input:          // pBuffer 终端信息报文
                // bufLen 报文长度
*************************************************/
void CCommandProcessor::OnClientUpdateData(BYTE *pBuffer, size_t bufLen)
{
	IBA_LOG(_T("OnClientUpdata - Start"));
	MANAGER_COMPUT_INFO ci = {0};
	GetBody(pBuffer, &ci, sizeof(ci));
	
	CComputerInfo computer(ci);
	IBA_LOG(_T("UDO传过来的PersonalID=%d"),computer.GetPersonalID());
	//依MemberID在m_ComputerArray中查找终端.若MemberId = 0???
	INT_PTR nIndex = m_lpLocalServer->ComputerList.Find(computer.GetMemberID());

	//清除有相同MemberID的用户信息的终端信息(换机时,上一台终端的用户信息可能还未被删除)
	if (nIndex >= 0)
	{
		CComputerInfo& computer2 = m_lpLocalServer->ComputerList.GetComputer(nIndex);
		computer2.ClearUserInfo();//只剩下大部分终端信息和终端信息上报中心时间(time_t m_lCheckTime)
		m_lpLocalServer->ComputerList.UpdateComputer(nIndex, computer2);//重置nIndex位置的ComputerInfo,设m_UpdateState为EUS_CHANGE,PCClassID保持不变.添加nIndex到m_UpdateIndexArray.
	}

	//添加到终端列表，刷新终端显示
	m_lpLocalServer->ComputerList.AddComputer(computer);  //(依终端号+IP查找)添加computerInfo.若已存在相同TermId+IP对象,则仅更新,否则添加.更新m_UpdateIndexArray及m_ComputerKeyMap
	m_lpLocalServer->GetComputerListView()->Refresh(FALSE); //刷新更新过的computerInfo的终端视图
	
	IBA_LOG(_T("CCommandProcessor::OnClientUpdateData - TermId = %s IP = %s IDOStauts = %d Status = %d ComputerStatus = %s PersonalID = %s"),computer.GetTerminalID(),computer.GetComputerIP(),computer.GetIDOStatus(),computer.GetStatus(),computer.GetStatusAsString(),computer.GetPersonalID());
	//TermId = 053 IP = 192.168.1.53 IDOStauts = 1 Status = 2 ComputerStatus = 用户在线 PersonalID = 362226199907042110

	// 激活列表中能找到用户,将终端attach到activeMember中
	// 未找到，则将该ActiveMember添加到Map中，设置激活时间为当前，预付款为余额(0?)，明细为余额(0?)
	//最后更新Map和DB，更新显示
	BOOL bExists = FALSE;
	CActiveMember ActiveMember;
	if (m_lpLocalServer->ActiveMemberList.GetActiveMember(computer.GetMemberID(), ActiveMember))
	{
		bExists = TRUE;
	}

	ActiveMember.Attach(ci);

	IBA_LOG(_T("Temp.OnClientUpdateData.SetAvailableBalance = %d, for MemberId = %d"),ActiveMember.GetAvailavleBalance(),ActiveMember.GetMemberID());

	//2015-0911 liyajun 从CMapClassID中获取PCClassID
	CString strTermId = computer.GetTerminalID();
	std::map<CString, UINT>::const_iterator pos = m_PCClassMap.find(strTermId);

	if (pos != m_PCClassMap.end())
	{
		computer.SetPCClassID(pos->second);
	}
	ActiveMember.SetPCClassID(computer.GetPCClassID());
	ActiveMember.SetActive(TRUE);

	// 2011/10/27-8210-gxx: 
	if (bExists)
	{
		if (ActiveMember.GetRoomMemberId() == 0)
		{
			UINT nRoomMemberID = 0;
			CLocalServer::GetInstance()->ActiveMemberList.IsRoomArea(computer.GetPCClassID(), nRoomMemberID);
			if (nRoomMemberID > 0) //若该区域中有用户的RoomMemberID>0，则认为该区域是包房区域。且设置该用户的RoomMemberId。
			{
				ActiveMember.SetRoomMemberId(nRoomMemberID);
				IBA_LOG(_T("Set activeMember.RoomMember = %d"),nRoomMemberID);
				bExists = FALSE;
			}
		}
	}	

	//不存在要更新数据库的预付款和备注等
	if (!bExists)
	{
		CIBALog::GetInstance()->WriteFormat(_T("OnClientUpdateDate - %d用户不存在,重新添加到视图."), ActiveMember.GetMemberID());

		ActiveMember.SetActivationTime( ActiveMember.GetCheckInTime() );
		ActiveMember.SetAdvancePayment(ActiveMember.GetAvailavleBalance());
		ActiveMember.SetCreditProcess(ActiveMember.GetAvailavleBalanceAsString());
	}

	m_lpLocalServer->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);//!bExists);
	m_lpLocalServer->GetActiveMemberView()->Refresh(FALSE);
	IBA_LOG(_T("OnClientUpdata - End"));
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
	//catch (...)
	//{
	//	CIBALog::GetInstance()->Write(_T("Capture Screen failed."));
	//}
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
	//catch (...)
	//{
	//	CIBALog::GetInstance()->Write(_T("Get Log failed."));
	//}
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
				IBA_LOG(_T("CCommandProcessor::OnPingClientResp - SetComputerStatus(CComputerInfo::ECS_OFFLINE) for TermId:%s IP:%s"),strTermID,strIP);
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
	
	IBA_LOG(_T("CCommandProcessor::OnAutoReturnResp - receive IServer return information-----------"));
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
			//char szIP[20];
			//char szTermId[15];

			// 下面是智多部分
			//char szCardId[21];		//上网卡号
			//int  iCardType;			//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
			//char szName[31]; 		//姓名
			//int   iCertType;		//证件类型
			//char szCertNumber[19];	//证件号码
			//int  iErrCode;			//错误号，详见错误定义表；0表示成功
			//char szErrMsg[101];		//错误描述，如果Error不为0的话
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
