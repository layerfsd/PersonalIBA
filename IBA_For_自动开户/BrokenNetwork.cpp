#include "stdafx.h"
#include ".\brokennetwork.h"
#include "Dal\ConfigInfo.h"
#include "Dal\IBADAL.h"
#include "IBA.h"
#include "Socket\LocalServer.h"
#include "Servlet\CommonMethod.h"
#include "BrokenNetworkResume.h"
#include "MainFrm.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "Servlet\LocalConsumeMethod.h"
#include "MsgBoxDlg.h"
#include "BossPwdDlg.h"
#include "Dal\RightsManagement.h"

using namespace NS_SERVLET;

static const UINT nTestingTime = (60*1000)*1;  //1分钟测试一次
static const UINT nUploadTime = (60*1000)*2;   //2分钟上传一次
static BOOL bHasResumedCenterUser = FALSE;      //是否已经补传了断网后的中心用户的结账信息。

static const TCHAR* lpKeyMemberID = _T("LocalModeConfig_1");  // 本地MemberID的累计
static const TCHAR* lpKeyCasher = _T("LocalModeConfig_2");
static const TCHAR* lpKeyControlOption = _T("LocalMode_ControlOption");
static const TCHAR* lpKeyCasherAccess = _T("LocalMode_Access"); // 收银员访问权限
static const TCHAR* lpKeyLocalModeStatus = _T("LocalModeStatus"); // 本地模式的状态


CBrokenNetwork::CBrokenNetwork(void)
{
	m_threadTestResume = new NS_COMMON::CThreadTemplate<CBrokenNetwork>;
	m_threadRestore = new NS_COMMON::CThreadTemplate<CBrokenNetwork>;

	m_threadTestResume->SetCallBack(this, TestingResume);
	m_threadRestore->SetCallBack(this, TestingUploadLocalConsume);

	m_bHasFirstTrying = FALSE;
}

CBrokenNetwork::~CBrokenNetwork(void)
{
	UNIT_AUTO_TRACE("----------------断网维护对象CBrokenNetwork析构----------------");

	try
	{
		UNIT_AUTO_TRACE("关闭线程");
		m_threadTestResume->Terminate();
		m_threadRestore->Terminate();

		Sleep(1000);
		delete m_threadTestResume;
		delete m_threadRestore;
	}
	catch (...)
	{
		
	}
	
}

CBrokenNetwork* CBrokenNetwork::GetInstance()
{
	static CBrokenNetwork bn;
	return &bn;
}

BOOL CBrokenNetwork::SaveLocalStatusInfo(CLocalStatusInfo& Info)
{
	CConfigInfo ConfigInfo;
	CString strTmp;

	// 收银员权限字符串
	strTmp = Info.GetCasherAccess();
	ConfigInfo.Clear();
	ConfigInfo.SetKeyString(lpKeyCasherAccess);
	ConfigInfo.SetKeyValue3(strTmp);
	CIBADAL::GetInstance()->UpdateSystemConfig(ConfigInfo,3);

	// 网吧控制参数
	ConfigInfo.Clear();
	ConfigInfo.SetKeyString(lpKeyControlOption);
	strTmp = Info.GetControlParam();
	ConfigInfo.SetKeyValue3(strTmp);
	CIBADAL::GetInstance()->UpdateSystemConfig(ConfigInfo,3);

	return TRUE;
}

BOOL CBrokenNetwork::LoadLocalStatusInfo()
{
	CConfigInfo info;

	// 收银员权限字符串
	info.Clear();
	info.SetKeyString(lpKeyCasherAccess);
	if (CIBADAL::GetInstance()->GetSystemConfig(info))
	{
		CString strPri = info.GetKeyValue3();
		theApp.GetCurCashier()->m_strEnableModuleList = strPri;
	}

	// 网吧控制参数
	info.Clear();
	info.SetKeyString(lpKeyControlOption);

	if (CIBADAL::GetInstance()->GetSystemConfig(info))
	{
		CString strPri = info.GetKeyValue3();
		CNetBarConfig::GetInstance()->PraseControlOption(strPri);	
	}

	return TRUE;
}

