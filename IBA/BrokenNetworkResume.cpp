#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "dal\IBADAL.h"
#include "RealName\IBARealName.h"
#include "MainFrm.h"
#include "IBAHelpper.h"
#include ".\brokennetworkresume.h"
#include "Socket\LocalServer.h"
#include "ComputerList.h"
#include "ComputerListView.h"
//#include "CommandProcessor.h"


CBrokenNetworkResume::CBrokenNetworkResume(void)
{
	m_bShouldResume = FALSE;
}

CBrokenNetworkResume::~CBrokenNetworkResume(void)
{
}

BOOL CBrokenNetworkResume::Init()
{
	return (m_bShouldResume = CheckShouldResume());
}

void CBrokenNetworkResume::SetShouldResume(BOOL bResume)
{
	IBA_LOG(_T("m_bShouldResume => %d"),(int)bResume);
	m_bShouldResume = bResume;
}

BOOL CBrokenNetworkResume::GetShouldResume()
{
	return m_bShouldResume;
}

/*************************************************
Function:       // CheckShouldResume
Description:    // 检查CheckOutInfo表中是否有记录
Calls:          // CIBADAL::GetCheckOutInfo(CCheckOutInfoArray& CheckOutArray)
Table Accessed: // CheckOutInfo
Table Updated: // None
Input:          // None
Output:         // None
Return:         // 表中有记录，返回TRUE，无记录，返回FALSE
Others:         // 其它说明
*************************************************/
BOOL CBrokenNetworkResume::CheckShouldResume()
{
	BOOL bRet = FALSE;

	CCheckOutInfoArray CheckOutArray;
	
	CIBADAL::GetInstance()->GetCheckOutInfo(CheckOutArray);
	
	if (CheckOutArray.GetCount() > 0)
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*************************************************
Function:       // StartResume
Description:    // 函数功能、性能等的描述
Calls:          //  CBrokenNetworkResume::Run
*************************************************/
void CBrokenNetworkResume::StartResume(INT nConnectionState)
{
	if (nConnectionState == -1)
		return;

	if (!m_bShouldResume)
		return;	

	m_bShouldResume = FALSE;
	IBA_LOG(_T("m_bShouldResume => FALSE"));

	Start();
}

/*************************************************
Function:       //Run
Description:    //为微信下机指令补上报会员下机(实名下机,刷新用户信息,用户视图,终端信息,机器视图)
Table Updated:  //CheckOutInfo
Return:         //0
Others:         //正常模式下,收银员Checkin,DoOnduty,DoRegister,DoCredit中如发现CheckOutInfo表中有记录则会启动
*************************************************/
INT CBrokenNetworkResume::Run()
{
	IBA_LOG(_T("Start thread:CBrokenNetworkResume"));

	CoInitialize(NULL);

	CSingleLock lock(&m_TestUploadLock, TRUE);   //锁定临界资源m_TestUploadLock

	//未上班or非正常模式,退出
	if (!theApp.GetCurCashier()->IsOnDuty() || CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)
	{
		theApp.IBAMsgBox(IDS_STRING326);
		CoUninitialize();
		return 0;
	}

	//获取CheckOutInfo表中记录
	CCheckOutInfoArray CheckOutArray;

	if(FALSE==CIBADAL::GetInstance()->GetCheckOutInfo(CheckOutArray))
	{
		IBA_LOG(_T("Abnormal.查询CCheckoutInfo表出错"));
		GXX_ASSERT(0,"查询CCheckoutInfo表出错");
		CoUninitialize();
		return 0;
	}

	//遍历CheckOutInfo表中记录
	INT i = 0;
	for (i = 0; i < CheckOutArray.GetCount(); i++)
	{
		CCheckOutinfo & CheckOutinfo = CheckOutArray.GetAt(i);

		//结账时用户不在线,取消激活
		if(CheckOutinfo.GetTerminalId().IsEmpty())
		{
			CActivationMethod Activation;
			Activation.SetMemberId(CheckOutinfo.GetMemberId());
			Activation.SetMode(CActivationMethod::ACTIVATION_NO);
			theApp.GetCurCashier()->DoActivation(Activation);
		}
		else//在线,执行Checkout2
		{
			CCheckout2Method Checkout2;
			Checkout2.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			Checkout2.SetMemberId(CheckOutinfo.GetMemberId());
			Checkout2.SetSafeLogout(0); 
			Checkout2.SetEndUsingTime(CheckOutinfo.GetCheckOutTime());
			Checkout2.SetOperator(theApp.GetCurCashier()->GetName());

			BOOL bRes = Checkout2.ExecuteCheckout2();
			CString strOperationLogMsg;
			if(bRes)
			{
				strOperationLogMsg.Format(_T("上报成功"));
			}
			else
			{
				strOperationLogMsg.Format(_T("上报失败！ Error: %s"),Checkout2.GetStatusMessage());
				//continue;
			}

			//写操作日志
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeMakeupLocalReturnVip);
			OperationInfo.SetUserName(CheckOutinfo.GetUserName());
			OperationInfo.SetNetId(CheckOutinfo.GetSerialNum());
			OperationInfo.SetOperationAmount(CheckOutinfo.GetBalanceMoney());
			OperationInfo.SetDescription(strOperationLogMsg);
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));	

			INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(Checkout2.GetMemberId());
			if (nIndex >= 0)
			{
				//实名下机
				CActiveMember ActiveMember;
				CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(CheckOutinfo.GetMemberId(),ActiveMember);
				CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
				theApp.GetRealName()->AuditUserCheckOut(computer.GetTerminalID(), computer.GetComputerIP(), Checkout2.GetMemberId());
				//智多实名
				if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
				{
					NS_REALNAME::CheckoutInfo coi;
					ZeroMemory(&coi, sizeof(coi));
					CString strName,strSex;
					strSex = ActiveMember.GetSex()?_T("女"):_T("男");
					strName.Format(_T("%s(%s)"),ActiveMember.GetUserName(),strSex);
					_tcsncpy(coi.szName, strName, 100);
					_tcsncpy(coi.szNetId, ActiveMember.GetNetId(), 100);
					_tcsncpy(coi.szTermId, ActiveMember.GetTerminalID(), 50);

					CString strIP;
					CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
					_tcsncpy(coi.szIP, strIP, 50);
					coi.nCheckoutType = CIBAGlobal::cot_MemLogout;

					theApp.GetRealName()->CheckOut(&coi);
				}

				//更新终端状态为锁屏,刷新终端视图
			/*	CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);*/
				computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
				computer.ClearUserInfo();
				CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);
				CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
			}
		}

		//激活列表删除用户,更新用户视图
		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(CheckOutinfo.GetMemberId());
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
	}
	Sleep(50);

	//删除表中所有记录
	CIBADAL::GetInstance()->DeleteAllCheckOutInfo();
	
	CoUninitialize();
	return 0;
}

