#include "stdafx.h"
#include ".\cashierhelper.h"
#include "Dal\IBADAL.h"
#include "OperationInfo.h"
#include "Dal\IDTypeInfo.h"
#include "ActiveMember.h"
#include "IBA.h"
#include "MainFrm.h"
#include "Cashier.h"
#include "Socket\LocalServer.h"
#include "ActiveMemberList.h"
#include "IBAHelpper.h"
#include "ComputerListView.h"
#include "Tts\IBATTS.h"
#include "DdnDataX.h"
#include "Encrypt.h"
#include "DdnPcpInc.h"
#include "LocalCenter\gxx_base.h"
#include "GlobalFunctions.h"
#include "ServletHellper.h"
#include "RealName\IBARealName.h"
#include "RealName\AuditInterface.h"
#include "RealName\RZXRealName.h"
#include <afxmt.h>
#include "BrokenNetwork.h"
#include "LocalCenter\GxxString.h"
#include "LocalCenter\gxx_base.h"
#include "LocalCenter\G2XCommonTool.h"
#include "LocalMember.h"
#include "NetBarConfig.h"

extern CCriticalSection g_csLocalCreditLock;

using namespace IBAGlobal;

CCashierHelper::CCashierHelper(void)
{
}

CCashierHelper::~CCashierHelper(void)
{
}

/*************************************************
Function:       //MakeLocalConsumeInfo
Description:    //根据参数ActiveMember的内容计算本地消费金额和退款金额，上下机时间，MD5加密后填充到CLocalConsumeInfo。要注意LocalExpense的算法。
Calls:          //GetLocalExpense,
Table Accessed: //None
Table Updated:  //None
Input:          //ActiveMember - 用来填充consumeInfo
Output:         //consumeInfo - 填充好的localsonsumeInfo
Return:         //None
Others:         //Note:GetLocalExpense时，1.默认进行0.5元向上取整 2.消费金额不满当前小时,就计算为当前小时
*************************************************/
void CCashierHelper::MakeLocalConsumeInfo(CActiveMember& ActiveMember, NS_DAL::CLocalConsumeInfo& consumeInfo)
{
	// 获取本地消费金额, 计算退款金额
	//获取本地消费金额 - bFilter决定是否进行0.5元的向上取整过滤，默认为是。如果扣率有误or登陆时间有误，直接返回ActiveMember.GetAmout，否则通过扣率进行计算。
	//计算时，如果消费金额不满当前小时，就计算为当前小时剩余所有时间。
	UINT nExpense = ActiveMember.GetLocalExpense();   
	UINT nReturn = 0; 
	if (nExpense > (UINT)ActiveMember.GetAdvancePayment())
	{
		nReturn = 0;
		nExpense = (UINT)ActiveMember.GetAdvancePayment();
	}
	else
	{
		nReturn = ActiveMember.GetAdvancePayment() - nExpense;
	}

	//设置SubmitTime为当前时间，RefNo为MemberID
	consumeInfo.SetSubmitTime( CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()) );
	consumeInfo.SetRefNo( ActiveMember.GetMemberID() );

	//设置CheckinTime和CheckoutTime，如ActiveMember中checkin时间无效，则都设置为当前时间，否则按值设置，但下机时间设为当前。没有上过机，上下机时间都设置为当前时间。
	if (!ActiveMember.GetTerminalID().IsEmpty())   
	{
		if (!CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()))
		{
			// 上机时间不正常，使用当前时间
			consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( COleDateTime::GetCurrentTime() ));
			consumeInfo.SetCheckoutTime( consumeInfo.GetCheckinTime() );
		}
		else
		{
			consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( ActiveMember.GetCheckInTime()) );
			consumeInfo.SetCheckoutTime( CIBAHelpper::FormatTime( COleDateTime::GetCurrentTime() ) );
		}	
	}
	else
	{
		// 没有上过机，上机时间和下机和保持一致
		consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( ActiveMember.GetActivationTime()) );
		consumeInfo.SetCheckoutTime( CIBAHelpper::FormatTime( ActiveMember.GetActivationTime()) );
	}

	//填充并通过MD5加密
	consumeInfo.SetConsumeAmount( nExpense );
	consumeInfo.SetCreditAmount( ActiveMember.GetAdvancePayment() );
	consumeInfo.SetReturnAmount( nReturn );
	consumeInfo.SetIdNumber( ActiveMember.GetPersonalID() );
	consumeInfo.SetMemberId( ActiveMember.GetMemberID() );
	consumeInfo.SetName( ActiveMember.GetUserName(TRUE) );
	consumeInfo.SetOperator( theApp.GetCurCashier()->GetName() );		
	consumeInfo.SetSerialNum( ActiveMember.GetNetId() );	
	consumeInfo.SetTermId( ActiveMember.GetTerminalID() );
	consumeInfo.SetClassId( ActiveMember.GetClassId() );
	consumeInfo.SetPCClass( ActiveMember.GetPCClassID() );
	consumeInfo.SetPayType( ActiveMember.GetPayType() );
	consumeInfo.MakeMD5();
}

/*************************************************
Function:       //ReturnLocalUser
Description:    //如果没有传入CLocalConsumeInfo，则生成本地消费记录，包含本地消费金额等。
				//将本地消费记录保存到LocalConsume表中
				//通知客户端下机锁屏，更新ActiveMemberMap，更新用户视图 --- 不更新终端视图？？
Calls:          //MakeLocalConsumeInfo,
Table Accessed: //None
Table Updated:  //LocalConsume
Input:          //ActiveMember - 用于生成LocalConsumeInfo
                //lpLocalInfo - 用于将信息传递给终端，进行锁屏下机
Output:         //None
Return:         //结账成功返回TRUE.否则返回FALSE
Others:         //只限于本地注册用户使用！！！
*************************************************/
//BOOL CCashierHelper::ReturnLocalUser(CActiveMember& ActiveMember,NS_DAL::CLocalConsumeInfo* lpLocalInfo)
//{
//	IBA_ASSERT2(ActiveMember.GetIsLocalUser(), "必需为本地用户才能执行此操作"); //只有本地注册用户才能使用该函数？IBA_ASSERT2并不能在release中阻止运行！！！
//	if(!ActiveMember.GetIsLocalUser())
//	{
//		AfxMessageBox(_T("不是本地注册用户，结账失败！"));
//		return FALSE;
//	}
//
//	CLocalConsumeInfo consumeInfoTmp;
//	if (lpLocalInfo == NULL)
//	{
//		//根据参数ActiveMember的内容计算本地消费金额和退款金额，上下机时间，MD5加密后填充到CLocalConsumeInfo。要注意LocalExpense的算法。
//		MakeLocalConsumeInfo(ActiveMember, consumeInfoTmp);
//		lpLocalInfo = &consumeInfoTmp;
//	}
//	CLocalConsumeInfo& consumeInfo = *lpLocalInfo;
//
//	//添加到localConsume表
//	if (CIBADAL::GetInstance()->AddLocalConsume( consumeInfo ))  //liyajun note: 如果LocalConsume中存在对应记录，直接关闭数据库Recordset？？？
//	{
//		COperationInfo OperationInfo;
//		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturn);
//
//		OperationInfo.SetIsBundTime( FALSE );
//		OperationInfo.SetUserName( consumeInfo.GetName() );
//		OperationInfo.SetNetId( consumeInfo.GetSerialNum() );
//		OperationInfo.SetMemberId( consumeInfo.GetMemberId() );
//		OperationInfo.SetOperationAmount( consumeInfo.GetReturnAmount() );
//		OperationInfo.SetAllAmount( consumeInfo.GetCreditAmount() );
//		OperationInfo.SetOperationTime( COleDateTime::GetCurrentTime() );
//		OperationInfo.SetDescription(_T("本地用户结账(本地模式)"));
//		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
//
//		//删除对应LastUserInfo记录
//		CIBADAL::GetInstance()->DeleteLastUserInfo(consumeInfo.GetTermId());
//
//		// 通知客户端结账锁屏
//		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(consumeInfo.GetMemberId());
//		if (nIndex >= 0)
//		{	
//			CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, consumeInfo.GetMemberId()); //向客户端发送下机消息
//			Sleep(2000);
//		}
//
//		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(consumeInfo.GetMemberId()); //从ActiveMemberMap中删除对象，从实名系统中下机
//		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
//
//		IBA_LOG(_T("本地用户退款: netId=%s, memberId=%d"), consumeInfo.GetSerialNum(), consumeInfo.GetMemberId());
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

