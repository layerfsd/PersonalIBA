#include "stdafx.h"
#include "..\IBA.h"
#include "..\ServletHellper.h"
#include "..\MainFrm.h"
#include "..\Socket\LocalServer.h"
#include "..\CurrentNetBarUser.h"
#include ".\glwrealname.h"
#include "..\Encrypt.h"
#include "..\GlobalDefine.h"
#include "..\GlobalFunctions.h"

using namespace NS_REALNAME;
//过滤王
CGLWRealName::CGLWRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CGLWRealName"));
}

CGLWRealName::~CGLWRealName(void)
{
}

BOOL CGLWRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	ExecuteProxy();

	return TRUE;
}

INT CGLWRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_AUTO_TRACE("CGLWRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x, DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	if ( CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen) == -1)
	{
		return 0;
	}

	if (0 == nMsgType)
	{
		CIBALog::GetInstance()->Write(_T("CGLWRealName::OnRealNameMsg"));

		//返回0就会再发OPENUSER

		if (!ReadUserInfo(m_NetBarUser))//从注册表读取信息
		{
			CIBALog::GetInstance()->Write(_T("ReadUserInfo Error!"));
			return 0;
		}

		// 2013-6-9 11:36:55 打印过滤王传输的信息
		IBA_LOG(_T("过滤王传递信息, 用户名：%s, 上网账号：%s, 证件号码：%s, 证件类型：%d"), 
			m_NetBarUser.GetUserName(), m_NetBarUser.GetNetId(), 
			m_NetBarUser.GetCertificateID(), m_NetBarUser.GetCertificateType());

		CString strNetId = m_NetBarUser.GetNetId();

		//{ 2011/08/01-8201-gxx: 
		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			OnLocalRealName(nMsgType, strNetId);
			return 0;
		}
		else
		{
			CActiveMember ActiveMember;
			BOOL bGetOK = (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strNetId, ActiveMember));
			if (bGetOK && ActiveMember.GetIsLocalUser())
			{
				OnLocalRealName(nMsgType, strNetId);
				return 0;
			}
		}
		//}


		if (!CheckCard(m_NetBarUser.GetNetId()))
		{
			return 0;
		}

		m_NetBarUser.SetMemberId(m_nMemberId);//取得用户号

#ifdef GLW_FUNCTION_FUZHOU
		// 福州的过滤王具备这样的功能，姓名为空但有证件号码也能开户
		if (m_NetBarUser.GetUserName().GetLength() < 2 && m_NetBarUser.GetCertificateID().GetLength() > 1) {
			m_NetBarUser.SetUserName(IBAGlobal::MakeUserName());
		}
#endif
		
	
		if (m_NetBarUser.GetUserName().IsEmpty()) //关户消息
		{
			return OnCloseUser(m_NetBarUser);
		}
		else //开户消息
		{
			return OnOpenUser(m_NetBarUser);
		}

		//theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\GLW\\OpenUser"));
	}

	return 1;
}

INT CGLWRealName::OnCloseUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CGLWRealName::OnCloseUser"));

	if (NetBarUser.GetMemberId() > 0)//已经存在
	{	
		if (QueryBalance())
		{	//满足自动退款条件
			if (CDeregisterHelpper::AllowAutoRefund(CCurrentNetBarUser::GetInstance()->Balance))
			{
				if (CDeregisterHelpper::DeregisterUser(CCurrentNetBarUser::GetInstance()->Balance))
				{
					return 0; //自动退款了
				}
			}

			if (CCurrentNetBarUser::GetInstance()->Balance.GetNeedActivation() == 0) 
			{
				CIBALog::GetInstance()->Write(_T("不往过滤王激活！"));

				AfxGetMainWnd()->PostMessage(WM_EXECUTEREALNAME);

				return 1;//不需要激活，表示已经激活过了,直接返回一就可以
			}
		}
	}
	else//未注册
	{
		//根据二代证，激活相应的实名卡，或者直接更改上网账号 
		
		if (AutoMatchIDCard(NetBarUser))
		{
			if (CCurrentNetBarUser::GetInstance()->Balance.GetNeedActivation() == 0)
			{
				AfxGetMainWnd()->PostMessage(WM_EXECUTEREALNAME);

				return 1;//不需要激活，表示已经激活过了,直接返回一就可以
			}
		}
	}

	return 0; //只有在线返回1，其他全部返回0
}