UINT CBrokenNetwork::RegisterMemberID()
{
	CConfigInfo configInfo;
	configInfo.Clear();
	configInfo.SetKeyString(lpKeyMemberID);

	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		UINT nID = configInfo.GetKeyValue1();
		ASSERT(nID >= 0x0FFFFFFF);
		nID -= 1;
		configInfo.SetKeyValue1(nID);
		CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
		return nID;
	}
	else
	{
		CConfigInfo configInfo;
		configInfo.SetKeyString(lpKeyMemberID);
		configInfo.SetKeyValue1((UINT)-1);
		CIBADAL::GetInstance()->UpdateSystemConfig(configInfo, 1);
		return (UINT)-1;
	}
}

void CBrokenNetwork::DisableNetwork()
{
	static INT s_nCountOfNo = 0;
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		switch(CNetBarConfig::GetInstance()->GetLocalModeStartKind())
		{
		case CIBAGlobal::emLocalModeForbid:
			return; // 禁用就无法进入应急模式

		case CIBAGlobal::emLocalModeManual:
			{
				CDlgBossPwdEx dlg;
				if (dlg.DoModal() != IDOK)
				{
					return;
				}	
			}
			break;
		case CIBAGlobal::emLocalModeAuto:
			{
				if (s_nCountOfNo > 0)
				{
					return ;
				}

				CMsgBoxDlg dlg;
				CIBAString strTmp;
				strTmp.LoadString(IDS_LOCALMODE_INFO);
				dlg.SetText(strTmp,FALSE);//_T("收银台与中心的网络连接已中断，是否进入应急模式?")
				dlg.SetType(MB_YESNO|MB_ICONQUESTION);

				UINT nRet = dlg.DoModal();

				//UINT nRet = //theApp.IBAMsgBox(_T("收银台与中心的网络连接已中断，是否进入应急模式?"),MB_YESNO|MB_ICONQUESTION);
				if (nRet == IDNO)
				{
					s_nCountOfNo++;
					return;
				}
			}
			break;
		}

		GetInstance()->m_threadTestResume->Start();
	}
	SettingNetworkStatus(CIBAGlobal::emNetworkBroken);

	// 2011/08/24-8201-gxx: 更新快速通道栏的右边的状态显示.
	theApp.UpdateIBAStatus();

	bHasResumedCenterUser = FALSE;
}

void CBrokenNetwork::EnableNetwork()
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		if( CLocalServer::GetInstance()->ActiveMemberList.HasLocalUser() )
		{
			SettingNetworkStatus(CIBAGlobal::emNetworkRestore);
			
		}
		else
		{
			SettingNetworkStatus(CIBAGlobal::emNetworkNormal);
		}
		
		theApp.UpdateIBAStatus();
	}

	GetInstance()->m_bHasFirstTrying = FALSE; // 使得下一次断网时，toolbar能显示"正在尝试网络连接"字样

	if ( !bHasResumedCenterUser )
	{
		if (CBrokenNetwork::GetInstance()->m_centerUserUpLoad.CheckShouldResume())
		{
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.SetShouldResume(TRUE);
			CBrokenNetwork::GetInstance()->m_centerUserUpLoad.StartResume(0);
		}
		bHasResumedCenterUser = TRUE;
	}
	
	if (theApp.GetCurCashier()->m_bIsLocalModeLogin)
	{
		UNIT_AUTO_TRACE("网络恢复后，自动重新登录...");

		CCashRegisterLoginMethod crLogin;
		theApp.GetCurCashier()->CheckIn(crLogin, theApp.GetCurCashier()->GetName());

		if (crLogin.GetStatusCode() != 0)
		{
			CIBALog::GetInstance()->Write(_T("网络恢复后，重新登录失败!"));
		}
	}
}


