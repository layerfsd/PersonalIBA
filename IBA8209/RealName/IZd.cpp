
#include "stdafx.h"
#include "IZd.h"
#include "..\IBA.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"
#include <atlenc.h>

std::map<int, std::string> CIZd::m_mapI2S;
SINGLETON_IMPLEMENT(CIZd)

CIZd::CIZd()
: m_pfn_V2_OpenUserEx(NULL),
m_hIKCardID(NULL), 
m_pfn_V2_Login(NULL),
m_pfn_V2_LoginEx(NULL),
m_pfn_V2_Logout(NULL),
m_pfn_V2_AddInsider(NULL),
m_pfn_V2_LockMac(NULL),
m_pfn_V2_SendICCardData(NULL),
m_pfn_V2_SendDeductData(NULL),
m_pfn_V2_VerifyCard(NULL),
m_pfn_V2_GetCardPWD(NULL),
m_pfn_V2_GetCardPWDEx(NULL),
m_pfn_V2_GetCustomer(NULL),
m_pfn_V2_GetMobileVerificationCode(NULL),
m_pfn_V2_GetMobileVerificationCodeReturn(NULL),
m_pfn_V2_WXAccounts(NULL),
m_pfn_V2_KEYBalance(NULL),
m_pfn_V2_UseMacVerfy(NULL),
m_pfn_V2_SetCRC(NULL),
m_bLoadSuccess(FALSE),
m_bInitSuccess(FALSE)
{
	IBA_LOG0(_T("CIZd 构造 2"));
	// 2014-10-27 - qsc
	FmtErrorString();
	Load();
}

CIZd::~CIZd()
{
	IBA_LOG0(_T("CIZd 析构"));
	if(m_hIKCardID != NULL)
	{
		::FreeLibrary(m_hIKCardID);
		m_hIKCardID = NULL;
	}
}

