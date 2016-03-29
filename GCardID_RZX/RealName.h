#pragma once

//不要改变下面的宏定义

#define WM_REALNAME WM_USER + 100

// RZX

#define RN_RZX_IDUSER 1
#define RN_RZX_ADDMEMBER 2

//GLW
#define RN_GLW_IDUSER 101

//******************************************************************************

//!< RZX相关函数接口（开始）

#define WM_USER_IDCARD_MANAGE    WM_USER + 2000

//WPARM 参数
#define IDCARD_MANAGE_ADD        	1 // 发卡
#define IDCARD_MANAGE_LOSD       	2 // 挂失
#define IDCARD_MANAGE_MODIFY     	3 // 修改密码
#define IDCARD_MANAGE_MEMBER     	4 // 加入会员
#define IDCARD_MANAGE_CLOSE     	5 // 关闭程序
#define IDCARD_MANAGE_GETPWD   		6//忘记密码（忘记密码后）
#define IDCARD_MANAGE_DOWNLOST		7//手动下载挂失库
#define IDCARD_MANAGE_ABOUT			8 //关于
#define IDCARD_MANAGE_VIEWCARD   	9//查看开卡记录
#define IDCARD_MANAGE_VIEWLOST		10//查看挂失记录


typedef struct REQ_REGISTER_INFO_tag
{
	char szName[31];		 //用户名
	char szCertificateType[3];//证件类型
	char szCertificateID[19]; //证件号码
	char szCompany[31];   //公司或地址名称
	char szCountry[11]; 	//国家名
	BYTE bFolk; // 名族
	char szDetail[30]; 		//其它信息
}REQ_REGISTER_INFO;


int IDUser(char *pszIDNo, char *pszUserNo, char *pszPassword,
			BYTE IDCardType, BOOL bValidate, const REQ_REGISTER_INFO& RegInfo);

int AddToMemberRZX(char *pszIDNo, char *pszUserNo, 
					char *pszPassword,BYTE IDCardType, const REQ_REGISTER_INFO& RegInfo);

int RzxNotiFy(int nNotifyType, const char * lpNotifyMsg);

//!< RZX相关函数接口（结束）

//******************************************************************************