//创建登出信息结构
void CCashierHelper::MakeCheckOutInfo(CActiveMember& ActiveMember, NS_DAL::CCheckOutinfo& CheckOutinfo)
{
	CheckOutinfo.SetCheckOutTime(CIBAHelpper::GetCurrentTimeAsString(TRUE)); //登出时间

	//各个顺序不能变化，必须和写入的时候一样

	CheckOutinfo.SetTerminalId(ActiveMember.GetTerminalID());  //终端号
	CheckOutinfo.SetSerialNum(ActiveMember.GetNetId());		   //账号
	CheckOutinfo.SetMemberId(ActiveMember.GetMemberID());      //会员号
	CheckOutinfo.SetUserName(ActiveMember.GetUserName());      //用户名
	CheckOutinfo.SetPersonalId(ActiveMember.GetPersonalID());  //证件号码
	CheckOutinfo.SetClassName(ActiveMember.GetUserClassName());//用户级别名
	CheckOutinfo.SetClassId(ActiveMember.GetClassId());        //用户级别
	CheckOutinfo.SetClassState(ActiveMember.IsMember());       //是否会员
	CheckOutinfo.SetBalanceMoney(ActiveMember.GetAvailavleBalance()); //余额
}

/***************************************************************************************************
Function:       //CreditLocalUser
Description:    //进行本地充值. 过程就是往localMember和localCredit表插入或更新记录.
Table Updated:  //LocalMember,LocalCredit
Input:          //ActiveMember - 已激活用户对象
                //nCreditMoeny - 充值金额
Return:         //返回错误信息.成功的错误信息为空.
********************************************************************************************************/
CString CCashierHelper::DoCredit_InLocal(CActiveMember& ActiveMember, UINT nCreditMoeny)
{
	IBA_ASSERT2(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken, "必须断网情况下调用");

	CString strRet;
	strRet.Empty();

	UINT nMemberId = ActiveMember.GetMemberID();
	if(nMemberId == 0 || nCreditMoeny == 0) { return strRet;}
	if(nMemberId == 0)
	{
		strRet.Empty();
		strRet.Format(_T("用户账号错误!"));
		return strRet;
	}
	if(nCreditMoeny == 0)
	{
		strRet.Empty();
		strRet.Format(_T("充值金额错误!"));
		return strRet;
	}	

	//查询localMember表是否已有用户记录,没有记录则先创建记录并插入
	CLocalMember lm;
	if(FALSE == CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,lm))
	{
		GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(nMemberId);
		if(memberPtr == NULL)
		{
			CActiveMember Am;
			if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,Am))
			{
				strRet.Empty();
				strRet.Format(_T("读取用户信息失败!"));
				IBA_LOG(_T("Abnormal! CCashierHelper::CreditLocalUser MemberId %d Amount %d 创建LocalMember记录失败，activemember中没有用户信息的信息"),nMemberId,nCreditMoeny);
			}			
			return strRet;
		}
		if (FALSE == CIBADAL::GetInstance()->InsertNewLocalMember(memberPtr))
		{
			strRet.Empty();
			strRet.Format(_T("数据库操作失败!"));
			IBA_LOG(_T("Abnormal! CCashierHelper::CreditLocalUser MemberId %d Amount %d 插入LocalMember记录失败"),nMemberId,nCreditMoeny);
			return strRet;
		}
	}

	/*这里要先插入localCredit,成功才代表本地充值成功,然后再更新localMember,如果更新localMember失败的话,就删除localCredit的记录,代表充值失败*/
	// 插入本地充值记录
	CString strCheckCode;
	if(FALSE == CIBADAL::GetInstance()->InsertNewLocalCredit(nMemberId,nCreditMoeny,strCheckCode))
	{
		strRet.Empty();
		strRet.Format(_T("数据库操作失败!"));
		IBA_LOG(_T("Abnormal! CCashierHelper::CreditLocalUser MemberId %d Amount %d 本地充值更新本地库(localCredit)失败"),nMemberId,nCreditMoeny);
		return strRet;
	}
	//根据充值金额更新记录,失败则删除充值记录
	if(FALSE == CIBADAL::GetInstance()->UpdateLocalMember_CreditInfo(nMemberId,nCreditMoeny))
	{
		strRet.Empty();
		strRet.Format(_T("数据库操作失败!"));
		IBA_LOG(_T("Abnormal! CCashierHelper::CreditLocalUser MemberId %d Amount %d 本地充值更新本地库(localMember)失败"),nMemberId,nCreditMoeny);
		CIBADAL::GetInstance()->DeleteLocalCredit(strCheckCode);
		return strRet;
	}

	IBA_LOG(_T("本地充值成功: MemberId %d Amount %d"),nMemberId,nCreditMoeny);
	strRet.Empty();
	return strRet;
}

BOOL CCashierHelper::CashSellDianka(LPCTSTR lpURL)
{
	CDataXMSender xmSender;
	if (xmSender.Create(0,_T("DdnPcp-CA591950-AA6A-495a-9ABE-C02374F479FE")))
	{
		CStringA strURL = CT2W(lpURL);
		INT nRes = xmSender.SendData(1,0,0,(LPVOID)strURL.GetString(),strURL.GetLength());
		IBA_LOG0(_T("弹出客户端请求现金购点卡的tip"));
		return nRes >= 0;
	}	
	else
	{
		IBA_LOG(_T("处理客户端发送过来的现金购买点卡的请求失败"));
		return FALSE;
	}
}

