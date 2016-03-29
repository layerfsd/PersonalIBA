#include "stdafx.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\ServletHellper.h"
#include "..\CurrentNetBarUser.h"
#include ".\ybwwrealname.h"

using namespace NS_REALNAME;

CYBWWRealName::CYBWWRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CYBWWRealName"));

	CIBAHelpper::MakeSurePathExists(theApp.GetWorkPath() + _T("\\IBARealName"));

	m_strYB2IBA = theApp.GetWorkPath() + _T("\\IBARealName\\YB2IBA.exe");
}

CYBWWRealName::~CYBWWRealName(void)
{
}

BOOL CYBWWRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	if (!ExecYB2IBA())
	{
		AfxMessageBox(m_strYB2IBA + _T(" 不存在！不能正常刷卡"));
	}

	Start();

	return TRUE;
}

BOOL CYBWWRealName::UnInitRealName()
{
	CRealNameBase::UnInitRealName();

	Terminate();

	return TRUE;
}

BOOL CYBWWRealName::ReadUserInfo(CNetBarUser & NetBarUser)
{
	CString strKey(_T("RealName\\YBWW"));

	CString strTmp = theApp.GetProfileString(strKey, _T("Hc1"));

	strTmp.Trim();

	if (strTmp.IsEmpty())
	{
		return FALSE;
	}

	//NetBarUser.SetIDNo(strTmp);
	NetBarUser.SetNetId(strTmp);

	NetBarUser.SetCertificateID(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Hc2"));
	strTmp.Trim();
	strTmp.Remove(' ');
	NetBarUser.SetUserName(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Hc3"));
	strTmp.Trim();

	if (strTmp == _T("男"))
	{
		NetBarUser.SetSex(0);
	}
	else
	{
		NetBarUser.SetSex(1);
	}

	NetBarUser.SetCertificateType(11);

	NetBarUser.SetDataFrom(1);

	return TRUE;
}

INT CYBWWRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_AUTO_TRACE("CYBWWRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x, DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen);

	if (0 == nMsgType)
	{
		ReadUserInfo(m_NetBarUser);

		//if (!CheckCard(m_NetBarUser.GetIDNo()))
		if (!CheckCard(m_NetBarUser.GetNetId()))
		{
			return 0;
		}

		if (m_nMemberId > 0) //老卡，先激活
		{
			m_NetBarUser.SetMemberId(m_nMemberId);

			if (QueryBalance())
			{	
				if (CDeregisterHelpper::AllowAutoRefund(CCurrentNetBarUser::GetInstance()->Balance)) //满足自动退款条件
				{
					if (CDeregisterHelpper::DeregisterUser(CCurrentNetBarUser::GetInstance()->Balance))//自动退款了
					{
						//由于网盾没有数据传给我们，所以虚拟用户数据
						OnNewUser(m_NetBarUser);
						return 0; 
					}
				}
			}

			OnOldUser(m_NetBarUser);
		}
		else //新卡，计费未注册
		{
			OnNewUser(m_NetBarUser);
		}
	}

	return 0;
}

INT CYBWWRealName::Run()
{
	while (TRUE)
	{
		ExecYB2IBA();

		Sleep(1000);
	}

	return 0;
}

BOOL CYBWWRealName::ExecYB2IBA()
{	
	BOOL bRet = FALSE;

	//HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("IBAREALNAME_YB2IBA"));

	//if (hMutex != NULL)
	//{
	//	CloseHandle(hMutex);
	//	bRet = TRUE;
	//}
	//else
	//{
	//	if (GetLastError() == ERROR_ACCESS_DENIED) //没权限访问
	//	{
	//		bRet = TRUE;
	//	}
	//}

	bRet = CL2Process::CheckRunning(_T("IBAREALNAME_YB2IBA"));

	if (!bRet)
	{
		if (PathFileExists(m_strYB2IBA))
		{
			CL2Process YB2IBA(m_strYB2IBA);

			return YB2IBA.Execute(_T("YBWW"));
		}
	}

	return bRet;
}