/*************************************************
Function:       // DeregisterUser
Description:    // 校验卡号IDCheckCard，到中心注销Deregister，智多实名下机，往LastUser,Returned,Member表中更新记录，调用实名接口的进行操作
Calls:          // DoCheckCard，DoDeregister，theApp.GetRealName()->CheckOut，AddReturnedRecord，theApp.GetRealName()->InvokeRealName
Table Accessed: // None
Table Updated: // LastUser,Returned,Member
Input:          // CheckOutInfo-从CheckOutInfo表中读取到的记录
Output:         // None
Return:         // DoDeregister成功，则返回TRUE，否则返回FALSE
Others:         // None
*************************************************/
BOOL CBrokenNetworkResume::DeregisterUser(CCheckOutinfo& CheckOutinfo)
{
	IBA_LOG(_T("CBrokenNetworkResume::DeregisterUser Start - SerialNum = %d"),CheckOutinfo.GetSerialNum());
	//到中心检验卡号
	CIDCheckCardMethod IDCheckCard;
	IDCheckCard.SetSerialNum(CheckOutinfo.GetSerialNum());
	theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

	if (IDCheckCard.GetStatusCode() != 0)
	{
		IBA_LOG(_T("CBrokenNetworkResume::DeregisterUser Result - FALSE , DoCheckCard Failed!"));
		return FALSE;
	}

	//注销操作
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(IDCheckCard.GetMemberId());
	Deregister.SetPersonalId(CheckOutinfo.GetPersonalId());
	Deregister.SetMemberName(CheckOutinfo.GetClassName());
	Deregister.SetCardId(IDCheckCard.GetCardId());
	Deregister.SetPassword(_T("PASSWORDPASSWORD"));
	Deregister.SetReturnMode(0);                      //0表示正常退款

	theApp.GetCurCashier()->DoDeregister(Deregister); //执行注销和实名下机操作

	if (Deregister.GetStatusCode() == 0)
	{	
		UINT nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计 Balance是应退充值帐户余额,GuarantyMoney是应退押金

		CString strNetId;
		strNetId.Format(_T("%s"), CheckOutinfo.GetSerialNum());

		//写退款信息日志
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeReturn);
		OperationInfo.SetIsBundTime(FALSE);
		OperationInfo.SetUserName(CheckOutinfo.GetUserName());
		OperationInfo.SetNetId(CheckOutinfo.GetSerialNum());
		OperationInfo.SetMemberId(CheckOutinfo.GetMemberId());
		OperationInfo.SetOperationAmount(Deregister.GetBalance());
		OperationInfo.SetAllAmount(nAll);
		OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
		OperationInfo.SetDescription(_T("断网后续传"));
		
		// 如果是智多实名，进行实名下机
		if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
		{
			NS_REALNAME::CheckoutInfo coi;
			ZeroMemory(&coi, sizeof(coi));
			_tcsncpy(coi.szName, CheckOutinfo.GetUserName(), 100);
			_tcsncpy(coi.szNetId, CheckOutinfo.GetSerialNum(), 100);
			_tcsncpy(coi.szTermId, CheckOutinfo.GetTerminalId(), 50);

			CString strIP;
			CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
			_tcsncpy(coi.szIP, strIP, 50);
			coi.nCheckoutType = CIBAGlobal::cot_TempLogout;

			theApp.GetRealName()->CheckOut(&coi);
		}

		//必须等待直到返回，否则容易造成线程不安全
		SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		//回馈信息写入数据库
		{//写数据库
			CReturnedInfo ReturnedInfo;

			ReturnedInfo.SetCardId(IDCheckCard.GetCardId());                                     //账号
			ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(Deregister.GetTranTime())); //退款时间
			ReturnedInfo.SetRefNo(Deregister.GetRefNo());                                        
			ReturnedInfo.SetDeposit(Deregister.GetGuarantyMoney());                              //押金
			ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());                         //收银员名称
			ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());               //网吧ID
			ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());      //收银台ID
			ReturnedInfo.SetMemberId(CheckOutinfo.GetMemberId());                                //会员号
			ReturnedInfo.SetSerialNo(Deregister.GetLocalSerialNo());                             //本地流水号
			ReturnedInfo.SetReturnAmount(nAll);                                                  //应退总金额
			ReturnedInfo.SetClassId(1);                                                          //用户级别为1，即会员？？
			CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);  //对已退款的账户进行数据表的更新,包括LastUser, Returned, Member表
		}

		//调用实名接口的操作，如退款，下机，充值等
		if (CNetBarConfig::GetInstance()->GetReturnOnlyClassId() != CheckOutinfo.GetClassId()) //除非设置了只退款-该值在收银台登入时通过CheckIn从中心接收到并赋值
		{
			theApp.GetRealName()->InvokeRealName(100, (LPVOID)strNetId.GetString());
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
