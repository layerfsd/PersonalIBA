#pragma once

class CActiveMember;

namespace NS_DAL{
class CCheckOutinfo;
class CLocalConsumeInfo;
}


class CCashierHelper
{
public:
	CCashierHelper(void);
	~CCashierHelper(void);

	static void MakeCheckOutInfo(CActiveMember& , NS_DAL::CCheckOutinfo&);

	static void MakeLocalConsumeInfo(CActiveMember& , NS_DAL::CLocalConsumeInfo&);

	static BOOL ReturnLocalUser(CActiveMember&, NS_DAL::CLocalConsumeInfo* lpLocalInfo = NULL );

	static BOOL ReturnLocalCenterUser(CActiveMember& , NS_DAL::CCheckOutinfo* lpCheckOut = NULL );

	static BOOL CreditLocalUser(CActiveMember&, UINT nCreditMoeny);

public:
	static BOOL InitDiankaPlatform();
	static BOOL UninitDiankaPlatform();

	// 点卡销售
	static BOOL SellDianka();

	// 处理客户端发送过来的现金购买点卡的请求
	static BOOL CashSellDianka(LPCTSTR lpURL);

	
};
