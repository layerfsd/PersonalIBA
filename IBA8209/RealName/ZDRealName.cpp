#include "stdafx.h"
#include "Proxy.h"
#include "..\IBAHelpper.h"
#include "..\IBA.h"
#include ".\zdrealname.h"
#include "..\RealName\IZd.h"
#include "..\ServletHellper.h"
#include "..\CurrentNetBarUser.h"
#include "..\DAL\IBADAL.h"
#include "..\Socket\LocalServer.h"
#include "..\ComputerListView.h"
#include "..\IBADialog.h"
#include "..\DlgRNZdCrc.h"

using namespace NS_REALNAME;

CZDRealName::CZDRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CZDRealName"));
}

CZDRealName::~CZDRealName(void)
{

}

BOOL CZDRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	// 启动删除一些过期数据
	CIBADAL::GetInstance()->DelZdRnOutDateData();

	BOOL bRet = FALSE;
	
	if(CIZd::GetInstance()->IsLoad())
	{
		CIZd::IC_CRC_RE icCRCRe;
		DWORD dwCRC = CIBAConfig::GetInstance()->GetZdCrc();
		
		if(0 == dwCRC)
		{
			CDlgRNZdCrc dlg;
			dlg.DoModal();
			dwCRC = dlg.GetCRC();
		}
		
		icCRCRe = CIZd::GetInstance()->m_pfn_V2_SetCRC(dwCRC, CT2A(CIBALog::GetInstance()->GetAppVer()));

		if(0 == icCRCRe.iRe)
		{
			IBA_LOG(_T("CRC校验成功"));	// 2014-8-28 - qsc
			CIZd::GetInstance()->SetInit(TRUE);
			bRet = TRUE;

			CIBAConfig::GetInstance()->SetZdCrc(dwCRC);
			CIBAConfig::GetInstance()->SaveConfig();	// 先保存一次
		}
		else
		{
			CIBAConfig::GetInstance()->SetZdCrc(0);	// 失败清空
			CIBAConfig::GetInstance()->SaveConfig();	

			CIZd::GetInstance()->SetInit(FALSE);
			bRet = FALSE;

			CString strShow;
			if(-1 == icCRCRe.iRe)
			{
				strShow = _T("CRC校验错误");
			}
			else if(-2 == icCRCRe.iRe)
			{
				strShow = _T("版本号错误");
			}
			CIBAHelpper::MsgBox(AfxGetMainWnd(), strShow, MB_OK);
		}
	}

	return bRet;
}

BOOL CZDRealName::UnInitRealName()
{
	CRealNameBase::UnInitRealName();


	return TRUE;
}

INT CZDRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
#ifdef _DEBUG
	CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen);
#endif
	IBA_LOG0(_T("CZDRealName::OnRealNameMsg"));		// 2014-8-28 - qsc
	// 2014-8-28 - qsc
	if(!CIZd::GetInstance()->IsInit())
	{
		IBA_LOG0(_T("未初始化"));
		return 0;
	}
	UNIT_AUTO_TRACE("CZDRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x, DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	COPYDATASTRUCT *pCopyDataStruct = (COPYDATASTRUCT *)lpData;

	DWORD dwOpera = pCopyDataStruct->dwData;

	switch (dwOpera)
	{
	case IK_ID_OPENUSER:	// 实名卡开户
	case IK_ID_CLOSEUSER:	//实名卡关户
		{
			CIZd::IC_USER iUser;
			ZeroMemory(&iUser, sizeof(CIZd::IC_USER));
			memcpy(&iUser, pCopyDataStruct->lpData, pCopyDataStruct->cbData);

			CIZd::T_IC_USER tiUser;
			CIZd::Conver_IC_USER(&iUser, &tiUser);

			CRealNameBase::OnRealNameMsg(nMsgType, tiUser.szikIDNo, sizeof(tiUser.szikIDNo));
			OnOpenCloseUser(dwOpera, &tiUser);
		}
		break;
	case IK_ID_LOGINOUT:	//实名卡解锁 也即让客户端上机
	case IK_ID_LOGININ:	//实名卡锁定 也即让客户端锁屏
		{
			CIZd::IC_LOGIN iLogin;
			ZeroMemory(&iLogin, sizeof(CIZd::IC_LOGIN));
			memcpy(&iLogin, pCopyDataStruct->lpData, pCopyDataStruct->cbData);

			CIZd::T_IC_LOGIN tiLogin;
			CIZd::Conver_IC_LOGIN(&iLogin, &tiLogin);	
			OnLoginOut(dwOpera, &tiLogin);
		}
		break;
	
	case IK_ID_GETIDCARDDATA:
		/*CIZd::IC_CARDDATA icCardData;
		CIZd::ID_RETURN idReturn = CIZd::GetInstance()->m_pfn_V2_SendICCardData();
		if(0 == idReturn.iErrCode)
		{
			IBA_LOG0(_T("m_pfn_V2_SendICCardData 成功"));
		}
		else
		{
			IBA_LOG(_T("m_pfn_V2_SendICCardData 失败:%s"), idReturn.szErrMsg);
		}*/

		break;
	default:
		break;
	}
	

	return 0;
}

