#include "stdafx.h"
#include "IBAConfig.h"
#include "IBAHelpper.h"
#include "resource.h"
#include "NetBarConfig.h"
#include ".\activemember.h"
#include "IBALog.h"
#include "IBA.h"

CActiveMember::CActiveMember(void)
{
	m_nStatus = 0;
	m_bIsActive = FALSE;
	GPropertyInit( IsOnLine, FALSE );
	GPropertyInit( Sex, 0 );
	GPropertyInit( ClassId, 0);

	GPropertyInit( IdType, 0 );
	GPropertyInit( NetbarID, 0 );
	GPropertyInit( MemberID, 0 );
	GPropertyInit( PCClassID, 0 );
	GPropertyInit( PayType, 0 );
	GPropertyInit( Amount, 0 );
	GPropertyInit( AdvancePayment, 0 );
	GPropertyInit( CasherID, 0 );
	GPropertyInit( RefNo, 0 );
	GPropertyInit( IsRegisterBT, FALSE); 
	GPropertyInit( AvailavleBalance, 0 );

	// 2011/07/08-8201-gxx: 
	GPropertyInit3( DynamicMaxAmount, IsLocalUser, Password ,
		            0,                FALSE,       _T(""));

	// 2011/10/25-8230-gxx: 
	GPropertyInit( RoomMemberId, 0);

	GPropertyInit( ScanType, 0);
}

CActiveMember::~CActiveMember(void)
{
}

void CActiveMember::Attach(MANAGER_COMPUT_INFO & computerInfo)
{
	static const size_t TempLen = 128;
	char szTmp[TempLen] = {0};

	ZeroMemory(szTmp, TempLen);
	CopyMemory(szTmp, computerInfo.termID, LEN_TERMID);
	CString strTermID = CA2W(szTmp);
	strTermID.Trim();
	GPropertyInit( TerminalID, strTermID );

	//ZeroMemory(szTmp, TempLen);
	//CopyMemory(szTmp, computerInfo.IPAddr, LEN_IPADDR);
	//m_strComputerIP = CA2W(szTmp);
	//m_strComputerIP.Trim();

	//ZeroMemory(szTmp, TempLen);
	//CopyMemory(szTmp, computerInfo.computerName, MAX_COMPUTERNAME_LENGTH);
	//m_strComputerName = CA2W(szTmp);
	//m_strComputerName.Trim();

	ZeroMemory(szTmp, TempLen);
	CopyMemory(szTmp, computerInfo.memberType, LEN_MEMBERTYPE);
	UserClassName = CA2W(szTmp);
	UserClassName.Trim();

	GPropertyInit( ClassId, CNetBarConfig::GetInstance()->GetUserClassId(UserClassName) );

	//{ 2011/05/16-gxx: 添加此判断的情况很特殊：
	//    数据库中保存了级别ID和级别名称的对应关系.在数据库中的这个关系如果没有和
	//    中心同步的话，可能存在以下问题: 客户端上报给IServer的classID，IServer
	//    如果在数据库中找不到这个classID的对应的级别名称，那么IServer就会把这个
	//    classID当成 级别名称传给IBA, 所以此处添加长度为1的判断，处理这宗问题的
	//    发生. (级别名称的长度不肯能为1，因此可以做此判断)

	if (UserClassName.GetLength() == 1)
	{
		int iClass = _ttoi(UserClassName);
		if (iClass == 0)
		{
			iClass = 1;
		}
		SetClassId( iClass );
	}

	////}
	

	//证件号码
	ZeroMemory(szTmp, TempLen);
	CopyMemory(szTmp, computerInfo.personalID, LEN_PERSONID);

	GPropertyInit( PersonalID, CA2W(szTmp));
	SET(PersonalID).Trim();

	//上网账号
	ZeroMemory(szTmp, TempLen);
	CopyMemory(szTmp, computerInfo.szCardSerialNo, LEN_NETID);
	CString strNetID = CA2W(szTmp);
	strNetID.Trim();
	SetNetId( strNetID );

	//用户名称
	ZeroMemory(szTmp, TempLen);
	CopyMemory(szTmp, computerInfo.userName, LEN_USERNAME);
	CString strUserName = CA2W(szTmp);
	strUserName.Trim();
	GPropertyInit( UserName, strUserName);
	IBA_LOG(_T("ci.UserName=%s"),UserName);
	//char *realna = "普通用户";

	GPropertyInit( Amount, computerInfo.amount );	
	GPropertyInit( AvailavleBalance, computerInfo.availavleBalance);

	GPropertyInit( IdType, computerInfo.idType );
	GPropertyInit( NetbarID, computerInfo.netbarID );

	GPropertyInit( PayType, computerInfo.payType );
	GPropertyInit( Sex, computerInfo.sex );
	GPropertyInit( MemberID, computerInfo.memberID );
	GPropertyInit( RefNo, computerInfo.refNo ); 

	CheckInTime = computerInfo.checkinTime;		
	//CString strCheckinTime;
	//strCheckinTime = CheckInTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	//IBA_LOG(_T("ci.CheckinTime = %s"),strCheckinTime);

	UpdateDataTime = COleDateTime::GetCurrentTime();

	m_nStatus = EStatus_LocalOnline;
	SetIsOnLine( TRUE );
}

