#include "StdAfx.h"
#include "WxMiddlewareImpl.h"
#include "Dal\IBADAL.h"
#include "Dal\ConfigInfo.h"
#include <atlutil.h>
#include "NetBarConfig.h"
#include "IBAConfig.h"
#include "IBALog.h"
#include "ComputerInfo.h"
#include "Socket\LocalServer.h"
#include "resource.h"
#include "IBAHelpper.h"
#include "ComputerListView.h"
#include "ActiveMemberView.h"
#include "WxMsgTips.h"

CWxMiddlewareImpl* CWxMiddlewareImpl::GetInstance()
{
	static CWxMiddlewareImpl* instance = NULL;
	if (instance == NULL) {
		instance = new CWxMiddlewareImpl();
	}

	return instance;
}

CWxMiddlewareImpl::CWxMiddlewareImpl()
{
	
}

CWxMiddlewareImpl::~CWxMiddlewareImpl()
{

}

void CWxMiddlewareImpl::getNetbarIdAndDomainId( UINT * netbarId, UINT * domainId )
{
	*domainId = CNetBarConfig::GetInstance()->GetDomainId();
	*netbarId = CNetBarConfig::GetInstance()->GetNetBarId();
}

BOOL CWxMiddlewareImpl::getMiddlewareCenterIPPort( TCHAR* ip, UINT * port )
{
	//char szIP[64];
	//GetPrivateProfileStringA("WXMiddleWare","IP","192.168.10.193",szIP,sizeof(szIP),".\\IBAConfig\\IBA.ini");

	//*port = GetPrivateProfileInt(_T("WXMiddleWare"),_T("port"),8080,_T(".\\IBAConfig\\IBA.ini"));

	//in_addr addr;
	//addr.S_un.S_addr = inet_addr(szIP);
	//char* pszIp = inet_ntoa(addr);
	//_tcscpy(ip,CA2T(pszIp));

	//return TRUE;
	IBA_TRACE(_T("获取IP微信中间件IP和端口"));
	
	hostent* pHost = gethostbyname("notice.dodonew.com");
	if (pHost)
	{
		in_addr addr;
		char* pList = pHost->h_addr_list[0];
		if (!pList)
		{
			IBA_LOG0(_T("获取列表失败"));
			return FALSE;
		}

		memcpy(&addr.S_un.S_addr, pList, pHost->h_length);
		char* pszIp = inet_ntoa(addr);
		_tcscpy(ip, CA2T(pszIp));

		CConfigInfo configInfo;
		configInfo.SetKeyString(_T("noticeServerAddress"));
		if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
		{
			CUrl url;
			url.CrackUrl(configInfo.GetKeyValue3());
			*port = (UINT)url.GetPortNumber();

			return TRUE;
		}
	}
	else
	{
		IBA_LOG0(_T("获取域名入口失败。"));
	}

	return FALSE;
}

VOID CALLBACK CWxMiddlewareImpl::TimerProc(        //时间回调函数
							   HWND hwnd,         // handle to window
							   UINT uMsg,         // WM_TIMER message
							   UINT_PTR idEvent,  // timer identifier
							   DWORD dwTime       // current system time
							   )
{
	//std::vector<CheckinInfo>::iterator it = CWxMiddlewareImpl::GetInstance()->m_termsArray.begin();
	std::vector<OperationInfo>::iterator it = CWxMiddlewareImpl::GetInstance()->m_termsArray.begin();
	for (; it != CWxMiddlewareImpl::GetInstance()->m_termsArray.end(); ++it)
	{
		if (it->nTimerId == idEvent) 
		{
			KillTimer(NULL, idEvent);

			if (0 == it->nOperate)			// checkin
			{
				//从激活列表查找用户,找到或找不到都通知中间件结果. 
				CString strWhere;
				//strWhere.Format(_T("netId like '%%%s%%' and TermId='%s'"), it->strNetId.GetBuffer(), it->strTermId.GetBuffer());
				strWhere.Format(_T("netId like '%%%s%%' and TermId='%s'"), it->strNetId, it->strTermId);		

				if (CIBADAL::GetInstance()->GetTableRecordCount(_T("ActiveMember"), strWhere) > 0)
				{
					// 告知中间件
					CWxMiddlewareImpl::GetInstance()->CallLoginStatus(it->strTermId, 0);
				}
				else
				{
					// 告知中间件
					CWxMiddlewareImpl::GetInstance()->CallLoginStatus(it->strTermId, 1);
				}
			}
			else if (1 == it->nOperate)		// checkout  // 0:checkin  1:checkout 2:message
			{
				CString strWhere;
				//strWhere.Format(_T("netId like '%%%s%%' and TermId='%s'"), it->strNetId.GetBuffer(), it->strTermId.GetBuffer());
				strWhere.Format(_T("netId like '%%%s%%' and TermId='%s'"), it->strNetId, it->strTermId);

				if (CIBADAL::GetInstance()->GetTableRecordCount(_T("ActiveMember"), strWhere) > 0)
				{
					// 告知中间件
					CWxMiddlewareImpl::GetInstance()->CallLogoutStatus(it->strTermId, 1);
				}
				else
				{
					// 告知中间件
					CWxMiddlewareImpl::GetInstance()->CallLogoutStatus(it->strTermId, 0);
				}
				
			}
			else if (2 == it->nOperate)
			{
				CWxMiddlewareImpl::GetInstance()->CallMessageStatus(1);
			}
			else
			{
				continue;
			}

			GetInstance()->m_termsArray.erase(it);
		}
	}

	
}

