#pragma once
#include "LocalMember.h"

class CActiveMember;

namespace NS_DAL{
class CCheckOutinfo;
class CLocalConsumeInfo;
}

class GxxDictionary;


class CCashierHelper
{
public:
	CCashierHelper(void);
	~CCashierHelper(void);

public:
	static BOOL InitDiankaPlatform();
	static BOOL UninitDiankaPlatform();

	static BOOL SellDianka();// 点卡销售

	static BOOL CashSellDianka(LPCTSTR lpURL);// 处理客户端发送过来的现金购买点卡的请求

public://本地模式

	static void MakeCheckOutInfo(CActiveMember& , NS_DAL::CCheckOutinfo&);

	static void MakeLocalConsumeInfo(CActiveMember& , NS_DAL::CLocalConsumeInfo&);

	static BOOL RemoveLocalCredit(CString checkCode);//删除本地充值记录

	static BOOL RemoveLocalCredit(UINT MemberId);//删除本地充值记录

	static UINT CalcExpense(CString rate,time_t lastUpdateTime,UINT nLastBalance);//本地计费

	static UINT CalcRemainTime(UINT nBalance,time_t LastUpdateTime,CString strRate);//计算剩余时间

	static CString DoCredit_InLocal(CActiveMember&, UINT nCreditMoeny);//本地充值
	
	static void ReportLocalCredit(GxxDictionary* pLocalMember);// 上报本地充值记录给中心
	
	static void ReportLocalReturn(CLocalMember& localMember);//上报本地退款记录
	
	static void ReportLocalReturnVip(CLocalMember& localMember);//上报本地结账记录
	
	static void ReportLocalConsume(CLocalMember& localMember);//上报本地消费记录
};
