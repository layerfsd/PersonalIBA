//------------------------------------------------------
//	这个接口类，需要金桥科技的动态库
//
//		ijqrnlog.dll
//		
//------------------------------------------------------
#pragma once

#include "..\Singleton.h"
#include <string>
#include <map>


//#include "Communication.h"

#define IK_BASE WM_USER + 1100
#define IK_ID_GETIDCARDDATA		IK_BASE + 10	//获得绑定卡相关数据
#define IK_ID_OPENUSER			IK_BASE + 11	//实名卡开户
#define IK_ID_CLOSEUSER			IK_BASE + 12	//实名卡关户
#define IK_ID_LOGINOUT			IK_BASE + 13	//实名卡解锁
#define IK_ID_LOGININ			IK_BASE + 14	//实名卡锁定
#define ID_ID_TOCLOSEOUT		IK_BASE + 15	//网吧停业


class CIZd
{
	SINGLETON_DECLARE(CIZd)	
public:
	static CIZd* GetInstance();

	typedef struct IC_CRC_RE_TAG
	{
		int iRe;					//CRC校验返回值  0：校验成功  -1：CRC校验错误  -2：版本号错误
		BOOL bIsHeadInputCard;		//是否允许手工输入上网卡号  TURE：允许  FALSE：不允许
		double dDeductMoney;		//单次刷卡扣点的金额，0.5元或其他金额
		int iDeductDistantTime;		//刷卡扣点间隔时间，单位：秒
		BOOL bIsMacAllInOrOut;		//是否允许全部终端锁定或解锁 TURE：允许  FALSE：不允许
	}IC_CRC_RE;

	// 开户和关户数据结构
	typedef struct T_IC_USER_TAG
	{
		TCHAR szikIDNo[21];			//实名卡编号
		TCHAR szikName[31]; 		//姓名
		int		iCertType;			//证件类型
		TCHAR szCertNumber[19];		//证件号码
		int   iCardType;			//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
		TCHAR szMobile[12];			//手机号	
		int	  iValidate;			//网络操作是否成功
	}T_IC_USER;

	// 开户和关户数据结构  
	typedef struct IC_USER_TAG
	{
		char szikIDNo[21];			//实名卡编号
		char szikName[31]; 		//姓名
		int		iCertType;			//证件类型
		char szCertNumber[19];		//证件号码
		int   iCardType;			//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
		char szMobile[12];			//手机号	
		int	  iValidate;			//网络操作是否成功
	}IC_USER;

	// 解锁和锁定数据结构
	typedef struct T_IC_LOGIN_TAG
	{
		TCHAR szikICNo[21];			//用户的绑定卡编号
		TCHAR szikIDNo[21];			//实名卡编号
		TCHAR szikIP[16];			//客户机IP地址
		TCHAR szikHost[31];			//客户机名称
		TCHAR szicPassword[16];		//实名卡密码
	}T_IC_LOGIN;

	// 解锁和锁定数据结构
	typedef struct IC_LOGIN_TAG
	{
		char szikICNo[21];			//用户的绑定卡编号
		char szikIDNo[21];			//实名卡编号
		char szikIP[16];			//客户机IP地址
		char szikHost[31];			//客户机名称
		char szicPassword[16];		//实名卡密码
	}IC_LOGIN;

	// 添加会员数据结构
	typedef struct ID_ADDINSIDR_TAG
	{
		char szikICNo[21];			//绑定卡编号
		char szikIDNo[21];			//实名卡编号
		char szikName[31]; 		//姓名
		int   iIDCardType;			//证件类型
		char  szCertificateID[19];	//证件号码
		int   iErrCode;				//错误号，详见错误定义表；0表示成功
		char  szErrMsg[101];		//错误描述，如果Error不为0的话
		int   iCardGetType;			//刷证类型
		int   iIsCheck;				//是否审核 0表示没有1个表示未审核2表示通过
	}ID_ADDINSIDR;