BOOL CZDRealName::OnOpenCloseUser(int nType, CIZd::T_IC_USER* pICUser)
{
	IBA_LOG(_T("智多传递开关户信息:实名卡编号 %s, 姓名 %s, 证件类型 %d, 证件号码 %s, 上网卡获取类型 %d, 手机号 %s , 网络操作是否成功 %d"),
		pICUser->szikIDNo, pICUser->szikName, pICUser->iCertType, pICUser->szCertNumber, pICUser->iCardType, pICUser->szMobile, pICUser->iValidate);

	CNetBarUser NetBarUser;
	NetBarUser.SetNetId(pICUser->szikIDNo);
	NetBarUser.SetCertificateID(pICUser->szCertNumber);
	NetBarUser.SetCertificateType(CIZd::SwitchZd2Ddn(pICUser->iCertType));
	NetBarUser.SetUserName(pICUser->szikName);
	NetBarUser.SetSex(0);
	NetBarUser.SetTel(pICUser->szMobile);

	if (!CheckCard(NetBarUser.GetNetId()))
	{
		return 0;
	}

	//ReplyMessage(0);

	if (m_nMemberId > 0) //老卡，先激活
	{
		NetBarUser.SetMemberId(m_nMemberId);
		if(IK_ID_OPENUSER == nType)
		{
			if (QueryBalance())
			{	
				if (CDeregisterHelpper::AllowAutoRefund(CCurrentNetBarUser::GetInstance()->Balance)) //满足自动退款条件
				{
					if (CDeregisterHelpper::DeregisterUser(CCurrentNetBarUser::GetInstance()->Balance))//自动退款了
					{
						//由于网盾没有数据传给我们，所以虚拟用户数据


						OnNewUser(NetBarUser);

						return 0; 
					}
				}
			}

			OnOldUser(NetBarUser);
			AutoChangeRealNamePwd(NetBarUser);
		}
		else if(IK_ID_CLOSEUSER == nType)
		{
			OnCloseUser(NetBarUser);
		}
	}
	else //新卡，计费未注册
	{
		if(IK_ID_OPENUSER == nType)
		{
			OnNewUser(NetBarUser);
		}
		else if(IK_ID_CLOSEUSER == nType)
		{
			// 该卡不存在，不做任何操作
		}
		
	}
	return 0;
}

BOOL CZDRealName::OnLoginOut(int nType, CIZd::T_IC_LOGIN* ptLogin)
{
	IBA_LOG(_T("智多传递开关户信息:用户的绑定卡编号 %s, 实名卡编号 %s, 客户机IP地址 %s, 客户机名称 %s"),
		ptLogin->szikICNo, ptLogin->szikIDNo, ptLogin->szikIP, ptLogin->szikHost);
	
	if (!CheckCard(ptLogin->szikICNo))
	{
		return FALSE;
	}
	
	if(m_nMemberId <= 0)
	{
		return FALSE;
	}

	if(IK_ID_LOGININ == nType)
	{
		CheckIn(ptLogin->szikHost, ptLogin->szikIP, ptLogin->szikICNo, ptLogin->szicPassword, m_nMemberId);
	}
	else if(IK_ID_LOGINOUT == nType)
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);

		if (nIndex >= 0)
		{	
			CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, m_nMemberId, 2);

			Sleep(500);
		}

		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
	}
	

	return TRUE;
}

BOOL CZDRealName::CheckRealName()
{
	CRealNameBase::CheckRealName();


	return TRUE;
}

BOOL CZDRealName::IsRegisterEnabled()
{
	return TRUE;
}

