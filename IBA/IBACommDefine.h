
#pragma  once

namespace NS_IBACommDef{
	enum Seat2Type { MainIBA = 0, WC = 1, Entrance = 2, Refrigerator = 3, Seat2Count = 4};
	enum EUserClass{
		Euc_TempUser = 1,		//!>临时用户
		Euc_MemberUser = 2,		//!>会员用户
		Euc_PostPaid = 3,		//!>后付费
		Euc_Employee = 4,     //!>员工卡
	};
	// 用户的付款类型
	enum EMemberPayType{
		EPayType_General = 0,    //!>普通计费
		EPayType_Capped = 1,     //!>累计封顶包时
		EPayType_Withholding =2,  //!>足额预扣包时
		EPayType_Coupon		=3,	 //!>配套包时
		EPayType_Package	=4,	 //!>优惠包时
	};
	const UINT PostPaidMoney = 999900;
	const TCHAR SZPostPaidMoney[] = _T("9999");
}