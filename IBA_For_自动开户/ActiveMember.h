#pragma once

#include "Socket\BufferProcess.h"

//
//class:CActiveMember  活动用户类(属性类:用户的基本属性)
//
class GPropertyClass CActiveMember
{
	friend class CActiveMemberList;

public:
	CActiveMember(void);
	~CActiveMember(void);

	// 用户的状态
	enum EMemberStatus{
		EStatus_Waiting = 0 ,    //!>待上机
		EStatus_LocalOnline =1,  //!>本地在线 (UpdateDataTime表示ISERVER上报时间)
		EStatus_CenterOnLine =2, //!>中心在线 (UpdateDataTime表示下次往中心查询时间)
		EStatus_AutoReturn =3,   //!>自动退款 (UpdateDataTime表示下机时间)
		EStatus_RoomAutoReturn = 4 // 2011/11/01-8210-gxx: 包房用户的自动退款处理窗台
	};
	// 用户的付款类型
	enum EMemberPayType{
		EPayType_General = 0,    //!>普通计费
		EPayType_Capped = 1,     //!>累计封顶包时
		EPayType_Withholding =2  //!>足额预扣包时
	};

	//-----------------------属性-------------------------//
public:
	GProperty( MemberID,               UINT ); // 用户ID
	GProperty( NetId,               CString ); // 上网账号
	GProperty( PersonalID,          CString ); // 证件号码
	GProperty( IdType,                 UINT ); // 证件类型
	GProperty( NetbarID,               UINT ); // 网吧ID
	GProperty( CasherID,               UINT ); // 2011-3-29-gxx-收银台ID
	GProperty( RefNo,                  UINT ); // 引用号
	GProperty( Sex,                    UINT ); // 性别
	GProperty( CreditProcess,       CString ); // 详情
	GProperty( AdvancePayment,          INT ); // 预付款
	GProperty( Remark,              CString ); // 备注
	GProperty( IsRegisterBT,           BOOL ); // 是否开户包时
	GProperty( PayType,                UINT ); // 付款方式
	GProperty( IsOnLine,               BOOL ); // 上机
	GProperty( PCClassID,              UINT ); // 上机区域ID
	GProperty( Amount,                  INT ); // 费用
	GProperty( CheckInTime,    COleDateTime ); // 上机时间
	GProperty( UpdateDataTime, COleDateTime ); // 最后一次的更新时间
	GProperty( ActivationTime, COleDateTime ); // 激活时间
	GProperty( NextCheckinTime,        UINT ); // 2011/08/10-8201-gxx: 下一次上机时间，包时时用到
	GProperty( DynamicMaxAmount,        INT ); // 2011/07/08-8201-gxx: 累计封顶额 
	GProperty( IsLocalUser,            BOOL ); // 2011/07/08-8201-gxx: 是否为本地用户
	GProperty( Password,            CString ); // 2011/07/08-8201-gxx: 上机密码
	GProperty( AvailavleBalance,        INT ); // 余额
	GProperty( NetIdAlias,          CString ); // 2011/07/12-8201-gxx: 上网账号别名, 收银台不做控制,为了写入数据库，方便客户端访问
	GProperty( PolicyInfo,          CString ); // 2011/07/18-8201-gxx: 扣率信息

	GPropertyNeedSet( TerminalID,   CString ); // 终端号

	GPropertyNeedGet( UserName,     CString ); // 用户姓名
	GPropertyNeedSet( ClassId,         UINT ); // 用户级别ID

	GPropertyReadOnly(UserClassName, CString); // 用户级别名称

	GProperty( RoomMemberId,            UINT); // 包房房主的用户号
	GProperty( ScanType,                 INT); // 扫描类型


	//-----------------------方法-------------------------//
public:
	void Attach(MANAGER_COMPUT_INFO & computerInfo);

	BOOL LoadFromDB(CString strNetId);

	//本地可能离线了
	BOOL IsOffLinePossibility(); 

	BOOL IsMember(); //会员

	BOOL IsNeedToQuery(); //真的没钱了

	//!> 获取用户状态  EMemberStatus
	INT GetStatus() const { return m_nStatus; }
	//!> 设置用户状态  EMemberStatus
	void SetStatus(INT newVal) { m_nStatus = newVal; }

	BOOL IsActive(); //激活了吗
	void SetActive(BOOL newVal) { m_bIsActive = newVal; }

	
	void SetTerminalID(CString newVal);	

	CString GetTermIdInfo();
	
	
	CString GetUserName(BOOL bWithSex = FALSE);

	
	CString GetPayTypeAsString();

	CString GetAvailavleBalanceAsString();

	
	CString GetPCClass(); // 上机区域名称

	CString GetSexAsString(); 

	
	CString GetAmountAsString();

	CString GetCheckInTimeAsString();
	CString GetUseTimeAsString();
	
	
	void SetClassId(UINT newVal);

	CString GetActivationTimeAsString();
	CString GetAdvancePaymentAsString();

	// 2011/05/16-gxx: 获取自动退款时间
	static UINT GetRefundmentTime();

	// 2011/07/18-8201-gxx: 获取本地费用, bFilter决定是否进行0.5元的向上取整过滤
	UINT GetLocalExpense(BOOL bFilter=TRUE);

private:

	INT m_nStatus;

	BOOL m_bIsActive;
};