BOOL CZDRealName::IsCheckInEnabled()
{
	if(CIZd::GetInstance()->IsInit())
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

INT CZDRealName::OnCloseUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CZDRealName::OnCloseUser"));

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

INT CZDRealName::OnOpenUser(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CZDRealName::OnOpenUser"));

	if (NetBarUser.GetMemberId() > 0)
	{
		// 2011/09/05-8201-gxx: 在OnOpenUser里判断当前刷卡用户的用户号是否和前一次查余额时的用户号一致，如果不一致，那么就再重新查一次余额
		if (NetBarUser.GetMemberId() != CCurrentNetBarUser::GetInstance()->Balance.GetMemberId())
		{
			QueryBalance();
		}

		//密码改为实名密码
		//AutoChangeRealNamePwd(NetBarUser);

		if (AutoModifyMemberInfo()) //更改资料
		{
			QueryBalance();
		}

		// 2011/11/29-8211-gxx: 
		int nRes = OnOldUser(NetBarUser);//旧用户
		AutoChangeRealNamePwd(NetBarUser);
		IBA_LOG(_T("OnOpenUser Active return %d"),nRes);
		return nRes;

	}
	else 
	{
		if (AutoMatchIDCard(NetBarUser)) //找到匹配的人了
		{
			// 2011/11/29-8211-gxx: 
			int nRes = OnOldUser(NetBarUser);//旧用户
			AutoChangeRealNamePwd(NetBarUser);
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

BOOL CZDRealName::CheckTakeUp(CString strTermId)
{
	//查询该终端是否被占用
	BOOL bTakeUp = FALSE;

	CLastUserInfo LastUserInfo;

	if (CIBADAL::GetInstance()->GetLastUserInfo(LastUserInfo, 0, strTermId))
	{
		if (LastUserInfo.GetSuspend())//挂机
		{
			bTakeUp = TRUE;
		}
		else//非挂机
		{
			COleDateTime updateTime;
			updateTime.ParseDateTime(LastUserInfo.GetUpdateTime());
			COleDateTimeSpan interval = COleDateTime::GetCurrentTime() - updateTime;

			if (interval.GetTotalMinutes() < 10)//被占用，并且用户还有效
			{
				bTakeUp = TRUE;
			} 
		}
	}

	return bTakeUp;
}


void CZDRealName::CheckoutOldComputer(const CString& strTermID, int nMemberId)
{
	if (strTermID.IsEmpty())
	{
		return;
	}

	INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.LookFor(0, strTermID);

	if (nIdx < 0)
	{
		return;
	}

	IBA_LOG(_T("%s 换机下机"), strTermID);
	CLocalServer::GetInstance()->LocalCheckOutUser(nIdx, nMemberId, 1);

	Sleep(1000);

	CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);
}

BOOL CZDRealName::CheckIn(LPCTSTR szTermID, LPCTSTR szComputerIP, LPCTSTR szNetId, LPCTSTR szPwd, int nMemberId)
{
	INT_PTR nIdx = CLocalServer::GetInstance()->ComputerList.Find(szTermID, szComputerIP);
	CComputerInfo & computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIdx);

	if (CheckTakeUp(computer.GetTerminalID())) //占用了
	{
		CIBAHelpper::MsgBox(AfxGetMainWnd(), IDS_CHANGECOMPUTERERROR);
		return FALSE;
	}
	
	//添加到数据库
	CLastUserInfo LastUserInfo;
	LastUserInfo.SetNetId(szNetId);
	LastUserInfo.SetMemberId(nMemberId);
	LastUserInfo.SetTermId(computer.GetTerminalID());
	LastUserInfo.SetLoginStyle(1/*GetCheckIn() ? 1 : 0*/); //自动登录要求传强制登录参数
	LastUserInfo.SetUpdateTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));

	// 中心用户
	CString strTmp = CIBAHelpper::EncodePassword(szPwd);
	LastUserInfo.SetPassWord(strTmp);

	CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
	CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);


	//请求登录
	
	if (computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
	{
		CLocalServer::GetInstance()->LocalAutoCheckIn(nIdx);
	}
	else
	{
		IBA_LOG0(_T("自动开机"));
		CLocalServer::GetInstance()->LocalStartupComputer(nIdx);
	}

	CIBAHelpper::MsgBoxFormat(AfxGetMainWnd(), MB_OK, IDS_CHANGECOMPUTERSUCCESS, LastUserInfo.GetTermId());
	return TRUE;
}

