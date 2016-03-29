#pragma once

#include "..\Singleton.h"

#include "ConfigInfo.h"
#include "NetBarInfo.h"
#include "IDTypeInfo.h"
#include "DALJXC.h"
#include "DALUSER.h"
#include "..\Servlet\UserClassInfo.h"
#include "..\Servlet\BalanceMethod.h"
#include "..\NetBarUser.h"
#include "..\Servlet\MemberInfoMethod.h"
#include "..\LocalCenter\gxx_base.h"
#include "..\activemember.h"
#include "..\LocalCenter\LocalCenter.h"
#include "..\LocalMember.h"

using namespace NS_SERVLET;

namespace NS_DAL
{

class CIBADAL
{
	SINGLETON_DECLARE(CIBADAL)
	
public :
	static CIBADAL* GetInstance();

	BOOL GetDBConnInfo(CADODBConnInfo* & pDb, BOOL bForceConnect = TRUE);

	BOOL ExecuteSQL(CString strSQL);

	// @return -1: 执行失败
	// @return >=0: 返回受影响的行数
	int ExecuteSQLEx(CString strSQL);

	int CountOfRecordsWithSQL(CString strSQL);

	BOOL AddCashierToDB();
	BOOL UpdateInternetCafeInfo();
	//系统配置相关
	BOOL GetAllSystemConfig(CSystemConfigArray & ConfigArray);
	BOOL GetMaxKeyId(INT & nMaxKeyId);
	BOOL AddSystemConfig(CConfigInfo & configInfo);
	BOOL GetSystemConfig(CConfigInfo & configInfo);
	BOOL UpdateSystemConfig(CConfigInfo & configInfo, UINT whichKey = 1);

	BOOL GetAllIDTypeInfo(CIDTypeInfoArray& IdTypeInfos);
	BOOL GetAllNetBarInfo(CNetBarInfoArray& NetBarInfoArray);
	BOOL GetAllPCClassInfo(CIDNameRecordArray& IDNameRecords);
	BOOL GetAllUserClassInfos(CUserClassInfoArray & UserClassInfoRecords);

	BOOL GetShiftCash(CShiftCashArray& ShiftCashArray, CString strStartTime, CString strEndTime, CString strCashier);
	int GetLocalCounting(int &Keyvalue1, int &Keyvalue2);
	BOOL UpdateLastUpdateTime_LocalCounting();
	BOOL UpdateCashierandTime_LocalCounting();
	BOOL AddLocalCounting();
	
	//充值退款等

	BOOL AddCardCreditRecord(CCreditInfoDB & CreditInfo);
	BOOL AddCreditRecord(CCreditInfoDB & CreditInfo);
	BOOL AddCardOpenRecord(CCardOpenInfo & CardOpenInfo);
	BOOL AddCardCloseRecord(CCardCloseInfo & CardCloseInfo);
	BOOL AddReturnedRecord(CReturnedInfo & ReturnedInfo);
	BOOL AddShiftCashRecord(CShiftCash & ShiftCash);
	BOOL GetShiftCashRecord(INT &nRemainBalance);
	BOOL AddCreditReverseInfo(CCreditInfoDB& CreditInfo);

	

	// 2011/07/19-8201-gxx:
	BOOL AddLocalConsume(CLocalConsumeInfo& ConsumeInfo); 
	BOOL DeleteLocalConsume(UINT nMemberID);
	void GetLocalConsumes(CArray<CLocalConsumeInfo,CLocalConsumeInfo&>& ConsumeInfoArray);

	/**
	* 获取最后上机的用户
	*/
	BOOL GetTermLastUser(CString strTermId, UINT& nMemberId, CString& strNetId);
	BOOL GetNetBarUserInfo(UINT nMemberId, CNetBarUser & NetBarUser);
	BOOL GetMemberId(CString strNetId, UINT &nMemberId);
	BOOL DeleteLastUser(UINT nMemberId);

	//进销存相关
	//取得商品单位
	BOOL GetCommodityUint(CString strUintCode, CString & strCNUintName);

	BOOL GetCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition);

