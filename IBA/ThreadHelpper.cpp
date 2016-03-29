#include "StdAfx.h"
#include "ThreadHelpper.h"
#include "NetBarConfig.h"
#include ".\Servlet\GetSystemConfigMethod.h"
#include "Servlet\CommonParam.h"
#include "IBALog.h"

using namespace NS_SERVLET;

void CThreadHelpper::StartThead(IN LPTHREAD_START_ROUTINE lpStartAddress)
{
	HANDLE hThread = CreateThread(NULL, 0, lpStartAddress, NULL, 0, 0);
	//SetThreadPriority(hThread,THREAD_PRIORITY_LOWEST);
	CloseHandle(hThread);
}

DWORD CThreadHelpper::GetSysConfig(LPVOID pParam)
{
	// 查询是否启用微信支付
	CCommonParam commmonParam;	
	commmonParam.SetParam(_T("wxpay"));
	commmonParam.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	commmonParam.Execute();
	if(0 == commmonParam.GetStatusCode())
	{	
		CString strRequset = commmonParam.GetCommonRequest();
		UINT nRequest = _ttoi(strRequset);
		IBA_LOG(_T("是否开通微信支付:%d"), nRequest);

		CNetBarConfig::GetInstance()->SetOpenWx(nRequest);
	}

	// 查询是否启用电话验证
	commmonParam.SetParam(_T("telverify"));
	commmonParam.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	commmonParam.Execute();
	if(0 == commmonParam.GetStatusCode())
	{	
		CString strRequset = commmonParam.GetCommonRequest();
		UINT nRequest = _ttoi(strRequset);
		IBA_LOG(_T("中心电话验证设置:%d"), nRequest);
		CNetBarConfig::GetInstance()->SetTelVerify(nRequest);
	}

	// 查询fileList地址并存储到数据库表
	CGetSystemConfigMethod getSystemConfigMethod;
	getSystemConfigMethod.SetStringField(_T("versionManageHost"));
	getSystemConfigMethod.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	getSystemConfigMethod.Execute();

	if(0 == getSystemConfigMethod.GetStatusCode())
	{
		CString strFieldValue = getSystemConfigMethod.GetStringFieldValue();
		UINT Value1 = getSystemConfigMethod.GetValue1();
		UINT Value2 = getSystemConfigMethod.GetValue2();
		CNetBarConfig::GetInstance()->SetSystemConfig(_T("versionManageHost"), 
			strFieldValue, Value1, Value2);
	}

	getSystemConfigMethod.SetStringField(_T("noticeServerAddress"));
	getSystemConfigMethod.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	getSystemConfigMethod.Execute();

	if(0 == getSystemConfigMethod.GetStatusCode())
	{
		CString strFieldValue = getSystemConfigMethod.GetStringFieldValue();
		UINT Value1 = getSystemConfigMethod.GetValue1();
		UINT Value2 = getSystemConfigMethod.GetValue2();
		CNetBarConfig::GetInstance()->SetSystemConfig(_T("noticeServerAddress"), 
			strFieldValue, Value1, Value2);
	}

	getSystemConfigMethod.SetStringField(_T("LoginQrCode"));
	getSystemConfigMethod.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	getSystemConfigMethod.Execute();

	if(0 == getSystemConfigMethod.GetStatusCode())
	{
		CString strFieldValue = getSystemConfigMethod.GetStringFieldValue();
		UINT Value1 = getSystemConfigMethod.GetValue1();
		UINT Value2 = getSystemConfigMethod.GetValue2();
		CNetBarConfig::GetInstance()->SetSystemConfig(_T("LoginQrCode"), 
			strFieldValue, Value1, Value2);
	}

	getSystemConfigMethod.SetStringField(_T("LogoutQrCode"));
	getSystemConfigMethod.SetNetbarId(CNetBarConfig::GetInstance()->GetNetBarId());
	getSystemConfigMethod.Execute();

	if(0 == getSystemConfigMethod.GetStatusCode())
	{
		CString strFieldValue = getSystemConfigMethod.GetStringFieldValue();
		UINT Value1 = getSystemConfigMethod.GetValue1();
		UINT Value2 = getSystemConfigMethod.GetValue2();
		CNetBarConfig::GetInstance()->SetSystemConfig(_T("LogoutQrCode"), 
			strFieldValue, Value1, Value2);
	}
	return 0;
}