	// 扣点查询数据结构
	typedef struct IC_DEDUCT_TAG
	{
		TCHAR szBeginTime[15]; 	//开始时间
		TCHAR szEndTime[15]; 	//结束时间
		TCHAR szCashMacNum[31];	//收银端编号(可为空)
	}IC_DEDUCT;
	// 说明：时间定义14位，格式为：4位年，2位月，2位日，2位小时，2位分钟，2位秒

	// 获取上网卡密码数据结构
	typedef struct ID_GETCARDPWD_TAG
	{
		char szPWD[33];			//上网卡密码(MD5值进行BASE64)
		int  iErrCode;			//错误号，详见错误定义表；0表示成功
		char szErrMsg[101];		//错误描述，如果Error不为0的话
	}ID_GETCARDPWD;

	// 返回值信息
	typedef struct ID_RETURN_TAG
	{
		int  iErrCode;			//错误号，详见错误定义表；0表示成功
		char szErrMsg[101];		//错误描述，如果Error不为0的话
	}ID_RETURN;

	// 返回值描述结构
	typedef struct ID_RETURNS_TAG
	{
		TCHAR szRenturn[101]; 	//返回值描述
	}ID_RETURNS;

	// 上机返回值信息
	typedef struct ID_LOGINRETURN_TAG
	{
		char szCardId[21];		//上网卡号
		int  iCardType;			//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
		char szName[31]; 		//姓名
		int   iCertType;		//证件类型
		char szCertNumber[19];	//证件号码
		int  iErrCode;			//错误号，详见错误定义表；0表示成功
		char szErrMsg[101];		//错误描述，如果Error不为0的话
	}ID_LOGINRETURN;

	// 返回KEY剩余点数
	typedef struct KEYBALANCERETURN_TAG
	{
		int  iKEYBalance;		//KEY剩余点数
		int  iErrCode;		//错误号，详见错误定义表；0表示成功
		TCHAR szErrMsg[101];	//错误描述，如果Error不为0的话
	}ID_KEYBALANCERETURN;

	// 使用终端数据检查信息
	typedef struct ID_USEMACVERIFY_TAG
	{
		TCHAR szMacName[17];	//机器名称
		TCHAR szCardId[21];	//上网卡号
		int  iCardType;		//上网卡获取类型 0：二代证 1：手机 2：指纹 3：电子登记
		TCHAR szName[31]; 	//姓名
		int   iCertType;		//证件类型
		TCHAR szCertNumber[19];	//证件号码
	}ID_USEMACVERIFY;

	typedef struct IC_CARDDATA_TAG
	{
		TCHAR szikICNo[21]; //计费软件绑定卡编号
		int   iICCardType;  //卡类型（1–普通计费卡，2–会员卡，3 –管理卡，9 –其它）
	}IC_CARDDATA;

	//参数说明：
	//DWORD dwCRC		CRC值，用于有效性校验
	//char * FeeSoftVer	计费软件的版本号
	//返回值：
	//返回校验结果数据
	typedef IC_CRC_RE (__stdcall *FUN_V2_SetCRC)(DWORD dwCRC, char * FeeSoftVer);




	//计费
	//说明：
	//计费系统在接受到iKeeper系统已开户消息后，调用接口，通知ikeeper系统，等待返回成功后再给用户真正计费，否则根据函数返回值提示错误
	//参数说明：
	//char * pszikICNo	用户的绑定卡编号，长度为20位，若没有则为空值
	//char * pszikIDNo	用户的实名卡号，长度为20位，不能为空值
	//char * pszPwd		实名卡密码，长度为15位，可以为空值
	//char * pszikIP	客户机IP地址，格式如：192.168.0.1
	//char * pszDeductDotSign	是否扣点的标志  0：不扣点  1：扣点
	//char * pszCurTime	当前时间
	//char * pszCashMacNum 当前收银端编号，可为空值
	//pszCurTime 14位，格式为：4位年，2位月，2位日，2位小时，2位分钟，2位秒
	//例如：20071107123055->2007年11月7日，12点30分55秒
	typedef ID_LOGINRETURN (__stdcall *FUN_V2_OpenUserEx)(char * pszikICNo, char * pszikIDNo, char * pszPwd, char * pszikIP, char * pszDeductDotSign, char * pszCurTime, char * pszCashMacNum);