	BOOL GetStoreCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition);
	//按价格分类
	BOOL GetPriceCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition);
	//按种类和价格分类
	BOOL GetSortPriceCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition);
	BOOL AddRetailVoucher(CRetailVoucher& RetailVoucher);
	BOOL AddRetailDetail(CRetailDetail& RetailDetail);
	BOOL GetRetailInfo(CRetailInfoArray & RetailInfoArray, CRetailQueryCondition & RetailQueryCondition);
	BOOL GetCommoditySoldInfo(CString strCommodityCode, CCommoditySoldInfo& CommoditySoldInfo);
	
	BOOL ZeroAllCommodityRemainQuantity();
	BOOL UpdateCommodityRemainQuantity(CString strCommodityCode, UINT nRemainQuantity);
	
	BOOL ZeroAllCommodityStoreInQuantity();
	BOOL UpdateCommodityStoreInQuantity(CString strCommodityCode, UINT nStoreInQuantity, UINT nStoreInAmount);

	//商品统计数据
	BOOL GetCommoditySoldInfo(CCommoditySoldInfoArray& CommoditySoldInfoArray);
	
	//用户升级
	BOOL AddMemberUpgradeRecord(CMemberUpgradeDB& MemberUpgradeInfo);
	BOOL GetMemberUpgradeRecord(UINT nMemberId, CMemberUpgradeDB& MemberUpgradeInfo);

	//客户端消息
	BOOL AddClientShortMsg(CClientShortMessage& MemberUpgradeInfo);
	BOOL GetClientShortMsg(CClientShortMsgArray& MsgArray, CString strStart, CString strEnd);

	//自动生成的账号
	BOOL SetAutoNetIdFromRecord(CString newValue);
	CString GetAutoNetIdFromRecord();

	//操作员信息
	BOOL AddOperatorRecord(CString& strOperator);
	BOOL GetOperatorRecord(COperatorArray& OperatorArray);

	//全场退款数据库操作

	UINT GetRoundType(UINT nNetbarId);
	BOOL DeleteAllCheckOutInfo();

	//更新当前在线的用户信息
	BOOL UpdateCheckOutInfo(CString strNetId, UINT nReturnState);
	BOOL AddCheckOutInfo(CCheckOutinfo& CheckOutinfo);
	BOOL GetCheckOutInfo(CCheckOutInfoArray& CheckOutArray);

	//上机用户资料
	BOOL UpdateLastUserInfo(CString strTermId,  CString strUpdateTime, UINT nMemberId);
	BOOL AddLastUserInfo(CLastUserInfo& LastUserInfo);
	
	//查询，可按两种方式查询，如果按nMemberId查询，strTermId为空；按strTermId，nMemberId = 0；
	BOOL GetLastUserInfo(CLastUserInfo& LastUserInfo, UINT nMemberId = 0, CString strTermId = _T(""));
	
	//删除特定终端项
	BOOL DeleteLastUserInfo(CString strTermId);
	
	BOOL UpdateMemberInfo(CBalanceMethod & Balance);

	// 更新民族，发证单位，身份证地址
	BOOL UpdateMemberInfo(int MemberID, LPCTSTR szNation, LPCTSTR szIDDepart, LPCTSTR szIDAddr, LPCTSTR szTel);

	// 2011/08/16-8201-gxx: 上传本地用户信息时，需要写入新的memberID的用户信息到member表
	BOOL AddMemberInfo(CLocalRegisterInfo & registerInfo);
	BOOL UpdateMemberInfo(CLocalConsumeInfo & localInfo, UINT nNewMemberID);

	// 2011/10/25-8230-gxx: 获取包房的终端总数
	BOOL GetRoomsComputerCount(CRoomInfo& roomInfo);

	// 2014-6-10 - qsc 获取表记录数
	ULONG GetTableRecordCount(LPCTSTR szTable, LPCTSTR szWhere = NULL);

	// 2014-10-10 - qsc 通过终端ID获取IP
	BOOL GetIPFromTermId(LPCTSTR szTermId, CString &strIP);

	// 2014-10-15 - qsc 智多的相关操作
	BOOL AddZdRnInfo(LPCTSTR szSN, LPCTSTR szTermId, LPCTSTR szUserName );
	BOOL DelZdRnInfo(LPCTSTR szSN);
	BOOL GetZdRnInfo(LPCTSTR szSN, CString &strTermId, COleDateTime& updateTime);
	void DelZdRnOutDateData();
	// 2014-10-29 - qsc
	BOOL GetZdRnInfoByTermId( LPCTSTR szTermId, CString &strSN, CString &strName );
	BOOL SetHaveChange( LPCTSTR szSN, int HaveChange );
	BOOL GetHaveChange( LPCTSTR szSN, int& HaveChange );
	// 2015-0820 liyajun 获取扣率 返回每小时多少分
	BOOL GetCostRate(int nWeek, int nHour, int nClassId, int nPCClass , int& nCostRate );
	CString GetCostRateFromCostRate(int nClassId, int nPCClass);
	//AllowReturn表操作，值来自Balance接口
	BOOL UpdateAllowReturn(UINT UserClass,UINT AllowReturn,BOOL IsMember);
	int GetAllowReturn(UINT UserClass, UINT* pIsMember = NULL);


	/**
	 * 得到终端ID所在的机器区域ID, @return 0:表示无效的机器区域ID
	*/
	int GetPCClassIDWithTermID(LPCTSTR lpszTermID);
	GxxArrayPtr ReadRecordsFromDB( LPCTSTR lpszSql);
	GxxDictionaryPtr ReadRecordFromDB (LPCTSTR lpszSql);

	//LocalMember表操作
	int UpdateLocalMember_CreditInfo(UINT nMemberId,UINT nCreditAmount);
	int UpdateLocalMember_RateInfo(UINT nMemberId,CString strRate);
	int UpdateLocalMember_PCClassId(UINT nMemberId,UINT nPCClassId);
	int UpdateLocalMember_CheckInInfo(UINT nMemberId,CString strTermId,COleDateTime localcheckintime,COleDateTime CheckinTime=COleDateTime(0,0,0,0,0,0), BOOL bIsLocalCheckin=FALSE);
	int UpdateLocalMember_PayTypeInfo(UINT nMemberId,UINT nPayType,int nextCheckinTime,BOOL bUpdateLastUpdateTime = TRUE);
	int UpdateLocalMember_BalanceInfo(UINT nMemberId,UINT nNewBalance);
	int UpdateLocalMember_CheckOutInfo(UINT nMemberId,COleDateTime CheckOutTime,BOOL bIsReturned);
	BOOL GetLocalMemberByMemberID(int nMemberId, CLocalMember& LocalMember);
	BOOL GetLocalMemberByTermID(CString strTermID,CLocalMember& LocalMember);
	BOOL GetAllLocalMember(LocalMembers& lMembers);
	BOOL RemoveLocalMember(UINT nMemberId);
	int GetCountOfOnlineUser();

	GxxDictionaryPtr CreateNewLocalMemberFromActiveMember(CActiveMember &ActiveMember, LPCTSTR lpszTermID= NULL);   //从ActiveMember对象创建localMember记录
	GxxDictionaryPtr CreateNewLocalMemberWithMemberID(UINT nMemberID, LPCTSTR lpszTermID = NULL);   //从ActiveMember表和CostRate表创建localMember记录
	BOOL InsertNewLocalMember(GxxDictionaryPtr memberPtr);
	
	//LocalCredit操作
	BOOL InsertNewLocalCredit(UINT nMemberId, UINT nCreditAmount, CString strCheckCode);
	BOOL DeleteLocalCredit(CString strCheckCode);

	//LocalReport操作
	BOOL GetLocalReport(CString strNetId, CString strPassword, CLocalReport& lr);

	//Member操作
	BOOL GetUserNameFromMember(CString strSerialNum, CString& strUserName);
	BOOL GetCardIdFromMember(CString strSerialNum,UINT& nCardId);
	BOOL GetDepositFromMember(CString strSerialNum,UINT& nDeposit);
	GxxDictionaryPtr GetMemberFromMember(CString strSerialNum, BOOL bUseNetbarId = FALSE);

private :
	CADODBConnPool m_DBPool;//远程中心

	UINT m_nConnectState; //!< 第一次连接成功还是失败
};

}

using namespace NS_DAL;