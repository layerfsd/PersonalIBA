// CallIBARegUser.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "CallIBARegUser.h"
#include "MD5.H"
#include <atlbase.h>
#include "HelpFuc.h"
#include "RealnameDllDef.h"

#define CALLIBAREGUSER_CFN_API extern "C" __declspec(dllexport)
// The following symbol used to force inclusion of this module for _USRDLL
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// 这是导出函数的一个示例。
extern "C" __declspec(dllexport) int __cdecl fnCallIBARegUser(PRegUserData pRegUserData)
{
	const UINT WM_REALNAME  = WM_USER + 100;
	
	// 返回0开户成功
	// 返回1收银台未启动
	// 返回2注册表操作出错
	// 返回3传递的数据结构大小有误
	// 返回4传递指针为空

	if(!pRegUserData)
		return 4;
	if(pRegUserData->cbCount != sizeof(RegUserData))
		return 3;

	HWND hIBA = ::FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);
	if(hIBA != NULL)
	{

		CString strID = CDllHelper::AC2TC(pRegUserData->szCertificateID);
		// 2014-8-14 - qsc 补0
		{
			if (strID.GetLength() < 10)
			{
				strID.Insert(0, CString(_T("0000000000")).Left(10 - strID.GetLength()));
			}
		}
		CString strCID = strID;
		int nCIDTpe = atoi(pRegUserData->szCertificateType);
		CString strName = CDllHelper::AC2TC(pRegUserData->szName);
		CString strPWD = CDllHelper::AC2TC(pRegUserData->szPassword);
		CString strCountry = CDllHelper::AC2TC(pRegUserData->szCountry);
		CString strAddress = CDllHelper::AC2TC(pRegUserData->szAddr);

		CString strSign, strTime, strKey("jqdodonew1035");
		strKey.Format(_T("%s"), strKey);
		strTime.Format(_T("%ld"), time(NULL));

		// 因为上网账号是数字和英文，所以编码可以不用考虑
		strSign.Format(_T("%s%s%s"), strID, strTime, strKey);
		// 账号是数字和字母组合，不考虑编码问题
		CString strSignMd5 = CMD5::MD5String(strSign);

		CRegKey RegKey;
		if(ERROR_SUCCESS == RegKey.Create(HKEY_CURRENT_USER, _T("Software\\sunlike\\IBA\\RealName\\JQ\\OpenUser")))
		{
			RegKey.SetStringValue(_T("IDNo"), strID);
			RegKey.SetStringValue( _T("CertificateID"), strCID);
			RegKey.SetStringValue( _T("Name"), strName);
			RegKey.SetStringValue(_T("Country"), strCountry);
			RegKey.SetStringValue( _T("Address"), strAddress);	
			RegKey.SetStringValue( _T("Password"), strPWD);
			RegKey.SetStringValue( _T("Other"), _T(""));
			RegKey.SetStringValue( _T("vValue1"), strTime);
			RegKey.SetStringValue( _T("vValue2"), strSignMd5);
			RegKey.SetDWORDValue( _T("CertificateType"), nCIDTpe);
			RegKey.SetDWORDValue( _T("Sex"), pRegUserData->Sex);
		}
		else
		{
			return 2;
		}
		RegKey.Close();

		int nRet = (int)::SendMessage(hIBA, WM_REALNAME, 0, 0);
		return (nRet == 1);
	}
	return 1;
}