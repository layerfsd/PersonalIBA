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
		ECS_BIRTHDAY,
		ECS_VIP,
		ECS_BLACK,
		ECS_LITTERMONEY,
		ECS_THIEF,
		ECS_CONFLICT,
		ECS_NORECORD,
	};

private :

	CString m_strComputerIP;
	CString m_strTerminalID;
	CString m_strComputerName;
	CString m_strMemberType;
	CString m_strUserName;
	CString m_strPersonalID;
	CString m_strClientVersion;
	CString m_strLcation;
	CString m_strPDK;
	CString m_strSerialNo;	
	CString m_strAuditID;

	UINT m_nPCClassID;

	UINT m_nMemberID;
	
	UINT m_nRefNo;
	UINT m_nSerialNo;
	UINT m_nNetbarID;
	
	UCHAR m_Sex;
	UINT m_IdType;
	UINT m_PayType;
	INT m_Amount;
	UINT m_CreditAmount;
	UINT m_UseAmount;
	ULONG m_FreezeBalance;
	LONG m_AvailavleBalance;
	
	COleDateTime m_CheckInTime;
	COleDateTime m_UpdateTime;

	UINT m_LastNetBarID;
	UINT m_LastAccountType;
	UINT m_IDOStatus; //	记录IDO的状态：计费，挂机，锁屏
	UINT m_Status;

public:
	//部分设置项
	void SetUserName(CString val) { m_strUserName = val; }
	void SetTerminalID(CString val) { m_strTerminalID = val; }
	void SetPCClassID(UINT newVal) { m_nPCClassID = newVal; }
	void SetAvailavleBalance(ULONG val) { m_AvailavleBalance = val; }
	void SetMemberID(UINT val) { m_nMemberID = val; }
	void SetNetId(CString val) { m_strSerialNo = val; m_bHasUserInfo = TRUE; }
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

public :

	void Attach(MANAGER_COMPUT_INFO & computerInfo);

	// 2011/06/09-gxx: 
	void AttachActiveMember(CActiveMember& ActiveMember);

private :

	void InitState();

	void CheckStatus();

	EUPDATESTATE m_UpdateState;
	
	ECOMPUTERSTATUS m_ComputerStatus;

	BOOL m_bHasUserInfo;
	
public :

	BOOL HasUser() const;
	BOOL HasUserInfo() const { return m_bHasUserInfo; }
	BOOL HasClient() const;

	ECOMPUTERSTATUS GetComputerStatus() const { return m_ComputerStatus; }
	void SetComputerStatus(ECOMPUTERSTATUS newVal);

	EUPDATESTATE GetUpdateState() const { return m_UpdateState; }
	void SetUpdateState(EUPDATESTATE newVal) { m_UpdateState = newVal; }

	void ClearUserInfo();
};
