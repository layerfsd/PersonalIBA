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
#include "Servlet\CreditMethod.h"
#include "LocalCenter\gxx_base.h"
#include "LocalCenter\LocalCenter.h"
#include "Servlet\BuckleAmountMethod.h"
#include "GlobalFunctions.h"
#include "CashierHelper.h"
#include "ServletHellper.h"
#include "DdnDataX.h"
#include "RealName\RealNameDataX.h"
#include "RealName\IZd.h"
#include "RealName\RealNameBase.h"
#include "RealName\IBARealName.h"
#include "MainFrm.h"

//控制访问LocalMember以及LocalCredit表
CCriticalSection g_csLocalCreditLock;
CCriticalSection g_csLocalMemberLock;
//CCriticalSection g_csDisplay
//控制访问网络设置
CCriticalSection m_SettingNetworkStatusLock;

using namespace NS_SERVLET;
using namespace IBAGlobal;
using namespace NS_SERVLET;
//liyajun Temp
#ifdef _DEBUG
	static const UINT nTestingTime = 1000;  //1s钟测试一次
#else
	static const UINT nTestingTime = (60*1000)*1;  //1分钟测试一次
#endif

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
	m_threadTestBroken = new NS_COMMON::CThreadTemplate<CBrokenNetwork>;

	m_threadTestResume->SetCallBack(this, TestingResume);
	m_threadRestore->SetCallBack(this, TestingUploadLocalConsume);	
	m_threadTestBroken->SetCallBack(this,TestingTestBroken);

	//m_bHasFirstTrying = FALSE;
	bTestBrokenStarted = FALSE;
}

CBrokenNetwork::~CBrokenNetwork(void)
{
	UNIT_AUTO_TRACE("----------------断网维护对象CBrokenNetwork析构----------------");

	//UNIT_AUTO_TRACE("关闭线程");
	m_threadTestResume->Terminate();
	m_threadRestore->Terminate();
	m_threadTestBroken->Terminate();

	Sleep(1000);
	delete m_threadTestResume;
	delete m_threadRestore;
	delete m_threadTestBroken;
}

CBrokenNetwork* CBrokenNetwork::GetInstance()
{
	static CBrokenNetwork bn; //只有程序启动时才唯一分配，所以只有一个
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
/*************************************************
Function:       //RestoreNetwork
Description:    //设置网络为恢复模式
Calls:          //SettingNetworkStatus
Other:			//只能从正常模式到恢复模式
*************************************************/
void CBrokenNetwork::RestoreNetwork()
{
	CSingleLock lock(&m_SettingNetworkStatusLock,TRUE);

	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)//只能从Normal到Restore
	{
		IBA_LOG(_T("NetworkStatus => Restore"));
		SettingNetworkStatus(CIBAGlobal::emNetworkRestore);
	}	
}
/*************************************************
Function:       //DisableNetwork
Description:    //设置网络为本地模式,设置bHasResumedCenterUser为FALSE
Calls:          //SettingNetworkStatus
*************************************************/
void CBrokenNetwork::DisableNetwork()
{
	CSingleLock lock(&m_SettingNetworkStatusLock,TRUE);

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("NetworkStatus => Broken"));
		SettingNetworkStatus(CIBAGlobal::emNetworkBroken);
		bHasResumedCenterUser = FALSE;
	}	
}
/*************************************************
Function:       //EnableNetwork
Description:    //设置网络为正常模式，检查IBA是否本地登录的，如是则重新执行登陆
Calls:          //SettingNetworkStatus
*************************************************/
void CBrokenNetwork::EnableNetwork()
{
	CSingleLock lock(&m_SettingNetworkStatusLock,TRUE);

	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)
	{
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)//只有从Broken恢复时，才打印日志，方便查日志
		{
			IBA_LOG(_T("NetworkStatus => Normal"));
		}		
		SettingNetworkStatus(CIBAGlobal::emNetworkNormal);
	}

	if (theApp.GetCurCashier()->m_bIsLocalModeLogin)
	{
		IBA_TRACE("网络恢复后，自动重新登录...");

		CCashRegisterLoginMethod crLogin;
		theApp.GetCurCashier()->CheckIn(crLogin, theApp.GetCurCashier()->GetName());

		if (crLogin.GetStatusCode() != 0)
		{
			IBA_LOG(L"网络恢复后，重新登录失败!");
		}
	}
}

//探测网络是否断开，断开返回FALSE,否则返回TRUE
BOOL CBrokenNetwork::TestingNetwork()
{
	CGetTimeMethod time;
	time.Execute();

	return time.GetStatusCode() >= 0 ;
}