CString CActiveMember::GetPayTypeAsString()
{
	CIBAString strTmp;

	if ( GetIsOnLine() )
	{
		switch ( GetPayType() )
		{
		case EPayType_General:     strTmp.LoadString(IDS_PAYTYPE0); break;
		case EPayType_Capped:      strTmp.LoadString(IDS_PAYTYPE1); break;
		case EPayType_Withholding: strTmp.LoadString(IDS_PAYTYPE2); break;
		}
	}

	// 2011/10/26-8230-gxx: 
	if (GetRoomMemberId() > 0)
	{
		strTmp.LoadString(IDS_ROOM_WHOLE);
	}


	return strTmp;
}

CString CActiveMember::GetAvailavleBalanceAsString()
{
	CString strTmp;	

	//double dblTmp = GetAvailavleBalance() / 100.0;
	int nTmp = GetAvailavleBalance();

	//仅当本地模式时，进行四舍五入取整。孙成龙要求不做四舍五入。
	/*if(CIBAGlobal::emNetworkBroken == CIBAGlobal::NetworkStatus)
	{
		nTmp -= nTmp % 50;
	}
	if (nTmp < 0)
	{
		nTmp = 0;
	}*/

	double dblTmp = (double)nTmp / 100.0;

#ifdef __USING_INTER_RESOURCE
	strTmp.Format(_T("￥%.2f"), dblTmp);
#else
	strTmp.Format(_T("%.2f"), dblTmp);
#endif

	return strTmp;
}

CString CActiveMember::GetAdvancePaymentAsString()
{
	CString strTmp;

	double dblTmp = GetAdvancePayment() / 100.0;

#ifdef __USING_INTER_RESOURCE
	strTmp.Format(_T("￥%.2f"), dblTmp);
#else
	strTmp.Format(_T("%.2f"), dblTmp);
#endif

	return strTmp;
}

CString CActiveMember::GetSexAsString()
{
	return CIBAHelpper::GetSexAsString( GetSex() );
}

CString CActiveMember::GetCheckInTimeAsString()
{
	CString strTmp;

	if ( GetIsOnLine() )
	{
		strTmp = CheckInTime.Format(_T("%H:%M"));
	}

	return strTmp;
}

CString CActiveMember::GetActivationTimeAsString()
{
	CString strTmp;

	if (IsActive())
	{
		strTmp = ActivationTime.Format(_T("%H:%M"));
	}

	return strTmp;
}

BOOL CActiveMember::IsActive()
{
	return m_bIsActive;
}

CString CActiveMember::GetAmountAsString()
{
	CString strTmp;

	// 2011/05/19-gxx: 如果是开户包时的话也可以得到消费金额
	// 2011/06/14-gxx: 消费金额不为0，都显示
	if (GetIsOnLine() || GetIsRegisterBT() || GetAmount()!=0)
	{
		int nTmp = GetAmount();

		//仅当本地模式时，进行四舍五入
		if(CIBAGlobal::emNetworkBroken == CIBAGlobal::NetworkStatus)
		{
			if(nTmp % 50 > 0)
			{
				//nTmp += 50;
				nTmp = 50*(nTmp/50 +1 ); 
			}
		}

		if (nTmp < 0)
		{
			nTmp = 0;
		}

		double dblTmp = (double)nTmp / 100.0;
		//double dblTmp = GetAmount() / 100.0;

#ifdef __USING_INTER_RESOURCE
		strTmp.Format(_T("￥%.2f"), dblTmp);
#else
		strTmp.Format(_T("%.2f"), dblTmp);
#endif
	}

	return strTmp;
}

void CActiveMember::SetTerminalID(CString newVal)
{
	GPropertyInit( TerminalID, newVal );

	if (GetTerminalID().IsEmpty())
	{
		SetIsOnLine( FALSE );
	}
}