	//结帐
	//说明：
	//计费系统在服务端结帐时，通知ikeeper系统，发送信息到接口。
	//参数说明：
	//char * pszikICNo	用户的绑定卡编号，长度为20位，若没有则为空值
	//char * pszikIDNo	用户的实名卡号，长度为20位，不能为空值
	//char * pszikIP	客户机IP地址，格式如：192.168.0.1
	//char * pszikHost	客户机名称
	//char * pszCurTime	当前时间
	//pszCurTime 14位，格式为：4位年，2位月，2位日，2位小时，2位分钟，2位秒
	//例如：20071107123055->2007年11月7日，12点30分55秒
	typedef ID_RETURN (__stdcall *FUN_V2_CloseUser)(char * pszikICNo, char * pszikIDNo, char * pszikIP, char * pszikHost, char * pszCurTime);


	//
	//上机
	//说明：
	//计费系统在终端上机，调用接口，通知ikeeper系统，等待返回成功后再给用户真正上机，否则根据函数返回值提示错误
	//
	//参数说明：
	//char * pszikICNo	用户的绑定卡编号，长度为20位，若没有则为空值
	//char * pszikIDNo	用户的实名卡号，长度为20位，不能为空值
	//char * pszikIP	客户机IP地址，格式如：192.168.0.1
	typedef ID_RETURN (__stdcall *FUN_V2_Login)(char * pszikICNo, char * pszikIDNo, char * pszikIP);




	//上机（带密码参数）
	//说明：
	//计费系统在终端上机，调用接口，通知ikeeper系统，等待返回成功后再给用户真正上机，否则根据函数返回值提示错误
	//
	//参数说明：
	//char * pszikICNo	用户的绑定卡编号，长度为20位，若没有则为空值
	//char * pszikIDNo	用户的实名卡号，长度为20位，不能为空值
	//char * pszPwd		实名卡密码，长度为15位，不能为空值
	//char * pszikIP	客户机IP地址，格式如：192.168.0.1
	typedef ID_LOGINRETURN (__stdcall *FUN_V2_LoginEx)(char * pszikICNo, char * pszikIDNo, char * pszPwd, char * pszikIP);


	//下机
	//说明：
	//计费系统在下机时，通知ikeeper系统，发送信息到接口。
	//
	//
	//参数说明：
	//char * pszikICNo	用户的绑定卡编号，长度为20位，若没有则为空值
	//char * pszikIDNo	用户的实名卡号，长度为20位，不能为空值
	//char * pszikIP	客户机IP地址，格式如：192.168.0.1
	typedef ID_RETURN (__stdcall *FUN_V2_Logout)(char * pszikICNo, char * pszikIDNo, char * pszikIP);



	//申请添加会员
	//说明：
	//计费系统在实名刷卡已通过实名后台验证成功的实名后，允许将此用户加为会员，通知ikeeper系统，发送信息到接口
	//
	//
	//参数说明：
	//char * pszikICNo		用户的绑定卡编号，长度为20位
	//char * pszikIDNo		用户的实名卡号，长度为20位
	//char * pszicPassword	实名卡号密码，长度为15位
	//
	//返回值：
	//返回实名数据
	typedef ID_ADDINSIDR (__stdcall *FUN_V2_AddInsider)(char * pszikICNo, char * pszikIDNo, char * pszicPassword);


	//锁屏
	//说明：
	//网吧计费终端在实名绑定出现不符合的情况下，调用接口通知ikeeper系统进行终端锁屏，并根据函数返回值提示错误。
	//
	//参数说明：
	//char * pszikICNo;	//用户的绑定卡编号，必须填写
	//char * pszikIP;  	//打开客户机的IP地址，必须填写
	//char * pszikHost;	//打开用户客户机的计算机名，可为空
	typedef ID_RETURN (__stdcall *FUN_V2_LockMac)(char * pszikICNo, char * pszikIP, char * pszikHost);