BOOL CWxMiddlewareImpl::checkin( LPCTSTR lpszNetId, LPCTSTR lpszTermId, LPCTSTR lpszPassword )
{
	IBA_LOG(_T("接收微信中间件上机消息"));

	CComputerInfo computer ;
	if (!CLocalServer::GetInstance()->ComputerList.GetComputerInfo(lpszTermId, computer))
	{
		IBA_LOG(_T("上机时，从机器视图获取账号：%s对应的机器：%s失败"), lpszNetId, lpszTermId);
		this->CallLoginStatus(lpszTermId, 1);
		return FALSE;
	}
		
	//{ 2011/07/27-8201-gxx: 
	CActiveMember ActiveMember;
	if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(lpszNetId, ActiveMember, FALSE))
	{
		IBA_LOG(_T("上机时，从用户视图获取账号：%s对应的用户失败"), lpszNetId);
		this->CallLoginStatus(lpszTermId, 1);
		return FALSE;
	}

	// 2011/11/03-8210-gxx: 包房主账号上机
	if (ActiveMember.GetMemberID() == ActiveMember.GetRoomMemberId())
	{
		if (ActiveMember.GetPCClassID() != computer.GetPCClassID())
		{
			IBA_LOG(LOAD_STRING(IDS_ROOM_CHECKIN_ERROR1));
			this->CallLoginStatus(lpszTermId, 1);
			return FALSE;
		}
	}
		
	//添加到数据库
	CLastUserInfo LastUserInfo;
	LastUserInfo.SetNetId(lpszNetId);
	LastUserInfo.SetMemberId(ActiveMember.GetMemberID());
	LastUserInfo.SetTermId(computer.GetTerminalID());
	LastUserInfo.SetLoginStyle(1/*GetCheckIn() ? 1 : 0*/); //自动登录要求传强制登录参数
	LastUserInfo.SetUpdateTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
	LastUserInfo.SetPassWord(lpszPassword);
	
	CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
	CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);
	IBA_LOG(_T("Temp.CWxMiddlewareImpl::checkin AddLastUserInfo"));
			
	if (computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
	{
		INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.Find(computer.GetTerminalID(), computer.GetComputerIP());
		CLocalServer::GetInstance()->LocalAutoCheckIn(nIdx);
	}

	// 给UDO发送上机消息成功
	//CheckinInfo info;
	OperationInfo info;
	info.strNetId = lpszNetId;
	info.strTermId = lpszTermId;
	info.nOperate = 0;
	info.nTimerId = SetTimer(NULL, 0, 1000, TimerProc);
	m_termsArray.push_back(info);

	return TRUE;
}