CString CActiveMember::GetUseTimeAsString()
{
	CString strTmp;

	if ( GetIsOnLine() )
	{
		COleDateTime dtNow = COleDateTime::GetCurrentTime();

		COleDateTimeSpan dts = dtNow - CheckInTime;

		//系统时间跟中心事件有差错时，可能存在为负的情况
		if (dts < COleDateTimeSpan(0, 0, 0, 0))//时间为负
		{
			strTmp.Format(_T("%.2d:%.2d"), 0, 0);
		}
		else//正常
		{
			//{ 2011/04/22-gxx: 修改原来支持最大时长24小时为总时长, GetHours()-->GetTotalHours()
			int nHours = (int)dts.GetTotalHours();
			strTmp.Format(_T("%.2d:%.2d"), nHours, dts.GetMinutes());
			//}		
		}
	}

	return strTmp;
}

void CActiveMember::SetClassId(UINT newVal)
{
	GPropertyInit( ClassId, newVal );

	UserClassName = CNetBarConfig::GetInstance()->GetUserClassName(GetClassId());
}

/*************************************************
Function:       //IsOffLinePossibility
Description:    //终端在线的用户，若距离上次OnClientUpdateData的时间超过90s，就认为可能离线了
Return:         //超过就返回TRUE.
*************************************************/
BOOL CActiveMember::IsOffLinePossibility()
{
	if ( GetIsOnLine() ) //CheckInTime存在，即终端有登陆
	{
		COleDateTimeSpan dtSpan = COleDateTime::GetCurrentTime() - UpdateDataTime; //当前时间距上一次的OnClientUpdateData时间

		// 2014-1-10-qsc 把6m30s改成2m30s 
		//return dtSpan.GetTotalSeconds() > (2 * 60 + 30); //上一次的更新时间和当前时间的间隔>2m30s
		int nSeconds =(int)(dtSpan.GetTotalSeconds()); 
		if(nSeconds > (3 * 60 + 30))
		{
			//IBA_LOG(_T("status = EStatus_LocalOnline, NeedQuery! Time past from last ClientUpdateData is %dS"),nSeconds);
		}		
		return nSeconds > (3 * 60 + 30); //上一次的更新时间和当前时间的间隔>1m30s
		// 可以认为可能离线了。
	}
	
	return FALSE;
}

BOOL CActiveMember::IsMember()
{
	return CNetBarConfig::GetInstance()->GetIsMember( GetClassId() );
}

CString CActiveMember::GetPCClass()
{
	return CNetBarConfig::GetInstance()->GetPCClassName( GetPCClassID() );
}

CString CActiveMember::GetTermIdInfo()
{
	CString strTmp;

	if (GetTerminalID().IsEmpty())
	{

	}
	else
	{
		strTmp = GetTerminalID();

		if (!IsActive() && GetAvailavleBalance() > 0) //失去激活且有钱
		{
			strTmp.Append(_T("(失去连接)"));
		}
	}

	return strTmp;
}

CString CActiveMember::GetUserName(BOOL bWithSex /*= FALSE*/)
{
	CString strTmp = UserName;

	if (bWithSex)
	{
		strTmp.AppendFormat(_T("(%s)"), GetSexAsString());
	}

	return strTmp;
}

/*************************************************
Function:       //IsNeedToQuery
Description:    //是否需要查询。
				//EStatus_LocalOnline - 超过90S就认为需要查询
				//EStatus_CenterOnline - 每次都需要查询
				//EStatus_AutoReturn - 上次更新时间无效或与上次时间间隔超过收银台设置，就认为需要查询
				//EStatus_RoomAutoReturn - 上次更新时间无效或与上次时间间隔超过2分钟，就认为需要查询
Return:			//需要查询就返回true.否则返回false.
*************************************************/
BOOL CActiveMember::IsNeedToQuery()
{
	if (GetStatus() == EStatus_LocalOnline) //本地在线
	{
		return IsOffLinePossibility(); 
	}
	else if (GetStatus() == EStatus_CenterOnLine) //中心在线
	{
		return COleDateTime::GetCurrentTime() >= UpdateDataTime;
	}
	else if (GetStatus() == EStatus_AutoReturn) //自动退款
	{
		//为invalid或null,需要查询
		if (UpdateDataTime.GetStatus() != COleDateTime::valid) 
		{
			return TRUE;
		}

		COleDateTimeSpan dtSpan = COleDateTime::GetCurrentTime() - UpdateDataTime; 

		//与上次时间间隔超过收银台设置，就认为需要查询
		if (dtSpan.GetTotalMinutes() >= GetRefundmentTime())//根据收银台设置 - 强制自动退款 = 0， 临时用户钱用完后几分钟后退款 = 具体设置
		{
			return TRUE;
		}
	}
 
	else if (GetStatus() == EStatus_RoomAutoReturn)   //包房自动退款
	{
		if (UpdateDataTime.GetStatus() != COleDateTime::valid)
		{
			return TRUE;
		}

		COleDateTimeSpan dtSpan = COleDateTime::GetCurrentTime() - UpdateDataTime; 

		int nSeconds = (int)(dtSpan.GetTotalSeconds());

		if (nSeconds >= 200.0) // 超过2分钟，主账号就可以自动退款了
		{
			return TRUE;
		}
	}

	return FALSE;
}
//若后台设置了强制自动退款，返回0，否则返回"收银台设置"-"充值退款设置"-临时用户钱用完后几分钟后退款
UINT CActiveMember::GetRefundmentTime()
{
	if (CNetBarConfig::GetInstance()->GetMustAutoReturn() == 1)
	{
		return 0;
	}

	return CIBAConfig::GetInstance()->GetRefundmentTime();
}

