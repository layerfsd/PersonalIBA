#pragma once

namespace NS_DAL
{

//------------------------------------------------------------------------
class GPropertyClass CClientShortMessage
{
public:
	CClientShortMessage()
	{
		GPropertyInit3( 
			MsgID, MsgType, MemberId,
			0,     0,       0);
	}

public:
	GProperty( MsgID,      UINT ); //客户端消息ID
	GProperty( MsgType,    UINT ); //消息类型
	GProperty( MemberId,   UINT ); //用户ID
	GProperty( TermIP,  CString ); //客户端IP
	GProperty( TermID,  CString ); //客户端ID
	GProperty( MsgBody, CString );//消息本体
	GProperty( RevTime, CString );//接收消息的时间
};
typedef CArray<CClientShortMessage, CClientShortMessage&> CClientShortMsgArray;


//------------------------------------------------------------------------
class GPropertyClass CIDNameRecord
{
public :
	CIDNameRecord()
	{
		GPropertyInit( ID, 0 );
	}

public:
	GProperty( Name, CString );
	GProperty( ID,      UINT );
};

typedef CArray<CIDNameRecord, CIDNameRecord&> CIDNameRecordArray;


//------------------------------------------------------------------------
class GPropertyClass CIDTypeInfo
{
public:
	CIDTypeInfo(void)
	{
		GPropertyInit( IDTypeID, 0 );
	}

public:
	GProperty( IDTypeName, CString );
	GProperty( IDTypeID,      UINT );
};
typedef CArray<CIDTypeInfo, CIDTypeInfo&> CIDTypeInfoArray;


//------------------------------------------------------------------------
class GPropertyClass CShiftCash
{
public:
	CShiftCash()
	{
		GPropertyInit4(
			ID,            StandbyAmount,  Deposit,        CreditAmount,
			0,             0,              0,              0);
		GPropertyInit4(
			PresentAmount, CardSoldAmount, CashSoldAmount, CardCreditedAmount,
			0,             0,              0,              0);
		GPropertyInit4(
			TotalAmount,   CostExpense,    CashRegisterID, FetchAmount,
			0,             0,              0,              0);
		GPropertyInit4(
			ReturnAmount,  Payout,         CountAmount,    NextRemainAmount,
			0,             0,              0,              0);
		
		GPropertyInit4( ReturnAmount, Dianka, StoreInAmount, DutyIncome,
			            0,            0,      0,             0);
	}
private:
	GProperty( ID,                 INT ); //
	GProperty( StartDate,      CString ); //
	GProperty( StandbyAmount,      INT ); //;
	GProperty( Deposit,            INT ); //;
	GProperty( CreditAmount,       INT ); //
	GProperty( PresentAmount,      INT ); //
	GProperty( ReturnAmount,       INT ); //
	GProperty( CardSoldAmount,     INT ); //
	GProperty( CashSoldAmount,     INT ); //
	GProperty( CardCreditedAmount, INT ); //
	GProperty( TotalAmount,        INT ); //
	GProperty( CostExpense,        INT ); //
	GProperty( Operator,       CString ); //
	GProperty( NextOperator,   CString ); //
	GProperty( Description,    CString ); //
	GProperty( EndDate,        CString ); //
	GProperty( CashRegisterID,     INT ); //
	GProperty( FetchAmount,        INT ); //
	GProperty( RemainAmount,       INT ); // 上班预留
	GProperty( NextRemainAmount,   INT ); // 下一班预留
	GProperty( Payout,             INT ); //
	GProperty( CountAmount,        INT ); //
	GProperty( Dianka,             INT ); // 点卡销售
	GProperty( StoreInAmount,      INT ); // 商品采购支出
	GProperty( AccountPay,         INT ); // 账户购物
	GProperty( ZSBConsume,         INT ); // 增收宝消费
	GProperty( ZSBBalance,         INT ); // 预存款余额
	GProperty( DutyIncome,         INT ); // 班次收入

};

typedef CArray<CShiftCash, CShiftCash&> CShiftCashArray;

typedef CArray<CString, CString&> COperatorArray;


//------------------------------------------------------------------------
class GPropertyClass CCheckOutinfo
{
public:
	CCheckOutinfo()
	{
		GPropertyInit3(
			MemberId,   ClassId,       BalanceMoney,
			0,          0,             0);

		GPropertyInit3(
			ClassState, GuarantyMoney, ReturnState,
			0,          0,             0);
	}
private:
	GProperty( CheckOutTime, CString ); // 结账时间
	GProperty( SerialNum,    CString ); // 上网账号
	GProperty( ClassName,    CString ); // 用户级别名称
	GProperty( TerminalId,   CString ); // 终端号
	GProperty( UserName,     CString ); // 用户名称
	GProperty( PersonalId,   CString ); // 证件号码
	GProperty( ClassState,      UINT ); // 用户级别状态 0:普通用户，1:会员
	GProperty( MemberId,        UINT ); // 用户ID
	GProperty( ClassId,         UINT ); // 用户级别ID
	GProperty( BalanceMoney ,   UINT ); // 结算总退款金额
	GProperty( GuarantyMoney,   UINT ); // 押金
	GProperty( ReturnState,     UINT ); // 退款状态
};

typedef CArray <CCheckOutinfo, CCheckOutinfo&> CCheckOutInfoArray;


//------------------------------------------------------------------------
class GPropertyClass CLastUserInfo
{
private:
	GProperty( TermId,     CString ); // 终端号
	GProperty( NetId,      CString ); // 上网账号
	GProperty( PassWord,   CString ); // 密码
	GProperty( MemberId,      UINT ); // 用户ID
	GProperty( UpdateTime, CString ); // 更新时间
	GProperty( Suspend,       UINT ); // 挂机状态
	GProperty( LoginStyle,    UINT );

public:
	CLastUserInfo() 
	{
		GPropertyInit3(
			MemberId, Suspend, LoginStyle,
			0,        0,       0);
	}
};

// 2011/07/20-8201-gxx: 保存正常状态的相关信息
class GPropertyClass CLocalStatusInfo
{
public:
	GProperty( CasherAccess, CString ); // 收银员权限字符串
	GProperty( ControlParam, CString ); // 中心指定的网吧控制参数
};

// 2011/07/14-8201-gxx: 注册本地用户
class GPropertyClass CLocalRegisterInfo
{
public:
	GProperty( NetbarId,      UINT ); // 网吧ID
	GProperty( MemberId,      UINT );
	GProperty( UserClass,     UINT ); // 默认临时用户，本地用户只支持临时用户
	GProperty( NetId,      CString ); // 上网账号
	GProperty( IdType,        UINT ); // 证件类型
	GProperty( PersonalId, CString ); // 证件号
	GProperty( Sex,           UINT );
	GProperty( UserName,   CString );
	GProperty( Money,         UINT ); // 开户金额
	GProperty( Password,    CString); 

public:
	CLocalRegisterInfo()
	{
		GPropertyInit3( NetbarId, MemberId, UserClass,
			            0,        0,        0);
		GPropertyInit3( IdType, Sex, Money,
			            0,      0,   0);
	}
};

// 2011/07/18-8201-gxx: 本地消费记录
class GPropertyClass CLocalConsumeInfo
{
public:
	GProperty( SubmitTime,   CString ); 
	GProperty( NetbarId,        UINT );
	GProperty( RefNo,           UINT );
	GProperty( SerialNo,        UINT );
	GProperty( SerialNum,    CString );
	GProperty( Name,         CString );
	GProperty( IdNumber,     CString );
	GProperty( MemberId,        UINT );
	GProperty( CreditAmount,    UINT );
	GProperty( ConsumeAmount,   UINT );
	GProperty( ReturnAmount,    UINT );
	GProperty( CheckinTime,  CString );
	GProperty( CheckoutTime, CString );
	GProperty( TermId,       CString );
	GProperty( TimeConsume,     UINT );  // 可忽略，使用默认
	GProperty( Operator,     CString );
	GProperty( DataStatus,       INT );  // 可忽略，使用默认
	GProperty( ClassId,         UINT );  
	GProperty( PayType,         UINT );
	GProperty( PCClass,         UINT );  // 机器区域ID
	GPropertyReadOnly( CheckData,    CString );

public:
	void MakeMD5();

public:
	CLocalConsumeInfo();
};

// 2011/10/25-8230-gxx: 包房信息
class GPropertyClass CRoomInfo
{
public:
	GProperty( RoomID,       UINT );       // 包房ID， 即机器区域
	GProperty( RoomName,  CString );  // 包房名称
	GProperty( MemberID,     UINT ); // 包房主账号的用户号,如果有主账号,那么此包房状态为:包房计费
	                             // 如果没有主账号并且包房区域内没有人上机，那么包房状态为：空闲包房
	                             // 如果没有主账号并且包房区域内有人上机，那么包房状态为：单击计费
	GProperty( NetId,     CString );    // 主上网账号
	GProperty( OnlineCount,   INT );    // 包房在线人数
	GProperty( ComputerCount, INT );    // 包房机器总数
	GProperty( TermID,    CString );    // 包房里的某台电脑的终端号
	GProperty( OpenTime,  COleDateTime ); // 如果为包房计费，那么就表示包房的开通时间,其他情况都是无效时间

	GProperty( UserClass,     INT );    // 用户级别
public:
	CRoomInfo()
	{
		GPropertyInit4(RoomID,MemberID,OnlineCount,ComputerCount,0,0,0,0);
		GPropertyInit(UserClass, 0);
	}
};
typedef CArray<CRoomInfo,CRoomInfo&> CRoomInfoList;


typedef CArray  <CLastUserInfo, CLastUserInfo&> CLastUserArray;

}