BOOL CIZd::Load()
{
	IBA_LOG0(_T("Enter CIZd Loads"));

	CString tmpAppPath = theApp.GetWorkPath() + _T("\\IBABin\\");
	CString tmpLoadDllPath = tmpAppPath + _T("IKCardID.dll");
	m_hIKCardID = ::LoadLibrary(tmpLoadDllPath);

	BOOL bSuccess = FALSE;
	if (m_hIKCardID == NULL)
	{
		IBA_LOG0(_T("CIZd Loads Load IKCardID.dll 失败"));
		bSuccess = FALSE;
	}
	else
	{
		m_pfn_V2_OpenUserEx			= (FUN_V2_OpenUserEx)::GetProcAddress(m_hIKCardID, "V2_OpenUserEx");
		m_pfn_V2_Login				= (FUN_V2_Login)::GetProcAddress(m_hIKCardID, "V2_Login");
		m_pfn_V2_LoginEx			= (FUN_V2_LoginEx)::GetProcAddress(m_hIKCardID, "V2_LoginEx");
		m_pfn_V2_Logout				= (FUN_V2_Logout)::GetProcAddress(m_hIKCardID, "V2_Logout");
		m_pfn_V2_AddInsider			= (FUN_V2_AddInsider)::GetProcAddress(m_hIKCardID, "V2_AddInsider");
		m_pfn_V2_LockMac			= (FUN_V2_LockMac)::GetProcAddress(m_hIKCardID, "V2_LockMac");
		m_pfn_V2_SendICCardData		= (FUN_V2_SendICCardData)::GetProcAddress(m_hIKCardID, "V2_SendICCardData");
		m_pfn_V2_SendDeductData		= (FUN_V2_SendDeductData)::GetProcAddress(m_hIKCardID, "V2_SendDeductData");
		m_pfn_V2_VerifyCard			= (FUN_V2_VerifyCard)::GetProcAddress(m_hIKCardID, "V2_VerifyCard");
		m_pfn_V2_GetCardPWD			= (FUN_V2_GetCardPWD)::GetProcAddress(m_hIKCardID, "V2_GetCardPWD");
		m_pfn_V2_GetCardPWDEx		= (FUN_V2_GetCardPWDEx)::GetProcAddress(m_hIKCardID, "V2_GetCardPWDEx");
		m_pfn_V2_GetCustomer		= (FUN_V2_GetCustomer)::GetProcAddress(m_hIKCardID, "V2_GetCustomer");

		m_pfn_V2_GetMobileVerificationCode			= (FUN_V2_GetMobileVerificationCode)::GetProcAddress(m_hIKCardID, "V2_GetMobileVerificationCode");
		m_pfn_V2_GetMobileVerificationCodeReturn	= (FUN_V2_GetMobileVerificationCodeReturn)::GetProcAddress(m_hIKCardID, "V2_GetMobileVerificationCodeReturn");
		m_pfn_V2_WXAccounts			= (FUN_V2_WXAccounts)::GetProcAddress(m_hIKCardID, "V2_WXAccounts");
		m_pfn_V2_KEYBalance			= (FUN_V2_KEYBalance)::GetProcAddress(m_hIKCardID, "V2_KEYBalance");
		m_pfn_V2_UseMacVerfy		= (FUN_V2_UseMacVerfy)::GetProcAddress(m_hIKCardID, "V2_UseMacVerfy");
		m_pfn_V2_CloseUser			= (FUN_V2_CloseUser)::GetProcAddress(m_hIKCardID, "V2_CloseUser");
		m_pfn_V2_SetCRC				= (FUN_V2_SetCRC)::GetProcAddress(m_hIKCardID, "V2_SetCRC");

		// QSCTAG
		/*	if(!m_pfn_V2_SendICCardData)
		{
			IBA_LOG0(_T("m_pfn_V2_SendICCardData为空"));
		}
		if(!m_pfn_V2_UseMacVerfy)
		{
			IBA_LOG0(_T("m_pfn_V2_UseMacVerfy为空"));
		}*/
		
		if(!m_pfn_V2_OpenUserEx)
		{
			IBA_LOG0(_T("m_pfn_V2_OpenUserEx读取失败"));
		}
		if(!m_pfn_V2_Login)
		{
			IBA_LOG0(_T("m_pfn_V2_Login读取失败"));
		}
		if(!m_pfn_V2_LoginEx)
		{
			IBA_LOG0(_T("m_pfn_V2_LoginEx读取失败"));
		}
		if(!m_pfn_V2_Logout)
		{
			IBA_LOG0(_T("m_pfn_V2_Logout读取失败"));
		}
		if(!m_pfn_V2_AddInsider)
		{
			IBA_LOG0(_T("m_pfn_V2_AddInsider读取失败"));
		}
		if(!m_pfn_V2_LockMac)
		{
			IBA_LOG0(_T("m_pfn_V2_LockMac读取失败"));
		}
		if(!m_pfn_V2_SendICCardData)
		{
			IBA_LOG0(_T("m_pfn_V2_SendICCardData读取失败"));
		}
		if(!m_pfn_V2_UseMacVerfy)
		{
			IBA_LOG0(_T("m_pfn_V2_UseMacVerfy读取失败"));
		}
		if(!m_pfn_V2_SendDeductData)
		{
			IBA_LOG0(_T("m_pfn_V2_SendDeductData读取失败"));
		}
		if(!m_pfn_V2_VerifyCard)
		{
			IBA_LOG0(_T("m_pfn_V2_VerifyCard读取失败"));
		}
		if(!m_pfn_V2_GetCardPWD)
		{
			IBA_LOG0(_T("m_pfn_V2_GetCardPWD读取失败"));
		}
		if(!m_pfn_V2_GetCardPWDEx)
		{
			IBA_LOG0(_T("m_pfn_V2_GetCardPWDEx读取失败"));
		}
		if(!m_pfn_V2_GetCustomer)
		{
			IBA_LOG0(_T("m_pfn_V2_GetCustomer读取失败"));
		}
		if(!m_pfn_V2_GetMobileVerificationCode)
		{
			IBA_LOG0(_T("m_pfn_V2_GetMobileVerificationCode读取失败"));
		}
		if(!m_pfn_V2_GetMobileVerificationCodeReturn)
		{
			IBA_LOG0(_T("m_pfn_V2_GetMobileVerificationCodeReturn读取失败"));
		}
		if(!m_pfn_V2_WXAccounts)
		{
			IBA_LOG0(_T("m_pfn_V2_WXAccounts读取失败"));
		}
		if(!m_pfn_V2_KEYBalance)
		{
			IBA_LOG0(_T("m_pfn_V2_KEYBalance读取失败"));
		}
		if(!m_pfn_V2_CloseUser)
		{
			IBA_LOG0(_T("m_pfn_V2_CloseUser读取失败"));
		}
		if(!m_pfn_V2_SetCRC)
		{
			IBA_LOG0(_T("m_pfn_V2_SetCRC读取失败"));
		}

		if(m_pfn_V2_OpenUserEx			!= NULL &&
			m_pfn_V2_Login				!= NULL &&
			m_pfn_V2_LoginEx			!= NULL &&
			m_pfn_V2_Logout				!= NULL &&
			m_pfn_V2_AddInsider			!= NULL &&
			m_pfn_V2_LockMac			!= NULL &&
			//m_pfn_V2_SendICCardData		!= NULL &&
			//m_pfn_V2_UseMacVerfy		!= NULL &&
			m_pfn_V2_SendDeductData		!= NULL &&
			m_pfn_V2_VerifyCard			!= NULL &&
			m_pfn_V2_GetCardPWD			!= NULL &&
			m_pfn_V2_GetCardPWDEx		!= NULL &&
			m_pfn_V2_GetCustomer		!= NULL &&
			m_pfn_V2_GetMobileVerificationCode			!= NULL &&
			m_pfn_V2_GetMobileVerificationCodeReturn	!= NULL &&
			m_pfn_V2_WXAccounts			!= NULL &&
			m_pfn_V2_KEYBalance			!= NULL && 
			m_pfn_V2_CloseUser			!= NULL &&
			m_pfn_V2_SetCRC				!= NULL)
		{
			IBA_LOG0(_T("加载成功"));
			m_bLoadSuccess = TRUE;
		}
		else
		{
			IBA_LOG0(_T("有函数读取失败"));
			m_bLoadSuccess = FALSE;
		}
	}
	IBA_LOG0(_T("Leave CIZd Loads"));
	return m_bLoadSuccess;
}