	//发送绑定卡数据
	//说明：
	//计费系统在收到 IK_ID_GETIDCARDDATA 命令消息后，在WM_COPYDATA消息返回前，将绑定卡的数据发送给ikeeper系统
	//
	//参数说明：
	//IC_CARDDATA * pstICCardData		用户的绑定卡数据
	typedef ID_RETURN (__stdcall *FUN_V2_SendICCardData)(IC_CARDDATA * pstICCardData);


	//发送扣点数据查询
	//说明：
	//计费系统在查询扣点数据时，通知ikeeper系统，发送信息到接口。
	//
	//
	//参数说明：
	//IC_DEDUCT * pstDeductData    查询参数
	//
	//返回值：
	//-1：失败
	//非负数值：点扣除总数
	typedef int (__stdcall *FUN_V2_SendDeductData)(IC_DEDUCT * pstDeductData);

	//校验卡号是否正确
	//说明：
	//校验37位和39位磁卡、10位ID卡是否为场所用卡。校验成功在pszAvailableCard参数中返回正确卡号。
	//
	//参数说明：
	//pszSourceCard			待校验的源卡号(37或39位磁卡、10位ID卡)
	//pszAvailableCard		返回校验正确的卡号(10位卡号)
	//
	//返回值：是否校验成功
	//FALSE：失败
	//TURE：成功
	typedef BOOL (__stdcall *FUN_V2_VerifyCard)(char * pszSourceCard, char * pszAvailableCard);

	//获取实名卡密码
	//说明：
	//获取指定实名卡在本地暂存的密码，该密码为MD5加密值。
	//
	//
	//参数说明：
	//char * pszikIDNo		用户的实名卡号，长度为20位
	//
	//返回值：
	//返回结果数据
	typedef ID_GETCARDPWD (__stdcall *FUN_V2_GetCardPWD)(char * pszikIDNo);


	//获取后台服务器上的实名卡密码
	//说明：
	//获取指定实名卡在后台服务器保存的密码，该密码为MD5加密值。
	//
	//参数说明：
	//char * pszikIDNo		用户的实名卡号，长度为20位
	//
	//返回值：
	//返回结果数据
	//
	//接口定义：
	typedef ID_GETCARDPWD (__stdcall *FUN_V2_GetCardPWDEx)(char * pszikIDNo);

	//获取实名信息
	//说明：
	//计费系统不需要验证密码，申请获取实名信息。
	//
	//
	//参数说明：
	//char * pszikICNo		用户的绑定卡编号，长度为20位
	//char * pszikIDNo		用户的实名卡号，长度为20位
	//
	//返回值：
	//返回实名数据
	typedef ID_ADDINSIDR (__stdcall *FUN_V2_GetCustomer)(char * pszikICNo, char * pszikIDNo);

	//获取手机刷卡上网的验证码
	//说明：
	//使用手机刷卡上网时，需要获取随机的验证码，并用短信发送到指定的手机号码中。
	//
	//参数说明：
	//pszMobile				手机号
	//pszSMSSource			验证码短信来源
	//0:终端机获取
	//1:计费程序点右键菜单获取
	//2:计费程序刷卡确定时获取
	//
	//返回值：发送短信是否成功
	//FALSE：失败
	//TURE：成功
	typedef BOOL (__stdcall *FUN_V2_GetMobileVerificationCode)(char * pszMobile, char * pszSMSSource);


	//获取验证码发送传回值
	//说明：
	//发送验证码后，获取发送返回值
	//
	//参数说明：
	//pszMobile				手机号
	//返回值：发送短信返回值，若有错误则返回完整的错误描述
	typedef ID_RETURNS (__stdcall *FUN_V2_GetMobileVerificationCodeReturn)(char * pszMobile);

	//获取万象计费软件的帐号
	//说明：
	//获取万象计费软件的帐号数据。 
	//
	//参数说明：
	//pszAccounts				帐号
	typedef ID_RETURNS (__stdcall *FUN_V2_WXAccounts) (char * pszAccounts);