void NS_REALNAME::CZDRealName::OnCheckOut( const CheckoutInfo* pCheckoutInfo )
{
	IBA_LOG0(_T("OnCheckOut Enter"));
	// 主收银台才做这个操作
	if (CIBAConfig::GetInstance()->GetCashRegisterID() == 1)
	{
		if(-1 == pCheckoutInfo->nCheckoutType)
		{
			IBA_LOG0(_T("异常数据"));
			ASSERT(FALSE);
		}
		else
		{
			// 不是换机
			if(CIBAGlobal::cot_Change != pCheckoutInfo->nCheckoutType)
			{
				
				CStringA strId = CT2A(pCheckoutInfo->szNetId);

				CStringA strTermId = pCheckoutInfo->szTermId;
				strTermId.Trim();

				// 收银台的结账消息，只处理结账消息，不处理下机消息
				// 如果收银台的结账消息，终端ID不为空，就不做操作，等客户端发来结账消息再操作(换机失败的情况需要处理终端不为空的)

				if(pCheckoutInfo->bClientCheckOut)
				{
					IBA_LOG0(_T("客户端结账消息"));
					CIZd::GetInstance()->V2_Logout(strId, strId, CT2A(pCheckoutInfo->szIP));
					CIZd::GetInstance()->V2_CloseUser(strId, strId,
						CT2A(pCheckoutInfo->szIP), CT2A(pCheckoutInfo->szTermId));
					CIBADAL::GetInstance()->DelZdRnInfo(CA2T(strId));
				}
				else
				{
					if(strTermId.IsEmpty())
					{
						IBA_LOG0(_T("收银台未上机结账消息"));
						CIZd::GetInstance()->V2_CloseUser(strId, strId,
							CT2A(pCheckoutInfo->szIP), CT2A(pCheckoutInfo->szTermId));
						CIBADAL::GetInstance()->DelZdRnInfo(CA2T(strId));
					}
					else
					{
						IBA_LOG0(_T("收银台换机结账消息"));
						// 1. 收银台结账消息的终端和数据库终端不一致，说明是换机失败了
						// 以数据库的为准

						// 2. 终端相同，但是有可能是客户端直接换机（只是锁屏），然后直接收银台结账，终端号存在
						CString strDBTermId;
						COleDateTime oleTime;
						CIBADAL::GetInstance()->GetZdRnInfo(CA2T(strId), strDBTermId, oleTime);	

						bool bHaveChangeLogout = false;
						if(0 != strDBTermId.CompareNoCase(pCheckoutInfo->szTermId))
						{
							bHaveChangeLogout = true;
						}
						else		
						{
							int HaveChange = 0;
							CIBADAL::GetInstance()->GetHaveChange(pCheckoutInfo->szNetId, HaveChange);
							if(HaveChange)
							{
								bHaveChangeLogout = true;
							}
						}

						if(bHaveChangeLogout)
						{
							CString strIP;
							CIBADAL::GetInstance()->GetIPFromTermId(strDBTermId, strIP);

							CIZd::GetInstance()->V2_Logout(strId, strId, CT2A(strIP));
							CIZd::GetInstance()->V2_CloseUser(strId, strId,
								CT2A(strIP), CT2A(strDBTermId));
							CIBADAL::GetInstance()->DelZdRnInfo(CA2T(strId));
						}
					}
				}
			}
			else
			{
				CIBADAL::GetInstance()->SetHaveChange(pCheckoutInfo->szNetId, 1);
			}
		}	
	}
			
	IBA_LOG0(_T("OnCheckOut Leave"));
}

//检查中心和实名的密码是否相同
BOOL NS_REALNAME::CZDRealName::AutoChangeRealNamePwd(CNetBarUser& NetBarUser)
{	
	//密码不为空，设置为空
	if (!CCurrentNetBarUser::GetInstance()->Balance.GetPassword().IsEmpty())
	{
		CResetPwdMethod ResetPwd;

		ResetPwd.SetMemberId(NetBarUser.GetMemberId());
		ResetPwd.SetPassword(NetBarUser.GetPassword());
		ResetPwd.SetPersonalId(CCurrentNetBarUser::GetInstance()->Balance.GetPersonalId());

		theApp.GetCurCashier()->DoResetPwd(ResetPwd);

		return FALSE;
	}

	return TRUE;
}