/*************************************************
Function:       // TestingResume
Description:    // 线程函数。探测网络是否已恢复一定时间，如果是则设置网络为已恢复，并开启断网补传线程。
				// 网络断开后DisableNetwork时或程序启动时载入上次网络状态为断网时启动该线程。网络恢复后，结束该线程。
Calls:          // TestingNetwork,EnableNetwork,TestingUploadLocalConsume
Return:         // 网络恢复正常一定时候后，返回
Others:         // 线程结束后，下次符合条件时又会重新启动，但使用不同的线程ID
*************************************************/
void CBrokenNetwork::TestingResume()
{
	IBA_LOG(_T("Start thread:Test network resume"));
#ifdef _DEBUG
	const DWORD minRestoreTick = 500;
#else
	const DWORD minRestoreTick = 3 * 60 * 1000;// 网络至少恢复3分钟了，才真正认为网络已恢复
	//const DWORD minRestoreTick = 500;
#endif

	CoInitialize(NULL);
	DWORD dwTickcout = 0;

	while(TRUE)
	{
		if (TestingNetwork())            //探测网络是否正常
		{
			if (dwTickcout == 0)
			{
				dwTickcout = GetTickCount();
			}
			else
			{
				if (GetTickCount() - dwTickcout >= minRestoreTick)   //离第一次探测网络正常>=某个数值
				{
					IBA_TRACE(_T("网络已恢复"));
					IBA_LOG(_T("网络已恢复"));
					//只有当localMember有记录时，才显示开始上报本地模式
					CString sql;
					sql.Format(_T("select count(*) from localMember"));
					if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0)
					{
						COperationInfo OperationInfo ;
						OperationInfo.SetDescription (_T("开始上报本地模式操作数据..."));
						SendMessage(CMainFrame ::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM )(&OperationInfo));
					}
					EnableNetwork();                                //设置网络状态正常
					GetInstance()->m_threadRestore->Start();        //开始执行断网恢复线程
					return ;
				}
			}
		}
		else
		{
			dwTickcout = 0;
		}


#ifdef _DEBUG
		//Sleep(15000);
		Sleep(1000);
#else
		Sleep(nTestingTime);
		//Sleep(1000);
#endif 

	}
	CoUninitialize();
}

/*************************************************
Function:       //TestingUploadLocalConsume
Description:    //从本地模式恢复到正常模式后,为本地模式下的操作进行上报工作. - 只上报不在线的用户。
Table Accessed: //LocalMember, LocalCredit,CheckOutInfo
Table Updated:  //LocalMember, LocalCredit
Others:         //线程一直运行不退出
*************************************************/
void CBrokenNetwork::TestingUploadLocalConsume()
{
	IBA_LOG(_T("Start thread:Upload local mode data for offline users"));
	CoInitialize(NULL);

	while(TRUE)
	{
#ifdef _DEBUG
		Sleep(30000);
#else
		Sleep(nUploadTime);  //2分钟
#endif	

		CSingleLock lock(&m_TestUploadLock, TRUE);

		//是否上报了任何记录
		bool bHasResumedSth(FALSE);

		//网络未恢复，不工作
		if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)
		{
			continue;
		}

		//遍历本地库中(LocalMember)的记录
		GxxArrayPtr localNotReturnedArrayPtr = CIBADAL::GetInstance()->ReadRecordsFromDB(_T("select * from localMember"));
		for (int i = 0; i < localNotReturnedArrayPtr->count(); i++)                        
		{
			GxxDictionaryPtr memberPtr = localNotReturnedArrayPtr->valueAtIndex(i)->getDictionary();
			UINT nMemberId = memberPtr->valueForKey("memberId")->intValue();
			CLocalMember lMember;
			CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,lMember);
			//在线的用户,不在此处进行上报
			CActiveMember ActiveMember;
			if(CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember))//
			{
				if(ActiveMember.GetIsOnLine())
				{
					continue;
				}
			}

			//代表上传了数据
			bHasResumedSth = TRUE;

			//上报本地充值
			CCashierHelper::ReportLocalCredit(memberPtr);

			//是否结账
			if(lMember.nIsReturned)//已结帐
			{
				//是否本地模式下登陆
				if(lMember.nIsLocalCheckin)//本地模式下登陆
				{
					//上报本地消息
					CCashierHelper::ReportLocalConsume(lMember);
					if(CNetBarConfig::GetInstance()->GetIsMember(lMember.nClassId))//会员-取消激活
					{
						CActivationMethod Activation;
						Activation.SetMemberId(lMember.nMemberId);
						Activation.SetMode(CActivationMethod::ACTIVATION_NO);
						theApp.GetCurCashier()->DoActivation(Activation);
					}
					else//临时用户-上报本地退款
					{
						CCashierHelper::ReportLocalReturn(lMember);
					}
				}
				else//正常模式下登陆
				{
					if(CNetBarConfig::GetInstance()->GetIsMember(lMember.nClassId))//会员-上报本地结账
					{
						CCashierHelper::ReportLocalReturnVip(lMember);
					}
					else//临时用户-上报本地退款
					{
						CCashierHelper::ReportLocalReturn(lMember);
					}
				}
			}
			else//未结账
			{
				//查询余额
				CIDCheckCardMethod CheckCard;
				CString strTmp = lMember.strNetId;
				IBA_LOG(_T("strTmp = %s"),strTmp);
				if (strTmp.GetLength() < 10)
				{
					strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
				}
				CheckCard.SetSerialNum(strTmp);
				theApp.GetCurCashier()->DoCheckCard(CheckCard);

				if(CheckCard.GetStatusCode() == 0)
				{
					int nCardId = CheckCard.GetCardId();
					CBalanceMethod Balance;
					Balance.SetMemberId(nMemberId);
					Balance.SetCardId(nCardId);
					//Balance.SetRound(0);

					theApp.GetCurCashier()->DoBalance(Balance);
				}
				//激活
				CActivationMethod Activation;

				Activation.SetMemberId(lMember.nMemberId);
				Activation.SetMode( CActivationMethod::ACTIVATION_YES);
				//Activation.SetDotType(nActivationType);
				theApp.GetCurCashier()->DoActivation(Activation);
			}

			//更新用户视图
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

			//删除用户记录
			if(FALSE == RemoveLocalMember(nMemberId))
			{
				IBA_LOG(_T("Serious Error!删除本地库记录失败!程序将退出."));
				TerminateProcess(GetCurrentProcess(),0);
			}
			Sleep(500);
		}

		//检查本地库,都没有记录了,就弹出上报完成日志
		GxxArrayPtr localArrayPtr = CIBADAL::GetInstance()->ReadRecordsFromDB(_T("select * from localMember"));
		if(0 == localArrayPtr->count() && bHasResumedSth)
		{
			COperationInfo OperationInfo ;
			OperationInfo.SetDescription (_T("上报本地模式数据完成!"));
			SendMessage(CMainFrame ::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM )(&OperationInfo));	
		}
	}
	CoUninitialize();
}