	//获取KEY剩余点数
	//说明：
	//获取KEY中的剩余点数。 
	typedef ID_KEYBALANCERETURN (__stdcall *FUN_V2_KEYBalance) ();


	//使用终端数据检查
	//说明：
	//计费程序将已经登录正在使用的终端的信息，以数据结构列表的方式发送到安全软件，以进行数据检查。 
	//参数说明：
	//pstUseMacVerfy		使用终端的信息，可为多条数据
	//iNum				终端信息条数
	typedef ID_KEYBALANCERETURN (__stdcall *FUN_V2_UseMacVerfy) (ID_USEMACVERIFY * pstUseMacVerfy, int iNum);

public:
	BOOL Load();
public:
	FUN_V2_OpenUserEx						m_pfn_V2_OpenUserEx;
	FUN_V2_Login							m_pfn_V2_Login;
	FUN_V2_LoginEx							m_pfn_V2_LoginEx;
	FUN_V2_Logout							m_pfn_V2_Logout;
	FUN_V2_AddInsider						m_pfn_V2_AddInsider;
	FUN_V2_LockMac							m_pfn_V2_LockMac;
	FUN_V2_SendICCardData					m_pfn_V2_SendICCardData;
	FUN_V2_SendDeductData					m_pfn_V2_SendDeductData;
	FUN_V2_VerifyCard						m_pfn_V2_VerifyCard;
	FUN_V2_GetCardPWD						m_pfn_V2_GetCardPWD;
	FUN_V2_GetCardPWDEx						m_pfn_V2_GetCardPWDEx;
	FUN_V2_GetCustomer						m_pfn_V2_GetCustomer;
	FUN_V2_GetMobileVerificationCode		m_pfn_V2_GetMobileVerificationCode;
	FUN_V2_GetMobileVerificationCodeReturn	m_pfn_V2_GetMobileVerificationCodeReturn;
	FUN_V2_WXAccounts						m_pfn_V2_WXAccounts;
	FUN_V2_KEYBalance						m_pfn_V2_KEYBalance;
	FUN_V2_UseMacVerfy						m_pfn_V2_UseMacVerfy;
	FUN_V2_SetCRC							m_pfn_V2_SetCRC;
	FUN_V2_CloseUser						m_pfn_V2_CloseUser;
	
	
	BOOL IsInit()	{ return m_bInitSuccess; }
	BOOL IsLoad()	{ return m_bLoadSuccess; }
	void SetInit(BOOL newVal) { m_bInitSuccess = newVal; }

	// 数据转化函数
	static void Conver_IC_USER( const IC_USER* pOrg, T_IC_USER *pDest );
	static void Conver_IC_LOGIN(IC_LOGIN* pOrg, T_IC_LOGIN *pDest );

	CIZd::ID_LOGINRETURN V2_LoginEx( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd, LPCSTR szIP);
	CIZd::ID_RETURN V2_Logout( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szIP);
	CIZd::ID_RETURN V2_CloseUser( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szIP, LPCSTR szTermId);
	// 最后一个参数是扣点标志 0：不扣点  1：扣点
	CIZd::ID_LOGINRETURN V2_OpenUserEx( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd, LPCSTR szIP, LPCSTR szDeductDotSign);
	CIZd::ID_ADDINSIDR V2_AddInsider( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd);
	CIZd::ID_GETCARDPWD V2_GetCardPWDEx(LPCSTR szIDNO);
	bool IsIDPwdCorrect(LPCSTR szID, LPCSTR szPwd);
	static int SwitchZd2Ddn(int nSwitch);
private:
	HMODULE m_hIKCardID;
	BOOL m_bLoadSuccess;
	BOOL m_bInitSuccess;
	static std::map<int, std::string> m_mapI2S;
	void GetErrorString(int nError, std::string& strError);
	void FmtErrorString();
	bool IsErrorCodeExist(int nError);
	void DealErrorMsg(int nError, char *szError);
	static std::string Encode(LPCSTR szPWD);
	
};