BOOL CCashierHelper::InitDiankaPlatform()
{
	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		IBA_LOG0(_T("error: 初始化点卡平台失败, 收银员处于未上班状态"));
		return FALSE;
	}


	// 2012/02/29-8237-gxx: 
	{
		CFileVersionInfo fvi;
		CString strPath = theApp.GetWorkPath() +  + _T("\\IBABin\\");
		strPath.TrimRight(_T('\\'));
		strPath += _T("\\DdnPcp.dll");
		if (!fvi.Create(strPath))
		{
			IBA_LOG0(_T("error: 读取DdnPcp.dll的版本号失败"));
		}
		else
		{
			CString strVer = fvi.GetProductVersion();

			strVer.Replace(_T(", "), _T("."));

			IBA_LOG(_T("DdnPcp.dll (version: %s)"), strVer);

			if (strVer.CompareNoCase(_T("2.0.0.3")) < 0)
			{
				CString strInfo;
				strInfo.Format(_T("IBA.exe %s 只能支持DdnPcp.dll %s 以上版本"), 
					CIBALog::GetInstance()->GetAppVer(), strVer);
				IBA_LOG0(strInfo);
				theApp.IBAMsgBox(strInfo,MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	
	IBA_LOG0(_T("初始化点卡平台参数"));
	


	//ShowClientDianKa(m_hWnd,_T("http://www.baidu.com/"),WM_CLIENT_DIANKA,0,0);
	//	1.地址格式：
	//http://计费中心:8080/netbar/common/pointcard.jsp?
	//	userId=zhoufeng&netBarId=4&dutyId=12348709&cashRegisterId=1
	//		&serial=1311144479&check=150c5c46996ca7726fe4e73c28cbae0a 
	//		2.参数含义：
	//userId: 当前收银员的登录账号
	//netBarId: 当前网吧id
	//dutyId: 当前班次id，此字段为以后扩展
	//cashRegisterId: 收银台id，此字段为以后扩展
	//serial: 以整数表示的时间戳，即time()的返回值，中心允许10分钟的时间差。
	//check: 校验串，check=md5(userId+netBarId+dutyId+cashRegisterId+serial+md5(userPwd))，其中userPwd为收银员的密码

	UINT nNetbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strCasherID = theApp.GetCurCashier()->GetName();
	CString strPwd = theApp.GetCurCashier()->GetPassword();
	CString strTime;
	CTime tm = CTime::GetCurrentTime();
	strTime.Format(_T("%lld"), tm.GetTime());
	UINT nDutyId = theApp.GetCurCashier()->GetDutyID();
	UINT nCashRegisterId = CIBAConfig::GetInstance()->GetCashRegisterID();

	CString strOrgCode;
	strOrgCode.AppendFormat(_T("%s"),strCasherID);
	strOrgCode.AppendFormat(_T("%d"), nNetbarID);
	strOrgCode.AppendFormat(_T("%d"), nDutyId);
	strOrgCode.AppendFormat(_T("%d"), nCashRegisterId);
	strOrgCode.AppendFormat(_T("%lld"), tm.GetTime());
	CString strPwdMd5;
	CEncrypt::CalcMD5(theApp.GetCurCashier()->GetPassword(),strPwdMd5);
	strOrgCode.AppendFormat(_T("%s"), strPwdMd5);
	CString strNewCode;
	CEncrypt::CalcMD5(strOrgCode,strNewCode);

	CString strParam;
	strParam.AppendFormat(_T("userId=%s"),strCasherID);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("netBarId=%d"), nNetbarID);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("dutyId=%d"), nDutyId);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("cashRegisterId=%d"), nCashRegisterId);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("serial=%lld"), tm.GetTime());
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("check=%s"), strNewCode);


	CString strUrlMain = _T("http://");
	strUrlMain += CNetBarConfig::GetInstance()->GetMainCenterIP();
	strUrlMain.AppendFormat(_T(":%d"), CNetBarConfig::GetInstance()->GetMainCenterPort());
	strUrlMain += _T("/netbar/common/pointcard.jsp");

	CString strUrlBakup = _T("http://");
	strUrlBakup += CNetBarConfig::GetInstance()->GetBackupCenterIP();
	strUrlBakup.AppendFormat(_T(":%d"), CNetBarConfig::GetInstance()->GetBackupCenterPort());
	strUrlBakup += _T("/netbar/common/pointcard.jsp");

	CString strUrls;
	strUrls += strUrlMain;
	strUrls += _T("|");
	strUrls += strUrlBakup;

	//strUrl += strParam;

	//strUrl = _T("http://msdn.microsoft.com/library");

	/*<DdnPayCard Version= "1" DomainId="99" 
		NetbarId="4" Cashier="pm" DdnServer="192.168.10.253:8080">URL</DdnPayCard>*/

	

	CString strDDNPro;
	strDDNPro = _T("<DdnPayCard Version=\"1\" ");   
	strDDNPro.AppendFormat(_T("CashId=\"%d\" "), nCashRegisterId); // 收银台ID
	strDDNPro.AppendFormat(_T("DomainId=\"%d\" "), CNetBarConfig::GetInstance()->GetDomainId());
	strDDNPro.AppendFormat(_T("NetbarId=\"%d\" "), nNetbarID);
	strDDNPro.AppendFormat(_T("Cashier=\"%s\" "), strCasherID);// 收银员名称
	strDDNPro.AppendFormat(_T("DutyId=\"%d\" "), nDutyId);
	strDDNPro.AppendFormat(_T("CasherPwd=\"%s\" "), theApp.GetCurCashier()->GetPassword());

	strDDNPro.AppendFormat(_T("DdnServer=\"%s\">"), CNetBarConfig::GetInstance()->GetDodoNewHost());
	strDDNPro.AppendFormat(_T("%s</DdnPayCard>"),strUrls);


	if (!DdnInitializePcp(strDDNPro))
	{
		IBA_LOG0(_T("error: 调用  DdnPcp.dll 的导出函数DdnInitializePcp失败"));
		return FALSE;
	}

	
	return TRUE;
}

BOOL CCashierHelper::UninitDiankaPlatform()
{
	DdnTerminatePcp();
	IBA_LOG0(_T("卸载点卡平台"));
	return TRUE;
}

BOOL CCashierHelper::SellDianka()
{
	IBA_LOG0(_T("点击点卡销售"));
	DdnShowPcp();
	return TRUE;
}