BOOL CBrokenNetwork::TestingNetwork()
{
	CGetTimeMethod time;
	time.Execute();

	return time.GetStatusCode() >= 0 ;
}

void CBrokenNetwork::TestingResume()
{
	CoInitialize(NULL);
	while(TRUE)
	{
#ifdef _DEBUG
		Sleep(15000);
#else
		Sleep(nTestingTime);
#endif // _DEBUG
		
		try
		{
			if (TestingNetwork())
			{
				EnableNetwork();
				GetInstance()->m_threadRestore->Start();
				return ;
			}
		}
		catch (...)
		{
		}
		
	}
	CoUninitialize();
}

void CBrokenNetwork::TestingUploadLocalConsume()
{
	CoInitialize(NULL);

	while(TRUE)
	{
#ifdef _DEBUG
		Sleep(30000);
#else
		Sleep(nUploadTime);
#endif	

		CSingleLock lock(&m_TestUploadLock, TRUE);

		try
		{
			if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
			{
				continue;
			}

			CArray<CLocalConsumeInfo,CLocalConsumeInfo&> ArrayConsumeInfo;
			CIBADAL::GetInstance()->GetLocalConsumes(ArrayConsumeInfo);

			if( !CLocalServer::GetInstance()->ActiveMemberList.HasLocalUser() && 
				CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore &&
				ArrayConsumeInfo.GetCount() == 0)
			{
				SettingNetworkStatus(CIBAGlobal::emNetworkNormal); // 没有本地用户了，可将网络状态改为正常状态
				theApp.UpdateIBAStatus();
			}

			INT nUploadCount = 0;
		
			for (int i = 0; i < ArrayConsumeInfo.GetCount(); i++)
			{
				if (!theApp.GetCurCashier()->IsOnDuty())
				{
					continue;
				}

				CLocalConsumeInfo& Info = ArrayConsumeInfo.GetAt(i);

				CLocalConsumeMethod ConsumeMethod;
				ConsumeMethod.SetCashRegisterId( CIBAConfig::GetInstance()->GetCashRegisterID() );
				ConsumeMethod.SetCheckinTime( CIBAHelpper::TranformTime(Info.GetCheckinTime()) );
				ConsumeMethod.SetCheckoutTime( CIBAHelpper::TranformTime(Info.GetCheckoutTime()) );
				ConsumeMethod.SetClassId( Info.GetClassId() );
				ConsumeMethod.SetConsumeAmount( Info.GetConsumeAmount() );
				ConsumeMethod.SetCreditAmount( Info.GetCreditAmount() );
				ConsumeMethod.SetDataStatus( Info.GetDataStatus() );
				ConsumeMethod.SetDutyId( theApp.GetCurCashier()->GetDutyID() );
				ConsumeMethod.SetIdNumber( Info.GetIdNumber() );
				ConsumeMethod.SetMemberId( Info.GetMemberId() );
				ConsumeMethod.SetNetBarId( Info.GetNetbarId() );
				ConsumeMethod.SetOperator( theApp.GetCurCashier()->GetName() );
				ConsumeMethod.SetPayType( Info.GetPayType() );
				ConsumeMethod.SetPcClass( Info.GetPCClass() );
				ConsumeMethod.SetReturnAmount( Info.GetReturnAmount() );
				ConsumeMethod.SetSerialNum( Info.GetSerialNum() );
				ConsumeMethod.SetTermId( Info.GetTermId() );
				ConsumeMethod.SetUserName( Info.GetName() );

				if (!theApp.GetCurCashier()->DoLocalConsume(ConsumeMethod))
				{		
					CIBALog::GetInstance()->WriteFormat(_T("补传本地消费记录失败,memberID=%d, netID=%s"), (int)Info.GetMemberId(),
						Info.GetSerialNum());
				}
				else if(ConsumeMethod.GetStatusCode() < 0)
				{
					CIBALog::GetInstance()->WriteFormat(_T("补传本地消费信息时，网络又中断了"));
				}
				else if(ConsumeMethod.GetStatusCode() > 0)
				{
					if (ConsumeMethod.GetStatusCode() == ESC_SWSB73)
					{
						CIBALog::GetInstance()->WriteFormat(_T("补传本地消费记录失败，原因:收银员被强制下班"));
						continue;
					}
					CIBALog::GetInstance()->WriteFormat(_T("补传本地消费记录失败,memberID=%d, netID=%s,记录被删除"), (int)Info.GetMemberId(),
						Info.GetSerialNum());
					
					CIBADAL::GetInstance()->DeleteLocalConsume(Info.GetMemberId());
				}
				else
				{
					if (!CIBADAL::GetInstance()->DeleteLocalConsume(Info.GetMemberId()))
					{
						CIBALog::GetInstance()->WriteFormat(_T("从数据库中清除本地消费记录失败,memberID=%d, netID=%s"), (int)Info.GetMemberId(),
							Info.GetSerialNum());
					}
					
					// 写入Member表
					if (ConsumeMethod.GetMemberID() > 0)
					{
						CIBADAL::GetInstance()->UpdateMemberInfo(Info, ConsumeMethod.GetMemberID());
					}
					
					COperationInfo OpInfo;
					OpInfo.SetOperationType( CIBAGlobal::emOpTypeLocalReturn );
					OpInfo.SetAllAmount( Info.GetConsumeAmount() + Info.GetReturnAmount() );
					OpInfo.SetMemberId( Info.GetMemberId() );
					OpInfo.SetNetId( Info.GetSerialNum() );
					OpInfo.SetOperationAmount( Info.GetReturnAmount() );
					OpInfo.SetOperationTime( COleDateTime::GetCurrentTime() );
					OpInfo.SetDescription(_T("断网恢复后补传消费信息给中心"));
					OpInfo.SetUserName( Info.GetName() );
					OpInfo.SetIsResumeData(TRUE);

					::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OpInfo);
				}

				nUploadCount++;

				Sleep(1000); // 等待1秒，防止中心处理过忙
			}

			if ( CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal )
			{
				CoUninitialize();
				return;
			}

			if (nUploadCount == ArrayConsumeInfo.GetCount() && 
				CLocalServer::GetInstance()->ActiveMemberList.HasLocalUser() == FALSE)
			{
				// 本地消费信息全部上传完毕并且用户视图没有本地用户
				SettingNetworkStatus(CIBAGlobal::emNetworkNormal);
				theApp.UpdateIBAStatus();
				CoUninitialize();
				return ;
			}

		}
		catch (...)
		{
			CoUninitialize();
			return;
		}
	}
	CoUninitialize();
}


void CBrokenNetwork::SettingNetworkStatus(CIBAGlobal::emNetworkStatus status)
{
	CIBAGlobal::NetworkStatus = status;
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("localModeStatus"));
	configInfo.SetKeyValue1(status);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);
}

CIBAGlobal::emNetworkStatus CBrokenNetwork::LoadLocalModeStatus()
{
	CConfigInfo configInfo;
	configInfo.SetKeyString(lpKeyLocalModeStatus);
	
	if (CIBADAL::GetInstance()->GetSystemConfig(configInfo))
	{
		switch(configInfo.GetKeyValue1())
		{
		case 0:
			CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkNormal;
			break;
		case 1:
			CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkBroken;
			break;
		case 2:
			CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkRestore;
			break;
		default:
			CIBAGlobal::NetworkStatus = CIBAGlobal::emNetworkNormal;
			break;
		}
	}

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		GetInstance()->DisableNetwork();
		GetInstance()->m_threadTestResume->Start();
	}
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
	{
		GetInstance()->m_threadTestResume->Start();
	}

	return CIBAGlobal::NetworkStatus;
}

