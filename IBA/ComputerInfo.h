#pragma once

#include "Socket\BufferProcess.h"

class CComputerList;
class CActiveMember;

class CComputerInfo
{
public:

	friend class CComputerList;

	CComputerInfo(void);
	CComputerInfo(MANAGER_COMPUT_INFO& computerInfo);
	~CComputerInfo(void);

	enum EUPDATESTATE
	{
		EUS_NORMAL, EUS_ADD, EUS_CHANGE
	};

	enum ECOMPUTERSTATUS
	{
		ECS_OFFLINE,      //离线
		ECS_ONLINE,       //在线
		ECS_SUSPEND,      //停止 
		ECS_LOCKED,       //锁定
		ECS_UNLOCK,       //未锁定
		ECS_BIRTHDAY,     //生日
		ECS_VIP,		  //VIP
		ECS_BLACK,        //黑名单用户
		ECS_LITTERMONEY,  //余额不多
		ECS_THIEF,        //问题电脑(没有客户端连接，但电脑能Ping通)
		ECS_CONFLICT,     //冲突(TermId冲突，即多台终端使用了同一个TermId)
		ECS_NORECORD,	  //无记录
	};

private :

	/**************检查时间,终端信息上报中心时间********************/
	time_t m_lCheckTime; //上报时间 - Attach时设为0，以后在上报线程中设置为上报时的时间

	/**************终端信息********************/
	CString m_strComputerIP;
	CString m_strMac;				// Mac地址 - Attach时设为空字符串，以后在上报线程中设置为被上报的终端IP
	CString m_strTerminalID;
	CString m_strComputerName;
	UINT m_nPCClassID;
	CString m_strClientVersion;
	COleDateTime m_UpdateTime;		//升级时间
	EUPDATESTATE m_UpdateState;     //终端状态是否有改变  EUS_NORMAL 正常状态, EUS_ADD 增加状态,EUS_CHANGE 换机状态
	/*终端增加/改变/不变状态 - Attach时设为EUS_ADD增加，以后在每次updateComputer中设为EUS_CHANGE表示有登入登出等变化，
	在UpdateComputerInfoToRow中设为EUS_NORMAL，表示基本状态没有变化*/
	UINT m_Status;					//Iserver传过来的终端机器状态
	//#define TERM_OFFLINE			0x0001		// 离线  
	//#define TERM_ONLINE			0x0002		// 在线
	//#define TERM_SUSPEND			0x0003		// 挂机
	//#define TERM_LOCKED			0x0004		// 锁屏
	//#define TEMP_USER				0x0008		// 普通用户
	//#define MEMBER_USER			0x0010		// 会员
	//#define TERM_WARNINGBAL		0x0020		// 余额不多
	//#define TERM_IDCONFLICT		0x0040		/// TermId冲突, 即多台终端使用了同一个TermId
	//#define TERM_THIEF			0x0080		/// 问题电脑(没有客户端连接,但电脑能ping通)
	//#define BLACK_MAN				0x0100		// 黑名单用户
	//#define TERM_UNAUTHORIZED		0x0200		/// 无记录终端(有时会被称作"非法终端"以使用户加强重视)
	//#define BIRTHDAY				0x0400		// 顾客生日
	//#define VIP					0x0800		// 重点用户
	//#define TERM_MANUALUNLOCAL    0x0030      // 解屏
	//#define UNKNOWN_STATUS		0x8000		/// 未知状态
	UINT m_IDOStatus;				//记录IDO的状态： 1 计费 3 挂机 5 锁屏 7 维护
	ECOMPUTERSTATUS m_ComputerStatus;//根据m_Status和其他信息再细分出要在机器视图显示出的终端状态(对应资源中的字符串)
/*		* ECS_OFFLINE              离线     ------------   TERM_ONLINE == m_Status && m_IDOStatus !=3(不为挂机) && 没有UserInfo && m_strClientVersion为空 
		* ECS_ONLINE               在线
		* ECS_SUSPEND              挂机
		* ECS_LOCKED               锁屏      -----------    TERM_ONLINE == m_Status && m_IDOStatus !=3(不为挂机) && 没有UserInfo && m_strClientVersion不为空 
		* ECS_UNLOCK               未锁屏
		* ECS_BIRTHDAY             顾客生日
		* ECS_VIP                  会员用户
		* ECS_BLACK                黑名单用户
		* ECS_LITTERMONTY          余额不多
		* ECS_THIEF                问题电脑(没有客户端连接，但电脑能Ping通)
		* ECS_ CONFLICT            冲突(TermId冲突，即多台终端使用了同一个TermId)
		* ECS_NORECORD             无记录*/
	BOOL m_bShutdown;  // 是否关机 - Attach时设为FALSE，以后在上报线程中设置为被上报的终端的关机状态
	UINT m_LastNetBarID;
	UINT m_LastAccountType;

	/**************用户信息********************/
	BOOL m_bHasUserInfo;		//是否含有用户信息 - Attach时根据MemberId和UserName长度是否为0判断是否包含，以后在AttachActiveMember和更改会员账号的时候都设为TRUE
	CString m_strMemberType;	//会员类型
	CString m_strUserName;
	UCHAR m_Sex;
	CString m_strPersonalID;	
	CString m_strAuditID;		//公安接口身份ID，国内即身份证号
	UINT m_nMemberID;
	UINT m_IdType;				//ID卡类型
	UINT m_PayType;
	CString m_strPDK;			//用户个人分配密匙
	CString m_strSerialNo;		//上网账号
	UINT m_nSerialNo;			//上网账号