/*************************************************
Function:       // ReportLocalUser-》ReportLocalCenterUser
Description:    // 向中心补报在线用户于本地模式下的动作。恢复完后删除localMember中记录。
Table Updated:  // localMember，如果已经上报完毕则删除记录
Input:          // pLocalMember - 从localMember表中取出的记录
Return:         // 成功返回TRUE，否则返回FALSE
Others:         // 这是旧的用于本地注册用户的上报！！！现在改为为 在线用户上报！！
FlowChart Redefine:
中心激活，中心登陆的会员 -  + CheckOut + Activate + CheckCard&DaBalance
中心激活，中心登陆的临时用户 -  + CheckCard&DaBalance
中心激活，本地登陆的会员 - BuckleAmount + CheckCard&DaBalance
本地激活，本地登陆的会员 - BuckleAmount + CheckCard&DaBalance
中心激活，本地登陆的临时用户 - BuckleAmount + Activate + CheckCard&DaBalance
*************************************************/
//BOOL CCashierHelper::ReportLocalCenterUser( GxxDictionary* pLocalMember )
//{	
//	if (pLocalMember == NULL) 
//	{
//		IBA_LOG(_T("Abnormal .CCashierHelper::ReportLocalCenterUser: pLocalMember is Empty!"));
//		GXX_ASSERT(0, "pLocalMember一定不能为空");
//		return FALSE;
//	}
//	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
//	{
//		IBA_LOG(_T("Abnormal .CCashierHelper::ReportLocalCenterUser: Network is broken!"));
//		GXX_ASSERT(0, "不允许在断网情况下调用 CheckoutLocalUser");
//		return FALSE;
//	}
//
//	UINT nMemberID = pLocalMember->valueForKey("memberId")->intValue();
//
//	CActiveMember ActiveMember;
//	if(!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberID,ActiveMember))
//	{
//		IBA_LOG(_T("Error!CCashierHelper::ReportLocalUser 无法从激活列表中获取到MemberId=%d的对象,将导致localMember表中有残留.请高度注意."),nMemberID);
//		return FALSE;	
//	}
//
//	//提取checkinTime和checkoutTime，checkoutTime如果无效则将当前时间设置为checkoutTime
//	CString strCheckinTime = CIBAHelpper::TranformTime(StringValueForKey("checkinTime", pLocalMember));
//	CString strCheckoutTime = StringValueForKey("checkoutTime", pLocalMember);
//	COleDateTime dtCheckoutTime = CIBAHelpper::StringToOleDateTime(strCheckoutTime);
//	if (CIBAHelpper::IsValidTime(dtCheckoutTime)) 
//	{
//		strCheckoutTime = CIBAHelpper::TranformTime(strCheckoutTime);
//	}
//	else 
//	{
//		strCheckoutTime = CIBAHelpper::GetCurrentTimeAsString(FALSE);
//	}
//
//	UINT nClassID = pLocalMember->valueForKey("classId")->intValue();
//	UINT nPCClass = pLocalMember->valueForKey("pcClass")->intValue();
//	CString strTermID = StringValueForKey("termId", pLocalMember);
//	UINT nPayType = pLocalMember->valueForKey("payType")->intValue();
//
//	BOOL isLocalCheckin = (pLocalMember->valueForKey("isLocalCheckin")->intValue() == 1);
//
//	//只有本地模式登录的用户才执行DoBuckleAmount
//	if(pLocalMember->boolValueForKey("isLocalCheckin"))
//	{
//		//向中心补上报扣费
//		CBuckleAmountMethod buckleMethod;
//		buckleMethod.SetMemberId(nMemberID);
//		buckleMethod.SetCheckinTime(strCheckinTime);
//		buckleMethod.SetCheckoutTime(strCheckoutTime);
//		buckleMethod.SetPcClass(nPCClass);
//		buckleMethod.SetPayType(nPayType);
//		buckleMethod.SetTermId(strTermID);
//
//		//消费金额 = 总金额 - 余额
//		int nAmount = 0;
//		nAmount = pLocalMember->valueForKey("totalMoney")->intValue() - pLocalMember->valueForKey("balance")->intValue();
//
//		//对消费金额进行向上取整
//		if (nAmount < 0) 
//		{
//			nAmount = 0;
//		}
//		if(nAmount % 50 >0)
//		{
//			nAmount = 50 * (nAmount / 50 + 1);
//		}
//
//		buckleMethod.SetBuckleAmount(nAmount);
//
//		theApp.GetCurCashier()->DoBuckleAmount(buckleMethod);
//
//		if (buckleMethod.GetStatusCode() != 0) 
//		{
//			IBA_LOG(_T("Abnormal!CCashierHelper::ReportLocalUser Desp:Report center error! Proc:when doing BuckleAmount! StatusCode is %d"),buckleMethod.GetStatusCode());
//			return FALSE;	
//		}
//	}
//
//	//中心激活,中心登陆的会员需要CheckOut + Activate
//	if(CNetBarConfig::GetInstance()->GetIsMember(pLocalMember->intValueForKey("classId")) &&    //会员
//		ActiveMember.GetRemark().CompareNoCase(_T("LocalActivated")) != 0 && //中心激活
//		pLocalMember->boolValueForKey("isLocalCheckin") == 0) //中心登陆
//	{
//		IBA_LOG(_T("Temp.CCashierHelper::ReportLocalCenterUser :This User was activated in Center Mode , MemberId = %d"),nMemberID);
//		//CheckOut
//		CString strCheckoutTime = CIBAHelpper::GetCurrentTimeAsString(FALSE);
//
//		CCheckout2Method checkout2Method;
//		checkout2Method.SetMemberId(nMemberID);
//		checkout2Method.SetEndUsingTime(strCheckoutTime);
//		theApp.GetCurCashier()->DoCheckout2(checkout2Method, TRUE);
//		if (checkout2Method.GetStatusCode() == 0 || checkout2Method.GetStatusCode() == 31)
//		{
//			IBA_TRACE(_T("不上报checkout2成功, memberId=%d,  结账时间=%s"),nMemberID, strCheckoutTime);
//			IBA_LOG(_T("Temp.CCashierHelper::ReportLocalUser.会员上报数据后checkout成功, memberId=%d,结账时间=%s"), nMemberID,strCheckoutTime);
//		}
//		else
//		{
//			IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalCenterUser.会员上报数据后checkout失败！for memberId=%d"), nMemberID);
//			return FALSE;
//		}
//
//		//Activate
//		if (CServletHellper::ActivationMember(nMemberID))
//		{
//			IBA_TRACE(_T("中途激活成功"));
//			IBA_LOG(_T("Temp.CCashierHelper::ReportLocalCenterUser.会员上报数据并checkout后激活成功！for memberId=%d"), nMemberID);
//		} 
//		else 
//		{
//			IBA_TRACE(_T("中途激活失败"));
//			IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalUser.会员上报数据并checkout后激活失败！for memberId=%d"), nMemberID);
//			return FALSE;
//		}
//	}
//
//	//中心激活,本地登陆的临时用户需要Activate
//	if( !CNetBarConfig::GetInstance()->GetIsMember(pLocalMember->intValueForKey("classId")) &&    //临时用户
//		ActiveMember.GetRemark().CompareNoCase(_T("LocalActivated")) != 0 && //中心激活
//		pLocalMember->boolValueForKey("isLocalCheckin") == 1) //本地登陆		
//	{
//		IBA_LOG(_T("Temp.CCashierHelper::ReportLocalCenterUser :This User was activated in Center Mode , MemberId = %d"),nMemberID);
//		//Activate
//		if (CServletHellper::ActivationMember(nMemberID))
//		{
//			IBA_TRACE(_T("中途激活成功"));
//			IBA_LOG(_T("Temp.CCashierHelper::ReportLocalUser.会员上报数据并checkout后激活成功！for memberId=%d"), nMemberID);
//		} 
//		else 
//		{
//			IBA_TRACE(_T("中途激活失败"));
//			IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalUser.会员上报数据并checkout后激活失败！for memberId=%d"), nMemberID);
//			return FALSE;
//		}
//	}
//
//	//所有类型都需要更新用户信息，checkCard以及DoBalance
//	CIDCheckCardMethod CheckCard;
//	CString strTmp = ActiveMember.GetNetId();
//
//	if (strTmp.GetLength() < 10)
//	{
//		strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
//	}
//	CheckCard.SetSerialNum(strTmp);
//	theApp.GetCurCashier()->DoCheckCard(CheckCard);
//	if(CheckCard.GetStatusCode() != 0)
//	{
//		IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalUser.IDcheckCard失败！for memberId=%d"), nMemberID);
//	}
//	else
//	{
//		int nCardId = CheckCard.GetCardId();
//
//		CBalanceMethod Balance;
//		Balance.SetMemberId(nMemberID);
//		Balance.SetCardId(nCardId);
//		//Balance.SetRound(0);
//
//		theApp.GetCurCashier()->DoBalance(Balance);
//
//		if(Balance.GetStatusCode()!= 0)
//		{
//			IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalUser.DoBalance失败！for memberId=%d"), nMemberID);
//		}
//	}
//
//	// 从本地数据库中删除本地记录
//	if(!CBrokenNetwork::RemoveLocalMember(nMemberID))
//	{
//		IBA_LOG(_T("Abnormal!CCashierHelper::ReportLocalUser Desp:Access DB error! Proc:when delect from localMember!"));
//		return FALSE;
//	}
//	return TRUE;
//}