/*************************************************
Function:       // SettingNetworkStatus
Description:    // 设置网络状态.CIBAGlobal::NetworkStatus以及表SystemConfig.localMode.更新网络状态栏显示.
Calls:          // None
Table Accessed: // None
Table Updated: //  SystemConfig, 如果网络设置成断开，localModeStatus字段设为1，否则设为0
Input:          // 要设置成的网络状态 0-Normal, 1-Broken, 2-Restore
Output:         // None
Return:         // None
Others:         // None
*************************************************/
void CBrokenNetwork::SettingNetworkStatus(CIBAGlobal::emNetworkStatus status)
{
	/*IBA_LOG(_T("NetworkStatus => %s"),status?_T("Broken"):_T("Normal"));*/
	CIBAGlobal::NetworkStatus = status;
	//UINT nStatus(0);
	//if(CIBAGlobal::emNetworkBroken == status)
	//{
	//	nStatus = 1;
	//}
	//else
	//{
	//	nStatus = 0;
	//}
	CConfigInfo configInfo;
	configInfo.SetKeyString(_T("localModeStatus"));
	configInfo.SetKeyValue1(status);
	CIBADAL::GetInstance()->UpdateSystemConfig(configInfo);

	//让Restore模式显示尝试连接字样
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
	{
		theApp.UpdateIBAStatus(TRUE);
	}
	else
	{
		theApp.UpdateIBAStatus();
	}	
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

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken )
	{
		GetInstance()->DisableNetwork();
		GetInstance()->m_threadTestResume->Start();
	}
	//启动时，恢复模式视为正常模式，让其自动再次进入正常模式
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
	{
		GetInstance()->EnableNetwork();
	}

	return CIBAGlobal::NetworkStatus;
}

