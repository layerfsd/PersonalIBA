#include "stdafx.h"
#include "..\IBA.h"
#include "..\ServletHellper.h"
#include "..\MainFrm.h"
#include "..\Socket\LocalServer.h"
#include "..\CurrentNetBarUser.h"
#include ".\jqrealname.h"
#include "..\Encrypt.h"

using namespace NS_REALNAME;
//过滤王
CJQRealName::CJQRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CJQRealName"));
}

CJQRealName::~CJQRealName(void)
{
}

BOOL CJQRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	ExecuteProxy();

	return TRUE;
}

INT CJQRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_AUTO_TRACE("CJQRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x, DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	if ( CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen) == -1)
	{
		return 0;
	}

	if (0 == nMsgType)
	{
		CIBALog::GetInstance()->Write(_T("CJQRealName::OnRealNameMsg"));

		//返回0就会再发OPENUSER

		if (!ReadUserInfo(m_NetBarUser))//从注册表读取信息
		{
			CIBALog::GetInstance()->Write(_T("ReadUserInfo Error!"));
			return 0;
		}

		// 2013-6-9 11:36:55 打印过滤王传输的信息
		IBA_LOG(_T("金桥传递信息, 用户名：%s, 上网账号：%s, 证件号码：%s, 证件类型：%d, 地址:%d, 性别:%d"), 
			m_NetBarUser.GetUserName(), m_NetBarUser.GetNetId(), 
			m_NetBarUser.GetCertificateID(), m_NetBarUser.GetCertificateType(),
			m_NetBarUser.GetAddr(), m_NetBarUser.GetSex());

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
	
		if (m_NetBarUser.GetUserName().IsEmpty()) //关户消息
		{
			return OnCloseUser(m_NetBarUser);
		}
		else //开户消息
		{
			return OnOpenUser(m_NetBarUser);
		}

		theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\JQ\\OpenUser"));
	}

	return 1;
}

INT CJQRealName::OnCloseUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CJQRealName::OnCloseUser"));

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
				CIBALog::GetInstance()->Write(_T("不往金桥激活！"));

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


INT CJQRealName::OnOpenUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CJQRealName::OnOpenUser"));

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
BOOL CJQRealName::AutoChangeRealNamePwd(CNetBarUser& NetBarUser)
{	
	if (CNetBarConfig::GetInstance()->GetRealNamePassword() < 2)	//小于2，不处理
	{
		return TRUE;
	}

	//过滤王密码为空,不处理
	if (NetBarUser.GetPassword().IsEmpty())
	{
		CIBALog::GetInstance()->Write(_T("金桥密码为空，请检查实名密码设置是否正确！"));
		return TRUE;
	}

	//检验密码
	if (NetBarUser.GetPassword().Compare(CCurrentNetBarUser::GetInstance()->Balance.GetPassword()) != 0)
	{
		//密码不一致
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


INT CJQRealName::ChangeCertificateType(INT nTmp)
{
	return nTmp;
}

BOOL CJQRealName::ReadUserInfo(CNetBarUser & NetBarUser)
{
	//设定HKEY_CURRENT_USER\\software\\sunlike\\IBA\\下的访问键分支RealName\\GLW\\OpenUser
	
	CString strKey(_T("RealName\\JQ\\OpenUser"));

	CString strTmp = theApp.GetProfileString(strKey, _T("IDNo"));

	strTmp.Trim();

	if (strTmp.IsEmpty())
	{
		return FALSE;
	}
	else
	{
		CString strTime, strIDNo, strCode;
		// 验证刷卡信息
		strTime = theApp.GetProfileString(strKey, _T("vValue1"));
		strCode = theApp.GetProfileString(strKey, _T("vValue2"));

		time_t tm = time(NULL);
		long nTime = _ttol(strTime.GetString());

		if (tm - nTime > 5* 60)
		{
			AfxMessageBox(_T("无效的刷卡信息来源或者刷卡超时!"), MB_OK|MB_ICONERROR);
			IBA_LOG0(_T("刷卡超时!"));
			return FALSE;
		}

		CString strOrgCode, strMD5;
		strOrgCode += CT2A(strTmp);
		strOrgCode += CT2A(strTime);
		strOrgCode += "jqdodonew1035";

		// 账号是数字和字母组合，不考虑编码问题
		CEncrypt::CalcMD5(strOrgCode, strMD5);

		if (strCode != strMD5)
		{
			AfxMessageBox(_T("无效的刷卡信息来源!"), MB_OK|MB_ICONERROR);
			IBA_LOG0(_T("无效的刷卡信息来源!"));
			return FALSE;
		}
	}

	NetBarUser.SetNetId(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Name"));
	strTmp.Trim();
	strTmp.Remove(' ');
	NetBarUser.SetUserName(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("CertificateID"));
	NetBarUser.SetCertificateID(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Password"));
	NetBarUser.SetPassword(strTmp);

	INT nTmp = theApp.GetProfileInt(strKey, _T("CertificateType"), 11);
	NetBarUser.SetCertificateType(ChangeCertificateType(nTmp));
	
	// 国家 没去用
	strTmp = theApp.GetProfileString(strKey, _T("Country"));

	strTmp = theApp.GetProfileString(strKey, _T("Address"));	
	NetBarUser.SetAddr(strTmp);

	// jqadd
	int nSex = theApp.GetProfileInt(strKey, _T("Sex"), 0);
	NetBarUser.SetSex(nSex);

	// 其他信息 没去用
	strTmp = theApp.GetProfileString(strKey, _T("Other"));

	NetBarUser.SetDataFrom(1);

	return TRUE;
}


INT CJQRealName::OnExecuteMsg(UINT nExecuteType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (m_NetBarUser.GetMemberId() > 0)
	{
		OnOldUser(m_NetBarUser);
	}

	return 0;
}

BOOL CJQRealName::IsMenuEnabled()
{
	return FALSE;
}