/*************************************************
Function:       // ReportLocalCredit
Description:    // 向中心上报LocalCredit表中某个用户的充值记录
Table Updated: //  localCredit
Input:          // pLocalMember - 待操作用户的localMember记录
Otherss:		// 无论成败都会删除localCredit记录
*************************************************/
void CCashierHelper::ReportLocalCredit( GxxDictionary* pLocalMember )
{
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal) 
	{
		IBA_LOG(_T("Abnormal.CCashierHelper::ReportLocalCredit:Network is broken!"));
		GXX_ASSERT(0, "不允许在断网情况下调用 ReportLocalCredit");
		return;
	}

	//取出MemberID，strNetId
	UINT nMemberID = pLocalMember->valueForKey("memberId")->intValue();
	CString strNetId = StringValueForKey("netId", pLocalMember);
	CString strUserName = StringValueForKey("memberName", pLocalMember);

	// 从localCredit中取该memberId对应的充值记录
	CString sql;
	sql.Format(_T("select * from localCredit where memberId=%d"), nMemberID);
	GxxArrayPtr localCreditArrayPtr = CIBADAL::GetInstance()->ReadRecordsFromDB(sql.GetBuffer());
	sql.ReleaseBuffer();

	//遍历localCredit并上报充值
	for (int i = 0; i < localCreditArrayPtr->count(); i++)
	{
		CString strOperationLogMsg;//操作日志备注信息

		GxxDictionaryPtr localCreditPtr = localCreditArrayPtr->valueAtIndex(i)->getDictionary();
		
		UINT nAmount = localCreditPtr->valueForKey("amount")->intValue();                       //充值金额

		CString strRawTime = CA2T(localCreditPtr->valueForKey("creditTime")->stringValue());    //充值时间
		CString strTime = strRawTime;

		//校验充值记录,不匹配则删除
		CString oldCheckcode = CA2T(localCreditPtr->valueForKey("checkcode")->stringValue());   //校验码，要先转换成TCHAR
		CString nowCheckcode = CIBAHelpper::CalcLocalCreditCheckCode(nMemberID, strTime, nAmount);  //根据取出的值重新计算校验码，看是否匹配
		if (nowCheckcode.CompareNoCase(oldCheckcode) != 0)   
		{
			IBA_LOG(_T("Abnormal. CCashierHelper::ReportLocalCredit: 无效本地充值记录,数据被篡改.memberId:%d, 上网账号:%s, 充值金额:%.2f元"), nMemberID, strNetId, nAmount/100.f);
			strOperationLogMsg.Format(_T("上报失败！ Error: 无效充值记录."));
			RemoveLocalCredit(oldCheckcode);
			continue;
		}

		//充值金额为0则删除
		if(nAmount <= 0) 
		{	
			strOperationLogMsg.Format(_T("上报失败！ Error: 充值金额为0."));
			RemoveLocalCredit(oldCheckcode);
			continue;
		}

		/*****开始上报****/

		//如充值时间距现在超过8小时，中心会认为请求已过期。须将交易时间改为当前时间。
		COleDateTime oleT = CIBAHelpper::StringToOleDateTime(strTime);
		COleDateTimeSpan span = COleDateTime::GetCurrentTime() - oleT;
		if (span.GetTotalSeconds() > (8 * 3600 - 5))      
		{
			strTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);
		}

		CCreditMethod Credit;
		Credit.SetMemberId(nMemberID);                             //会员号
		Credit.SetTermTime(CIBAHelpper::TranformTime(strTime));    //充值时间
		Credit.SetActivityId(0);                                   
		Credit.SetNoPresent(0);                                    //无送充值
		Credit.SetAmount(nAmount);                                 //充值金额

		theApp.GetCurCashier()->MakeupCredit(Credit);

		if (Credit.GetStatusCode() == 0) 
		{
			strOperationLogMsg.Format(_T("上报成功"));
			IBA_LOG(_T("补传本地充值成功,memberId=%d, 上网账号:%s, 充值金额:%.2f元, 本地充值时间/当前时间:%s"), 
				nMemberID, strNetId ,nAmount/100.f, strTime);

			IBA_TRACE(_T("补传本地充值成功,memberId=%d, 上网账号:%s, 充值金额:%.2f元, 充值时间=%s"), 
				nMemberID, strNetId ,nAmount/100.f, strTime);
		}
		else                                          
		{
			strOperationLogMsg.Format(_T("上报失败！ Error: %s"),Credit.GetStatusMessage());
			IBA_LOG(_T("Error.补传本地充值失败,memberId=%d, 上网账号:%s, 充值金额:%.2f元, 本地充值时间/当前时间%s.LocalCredit记录将删除"), 
				nMemberID, strNetId ,nAmount/100.f, strTime);

			IBA_TRACE(_T("补传本地充值失败,memberId=%d, 上网账号:%s, 充值金额:%.2f元, 充值时间=%s"), 
				nMemberID, strNetId ,nAmount/100.f, strTime);
		}

		RemoveLocalCredit(oldCheckcode);//无论成功失败都删除记录

		//写操作日志
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeMakeupLocalCredit);
		OperationInfo.SetUserName(strUserName);
		OperationInfo.SetNetId(strNetId);
		OperationInfo.SetOperationAmount(nAmount);
		OperationInfo.SetDescription(strOperationLogMsg);
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
	}
	return;
}

/*************************************************
Function:       // ReportLocalReturn
Description:    // 向中心上报LocalMember表中临时用户的退款记录
Table Updated: //  localCredit
Input:          // pLocalMember - 待操作用户的localMember记录
*************************************************/
void CCashierHelper::ReportLocalReturn(CLocalMember& localMember)
{
	//本地模式下不允许上报.会员不是退款.
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal 
		|| CNetBarConfig::GetInstance()->GetIsMember(localMember.nClassId)
		|| 0 == localMember.nIsReturned)
	{
		return;
	}
	CString strOperationLogMsg;

	//先请求Balance获得参数
	CBalanceMethod balance;
	balance.SetMemberId(localMember.nMemberId);
	BOOL bRes = balance.ExecuteBalance();
	if(FALSE == bRes)
	{
		strOperationLogMsg.Format(_T("上报失败！ Error: %s"),balance.GetStatusMessage());
	}
	else//请求退款
	{
		CDeregisterMethod Deregister;

		Deregister.SetMemberId(localMember.nMemberId);
		Deregister.SetPersonalId(balance.GetPersonalId());
		Deregister.SetMemberName(balance.GetMemberName());
		Deregister.SetCardId(balance.GetCardId());
		Deregister.SetPassword(balance.GetPassword());
		CString strEndTime = localMember.checkoutTime.Format(_T("%Y%m%d%H%M%S"));
		Deregister.SetEndUsingTime(strEndTime);

		Deregister.SetDutyId(theApp.GetCurCashier()->GetDutyID());
		Deregister.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
		Deregister.SetOperator(theApp.GetCurCashier()->GetName());
		Deregister.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());

		BOOL bRes1 = Deregister.ExecuteDeregister();
		if(FALSE == bRes1)
		{
			strOperationLogMsg.Format(_T("上报失败！ Error: %s"),Deregister.GetStatusMessage());
		}
		else//成功
		{
			strOperationLogMsg.Format(_T("上报成功"));

			//保存交易信息到注册表
			Deregister.SaveTranInfo();

			//插入新记录到本地库
			CReturnedInfo ReturnedInfo;
			ReturnedInfo.SetCardId(_ttoi(localMember.strNetId));
			ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(Deregister.GetTranTime()));
			ReturnedInfo.SetRefNo(Deregister.GetRefNo());
			ReturnedInfo.SetDeposit(Deregister.GetGuarantyMoney());
			ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());
			ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			ReturnedInfo.SetMemberId(Deregister.GetMemberId());
			ReturnedInfo.SetSerialNo(Deregister.GetLocalSerialNo());
			ReturnedInfo.SetReturnAmount(Deregister.GetBalance() + Deregister.GetGuarantyMoney());
			ReturnedInfo.SetClassId(localMember.nClassId);

			CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);
		}		
	}
	//写操作日志
	COperationInfo OperationInfo;
	OperationInfo.SetOperationType(CIBAGlobal::emOpTypeMakeupLocalReturn);
	OperationInfo.SetUserName(localMember.strMemberName);
	OperationInfo.SetNetId(localMember.strNetId);
	OperationInfo.SetOperationAmount(localMember.nBalance);
	OperationInfo.SetDescription(strOperationLogMsg);
	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

}