BOOL CActiveMember::LoadFromDB(CString strNetId)
{
	return FALSE;
}

/*************************************************
Function:       //GetLocalExpense
Description:    //获取本地消费金额 - bFilter决定是否进行0.5元的向上取整过滤，默认为是。如果扣率有误or登陆时间有误，直接返回ActiveMember.GetAmout，否则通过扣率进行计算。
				//计算时，如果消费金额不满当前小时，就计算为当前小时剩余所有时间。
Calls:          //..
Table Accessed: //None
Table Updated:  //None
Input:          //bFilter - 是否进行0.5元向上取整。默认为TRUE.
Output:         //None
Return:         //计算出的本地消费金额
Others:         //只限于本地注册的用户使用？？？
*************************************************/
UINT CActiveMember::GetLocalExpense(BOOL bFilter) 
{
	if (PolicyInfo.IsEmpty())
	{
		return GetAmount();
	}
	if (PolicyInfo.GetLength() != 24*4)
	{
		IBA_ASSERT2(FALSE, "warning:扣率信息有误");
		CIBALog::GetInstance()->WriteFormat(_T("本地用户扣率信息有误,netId=%s, 扣率=%s"), NetId, PolicyInfo);
		return GetAmount();
	}
	if (CheckInTime.GetStatus() != COleDateTime::valid || CheckInTime == 0)
	{
		return GetAmount();
	}
	COleDateTime timeNow = COleDateTime::GetCurrentTime();
	COleDateTimeSpan timeSpan = timeNow - CheckInTime;

	UINT nPolicyHours[24] = {0};
	for (int i = 0; i < 96; i+=4)
	{
		nPolicyHours[i/4] = (UINT)_ttoi(PolicyInfo.Mid(i, 4));
	}

	double dExp = 0.0;
	int iStartHour = CheckInTime.GetHour();
	int iStartSecond = CheckInTime.GetSecond()+ CheckInTime.GetMinute()*60;
	int nTotalSecond = (int)timeSpan.GetTotalSeconds();
	for(int iSecond = nTotalSecond; iSecond > 0; ) // 计算expense循环，expense = 在当前小时的秒数 * 当前小时的秒扣率
	{
		if (nPolicyHours[iStartHour] == 0)
		{
			IBA_ASSERT2(FALSE,"warning:扣率信息有误,扣率为0");
			CIBALog::GetInstance()->WriteFormat(_T("本地用户扣率信息有误,netId=%s, 扣率=%s"), NetId, PolicyInfo);
			return GetAmount();
		}
		double dSecondPolicy = nPolicyHours[iStartHour%24] / 3600.0;  //当前小时每秒的费用

		int nSeconds = 3600 - iStartSecond; 
		if (nSeconds >= iSecond)  //如果开始消费时的那个小时的剩余秒数>总共花费的秒数，说明所有花费都在该小时内
		{
			nSeconds = iSecond;   //将总消费时间 = 那个小时的剩余秒数。 liyajun note: 这种舍入方法是否合理？只要消费时间未超过当前小时，那么都将计算成最低消费了当前小时的剩余时间？
		}
		
		dExp += dSecondPolicy * nSeconds; //当前小时消费 = 当前小时秒扣率 * 开始时那小时的剩余秒数。note:从第一个循环后，剩余秒数都设为0.
		iSecond -= nSeconds; 
		iStartHour ++;
		//2015-0828 liyajun
		if (iStartHour == 24)
		{
			iStartHour = 0;
		}

		iStartSecond = 0; // 第一次循环以后，起始秒都为0
	}

	if (bFilter) //Filter表示往上补入
	{
		UINT nExp = UINT(dExp+49)/50*50;  //往上进行补入，最低单位为50 
		return nExp;
	}
	else
	{
		return UINT(dExp);
	}
	
}