/*************************************************
Function:       //RemoveLocalMember
Description:    //从LocalMember中删除记录(先检查表中有无记录存在)
Calls:          //...
Table Accessed: //LocalMember
Table Updated:  //LocalMember
Input:          //MemberId - 待删除记录的MemberId
Return:         //删除成功返回TRUE
Others:         //
*************************************************/
BOOL CBrokenNetwork::RemoveLocalMember(UINT MemberId)
{
	IBA_LOG(_T("CBrokenNetwork::RemoveLocalMember - Start for MemberId = %d"),MemberId);
	CSingleLock lock(&g_csLocalMemberLock,TRUE);

	CString sql;

	sql.Format(_T("select * from localMember where memberId=%d"),MemberId);
	if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) < 1 )
	{
		lock.Unlock();
		IBA_LOG(_T("CBrokenNetwork::RemoveLocalMember - End. No record in table whose MemberId = %d"),MemberId);
		return TRUE;
	}

	sql.Format(_T("delete from localMember where memberId=%d"), MemberId);

	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) <1)
	{
		IBA_LOG(_T("CBrokenNetwork::RemoveLocalMember - Result is FALSE for MemberId = %d"),MemberId);
		lock.Unlock();
		GXX_ASSERT(0,"从localMember删除数据失败！");
		return FALSE;
	}

	IBA_LOG(_T("CBrokenNetwork::RemoveLocalMember - Result is TRUE for MemberId = %d"),MemberId);
	lock.Unlock();
	return TRUE;
}

/*************************************************
Function:       // TestingTestBroken
Description:    // 线程函数。当请求中心失败进入Restore模式后，开启探测网络是否真的断开(超过1分钟都请求失败).如果是就进入本地模式.进入本地模式或正常模式后退出线程. 
*************************************************/
void CBrokenNetwork::TestingTestBroken()
{
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		return;
	}

	IBA_LOG(_T("Start thread:Test network Broken"));	

	const DWORD minTimeTick = 1 * 60 * 1000;//尝试1分钟,如果网络仍然未恢复,才判断是否进入本地
	const DWORD nInterval = 1 * 1000; //尝试间隔为1分钟

	CoInitialize(NULL);
	DWORD dwTickcout = GetTickCount();   //开始计时时间
	DWORD dwTickcoutPast = 0;            //已测试时间
	DWORD dwTickcoutCurr;                //当前时间

	while(TRUE)
	{

		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			return;
		}
		CGetTimeMethod time;
		time.Execute();

		if(time.GetStatusCode() >= 0)
		{
			//成功->进入正常模式
			EnableNetwork();
			bTestBrokenStarted = FALSE;
			return;
		}
		else
		{
			//失败->判断是否已到1分钟
			dwTickcoutCurr = GetTickCount();
			dwTickcoutPast = dwTickcoutCurr - dwTickcout;
			if(dwTickcoutPast > minTimeTick)//超过1分钟,弹出窗口进行选择
			{
				//只需判断MainFrame是否已经创建		
				if(theApp.m_pMainWnd != NULL)
				{
					CIBAString strTemp;
					strTemp.LoadString(IDS_ENTERLOCALMODE_QUERY);
					while(true)//弹出询问是否进入本地模式循环
					{
						CMsgBoxDlg dlg(theApp.m_pMainWnd);
						dlg.SetText(strTemp,FALSE);
						dlg.SetType(MB_RETRYCANCEL|MB_ICONEXCLAMATION);

						if(IDENTERLOCAL == dlg.DoModal())//进入本地模式
						{
							if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
							{
								IBA_TRACE("与中心的网络连接已断开");

								CString strCap;
								CIBAString strCaption;
								strCaption.LoadString(IDS_LOCALMODE_CAPTION);
								CIBAString strTmp;
								strTmp.LoadString(IDS_LOCALMODE_INFO);

								CMsgBoxDlg dlg(theApp.m_pMainWnd);
								//收银台与计费中心的网络已中断，将进入本地模式。请注意以下几点：\n\n1.收银台只能使用激活，充值退款，结账功能，不允许新开用户\n2.新激活用户原来账户中的金额将不可见，只能在本地模式下充值使用，已经激活的用户不受影响\n3.客户端只能查看余额，扣率，请勿随意上下机，不允许更换机器\n4.网络恢复后，请稍等几分钟，待完成补传本地模式下数据后，再使用\n
								dlg.SetText(strTmp,FALSE);
								dlg.SetType(MB_OK|MB_ICONQUESTION);
								dlg.SetCaption(strCaption);

								dlg.DoModal();

								DisableNetwork(); 
								IBA_LOG(L"m_threadTestResume => Start");
								m_threadTestResume->Start();//开启线程。网络恢复后，为不在线用户补传本地记录线程函数，网络确定恢复以后启动。只有在网络状态为正常后才启动。恢复完后从localMember删除记录。

								bTestBrokenStarted = FALSE;
								return;
							}
							break;//出口1
						}
						else //重新尝试3次,成功则进入正常模式,失败继续返回循环
						{
							for(int i = 0; i<30 ; ++i)
							{					
								Sleep(1000);
								CGetTimeMethod time;
								time.Execute();

								if(time.GetStatusCode() >= 0)
								{
									EnableNetwork();
									bTestBrokenStarted = FALSE;
									return;
								}
							}
						}
					}
				}
			}
		}

		Sleep(nInterval);

	}
	CoUninitialize();
}