/*************************************************
Function:       // ReportLocalConsume
Description:    // 向中心上报LocalMember表中用户的本地消费记录
Input:          // pLocalMember - 待操作用户的localMember记录
*************************************************/
void CCashierHelper::ReportLocalConsume(CLocalMember& localMember)
{
	//本地模式下不允许上报.正常模式下登陆的不需要上报.
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal 
		|| FALSE == localMember.nIsLocalCheckin)
	{
		return;
	}
	//CString strOperationLogMsg;
	//进行上报扣费
	COleDateTime CheckinTime(localMember.checkinTime);

	CBuckleAmountMethod buckleMethod;
	buckleMethod.SetMemberId(localMember.nMemberId);
	buckleMethod.SetCheckinTime(CheckinTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	buckleMethod.SetCheckoutTime(localMember.checkoutTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	buckleMethod.SetPcClass(localMember.nClassId);
	buckleMethod.SetPayType(localMember.nPayType);
	buckleMethod.SetTermId(localMember.strTermId);

	//总消费金额 = totalMoney - balance
	int nAmount = 0;
	nAmount = localMember.ntotalMoney - localMember.nBalance;
	//对消费金额进行向上取整
	if (nAmount < 0) 
	{
		nAmount = 0;
	}
	if(nAmount % 50 > 0)
	{
		nAmount = 50 * (nAmount / 50 +1 ); 
	}
	buckleMethod.SetBuckleAmount(nAmount);

	theApp.GetCurCashier()->DoBuckleAmount(buckleMethod);  //往中心执行扣费，将该账户的消费金额从中心也扣掉
	if (buckleMethod.GetStatusCode() != 0) 
	{
		IBA_LOG(_T("上报扣费失败.MemberId:%d,ErrorCode:%d,ErrorMsg:%s "),localMember.nMemberId,buckleMethod.GetStatusCode(),buckleMethod.GetStatusMessage());
	}

	//写操作日志
	//COperationInfo OperationInfo;
	//OperationInfo.SetOperationType(CIBAGlobal::emOpTypeMakeupLocalReturn);
	//OperationInfo.SetUserName(localMember.strMemberName);
	//OperationInfo.SetNetId(localMember.strNetId);
	//OperationInfo.SetOperationAmount(localMember.nBalance);
	//OperationInfo.SetDescription(strOperationLogMsg);
	//::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

}
/*************************************************
Function:       // ReportLocalReturnVip
Description:    // 向中心上报LocalMember表中会员的结账记录
Table Updated: //  localCredit
Input:          // pLocalMember - 待操作用户的localMember记录
*************************************************/
void CCashierHelper::ReportLocalReturnVip(CLocalMember& localMember)
{
	//本地模式下不允许上报.临时用户不是结账.
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal 
		|| FALSE == CNetBarConfig::GetInstance()->GetIsMember(localMember.nClassId)
		|| 0 == localMember.nIsReturned)
	{
		return;
	}
	CString strOperationLogMsg;

	//检查登入时间和登出时间
	BOOL bIsOnlineWhileCheckout(FALSE);
	if(CIBAHelpper::IsValidTime(localMember.checkinTime) && localMember.checkinTime > COleDateTime(2015,1,1,0,0,0) //登入和登出时间都有效,表示本地下结账时用户在线
		&& CIBAHelpper::IsValidTime(localMember.checkoutTime) && localMember.checkoutTime > COleDateTime(2015,1,1,0,0,0))
	{
		bIsOnlineWhileCheckout = TRUE;
	}
	else if(!CIBAHelpper::IsValidTime(localMember.checkinTime) || localMember.checkinTime <= COleDateTime(2015,1,1,0,0,0)//否则表示本地下结账时,用户不在线
		&& !CIBAHelpper::IsValidTime(localMember.checkoutTime) || localMember.checkoutTime <= COleDateTime(2015,1,1,0,0,0))
	{
		bIsOnlineWhileCheckout = FALSE;
	}
	else//其他情况,肯定是出错了
	{
		return;
	}

	//结账时用户在线,补执行Checkout
	if(bIsOnlineWhileCheckout)
	{
		CCheckout2Method Checkout2;
		Checkout2.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		Checkout2.SetMemberId(localMember.nMemberId);
		Checkout2.SetSafeLogout(0); 
		CString strEndTime = localMember.checkoutTime.Format(_T("%Y%m%d%H%M%S"));
		Checkout2.SetEndUsingTime(strEndTime);
		Checkout2.SetOperator(theApp.GetCurCashier()->GetName());

		BOOL bRes = Checkout2.ExecuteCheckout2();
		if(bRes)
		{
			strOperationLogMsg.Format(_T("上报成功"));
		}
		else
		{
			strOperationLogMsg.Format(_T("上报失败！ Error: %s"),Checkout2.GetStatusMessage());
		}

		//写操作日志
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeMakeupLocalReturnVip);
		OperationInfo.SetUserName(localMember.strMemberName);
		OperationInfo.SetNetId(localMember.strNetId);
		OperationInfo.SetOperationAmount(localMember.nBalance);
		OperationInfo.SetDescription(strOperationLogMsg);
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));	
	}
	else//结账时用户不在线,补执行反激活.不输出日志.
	{
		CActivationMethod Activation;
		Activation.SetMemberId(localMember.nMemberId);
		Activation.SetMode(CActivationMethod::ACTIVATION_NO);
		BOOL bRes = theApp.GetCurCashier()->DoActivation(Activation);
		//if(bRes)
		//{
		//	strOperationLogMsg.Format(_T("上报成功"));
		//}
		//else
		//{
		//	strOperationLogMsg.Format(_T("上报失败！ Error: %s"),Activation.GetStatusMessage());
		//}
	}
}
/*************************************************
Function:       //WriteOperationLog
Description:    //结账输出活动日志
Calls:          //SendMessage
Input:          //ActiveMember - 日志内容来源
                //OperationType - 日志类型
*************************************************/
//void CCashierHelper::WriteReturnOperationLog(CActiveMember& ActiveMember,UINT OperatinType)
//{
//	COperationInfo OperationInfo ;
//	OperationInfo .SetOperationType (OperatinType);   
//	CString UserName;
//	UserName.Format(_T("%s(%s)"),ActiveMember.GetUserName(),ActiveMember.GetSexAsString());
//	OperationInfo.SetUserName (UserName);
//	OperationInfo.SetNetId ( ActiveMember.GetNetId());
//	OperationInfo.SetDescription(ActiveMember.GetTerminalID());
//	OperationInfo.SetClassId(ActiveMember.GetClassId());
//	OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
//	OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
//
//	//临时用户需要多输出几项
//	if(OperatinType == CIBAGlobal::emOpTypeBrokenReturn)
//
//	{
//		UINT nDeposit(0);
//		CString sql;
//		sql.Format(_T("select deposit from member where memberId=%d"),ActiveMember.GetMemberID());
//
//		GxxDictionaryPtr MemberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
//
//		if(!MemberPtr)
//		{
//			IBA_LOG(_T("Error! CCashierHelper::WriteReturnOperationLog Desp:Access DB Error! Proc: Read Deposit from Member for MemberId = %d"),ActiveMember.GetMemberID());
//		}
//		nDeposit = MemberPtr->valueForKey("deposit")->intValue();
//
//		UINT nAll = ActiveMember.GetAvailavleBalance() + nDeposit;
//
//		OperationInfo.SetAllAmount(nAll);
//		OperationInfo.SetDeposit(nDeposit);
//		OperationInfo.SetOperationAmount(ActiveMember.GetAvailavleBalance());
//		OperationInfo.SetTermId(ActiveMember.GetTerminalID());
//		OperationInfo.SetConsumeMoney(ActiveMember.GetAdvancePayment()-nAll);
//	}
//
//	::SendMessage (CMainFrame :: m_hOperationLog, WM_OPERATIONLOG , 0, (LPARAM )(& OperationInfo));
//}