INT CGLWRealName::OnOpenUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CGLWRealName::OnOpenUser"));

	if (NetBarUser.GetMemberId() > 0)
	{
		// 2011/09/05-8201-gxx: 在OnOpenUser里判断当前刷卡用户的用户号是否和前一次查余额时的用户号一致，如果不一致，那么就再重新查一次余额
		if (NetBarUser.GetMemberId() != CCurrentNetBarUser::GetInstance()->Balance.GetMemberId())
		{
			QueryBalance();
		}

		//密码改为实名密码
		AutoChangeRealNamePwd(NetBarUser);

		if (AutoModifyMemberInfo()) //更改资料
		{
			QueryBalance();
		}

		// 2011/11/29-8211-gxx: 
		int nRes = OnOldUser(NetBarUser);//旧用户
		IBA_LOG(_T("OnOpenUser Active return %d"),nRes);
		return nRes;

	}
	else 
	{
		if (AutoMatchIDCard(NetBarUser)) //找到匹配的人了
		{
			// 2011/11/29-8211-gxx: 
			int nRes = OnOldUser(NetBarUser);//旧用户
			IBA_LOG(_T("OnOpenUser Active return %d"),nRes);
			return nRes;
		}
		else
		{
			// 2011/11/25-8211-gxx: 改成有返回值
			INT nRes = OnNewUser(NetBarUser);
			IBA_LOG(_T("OnOpenUser Register return %d"),nRes);
			return nRes;
		}
	}

	// 2011/11/25-8211-gxx: 返回1改成返回0
	IBA_LOG0(_T("OnOpenUser return 0"));

	return 0;
}

//检查中心和实名的密码是否相同
BOOL CGLWRealName::AutoChangeRealNamePwd(CNetBarUser& NetBarUser)
{	
	if (CNetBarConfig::GetInstance()->GetRealNamePassword() < 2)	//小于2，不处理
	{
		return TRUE;
	}

	//过滤王密码为空,不处理
	if (NetBarUser.GetPassword().IsEmpty())
	{
		CIBALog::GetInstance()->Write(_T("过滤王密码为空，请检查实名密码设置是否正确！"));
		return TRUE;
	}

	//检验密码
	if (NetBarUser.GetPassword().Compare(CCurrentNetBarUser::GetInstance()->Balance.GetPassword()) != 0)
	{//密码不一致
		CResetPwdMethod ResetPwd;

		ResetPwd.SetMemberId(NetBarUser.GetMemberId());
		ResetPwd.SetPassword(NetBarUser.GetPassword());
		ResetPwd.SetPersonalId(CCurrentNetBarUser::GetInstance()->Balance.GetPersonalId());

		theApp.GetCurCashier()->DoResetPwd(ResetPwd);

		return FALSE;
	}

	return TRUE;
}

//中心与实名卡的用户资料是否完全相同


INT CGLWRealName::ChangeCertificateType(INT nTmp)
{
	switch (nTmp)
	{
	case 111: nTmp = 11; break; //身份证

	case 113: nTmp = 93; break; //户口薄

	case 414: nTmp = 94; break; //护照

	case 511: nTmp = 95; break; //台胞证

	case 516: nTmp = 96; break; //回乡证

	case 233: nTmp = 92; break; //士兵证

	case 114: nTmp = 90; break; //军官证

	case 123: nTmp = 91; break; //警官证

  case 335: nTmp = 20; break; //驾驶证

	default:
		nTmp = 99; //其他证件
	}	
	
	return nTmp;
}