void CIZd::Conver_IC_USER( const IC_USER* pOrg, T_IC_USER *pDest )
{
	ZeroMemory(pDest, sizeof(IC_USER));
	_tcsncpy(pDest->szikIDNo, CA2T(pOrg->szikIDNo), 21);
	_tcsncpy(pDest->szikName, CA2T(pOrg->szikName), 31);
	_tcsncpy(pDest->szCertNumber, CA2T(pOrg->szCertNumber), 19);
	_tcsncpy(pDest->szMobile, CA2T(pOrg->szMobile), 12);

	pDest->iCertType = pOrg->iCertType;
	pDest->iCardType = pOrg->iCardType;
	pDest->iValidate = pOrg->iValidate;
}

void CIZd::Conver_IC_LOGIN( IC_LOGIN* pOrg, T_IC_LOGIN *pDest )
{
	ZeroMemory(pDest, sizeof(IC_USER));
	_tcsncpy(pDest->szikICNo, CA2T(pOrg->szikICNo), 21);
	_tcsncpy(pDest->szikIDNo, CA2T(pOrg->szikIDNo), 21);
	_tcsncpy(pDest->szikIP, CA2T(pOrg->szikIP), 16);
	_tcsncpy(pDest->szikHost, CA2T(pOrg->szikHost), 31);
	_tcsncpy(pDest->szicPassword, CA2T(pOrg->szicPassword), 16);

}

