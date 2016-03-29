#pragma once

//对应
class CLocalMember
{
public:
	CLocalMember::CLocalMember(void):nMemberId(0),nBalance(0),nBeforeUsed(0),
		nNextCheckinTime(0),nPcClass(0),nClassId(0),nPayType(0),nIsLocalCheckin(0),
		nLastUpdateTime(0),nIsReturned(0){};
	CLocalMember::~CLocalMember(void){};

public:
	//对应ActiveMember表
	int nMemberId;                    //用户号
	int nBalance;		              //余额
	int ntotalMoney;                  //总金额(总金额在创建时=余额)
	int nBeforeUsed;				  //已消费金额-对应ActiveMember.Amount
	CString strMemberName;			  //用户名
	CString strNetId;				  //NetId
	CString strIdNumber;			  //Personal ID
	COleDateTime checkinTime;		  //终端登录时间
	int nNextCheckinTime;             //包时到期时间
	CString strTermId;				  //创建LocalMember记录时,可以传进参数.否则=ActiveMember.TermId
	int nPcClass;
	int nClassId;	
	int nPayType;	

	//独有字段
	COleDateTime localCheckinTime;   //HandleLocalLogOn时更新为当前时间
	int nIsLocalCheckin;             //HandleLocalLogOn时,如localMember中没有checkin时间,表示之前没登陆,说明是本地登录终端.设为1
	int nLastUpdateTime;			 //创建时为创建的当前时间,本地计费更新为计费的当前时间
	COleDateTime checkoutTime;       //本地模式下结账时的当前时间
	int nIsReturned;				 //是否已结账,创建时等于0
	CString strRate;				 //用pcclass和classId到CostRate表中查找的结果
};

typedef std::vector<CLocalMember> LocalMembers;

class CLocalReport
{
public:
	CLocalReport::CLocalReport(void):nMemberId(0){};
	CLocalReport::~CLocalReport(void){};

public:
	int nMemberId;
	CString strNetId;
	CString strPassword;
	COleDateTime reportTime;
};