/*************************************************
Function:       //UpdateLocalMemberForLocalReturn
Description:    //为本地模式结账更新LocalMember表记录.LocalMember表中有对应用户记录,更新其isReturned和checkoutTime字段,表中无记录,则创建记录插入记录再更新记录.
Calls:          //..
Table Accessed: //LocalMember
Table Updated:  //LocalMember
Input:          //nMemberID - 需要更新或插入的用户号
Output:         //None
Return:         //更新或创建插入更新失败都返回FALSE.
Others:         //None
*************************************************/
//BOOL CCashierHelper::UpdateLocalMemberForLocalReturn(UINT nMemberID)
//{
//	CString sql;
//	sql.Format(_T("select * from localMember where memberId=%d"), nMemberID);
//	GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
//
//	//表中有记录
//	if (memberPtr) 
//	{
//		CString strCheckoutTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);
//		sql.Format(_T("update localMember set checkoutTime='%s',isReturned=1 where memberId=%d"), strCheckoutTime, nMemberID);
//		if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) < 0)
//		{
//			IBA_LOG(_T("Error! CCashierHelper::UpdateLocalMemberForLocalReturn: Desp: Access DB fail! Proc: when update localMember for MemberId = %d"),nMemberID);
//			//GXX_ASSERT(0,"更新lLocalMember表错误");
//			return FALSE;
//		}
//		IBA_LOG(_T("TempUse.CCashierHelper::UpdateLocalMemberForLocalReturn: isReturned is set to 1 in localMember for MemberId = %d"),nMemberID);
//	}
//	//表中无记录
//	else 
//	{
//		memberPtr = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(nMemberID);  //创建记录
//		if (!memberPtr)
//		{
//			IBA_LOG(_T("Error! CCashierHelper::UpdateLocalMemberForLocalReturn: Desp: Access DB fail! Proc: when create localMember record for MemberId = %d"),nMemberID);
//			IBA_LOG(_T("CCashierHelper::UpdateLocalMemberForLocalReturn Result - FALSE, for MemberId = %d"),nMemberID);
//			return FALSE;
//		}
//		if (!CIBADAL::GetInstance()->InsertNewLocalMember(memberPtr))                    //插入记录
//		{
//			IBA_LOG(_T("Error! CCashierHelper::UpdateLocalMemberForLocalReturn: Desp: Access DB fail! Proc: when insert into localMember for MemberId = %d"),nMemberID);
//			IBA_LOG(_T("CCashierHelper::UpdateLocalMemberForLocalReturn Result - FALSE, for MemberId = %d"),nMemberID);
//			return FALSE;
//		}
//
//		IBA_TRACE(memberPtr->describe().c_str());
//		CString str = IBAGlobal::StringValueForKey("memberName", memberPtr);
//
//		CString strCheckoutTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);
//
//		sql.Empty();
//		sql.Format(_T("update localMember set checkoutTime='%s',isReturned=1 where memberId=%d"),strCheckoutTime, nMemberID);   //更新记录
//		if (CIBADAL::GetInstance()->ExecuteSQL(sql))
//		{
//			IBA_LOG(_T("TempUse.CCashierHelper::UpdateLocalMemberForLocalReturn: isReturned is set to 1 in localMember for MemberId = %d"),nMemberID);
//		}
//		else
//		{
//			IBA_LOG(_T("Error! CCashierHelper::UpdateLocalMemberForLocalReturn: Desp: Access DB fail! Proc: when update localMember for MemberId = %d"),nMemberID);
//			IBA_LOG(_T("CCashierHelper::UpdateLocalMemberForLocalReturn Result - FALSE, for MemberId = %d"),nMemberID);
//			return FALSE;
//		}
//	}
//	return TRUE;
//}

/*************************************************
Function:       //RealnameZDCheckOut
Description:    //智多实名下机
Calls:          //ZDRealName::CheckOut
Table Accessed: //None
Table Updated:  //None
Input:          //strUserName - 用户名
                //strNetId - 账号
				//strTermId - 终端号
Output:         //None
Return:         //None
Others:         //None
*************************************************/
//void CCashierHelper::RealnameZDCheckOut(CString strUserName,CString strNetId, CString strTermId)
//{
//	if(CIBAGlobal::emRealnameZD != CNetBarConfig::GetInstance()->GetAuditType())
//	{
//		return;
//	}
//
//	NS_REALNAME::CheckoutInfo coi;
//	ZeroMemory(&coi, sizeof(coi));
//	_tcsncpy(coi.szName, strUserName, 100);
//	_tcsncpy(coi.szNetId, strNetId, 100);
//	_tcsncpy(coi.szTermId, strTermId, 50);
//
//	CString strIP;
//	CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
//	_tcsncpy(coi.szIP, strIP, 50);
//	coi.nCheckoutType = CIBAGlobal::cot_MemLogout;
//
//	theApp.GetRealName()->CheckOut(&coi);
//}

/*************************************************
Function:       //UpdateDBForLocalReturn
Description:    //为结账更新本地数据库.
Calls:          //AddReturnedRecord
Table Accessed: //Member
Table Updated:  //LastUser - 删除记录, Returned - 新增记录, Member - 修改记录的Available为2(已退款)
Input:          //ActiveMember - 更新表的数据来源
Output:         //None
Return:         //更新失败返回FALSE
Others:         //None
*************************************************/
//BOOL CCashierHelper::UpdateDBForReturn(CActiveMember& ActiveMember)
//{   
//	UINT nMemberID = ActiveMember.GetMemberID();
//
//	CString sql;
//	sql.Format(_T("select deposit from member where memberId=%d"),nMemberID);
//	GxxDictionaryPtr MemberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
//	if(!MemberPtr)
//	{
//		IBA_LOG(_T("Error! CCashierHelper::UpdateDBForReturn Desp:Access DB Error! Proc: Read Deposit from Member for MemberId = %d"),ActiveMember.GetMemberID());
//		return FALSE;
//	}
//	int nDeposit = MemberPtr->valueForKey("deposit")->intValue();
//
//	UINT nAll = ActiveMember.GetAvailavleBalance() + nDeposit;
//
//	CReturnedInfo ReturnedInfo;
//
//	ReturnedInfo.SetCardId(_ttoi(ActiveMember.GetNetId()));
//	//ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(GetCurrentTime));
//	ReturnedInfo.SetReturnDate(CIBAHelpper::GetCurrentTimeAsString(TRUE));
//	ReturnedInfo.SetRefNo(ActiveMember.GetRefNo());
//	ReturnedInfo.SetDeposit(nDeposit);
//	ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());
//	ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
//	ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
//	ReturnedInfo.SetMemberId(nMemberID);
//	ReturnedInfo.SetSerialNo(_ttoi(ActiveMember.GetPersonalID()));
//	ReturnedInfo.SetReturnAmount(nAll);
//	ReturnedInfo.SetClassId(ActiveMember.GetClassId());
//
//	return CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);
//}

/*************************************************
Function:       //RemoveLocalCredit
Description:    //从LocalCredit中删除数据(先检查是否存在)
Table Updated:  //LocalCredit
Input:          //checkCode - 要删除的记录的checkCode
Return:         //表中无对应记录或成功，返回TRUE
*************************************************/
BOOL CCashierHelper::RemoveLocalCredit(CString checkCode)
{
	CSingleLock lock(&g_csLocalCreditLock,TRUE);

	CString sql;

	sql.Format(_T("select * from localcredit WHERE checkcode = '%s'"),checkCode);
	if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) < 1 )
	{
		return TRUE;
	}

	sql.Format(_T("DELETE from localcredit WHERE checkcode = '%s'"),checkCode);

	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) < 1)
	{
		IBA_LOG(_T("Error!CCashierHelper::RemoveLocalCredit 删除localCredit记录失败.sql=%s"),sql);
		return FALSE;
	}

	return TRUE;
}