	/**************费用信息********************/
	INT m_Amount;
	UINT m_CreditAmount;		//充值金额
	UINT m_UseAmount;			//使用金额
	ULONG m_FreezeBalance;		//冻结余额
	LONG m_AvailavleBalance;	//可用余额
	COleDateTime m_CheckInTime;	//登陆时间
	/**************网吧信息********************/	
	CString m_strLcation;
	UINT m_nNetbarID;		
	UINT m_nRefNo;				//中心返回的系统参考号，服务器为每个网吧维护这个号码，从0~999999循环递增，可确保当天唯一

public:
	//部分设置项
	void SetUserName(CString val) { m_strUserName = val; }
	void SetTerminalID(CString val) { m_strTerminalID = val; }
	void SetPCClassID(UINT newVal) { m_nPCClassID = newVal; }
	void SetAvailavleBalance(ULONG val) { m_AvailavleBalance = val; }
	void SetMemberID(UINT val) { m_nMemberID = val; }
	void SetNetId(CString val) { m_strSerialNo = val; m_bHasUserInfo = TRUE; }
	void SetMac(CString val) { m_strMac = val; }
	void SetIsShutdown(BOOL val) { m_bShutdown = val; }
	void SetCheckTime(time_t val) { m_lCheckTime = val; }
	////////////////////////////////////////

	//GPropertyReadOnly( ComputerIP, CString ); // 电脑IP
	//GPropertyReadOnly( TerminalID, CString ); // 终端号
	//GPropertyReadOnly( ComputerName, CString ); // 电脑名称
	//GPropertyReadOnly( PCClassID, UINT ); // 机器区域ID
	//GPropertyReadOnly( MemberID, UINT ); // 会员ID
	//GPropertyReadOnly( RefNo, UINT ); // 引用号
	//GPropertyReadOnly( SerialNo, UINT ); //

	CString GetComputerIP() const { return m_strComputerIP; }

	CString GetTerminalID() const { return m_strTerminalID; }
	
	CString GetComputerName() const { return m_strComputerName; }

	UINT GetPCClassID() const { return m_nPCClassID; }
	CString GetPCClass(); // 获取机器区域名称

	CString GetMemberType() const;
	UINT GetUserClassId() const;
	BOOL IsMember() const;

	UINT GetMemberID() const { return m_nMemberID; }
	CString GetMemberIDAsString();

	UINT GetRefNo() const { return m_nRefNo; }

	UINT GetSerialNo() const { return m_nSerialNo; }

	UINT GetNetbarID() const { return m_nNetbarID; }

	LONG GetAvailavleBalance() const { return m_AvailavleBalance; }
	CString GetAvailavleBalanceAsString();

	ULONG GetFreezeBalance() const { return m_FreezeBalance; }

	CString GetUserName(BOOL bWithSex = FALSE);

	CString GetSexAsString();
	UCHAR GetSex() const { return m_Sex; }

	CString GetPersonalID() const { return m_strPersonalID; }

	UINT GetIdType() const { return m_IdType; }

	CString GetPayTypeAsString();
	UINT GetPayType() const { return m_PayType; }

	UINT GetAmount() const { return m_Amount; }
	CString GetAmountAsString();

	UINT GetCreditAmount() const { return m_CreditAmount; }

	UINT GetUseAmount() const { return m_UseAmount; }

	CString GetCheckInTimeAsString();
	COleDateTime GetCheckInTime() const { return m_CheckInTime; }

	COleDateTime GetUpdateTime() const { return m_UpdateTime; }

	UINT GetLastAccountType() const { return m_LastAccountType; }

	UINT GetLastNetBarID() const { return m_LastNetBarID; }

	CString GetPDK() const { return m_strPDK; }

	CString GetAuditID() const { return m_strAuditID; }

	CString GetClientVersion() const { return m_strClientVersion; }

	CString GetLcation() const { return m_strLcation; }

	CString GetStatusAsString();

	UINT GetIDOStatus() const { return m_IDOStatus; }

	CString GetNetId() const;

	CString GetUseTimeAsString();

	CString GetMac() const { return m_strMac; }
	BOOL GetIsShutdown() const { return m_bShutdown; }
	time_t GetCheckTime() const { return m_lCheckTime; }

	UINT GetStatus() const {return m_Status;}

public :

	void Attach(MANAGER_COMPUT_INFO & computerInfo);

	// 2011/06/09-gxx: 
	void AttachActiveMember(CActiveMember& ActiveMember);

private :

	void InitState();

	void CheckStatus();
	
public :

	BOOL HasUser() const;
	BOOL HasUserInfo() const { return m_bHasUserInfo; }
	BOOL HasClient() const;

	ECOMPUTERSTATUS GetComputerStatus() const;
	void SetComputerStatus(ECOMPUTERSTATUS newVal);

	EUPDATESTATE GetUpdateState() const { return m_UpdateState; }
	void SetUpdateState(EUPDATESTATE newVal) { m_UpdateState = newVal; }

	void ClearUserInfo();
	//void Clear();
};
