
#pragma once

#pragma pack(1)

//******************************************************************************

//  结构体定义

//******************************************************************************

struct DataX_UserInfo    //用户信息
{
	DWORD cbLen;		 //结构大小
	DWORD dwVer;		 //版本
	
	char  realNameType;  //实名类型

	char  cardNo[31];	 //卡号
	int   cardType;		 //卡类型，参见表1：卡类型
	
	char  idNo[31];		 //证件号码
	int   idType;        //证件类型，参见表2：证件类型
	
	char  password[31];  //密码
	int   passFlag;      //密码标志，0表示任意密码；1表示指定密码
	
	char  name[31];      //用户姓名
	char  sex;           //性别

	char termId[MAX_COMPUTERNAME_LENGTH + 1]; //电脑名称
	char termIp[16];//电脑IP

	DWORD dwReserve1;
	DWORD dwReserve2;

	char  Reserve1[128]; //保留空间
};


//******************************************************************************

//	实名到计费

//******************************************************************************

#define DataX_RN2DDN_CARDINFO			101	//刷卡

#define DataX_RN2DDN_FORCECHECKOUT		102	//强制结账

#define DataX_RN2DDN_ADDMEMBER			103	//增加会员

//******************************************************************************

//	计费到实名

//******************************************************************************


#define DataX_DDN2RN_CASHIERCHECKIN		201	//收银员登录

#define DataX_DDN2RN_SHOWREALNAME		202	//显示实名系统相关界面

//******************************************************************************

#pragma pack()