BOOL CWxMiddlewareImpl::checkout( LPCTSTR lpszNetId, LPCTSTR lpszTermId )
{
	IBA_TRACE(_T("接收微信中间件下机消息"));
	IBA_LOG(_T("接收微信中间件下机消息"));

	//从机器视图获取终端,获取不到-return
	CComputerInfo computer;
	if (!CLocalServer::GetInstance()->ComputerList.GetComputerInfo(lpszTermId, computer))
	{
		IBA_LOG(_T("下机时，从机器视图获取账号：%s对应的机器：%s失败"), lpszNetId, lpszTermId);
		this->CallLogoutStatus(lpszTermId, 1);
		return FALSE;
	}

	//从激活列表获取用户,获取不到-return
	CActiveMember ActiveMember; //用户没钱自动锁屏，就是离线了
	if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(computer.GetMemberID(), ActiveMember))
	{
		IBA_LOG(_T("下机时，从用户视图获取账号：%s对应的用户失败"), lpszNetId);
		this->CallLogoutStatus(lpszTermId, 1);
		return FALSE;
	}

	//往CCheckOutinfo添加记录,结账时间为当前时间
	CCheckOutinfo CheckOutInfo;
	CheckOutInfo.SetCheckOutTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));
	CheckOutInfo.SetTerminalId(ActiveMember.GetTerminalID());
	CheckOutInfo.SetSerialNum(ActiveMember.GetNetId());
	CheckOutInfo.SetMemberId(ActiveMember.GetMemberID());
	CheckOutInfo.SetUserName(ActiveMember.GetUserName());
	CheckOutInfo.SetPersonalId(ActiveMember.GetPersonalID());
	CheckOutInfo.SetClassName(ActiveMember.GetUserClassName());
	CheckOutInfo.SetClassId(ActiveMember.GetClassId());
	CheckOutInfo.SetClassState(ActiveMember.IsMember());
	CheckOutInfo.SetBalanceMoney(ActiveMember.GetAvailavleBalance());
	CheckOutInfo.SetReturnState(1); // 设置状态为已退款

	BOOL bRes = CIBADAL::GetInstance()->AddCheckOutInfo(CheckOutInfo);
	if (!bRes)
	{
		IBA_LOG(_T("下机时，数据库更新失败，账号：%s对应的机器：%s"), lpszNetId, lpszTermId);
		this->CallLogoutStatus(lpszTermId, 1);
		return FALSE;
	}

	// 查询索引
	INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.Find(computer.GetTerminalID(), computer.GetComputerIP());
	if (nIdx < 0)
	{
		IBA_LOG(_T("下机时，查询机器索引失败，终端号：%s终端地址：%s"), computer.GetTerminalID(), computer.GetComputerIP());
		this->CallLogoutStatus(lpszTermId, 1);
		return FALSE;
	}

	// 通知客户端结账锁屏
	if (computer.GetComputerStatus() != CComputerInfo::ECS_LOCKED)
	{
		CLocalServer::GetInstance()->LocalCheckOutUser(nIdx, computer.GetMemberID(), 2);
		Sleep(2000);
		CIBADAL::GetInstance()->DeleteLastUserInfo(CheckOutInfo.GetTerminalId());
	}

	// 给UDO发送下机消息成功
	OperationInfo info;
	info.strNetId = lpszNetId;
	info.strTermId = lpszTermId;
	info.nOperate = 1;
	info.nTimerId = SetTimer(NULL, 0, 1000, TimerProc);
	m_termsArray.push_back(info);

	return TRUE;
}

void CWxMiddlewareImpl::showmsg(LPCTSTR lpszTermId, LPCTSTR lpszUserID, LPCTSTR lpszPayAmount, LPCTSTR lpszGiveAmount, LPCTSTR lpszTime)
{
	IBA_LOG0(_T("接收微信中间件显示消息"));

	static CWxMsgTips* pTips = NULL;
	if (pTips)
	{
		if (!IsWindow(pTips->GetSafeHwnd()))
			pTips = NULL;
	}

	if (!pTips)
	{
		pTips = new CWxMsgTips();
		if (!pTips->Create())
			pTips = NULL;
	}

	if (pTips)
	{
		pTips->SetUserID(lpszUserID);
		pTips->SetPayAmount(lpszPayAmount);
		pTips->SetGiveAmount(lpszGiveAmount);
		pTips->SetTermID(lpszTermId);
		pTips->SetTime(lpszTime);

		pTips->ShowRBCorner();
	}

	OperationInfo info;
	info.nOperate = 2;
	info.nTimerId = SetTimer(NULL, 0, 1000, TimerProc);
	m_termsArray.push_back(info);
}

void CWxMiddlewareImpl::LoadWxLibray()
{
	CString strWorkPath = CIBAConfig::GetInstance()->GetAppPath();
	strWorkPath.Append(_T("\\WXM.dll"));
	m_hMod = LoadLibrary(strWorkPath);

	if (m_hMod != NULL) {
		//AfxMessageBox(_T("load dll!"));
		m_procSetDelegate = (fnSetDelegateProc)::GetProcAddress(m_hMod, "SetDelegate");
		m_procLoginStatus = (fnLoginStatusProc)::GetProcAddress(m_hMod, "LoginStatus");
		m_procLogoutStatus = (fnLogoutStatusProc)::GetProcAddress(m_hMod, "LogoutStatus");
		m_procMessageStatus = (fnMessageStatusProc)::GetProcAddress(m_hMod, "MessageStatus");

		this->CallSetDelegate();

		m_thread.SetCallBack(this, Run);
		//m_thread.Start();
		//m_thread.Suspend();
	}
}

void CWxMiddlewareImpl::CallSetDelegate()
{
	m_procSetDelegate(this);
}

void CWxMiddlewareImpl::CallLoginStatus( LPCTSTR lpszTermId, int nStatus )
{
	m_procLoginStatus(lpszTermId, nStatus);
}

void CWxMiddlewareImpl::CallLogoutStatus(LPCTSTR lpszTermId, int nStatus)
{
	m_procLogoutStatus(lpszTermId, nStatus);
}

void CWxMiddlewareImpl::CallMessageStatus(int nStatus)
{
	m_procMessageStatus(nStatus);
}

void CWxMiddlewareImpl::Run()
{
	
}