CIZd::ID_LOGINRETURN CIZd::V2_LoginEx( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd, LPCSTR szIP)
{
	IBA_LOG0(_T("V2_LoginEx Enter"));
	ID_LOGINRETURN loginReturn;
	ZeroMemory(&loginReturn, sizeof(ID_LOGINRETURN));

	if(IsInit())
	{
		char szICNOTmp[21] = {0};
		char szIDNOTmp[21] = {0};
		char szPwdTmp[21] = {0};
		char szIPTmp[30] = {0};
		strncpy(szICNOTmp, szICNO, 21);
		strncpy(szIDNOTmp, szIDNO, 21);
		strncpy(szPwdTmp, szPwd, 21);
		strncpy(szIPTmp, szIP, 30);

		CStringA strLog;
		strLog.Format("传递给智多的信息%s,%s,PWD:%s,%s", szICNOTmp, szIDNOTmp, szPwdTmp, szIPTmp);
		IBA_LOG0(CA2T(strLog));

		loginReturn =  m_pfn_V2_LoginEx(szICNOTmp, szIDNOTmp, szPwdTmp, szIPTmp);
		DealErrorMsg(loginReturn.iErrCode, loginReturn.szErrMsg);

		strLog.Format("上网卡号:%s, 上网卡获取类型：%d, 姓名%s, 证件类型：%d, 证件号码：%s, 错误号：%d, 错误描述:%s", 
			loginReturn.szCardId, loginReturn.iCardType, loginReturn.szName, loginReturn.iCertType, loginReturn.szCertNumber, 
			loginReturn.iErrCode, loginReturn.szErrMsg);
		IBA_LOG0(CA2T(strLog));
#ifdef _DEBUG
		loginReturn.iErrCode = 0;
#endif
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		loginReturn.iErrCode = -1;
		strcpy(loginReturn.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_LoginEx Leave"));
	return loginReturn;
}

CIZd::ID_RETURN CIZd::V2_Logout( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szIP)
{
	IBA_LOG0(_T("V2_Logout Enter"));
	ID_RETURN idReturn;
	ZeroMemory(&idReturn, sizeof(idReturn));

	if(IsInit())
	{
		char szICNOTmp[21] = {0};
		char szIDNOTmp[21] = {0};
		char szIPTmp[30] = {0};
		strncpy(szICNOTmp, szICNO, 21);
		strncpy(szIDNOTmp, szIDNO, 21);
		strncpy(szIPTmp, szIP, 30);

		CStringA strLog;
		strLog.Format("传递给智多的信息%s,%s,%s", szICNO, szICNO, szIP);
		IBA_LOG0(CA2T(strLog));

		idReturn =  m_pfn_V2_Logout(szICNOTmp, szIDNOTmp, szIPTmp);
		DealErrorMsg(idReturn.iErrCode, idReturn.szErrMsg);

		strLog.Format("错误:%d, 错误描述：%s", 
			idReturn.iErrCode, idReturn.szErrMsg);
		IBA_LOG0(CA2T(strLog));
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		idReturn.iErrCode = -1;
		strcpy(idReturn.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_Logout Leave"));
	return idReturn;
}

CIZd::ID_GETCARDPWD CIZd::V2_GetCardPWDEx(LPCSTR szIDNO)
{
	IBA_LOG0(_T("V2_GetCardPWDEx Enter"));
	ID_GETCARDPWD cardPwd;
	ZeroMemory(&cardPwd, sizeof(cardPwd));

	if(IsInit())
	{
		char szIDNOTmp[21] = {0};
		strncpy(szIDNOTmp, szIDNO, 21);

		CStringA strLog;
		strLog.Format("传递给智多的信息%s", szIDNOTmp);
		IBA_LOG0(CA2T(strLog));

		cardPwd =  m_pfn_V2_GetCardPWDEx(szIDNOTmp);
		DealErrorMsg(cardPwd.iErrCode, cardPwd.szErrMsg);

		strLog.Format("错误:%d, 错误描述：%s", 
			cardPwd.iErrCode, cardPwd.szErrMsg);
		IBA_LOG0(CA2T(strLog));
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		cardPwd.iErrCode = -1;
		strcpy(cardPwd.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_GetCardPWDEx Leave"));
	return cardPwd;
}

CIZd::ID_RETURN CIZd::V2_CloseUser( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szIP, LPCSTR szTermId)
{
	IBA_LOG0(_T("V2_CloseUser Enter"));
	ID_RETURN idReturn;
	ZeroMemory(&idReturn, sizeof(idReturn));

	if(IsInit())
	{
		char szICNOTmp[21] = {0};
		char szIDNOTmp[21] = {0};
		char szIPTmp[30] = {0};
		char szTermId[20] = {0};
		char szCurTime[15] = {0}; 

		CTime tm  = CTime::GetCurrentTime();
		CStringA strCurTime;
		strCurTime.Format("%04d%02d%02d%02d%02d%02d", 
			tm.GetYear(), tm.GetMonth(), tm.GetDay(),
			tm.GetHour(), tm.GetMinute(), tm.GetSecond());

		strncpy(szICNOTmp, szICNO, 21);
		strncpy(szIDNOTmp, szIDNO, 21);
		strncpy(szIPTmp, szIP, 30);
		strncpy(szTermId, szTermId, 20);
		strncpy(szCurTime, strCurTime, 15);
		
		CStringA strLog;
		strLog.Format("传递给智多的信息%s,%s,%s,%s", szICNO, szICNO, szIP, szTermId);
		IBA_LOG0(CA2T(strLog));

		idReturn =  m_pfn_V2_CloseUser(szICNOTmp, szIDNOTmp, szIPTmp, szTermId, szCurTime);
		DealErrorMsg(idReturn.iErrCode, idReturn.szErrMsg);

		strLog.Format("错误:%d, 错误描述：%s",
			idReturn.iErrCode, idReturn.szErrMsg);
		IBA_LOG0(CA2T(strLog));
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		idReturn.iErrCode = -1;
		strcpy(idReturn.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_CloseUser Leave"));
	return idReturn;
}

CIZd::ID_LOGINRETURN CIZd::V2_OpenUserEx( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd, LPCSTR szIP, LPCSTR szDeductDotSign)
{
	IBA_LOG0(_T("V2_OpenUserEx Enter"));
	ID_LOGINRETURN loginReturn;
	ZeroMemory(&loginReturn, sizeof(ID_LOGINRETURN));

	if(IsInit())
	{
		char szICNOTmp[21] = {0};
		char szIDNOTmp[21] = {0};
		char szPwdTmp[21] = {0};
		char szIPTmp[30] = {0};
		char szCurTime[15] = {0};
		char szDeductDotSignTmp[20] = {0};
		char szCashNum[20] = {0};

		CTime tm  = CTime::GetCurrentTime();
		CStringA strCurTime;
		strCurTime.Format("%04d%02d%02d%02d%02d%02d", 
			tm.GetYear(), tm.GetMonth(), tm.GetDay(),
			tm.GetHour(), tm.GetMinute(), tm.GetSecond());

		CStringA strCashNum;
		strCashNum.Format("%d", CIBAConfig::GetInstance()->GetCashRegisterID());

		strncpy(szICNOTmp, szICNO, 21);
		strncpy(szIDNOTmp, szIDNO, 21);
		strncpy(szIPTmp, szIP, 30);
		strncpy(szPwdTmp, szPwd, 21);
		strncpy(szCurTime, strCurTime, 15);
		strncpy(szDeductDotSignTmp, szDeductDotSign, 20);
		strncpy(szCashNum, strCashNum, 20);

		CStringA strLog;
		strLog.Format("传递给智多的信息%s,%s,PWD:%s,%s,%s,%s,%s", szICNOTmp, szIDNOTmp, szPwdTmp, szIPTmp, szDeductDotSignTmp, szCurTime, szCashNum);
		IBA_LOG0(CA2T(strLog));

		loginReturn =  m_pfn_V2_OpenUserEx(szICNOTmp, szIDNOTmp, szPwdTmp, szIPTmp, szDeductDotSignTmp, szCurTime, szCashNum);
		DealErrorMsg(loginReturn.iErrCode, loginReturn.szErrMsg);

		strLog.Format("上网卡号:%s, 上网卡获取类型：%d, 姓名%s, 证件类型：%d, 证件号码：%s, 错误号：%d, 错误描述:%s", 
			loginReturn.szCardId, loginReturn.iCardType, loginReturn.szName, loginReturn.iCertType, loginReturn.szCertNumber, 
			loginReturn.iErrCode, loginReturn.szErrMsg);
		IBA_LOG0(CA2T(strLog));

#ifdef _DEBUG
		loginReturn.iErrCode = 0;
#endif
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		loginReturn.iErrCode = -1;
		strcpy(loginReturn.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_OpenUserEx Leave"));
	return loginReturn;
}

CIZd::ID_ADDINSIDR CIZd::V2_AddInsider( LPCSTR szICNO, LPCSTR szIDNO, LPCSTR szPwd)
{
	IBA_LOG0(_T("V2_AddInsider Enter"));
	ID_ADDINSIDR addInsidr;
	ZeroMemory(&addInsidr, sizeof(addInsidr));

	TCHAR szikICNo[21];			//绑定卡编号
	TCHAR szikIDNo[21];			//实名卡编号
	TCHAR szikName[31]; 		//姓名
	int   iIDCardType;			//证件类型
	TCHAR szCertificateID[19];	//证件号码
	int   iErrCode;				//错误号，详见错误定义表；0表示成功
	TCHAR szErrMsg[101];		//错误描述，如果Error不为0的话
	int   iCardGetType;			//刷证类型
	int   iIsCheck;				//是否审核 0表示没有1个表示未审核2表示通过

	if(IsInit())
	{
		char szICNOTmp[21] = {0};
		char szIDNOTmp[21] = {0};
		char szPwdTmp[21] = {0};
		strncpy(szICNOTmp, szICNO, 21);
		strncpy(szIDNOTmp, szIDNO, 21);
		strncpy(szPwdTmp, szPwd, 21);

		CStringA strLog;
		strLog.Format("传递给智多的信息%s,%s", szICNO, szICNO);
		IBA_LOG0(CA2T(strLog));

		addInsidr =  m_pfn_V2_AddInsider(szICNOTmp, szIDNOTmp, szPwdTmp);
		DealErrorMsg(addInsidr.iErrCode, addInsidr.szErrMsg);

		strLog.Format("绑定卡编号:%s, 实名卡编号：%s, 姓名%s, 证件类型：%d, 证件号码：%s, 错误号：%d, 错误描述:%s, 刷证类型:%d, 是否审核:%d", 
			addInsidr.szikICNo, addInsidr.szikIDNo, addInsidr.szikName, addInsidr.iIDCardType, addInsidr.szCertificateID, 
			addInsidr.iErrCode, addInsidr.szErrMsg, addInsidr.iCardGetType, addInsidr.iIsCheck);
		IBA_LOG0(CA2T(strLog));
	}
	else
	{
		IBA_LOG(_T("未初始化"));
		addInsidr.iErrCode = -1;
		strcpy(addInsidr.szErrMsg, "智多实名未初始化");
	}
	IBA_LOG0(_T("V2_AddInsider Leave"));
	return addInsidr;
}

bool CIZd::IsIDPwdCorrect(LPCSTR szID, LPCSTR szPwd)
{
	CIZd::ID_GETCARDPWD cardPwd = CIZd::GetInstance()->V2_GetCardPWDEx(szID);

	std::string strEncode = Encode(szPwd);
	CStringA strLog;
	strLog.Format("输入账号:%s,密码：%s, 输出：%s, ZD：%s",
		szID, szPwd, strEncode.c_str(), cardPwd.szPWD);
	IBA_LOG0(CA2T(strLog));
	
	if(strEncode == cardPwd.szPWD)
	{
		IBA_LOG0(_T("正确"));
		return true;
	}
	else
	{
		IBA_LOG0(_T("错误"));
		return false;
	}
}

bool CIZd::IsErrorCodeExist(int nError)
{
	if(m_mapI2S.count(nError) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CIZd::DealErrorMsg(int nError, char *szError)
{
	if(IsErrorCodeExist(nError))
	{
		std::string strError;
		GetErrorString(nError, strError);
		strncpy(szError, strError.c_str(), 100);
	}
}

void CIZd::GetErrorString(int nError, std::string& strError)
{
	if(IsErrorCodeExist(nError))
	{
		strError = m_mapI2S[nError];
	}
	else
	{
		strError = ""; 
	}

}

void CIZd::FmtErrorString()
{
	m_mapI2S[-1]	 = "未知错误";
	m_mapI2S[-3]	 = "服务器接口不存在或工作不正常";
	m_mapI2S[-4]	 = "服务器接口已经初始化";
	m_mapI2S[-5]	 = "服务器接口已经终止";
	m_mapI2S[-9]	 = "密码或序列号不正确";
	m_mapI2S[-10]	 = "新密码不规范";
	m_mapI2S[-11]	 = "无效的名称";
	m_mapI2S[-12]	 = "版本不正确";
	m_mapI2S[-13]	 = "无效令牌";
	m_mapI2S[-14]	 = "会话令牌失效";
	m_mapI2S[-15]	 = "上网场所登录失败，需要重新注册";
	m_mapI2S[-17]	 = "该卡号无效或尚未注册";
	m_mapI2S[-18]	 = "该卡号已注册";
	m_mapI2S[-19]	 = "该卡号已停用";
	m_mapI2S[-20]	 = "该卡号已注销";
	m_mapI2S[-21]	 = "该卡不能在该地区使用";
	m_mapI2S[-22]	 = "上网场所没有权限";
	m_mapI2S[-25]	 = "无效证件";
	m_mapI2S[-26]	 = "该证件已注册";
	m_mapI2S[-28]	 = "该顾客不具有上网资格";
	m_mapI2S[-29]	 = "场所未注册";
	m_mapI2S[-30]	 = "非法的场所编码";
	m_mapI2S[-31]	 = "场所未营业";
	m_mapI2S[-32]	 = "MAC错误";
	m_mapI2S[-33]	 = "狗号错误";
	m_mapI2S[-34]	 = "场所软件版本不正确";
	m_mapI2S[-35]	 = "场所需要重新绑定";
	m_mapI2S[-36]	 = "场所软件版本过期";
	m_mapI2S[-1000]	 = "KEY中的余额不足";
	m_mapI2S[-1001]	 = "编号不匹配";
	m_mapI2S[-1002]	 = "无法创建加密对象";
	m_mapI2S[-1003]	 = "解密失败";
	m_mapI2S[-1004]	 = "充值序列号重复";
	m_mapI2S[-1005]	 = "KEY状态异常";
	m_mapI2S[-1006]	 = "打开加密容器失败";
	m_mapI2S[-1007]	 = "建立加密key失败";
	m_mapI2S[-1008]	 = "加密失败";
	m_mapI2S[-1009]	 = "未插入KEY或者KEY硬件错误";
	m_mapI2S[-1010]	 = "数据异常";
	m_mapI2S[-1011]	 = "计费软件版本号不符合要求";
	m_mapI2S[-1012]	 = "上网卡在规定时间内未登录";
	m_mapI2S[-1013]	 = "终端机未在线";
	m_mapI2S[-1014]	 = "顾客数已超过终端机授权数量";
	m_mapI2S[-1015]	 = "终端机安全版本号不符合要求";
	m_mapI2S[-1016]	 = "未查找到上网卡密码";
}


int CIZd::SwitchZd2Ddn(int nSwitch)
{
	int nRet = 99;
	switch(nSwitch)
	{
	case 92:
	case 111:
		nRet = 12;
		break;

	case 112:
	case 98:
	case 91:
		nRet = 11;
		break;
	
	case 113:
		nRet = 93;
		break;

	case 114:
		nRet = 90;
		break;

	case 123:
		nRet = 91;
		break;

	case 133:
		nRet = 15;
		break;
	
	case 96:
	case 414:
		nRet = 94;
		break;

	case 94:
		nRet = 20;
		break;

	default:
		nRet = 99;
	}
	return nRet;
	//DDN:11:身份证|12:二代证|15:学生证|20:驾驶证|21:广东居住证|
	//22:深圳居住证|90:军官证|91:警官证|92:士兵证|93:户口薄|94:护照|95:台胞证|96:回乡证|99:其他证件|
	//ZD 111,'身份证'             
	//112,'临时身份证'          
	//113,'户口簿'              
	//114,'军官证'              
	//123,'警官证'              
	//133,'学生证'              
	//414,'护照'                
	//990,'其他证件'

	//手工录入：
	//91,"一代身份证"
	//92,"二代身份证"
	//93,"军人证"
	//94,"驾驶证"
	//95,"中华人民共和国社会保障卡"
	//96,"中国护照"
	//97,"国外及港澳台证件"
	//98,"临时身份证"
}


struct MD5_CTX
{
	ULONG num[2];			/* number of _bits_ handled mod 2^64 */
	ULONG buf[4];			/* scratch buffer */
	BYTE  input[64];		/* input buffer */
	BYTE  digest[16];		/* actual digest after MD5Final call */
};

void WINAPI MD5Init(MD5_CTX* lpCtx);
void WINAPI MD5Update(MD5_CTX* lpCtx, const void * lpBuf, unsigned int nLen);
void WINAPI MD5Final(MD5_CTX* lpCtx);
// 先MD5，再BASE64加密
std::string CIZd::Encode(LPCSTR szPWD)
{
	MD5_CTX ctx = {0};

	CStringA strTmp = szPWD;

	MD5Init(&ctx);

	MD5Update(&ctx, strTmp.GetBuffer(), strTmp.GetLength());

	MD5Final(&ctx);

	int iSize = 0;
	iSize = 16; //MD5固定长度16位
	int iLen = Base64EncodeGetRequiredLength(iSize, ATL_BASE64_FLAG_NOCRLF);
	LPSTR pData = new char[iLen+1];
	memset(pData, 0, iLen);
	Base64Encode((const BYTE *)ctx.digest, iSize, pData, &iLen, ATL_BASE64_FLAG_NOCRLF);
	
	char szTmp[64] = {0};
	strncpy(szTmp, pData, iLen);
	
	std::string stdRet = szTmp;
	return stdRet;
}
