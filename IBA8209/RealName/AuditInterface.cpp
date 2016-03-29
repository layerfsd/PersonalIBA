#include "stdafx.h"
#include "AuditInterface.h"
#include "..\IBA.h"

BOOL CAuditInterface::Load()
{
	CString strPath(theApp.GetWorkPath());

	strPath.Append(_T("\\IBABin\\AuditInterface.dll"));

	hdll = ::LoadLibrary(strPath);
	if (hdll)
	{
		init_ = (pfnInit)::GetProcAddress(hdll, "Init");
		uninit_ = (pfnUnInit)::GetProcAddress(hdll, "UnInit");
		userLogon_ = (pfnUserLogon)::GetProcAddress(hdll, "UserLogon");
		logout_ = (pfnUserLogout)::GetProcAddress(hdll, "UserLogout");
		regUser_ = (pfnRegUser)::GetProcAddress(hdll, "RegUser");
		sendRegImage_ = (pfnSendRegImage)::GetProcAddress(hdll, "SendRegImage");
		sendIDImage_ = (pfnSendIDImage)::GetProcAddress(hdll, "SendIDImage");
		IsRegisted_ = (pfnIsRegisted)::GetProcAddress(hdll, "IsRegisted");
		ServerUserLogon_ = (pfnServerUserLogon)::GetProcAddress(hdll, "ServerUserLogon");
		Ping_ = (pfnPing)::GetProcAddress(hdll, "Ping");
		sendRegImageEx_ = (pfnSendRegImageEx)::GetProcAddress(hdll, "SendRegImageEx");
		sendIDImageEx_ = (pfnSendIDImageEx)::GetProcAddress(hdll, "SendIDImageEx");
		GetImageRemote_ = (pfnGetImageRemote)::GetProcAddress( hdll, "GetImageRemote" );
		//SendIDImageRZX2008_ = (pfnSendIDImageRZX2008)::GetProcAddress( hdll, "SendIDImageRZX2008" );
		//SendRegImageRZX2008_ = (pfnSendRegImageRZX2008)::GetProcAddress( hdll, "SendRegImageRZX2008" );
		UserLogoutRZX2008_ = (pfnUserLogoutRZX2008)::GetProcAddress( hdll, "UserLogoutRZX2008" );
		UserLogonRZX2008_ = (pfnUserLogonRZX2008)::GetProcAddress( hdll, "UserLogonRZX2008" );

		if (init_ &&
			uninit_ &&
			userLogon_ &&
			logout_ &&
			regUser_ &&
			sendRegImage_ &&
			sendIDImage_ &&
			IsRegisted_ &&
			ServerUserLogon_ &&
			Ping_ &&
			sendRegImageEx_ &&
			sendIDImageEx_ &&
			//SendIDImageRZX2008_ &&
			//SendRegImageRZX2008_ &&
			UserLogoutRZX2008_ &&
			UserLogonRZX2008_)
		{
			//IBA_LOG0( "Leave CAuditInterface::Load()." );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CAuditInterface::CustomRegUser(RegUserInfo& regUserInfo)
{
	if(!Load())
	{
		IBA_LOG0(_T("加载实名接口动态库AuditInterface.dll失败！请检查AuditInterface.dll的版本！"));
		return FALSE;
	}
	uninit_();
	CStringA strIpA = CT2A(CNetBarConfig::GetInstance()->GetProxyIP());
	init_(strIpA, 7834);

	LONG ret = regUser_(regUserInfo.netbarID, regUserInfo.IDType, regUserInfo.strIDNumber, 
							regUserInfo.strUserName, regUserInfo.sex, regUserInfo.strCountryCode,
							regUserInfo.strAddress, regUserInfo.strTelphone, regUserInfo.memberID,
							regUserInfo.strBirthDay, regUserInfo.strOpenDay, regUserInfo.strValidDate,
							regUserInfo.national, regUserInfo.strCertificateOffice, regUserInfo.CardType,
							regUserInfo.strSerialNum, &regUserInfo.retCode);

	if (ret == 0)
	{
		if (regUserInfo.retCode == 1 || regUserInfo.retCode == -6 )
		{
			IBA_LOG(_T("%s 向实名接口传注册信息成功（任子行实名）!"), CA2T(regUserInfo.strSerialNum));
			return TRUE;
		}
	}
	else if (ret == ERROR_NETWORK_FAILED)
	{
		IBA_LOG(_T("%s 网络故障,上传用户信息失败!"), CA2T(regUserInfo.strSerialNum));
	}
	else
	{
		IBA_LOG(_T("%s 其它异常!"), CA2T(regUserInfo.strSerialNum));
	}
	return FALSE;
}