BOOL CGLWRealName::ReadUserInfo(CNetBarUser & NetBarUser)
{
	//设定HKEY_CURRENT_USER\\software\\sunlike\\IBA\\下的访问键分支RealName\\GLW\\OpenUser
	
	CString strKey(_T("RealName\\GLW\\OpenUser"));

	CString strTmp = theApp.GetProfileString(strKey, _T("IDNo"));

	strTmp.Trim();

	if (strTmp.IsEmpty())
	{
		return FALSE;
	}
	else
	{
		//// 验证刷卡信息
		//CString strIDNo = theApp.GetProfileString(strKey, _T("IDNo"));
		//CString strTime = theApp.GetProfileString(strKey, _T("tValue1"));
		//CString strCode = theApp.GetProfileString(strKey, _T("tValue2"));
		//BYTE byCheck[8] = {0x03,0xF1,0x00,0x0A,0x0B,0xED,0x02,0xC1};
		//time_t tm = time(NULL);
		//int nTime = _ttol(strTime.GetString());

		//if (tm - nTime > 5* 60)
		//{
		//	AfxMessageBox(_T("无效的刷卡信息来源或者刷卡超时!"), MB_OK|MB_ICONERROR);
		//	IBA_LOG0(_T("刷卡超时!"));
		//	return FALSE;
		//}

		//CStringA strOrgCode;
		//strOrgCode += CT2A(strIDNo);
		//strOrgCode += CT2A(strTime);
		//INT nLen = strOrgCode.GetLength();
		//BYTE *pBuff = new BYTE[nLen+8];
		//memcpy(pBuff,strOrgCode.GetString(), nLen);
		//memcpy(pBuff+nLen, byCheck, 8);
		//char pCode[33] = {0};

		//CEncrypt::CalcMD5Bytes(pBuff, nLen+8, pCode);

		//CStringA strCodeA = CT2A(strCode);
		//if (strCodeA.CompareNoCase(pCode) != 0)
		//{
		//	AfxMessageBox(_T("无效的刷卡信息来源!"), MB_OK|MB_ICONERROR);
		//	IBA_LOG0(_T("无效的刷卡信息来源!"));
		//	delete []pBuff;
		//	return FALSE;
		//}

		//delete []pBuff;
	}

	// 判断是否为指纹识别的刷卡
	if (strTmp.GetLength() > 2) {
		if (strTmp.Left(2).CompareNoCase(_T("FN")) == 0) {
			// 是指纹识别，去掉FN则是上网账号或证件号
			NetBarUser.SetActivationType(1);
			strTmp = strTmp.Mid(2);
		}
	}
	NetBarUser.SetNetId(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Name"));
	strTmp.Trim();
	strTmp.Remove(' ');
	NetBarUser.SetUserName(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("CertificateID"));
	// 判断是否为指纹识别的刷卡
	if (strTmp.GetLength() > 2) {
		if (strTmp.Left(2).CompareNoCase(_T("FN")) == 0) {
			// 是指纹识别，去掉FN则是上网账号或证件号
			NetBarUser.SetActivationType(1);
			strTmp = strTmp.Mid(2);
		}
	}
	NetBarUser.SetCertificateID(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Password"));
	NetBarUser.SetPassword(strTmp);

	INT nTmp = theApp.GetProfileInt(strKey, _T("CertificateType"), 111);
	NetBarUser.SetCertificateType(ChangeCertificateType(nTmp));
	
	NetBarUser.SetDataFrom(1);

	return TRUE;
}


INT CGLWRealName::OnExecuteMsg(UINT nExecuteType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (m_NetBarUser.GetMemberId() > 0)
	{
		OnOldUser(m_NetBarUser);
	}

	return 0;
}

BOOL CGLWRealName::IsMenuEnabled()
{
	return TRUE;
}


INT CGLWRealName::InvokeRealName(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (!theApp.GetCurCashier()->IsOnDuty())//收银员不在线，及下班，则不退
	{
		return FALSE;
	}

	CIBALog::GetInstance()->WriteFormat(_T("InvokeRealName: nMsgType = %d"), nMsgType);//消息写日志

	switch (nMsgType) //1发行卡 2挂失 3修改密码 4加入会员
	{
	case 1:
		return SendMsgToGLW(1);//开户

	case 2:
		return SendMsgToGLW(2);//挂失

	case 3:
		return SendMsgToGLW(3);//修改密码

	case 4:
		return SendMsgToGLW(4);//加入会员
	}

	return FALSE;	
}

BOOL CGLWRealName::SendMsgToGLW(UINT nMsgType)
{
	HWND hSureName = ::FindWindow(NULL, _T("MessageForm"));

	if (hSureName != NULL)
	{
		//控制台6

		HWND hTmp = ::FindWindow(NULL, _T("控制台6"));

		::ShowWindow(hTmp, SW_MAXIMIZE);

		::BringWindowToTop(hTmp); 
	}
	else
	{
		hSureName = ::FindWindow(_T("TFrmPrepareOpenUser"), _T("预开户列表"));

		if (!IsWindow(hSureName))
		{
			AfxMessageBox(_T("找不到过滤王控制台！"));

			return FALSE;
		}
	}

	#define UO_BASE WM_USER + 2000

	BOOL bSend = ::PostMessage(hSureName, UO_BASE, nMsgType, NULL);

	return bSend;
} 


BOOL CGLWRealName::NeedToCheckRegisterData()
{
	return TRUE;
}

BOOL CGLWRealName::OnCardInfo(DataX_UserInfo& dxUserInfo)
{
	CString strTmp = CA2T(dxUserInfo.cardNo);
	m_NetBarUser.SetNetId(strTmp);

	strTmp = CA2T(dxUserInfo.name);
	m_NetBarUser.SetUserName(strTmp);

	strTmp = CA2T(dxUserInfo.idNo);
	m_NetBarUser.SetCertificateID(strTmp);

	m_NetBarUser.SetCertificateType(ChangeCertificateType(dxUserInfo.idType));

	strTmp = CA2T(dxUserInfo.password);
	m_NetBarUser.SetPassword(strTmp);

	m_NetBarUser.SetDataFrom(1);

	if (!CheckCard(m_NetBarUser.GetNetId()))
	{
		return 0;
	}

	m_NetBarUser.SetMemberId(m_nMemberId);//取得用户号

	if (m_nMemberId > 0)
	{
		if (QueryBalance()) //满足自动退款条件
		{	
			if (CCurrentNetBarUser::GetInstance()->Balance.GetOnline() == 1)
			{
				ReplyMessage(2);
			}
			else if (CCurrentNetBarUser::GetInstance()->Balance.GetNeedActivation() == 0) 
			{				
				ReplyMessage(1); //返回1给GLW
			}

			AfxGetMainWnd()->PostMessage(WM_EXECUTEREALNAME);
		}

		ReplyMessage(0);
	}
	else
	{
		//{ 2012/02/01-8237-gxx: 
		INT nRes = OnNewUser(m_NetBarUser);

		IBA_LOG(_T("CGLWRealName::OnCardInfo  send result:%d"), nRes);
		ReplyMessage(nRes);
		//}
		
	}

	return TRUE;
}

BOOL CGLWRealName::OnForceCheckOut(DataX_UserInfo& dxUserInfo)
{
	CString strCardNo = CA2T(dxUserInfo.cardNo);

	CString strTermId = CA2T(dxUserInfo.termId);

	CString strTermIp = CA2T(dxUserInfo.termIp);

	CIBALog::GetInstance()->WriteFormat(_T("CRealNameBase::OnForceCheckOut %s,%s,%s,%d"), 
		strCardNo, strTermId, strTermIp, dxUserInfo.dwReserve1);

	m_NetBarUser.SetNetId(strCardNo);

	if (!CheckCard(m_NetBarUser.GetNetId()))
	{
		return FALSE;
	}

	m_NetBarUser.SetMemberId(m_nMemberId);

	if (1 == dxUserInfo.dwReserve1) //非法用户，强制踢卡
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);

		if (nIndex >= 0) //远程下机
		{
			CComputerInfo& ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);

			if (ComputerInfo.GetTerminalID().CompareNoCase(strTermId) == 0 ||
				ComputerInfo.GetComputerIP().CompareNoCase(strTermIp) == 0)
			{
				CLocalServer::GetInstance()->LocalForceCheckOut(nIndex, _T("系统错误，请联系技术支持人员"));
			}
		}
	
		CCheckout2Method Checkout2;
		Checkout2.SetSafeLogout(0); //扣到当前时间
		Checkout2.SetMemberId(m_nMemberId);
		theApp.GetCurCashier()->DoCheckout2(Checkout2);	

		return 1;
	}
	else
	{
		if (m_nMemberId > 0)
		{
			QueryBalance();

			if (CCurrentNetBarUser::GetInstance()->Balance.GetNeedActivation() == 0) 
			{
				CIBALog::GetInstance()->Write(_T("不往过滤王激活！"));

				AfxGetMainWnd()->PostMessage(WM_EXECUTEREALNAME);

				return 1;
			}
		}
	}

	return 2;
}
