#include "stdafx.h"
#include "..\IBA.h"
#include "..\ServletHellper.h"
#include ".\qdhxrealname.h"

using namespace NS_REALNAME;
//青岛恒信

CQDHXRealName::CQDHXRealName(void)
{//写日志
	CIBALog::GetInstance()->Write(_T("CQDHXRealName"));
}

CQDHXRealName::~CQDHXRealName(void)
{
}

INT CQDHXRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_AUTO_TRACE("CQDHXRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x, DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen);

	if (0 == nMsgType) //刷卡信息
	{
		//2015-1027
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			return 0;
		}

		CString strKey(_T("RealName\\HX\\OpenUser"));//青岛恒信

		CString strIdCardNumber = theApp.GetProfileString(strKey, _T("IDNo"));

		if (strIdCardNumber.IsEmpty())
		{
			return 0;
		}

		if (!CheckCard(strIdCardNumber))
		{
			return 0;
		}

		CNetBarUser NetBarUser;
		NetBarUser.SetNetId(strIdCardNumber);
		NetBarUser.SetDataFrom(1);

		if (m_nMemberId > 0) //老卡，先激活
		{
			NetBarUser.SetMemberId(m_nMemberId);

			QueryBalance();

			OnOldUser(NetBarUser);
		}
		else //新卡，计费未注册
		{
			NetBarUser.SetCertificateType(11);
			// 2013-12-6 15:31:50 qsc
			if (AutoMatchIDCard(m_NetBarUser))
			{
				AfxGetMainWnd()->BringWindowToTop();
				AfxGetMainWnd()->SetForegroundWindow();

				OnOldUser(m_NetBarUser);
			}
			else
			{
				OnNewUser(m_NetBarUser);
			}
		}
	}

	return 0;
}

BOOL CQDHXRealName::UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	return FALSE;
}
