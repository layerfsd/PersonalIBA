#pragma once

#include "ActiveMember.h"
#include "Servlet\BalanceMethod.h"
#include "Dal\IDTypeInfo.h"

using namespace NS_SERVLET;

class CActiveMemberList : public CThread
{
public:
	CActiveMemberList(void);
	~CActiveMemberList(void);

	void RefreshView();

	void SetWndGrid(HWND newVal) { m_hWndGrid = newVal; }

	typedef CMap<UINT, UINT, CActiveMember, CActiveMember&> ActiveMemberMap;

public:
	// 通知客户端尝试包时重新登录的结构描述
	struct TReLoginRequest
	{
		static const UINT TRY_MAX = 6;   // 最大尝试次数
		UINT memberId;		// 会员ID
		UINT lastRefNo;		// 用于判断是否尝试登录成功的引用标号
		UINT tryCount;		// 尝试的次数 
		UINT firstTryTime;	// 第一次尝试时间
	};

	// 2011-03-30-gxx: 添加一个客户端包时重新登录的尝试
	void AddReLoginRequest(TReLoginRequest& btt);
	
	void TryReLogin();

private:

	CArray<TReLoginRequest> m_arrReLoginRequest;

private:

	HWND m_hWndGrid;

	//!<存储全部激活用户数据
	ActiveMemberMap m_ActiveMemberMap;
	
	//!<存储更新的用户索引，用于快速刷新界面显示
	CArray<UINT, UINT> m_UpdateMemberIdArray;

	POSITION m_CurPos;

public :

	::CCriticalSection ActiveMemberListLock;

	BOOL LoadFromDB();

	void Copy(ActiveMemberMap& OtherActiveMemberMap);
	void Copy(CArray<CActiveMember>& arrMembers);

	void Clear();

	INT_PTR GetCount();

	// 2011/07/14-8201-gxx: 
	void AddActiveMember(NS_DAL::CLocalRegisterInfo& RegisterInfo);

	// 2011/05/19-gxx: 增加新用户	,修改函数的声明,添加2参数
	void AddActiveMember(CBalanceMethod& Balance, 
		BOOL bRegisterBT=FALSE, // 是否开户包时
		UINT nPayout=0,
		int nBundleTimeType = CActiveMember::EPayType_Capped,
		int nScanType=0); // 开户包时费用
	
	//更新用户其他信息
	void UpdateActiveMember(CActiveMember& ActiveMember, BOOL bUpdateDb = FALSE);
	
	//用户充值，购物等更新
	void UpdateActiveMember(UINT nMemberId, UINT nCreditMoney, UINT nCreditPresent, UINT nPayout = 0);

	//删除用户
	BOOL RemoveActiveMember(UINT nMemberId);
	
	//有关当前已经更新的用户
	BOOL GetUpdateActiveMemberId();

	//
	BOOL GetActiveMemberNetID(CString strNetId,CActiveMember& ActiveMember, BOOL bFullMatch=TRUE);

	//通过会员ID获取会员信息
	BOOL GetActiveMember(UINT nMemberId, CActiveMember& ActiveMember);

	//通过终端ID获取会员信息
	BOOL GetActiveMember(CString strTermId, CActiveMember& ActiveMember, BOOL bFullMatch=TRUE);

	// 2011/07/04-gxx: 	一个终端ID可能对应多个用户，返回上报时间最近的那个ActiveMember
	BOOL GetActiveMemberBest(CString strTermId, CActiveMember& ActiveMember, BOOL bFullMatch=TRUE);

	//枚举，调用之前必须先锁定
	void StartEnumActiveMember();
	BOOL GetEachActiveMember(CActiveMember& ActiveMember);

	//2011-03-21-gxx-添加：获取余额总和(只计算临时用户)
	INT  GetDynamicReserve(UINT nCasherID);

	//清空所有的会员，数据库中的会员
	BOOL ClearAllMembers();

	// 2011/06/13-gxx: (私有变成公有)
	void CheckActiveMember(UINT nCardId, UINT nMemberId, CActiveMember &ActiveMember);

	// 2011/06/29-gxx: 是否允许充值
	BOOL IsAllowCredit(UINT nMemberId);

	// 2011/07/08-8201-gxx: 某账号对应的用户是否为本地用户, emUserStateNULL,emUserStateCenter,           // 中心用户
	//                      emUserStateLocal 
	int GetUserStateByNetId(CString strNetId);

	// 2011/07/12-8201-gxx: 
	BOOL HasLocalUser();

	// 2011/07/15-8201-gxx: 检查本地用户
	void CheckLocalUser(CActiveMember& ActiveMember);

	// 2011/07/27-8201-gxx: 获取本地用户充值总额, nCasherID收银台ID
	INT GetLocalTotalMoney(UINT nCasherID);

	// 2011/08/10-8201-gxx: 
	void CheckBrokenCenterUser(CActiveMember& ActiveMember);

	// 2011/10/26-8230-gxx: 填充包房信息的在线数量，和主账号
	BOOL FillRoomInfo(NS_DAL::CRoomInfo& info);

	// 2011/10/26-8230-gxx: 判断某个区域是否为包房区域，如果是，那么返回
	BOOL IsRoomArea(UINT pcClass, UINT& nRoomMemberID);

	// 2011/10/26-8230-gxx: 
	BOOL GetRoomSMembers(UINT nRoomMemberID,CArray<UINT>& arrMembers);

	// 2011/11/02-8210-gxx: 
	void CheckRoomUser(CActiveMember& ActiveMember);

	// 2011/11/02-8210-gxx: 
	void ClearRoomMembers(UINT nRoomMemberID);

public:
	BOOL UpdateActiveMemberDB(CActiveMember &ActiveMember);
	
	BOOL RemoveActiveMemberDB(UINT nMemberId);

private :

	virtual INT Run();

	void CheckAllActiveMember();
	
private:

	BOOL AddActiveMemberDB(CActiveMember &ActiveMember);
};