/*************************************************
Function:       //RemoveLocalCredit
Description:    //从LocalCredit中删除数据(先检查是否存在)
Calls:          //...
Table Accessed: //LocalCredit
Table Updated:  //LocalCredit
Input:          //MemberId - 要删除的记录的MemberId
Output:         //None
Return:         //表中无对应记录或成功，返回TRUE
Others:         //None
*************************************************/
BOOL CCashierHelper::RemoveLocalCredit(UINT MemberId)
{
	IBA_LOG(_T("CCashierHelper::RemoveLocalCredit - Start"));
	CSingleLock lock(&g_csLocalCreditLock,TRUE);

	CString sql;

	sql.Format(_T("select * from localcredit WHERE memberId = %d"),MemberId);
	if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) < 1 )
	{
		lock.Unlock();
		IBA_LOG(_T("CCashierHelper::RemoveLocalCredit - End. No record in table whose MemberId = %d"),MemberId);
		return TRUE;
	}

	sql.Format(_T("DELETE from localcredit WHERE memberId = %d"),MemberId);

	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) <1)
	{
		IBA_LOG(_T("CCashierHelper::RemoveLocalCredit - Result is FALSE for memberId = %d"),MemberId);
		lock.Unlock();
		GXX_ASSERT(0,"从localCredit删除数据失败！");
		return FALSE;
	}

	IBA_LOG(_T("CCashierHelper::RemoveLocalCredit - Result is TRUE for memberId = %d"),MemberId);
	lock.Unlock();
	return TRUE;
}

/*************************************************
Function:       //CalcExpense
Description:    //计算本地计费时的费用
Input:          //strRate - 扣率字符串
				//lastUpdateTime - 上一次更新时间
				//nLastBalance - 上一次所剩余额
Return:         //新的余额
*************************************************/
UINT CCashierHelper::CalcExpense(CString strRate,time_t lastUpdateTime,UINT nLastBalance)
{
	UINT nNewBalance = nLastBalance;

	time_t nowT = time(NULL);
	//解析扣率
	char sRate[128];
	memcpy(sRate,CT2A(strRate.GetBuffer()),sizeof(sRate));
	G2XUtility::CGxxStringA rate = sRate;
	float fRates[24];
	for (int i = 0; i < 24; i++) 
	{
		fRates[i] = rate.Mid(i*4, 4).ParseInt() / 100.0f;
	}

	COleDateTime lastTime(lastUpdateTime);
	COleDateTime nowTime(nowT);

	//开始计算费用
	int nLastHour, nNowOur;
	nLastHour = lastTime.GetHour();
	nNowOur = nowTime.GetHour();
	int nLastMin = lastTime.GetMinute();
	int nLastSec = lastTime.GetSecond();

	int nTotalSec = nowT - lastUpdateTime;              //总秒数
	int nFrontSec = nLastMin*60 + nLastSec;    //上次计费时间的秒数
	IBA_LOG(_T("本地计费.总秒数%d"),nTotalSec);

	float fSum = 0;
	if (nFrontSec + nTotalSec <= 3600)                 //消费在同一个小时之内，取当前小时的扣率计算费用
	{
		fSum += fRates[nLastHour] * nTotalSec / 3600;
	}
	else 
	{
		fSum += fRates[nLastHour] * (3600 - nFrontSec) / 3600;   //当前小时的按当前小时扣率计算
		nTotalSec -= (3600 - nFrontSec);
		nLastHour++;
		for (; nTotalSec > 0; nTotalSec -= 3600, nLastHour += 1) //剩余的按各自小时的扣率计算
		{
			if (nLastHour == 24) //往上加有可能变成24
			{
				nLastHour = 0;
			}
			int nSe = 3600;
			if (nTotalSec < 3600) 
			{
				nSe = nTotalSec;
			}
			fSum += fRates[nLastHour] * nSe / 3600;
		}
	}

	
	int nConsume = int(fSum * 100);
	if (nConsume == 0)                                               //每次计费最低消费为1分钱
	{
		nConsume = 1;
	}
	IBA_LOG(_T("本地计费,总消费%d(分钱)"),nConsume);

	nNewBalance = nLastBalance - nConsume;			

	if (nNewBalance < 0) 
	{
		nNewBalance = 0;
	}
	return nNewBalance;
}

/*************************************************
Function:       //CalcRemainTime
Description:    //计算本地计费时的剩余可消费时间
Input:          //strRate - 扣率字符串
				//lastUpdateTime - 上一次更新时间
				//nBalance - 余额
Return:         //新的余额
*************************************************/
UINT CCashierHelper::CalcRemainTime(UINT nBalance,time_t LastUpdateTime,CString strRate)
{
	char sRate[128];
	memcpy(sRate,CT2A(strRate.GetBuffer()),sizeof(sRate));
	G2XUtility::CGxxStringA rate = sRate;
	if(rate.GetLength() != 96)
	{
		IBA_LOG(_T("Abnormal.CLocalCenter::HandleQueryBalance.无效的扣率串"));
		return 0;
	}
	GXX_ASSERT(rate.GetLength() == 96, "无效的扣率串");
	float fRates[24];
	float fMinRate = 1000.0f;
	for (int i = 0; i < 24; i++)
	{
		fRates[i] = rate.Mid(i*4, 4).ParseInt() / 100.0f;
		if (fRates[i] < fMinRate) 
		{
			fMinRate = fRates[i];
		}
	}

	// 计算RemainTime
	int nRemainTime = 0;

	float fBalance = (float)nBalance / 100.0f;
	COleDateTime lastTime(LastUpdateTime);
	int nLastHour = lastTime.GetHour();
	int nLastSec = 60 * lastTime.GetMinute() + lastTime.GetSecond();

	float fB = (3600 - nLastSec) * fRates[nLastHour] / 3600;    //上次上报时间所在的那个小时剩余的秒数*秒扣率=那小时如果要消费需要的金额
	if (fB < fBalance) 
	{
		nRemainTime += 3600 - nLastSec;   //如果余额够，nRemainTime = 那小时剩下的时间

		nLastHour += 1;
		if (nLastHour == 24) 
		{
			nLastHour = 0;
		}
		fBalance -= fB;

		nLastSec = 0;
		while(1) {

			if (fBalance - fRates[nLastHour] > 0.000001)   //循环，如果余额够消费下一个小时，则余额-=下一个小时需要的金额
			{
				fBalance -= fRates[nLastHour];
				nLastHour++;
				nRemainTime += 3600;
				if (nLastHour == 24)
				{
					nLastHour = 0;
				}
			}
			else                                           //直到余额不够为止，这样计算出还能消费的时间。精确到最后那个小时，而没有精确到分钟。
			{
				break;
			}
		}
	}

	float fCurPrice = fRates[nLastHour] / 3600; // 每一秒的价格

	while (fBalance > 0.000001)                   //balance为不够最后一个小时的金额
	{                                        

		nLastSec += 1;
		if (nLastSec >= 3600) 
		{
			nLastHour += 1;
			if (nLastHour == 24) 
			{
				nLastHour = 0;
			}
			nLastSec = 0;

			fCurPrice = fRates[nLastHour] / 3600;
		}
		fBalance -= 1 * fCurPrice;
		nRemainTime += 1;
	}                                    //计算剩余时间精确到秒数
	
	return nRemainTime;
}