
#include "stdafx.h"
#include ".\realnamebase.h"
#include "..\IBA.h"
#include "..\Cashier.h"
#include "..\MainFrm.h"
#include "..\NDReturnDlg.h"
#include "..\NDCreditDlg.h"
#include "..\CreditDlg.h"
#include "..\RegisterDlg.h"
#include "..\RegImageDlg.h"
#include "..\CheckMemberDlg.h"
#include "..\DialogManager.h"
#include "..\ScanReco.h"
#include "..\IBAHelpper.h"
#include "..\TTS\IBATTS.h"
#include "..\Socket\LocalServer.h"
#include "..\ServletHellper.h"
#include "..\..\Lib\icDllChain.h"
#include "..\ActiveMemberView.h"
#include "..\IdentityNumber.h"
#include "..\NetBarConfig.h"
#include "..\RealName\IBARealName.h"
#include "..\RealName\WDRealName.h"
#include "RZXRealName.h"
#include "..\Dal\IBADAL.h"
#include "..\ModifyMemberDlg.h"
#include "AuditInterface.h"
#include "..\LocalCenter\G2XCommonTool.h"
#include "..\GlobalFunctions.h"
#include "..\ReturnDlg.h"


// 2014-5-23 - qsc
static std::list<CAuditInterface::RegUserInfo> g_listRegUserInfo;
static CCriticalSection g_csRegUserInfo;
static HANDLE g_hRegUserInfo = NULL;

using namespace NS_REALNAME;
using namespace IBAGlobal;
//实名基类
CRealNameBase::CRealNameBase(void)
: m_bOpenReader(FALSE), m_nCardId(0), m_nMemberId(0)
, m_DataXReceiver(this, OnDataArrival)
{	
	CIBALog::GetInstance()->Write(_T("CRealNameBase"));
	
	//保证路径存在
	CIBAHelpper::MakeSurePathExists(theApp.GetWorkPath() + _T("\\IBATemp"));
	//CIBAHelpper::MakeSurePathExists(theApp.GetWorkPath() + _T("\\IBARealName"));
}

CRealNameBase::~CRealNameBase(void)
{
}

/*************************************************
Function:       // ActivationMember
Description:    // 显示刷卡记录，根据后台设置上传照片和修改用户资料，进行中心激活，用户视图中删除后重新增加以显示在第一个
Calls:          // CServletHellper::ActivationMember
Input:          // netBarUser - 包含MemberId, Department, Address, Nation等用户资料
Return:         // 激活成功返回TRUE，其余返回FALSE
Others:         // 如果是实名刷卡，Balance未通过QueryBalance是否有值？？
*************************************************/
BOOL CRealNameBase::ActivationMember(const CNetBarUser& netBarUser)
{
	IBA_LOG(_T("CRealNameBase::ActivationMember Start - for MemberID = %d"),netBarUser.GetMemberId());

	UINT nMemberID = netBarUser.GetMemberId();
	
	SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 1, NULL); //显示刷卡记录

	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
	
	//如管家设置了上传照片，上传照片
	if(0 != CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		CString strDest = CIBAHelpper::GetUserZjImgFilePath(Balance.GetIdType(), Balance.GetPersonalId());
		// 2014-8-13 - qsc 每次都覆盖 if(!PathFileExists(strDest))
		{
			const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
			CopyFile(strSource, strDest, FALSE);
			// 2014-7-4 - qsc 拷贝完做下备份,再删除原文件
			CString strBak = theApp.GetWorkPath() + _T("\\IBATemp\\ID_bak.jpg");
			CopyFile(strSource, strBak, FALSE);
			DeleteFile(strSource);
		}		
	}

	if(2 == CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		CString strDest = CIBAHelpper::GetUserZcImgFilePath(Balance.GetIdType(), Balance.GetPersonalId());
		if(!PathFileExists(strDest))
		{
			if(!CIBAHelpper::TakePhoto(CIBAHelpper::GetUserZcImgPath(Balance.GetIdType(), Balance.GetPersonalId()), Balance.GetPersonalId()))
			{
				IBA_LOG(_T("CRealNameBase::ActivationMember Result is FALSE caused by taking photo failed- for MemberID = %d"),netBarUser.GetMemberId());
				return FALSE; 
			}
		}		
	}

	BOOL bUpdateDataBase = FALSE;

	//2014-1-2-qsc 激活修改用户表资料     如果管家设置了深圳or验证电话号码
	if(0 != CNetBarConfig::GetInstance()->GetShenzhen() ||	(0 != CNetBarConfig::GetInstance()->GetTelVerify()))
	{
		CString strTel(Balance.GetTel());
		strTel.Trim();
		CString strDepart(Balance.GetIdDepart());
		strDepart.Trim();
		CString strAddress(Balance.GetAddress());
		strAddress.Trim();
		CString strNation(Balance.GetJob());
		strNation.Trim();

		//如果电话号码为空，则修改用户资料，输入电话号码
		if(0 != CNetBarConfig::GetInstance()->GetTelVerify() &&	strTel.IsEmpty())
		{
			IBA_LOG0(_T("手机号码为空"));
			if (AfxGetMainWnd()->IsIconic())//判断当前是最小窗口
			{
				AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);//最大化
			}

			CModifyMemberDlg dlg(NULL, TRUE, FALSE);
			dlg.SetUserInfo(Balance);
			if(IDOK == dlg.DoModal())
			{
				strTel = dlg.GetTelNum();
				bUpdateDataBase = TRUE;
			}
			else
			{
				IBA_LOG(_T("CRealNameBase::ActivationMember Result is FALSE caused by CModifyMemberDlg canceled- for MemberID = %d"),netBarUser.GetMemberId());
				return FALSE;
			}
		}

		CString strBDeparet = Balance.GetIdDepart();
		CString strBAddr = Balance.GetAddress();
		CString strBNation = Balance.GetJob();

		CString strNDeparet = netBarUser.GetDepartment();
		CString strNAddr = netBarUser.GetAddr();
		CString strNNation = netBarUser.GetNation();

		IBA_LOG(_T("本地数据:%s,%s,%s, 中心数据:%s,%s,%s"), strNDeparet, strNAddr, strNNation,
			strBDeparet, strBAddr, strBNation);

		// 2014-5-8 这里可以做优化，放到上面去(和CModifyMemberDlg功能重复了)，不过等稳定了再优化吧
		if(strBDeparet != strNDeparet ||
			strBAddr != strNAddr ||
			strBNation != strNNation)
		{
			// 2014-7-16 - qsc 中心返回三个数据都不为空的话就不更新数据
			bool bCenterThreeNoEmpty = false;
			if(!strBDeparet.IsEmpty() &&
				!strBAddr.IsEmpty() &&
				!strBNation.IsEmpty())
			{
				bCenterThreeNoEmpty = true;
			}

			bool bLocalThreeNoEmpty = false;
			if(!strNDeparet.IsEmpty() &&
				!strNAddr.IsEmpty() &&
				!strNNation.IsEmpty())
			{
				bLocalThreeNoEmpty = true;
			}

			// 2014-2-27 - qsc
			if(!strNAddr.IsEmpty())
			{
				strAddress = strNAddr;
			}
			if(!strNDeparet.IsEmpty())
			{
				strDepart = strNDeparet;
			}
			if(!strNNation.IsEmpty())
			{
				strNation = strNNation;
			}
			bUpdateDataBase = TRUE;


			// 2014-7-16 - qsc 中心的三个数据有一个为空，而且本地的三个数据都不为空才去修改
			if(bLocalThreeNoEmpty && !bCenterThreeNoEmpty)
			{
				CModifyMemberMethod ModifyMember;

				ModifyMember.SetSex(Balance.GetSex());
				ModifyMember.SetUsername(Balance.GetUserName());

				ModifyMember.SetIdType(Balance.GetIdType());
				ModifyMember.SetPersonalId(Balance.GetPersonalId());
				ModifyMember.SetOldPersonalId(Balance.GetPersonalId());
				ModifyMember.SetOpenNetBarId(Balance.GetNetBarId());
				ModifyMember.SetPassword(Balance.GetPassword());
				ModifyMember.SetMemberName(Balance.GetMemberName());
				ModifyMember.SetCountryId(_T("CN"));
				ModifyMember.SetAddress(strAddress);
				ModifyMember.SetIdDepart(strDepart);
				ModifyMember.SetJob(strNation);

				ModifyMember.SetTel(strTel);

				theApp.GetCurCashier()->DoModifyMember(ModifyMember);

				if (ModifyMember.GetStatusCode() != 0)
				{
					IBA_LOG(_T("修改用户资料失败:%s"), ModifyMember.GetStatusMessage());
				}
			}
		}
		if(bUpdateDataBase)
		{
			CIBADAL::GetInstance()->UpdateMemberInfo(nMemberID,strNation, strDepart, strAddress, strTel);
			// 2014-5-8 - qsc
			//ExeSfreg(strNation, strDepart, strAddress, strTel);
			
		}	
		// 2014-5-23 - qsc
		SendRegDataToProxy(strNation, strDepart, strAddress, strTel);

	}

	//TotalBalacne在BalanceMethod中计算 - 结算的总账户= 现金额 + 用户赠送金额 + 当前充值的金额 + 上网预留金
	INT nAllMoney = Balance.GetTotalBalacne();

	if (Balance.GetNeedActivation() == 1)
	{
		UINT nType = netBarUser.GetActivationType();

		if (CServletHellper::ActivationMember(nMemberID, nType)) //激活成功
		{
			//先删除，并更新界面，这样可保证显示在最前面
			//CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(Balance.GetMemberId());
			CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMemberDBPostActivation(Balance.GetMemberId());
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
			//CLocalServer::GetInstance()->ActiveMemberList.AddActiveMember(Balance);
			CLocalServer::GetInstance()->ActiveMemberList.AddActiveMemberPostActivation(Balance);
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);

			CIBATTS::GetInstance()->SpeakActivation(nAllMoney, SpeakBirthday(m_NetBarUser));

            IBA_LOG(_T("CRealNameBase::ActivationMember Result is TRUE - for MemberID = %d"),netBarUser.GetMemberId());
			return TRUE;
		}
	}
	else
	{
		IBA_LOG(_T("CRealNameBase::ActivationMember Result is FALSE - Balance.GetNeedActivation is FALSE"));
	}

	CIBATTS::GetInstance()->SpeakBalance(nAllMoney);   //不会运行到此处？

	IBA_LOG(_T("CRealNameBase::ActivationMember Result is FALSE - Balance.GetNeedActivation is FALSE or ActivationMember failed for MemberID = %d"),netBarUser.GetMemberId());
	return FALSE;
}


DWORD RegUserThread(void* pParam)
{
	while(1)
	{
		while(g_listRegUserInfo.empty())
		{
			Sleep(100);
		}


		CAuditInterface auditInterface;
		while(!g_listRegUserInfo.empty())
		{

			CSingleLock lock(&g_csRegUserInfo, TRUE);
			CAuditInterface::RegUserInfo regUserInfo = g_listRegUserInfo.back();
			g_listRegUserInfo.pop_back();
			lock.Unlock();

			//try
			//{
			auditInterface.CustomRegUser(regUserInfo);	
			//}
			//catch (...)
			//{
			//	IBA_LOG(_T("%s 注册异常"), CA2T(regUserInfo.strSerialNum));
			//}
		}


	}
	return 0;
}

// 2014-5-23 - qsc 
void CRealNameBase::SendRegDataToProxy(LPCTSTR szNation, LPCTSTR szDepart, LPCTSTR szAddress, LPCTSTR szTel)
{
	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
	CAuditInterface::RegUserInfo regUserInfo;

	COleDateTime dt;
	dt = COleDateTime::GetCurrentTime();
	CString strDatetime;
	strDatetime = dt.Format(_T("%Y-%m-%d"));

	regUserInfo.netbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	regUserInfo.IDType = Balance.GetIdType();
	regUserInfo.strIDNumber = CT2A(Balance.GetPersonalId());
	regUserInfo.strUserName = CT2A(Balance.GetUserName());
	regUserInfo.sex = Balance.GetSex();
	regUserInfo.strCountryCode = "CN";
	regUserInfo.strAddress = CT2A(szAddress);
	regUserInfo.strTelphone = CT2A(szTel);
	regUserInfo.memberID = Balance.GetMemberId();
	regUserInfo.strBirthDay = CT2A(NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ(Balance.GetPersonalId()));
	regUserInfo.strOpenDay = CT2A(strDatetime);
	regUserInfo.strValidDate = CStringA("2050-01-01");
	regUserInfo.national = NS_REALNAME::CRZXRealName::GetNationCode(szNation);
	regUserInfo.strCertificateOffice = CT2A(szDepart);
	regUserInfo.CardType = Balance.GetUserClass();
	regUserInfo.strSerialNum = CT2A(Balance.GetSerailNum());

	CSingleLock lock(&g_csRegUserInfo);
	g_listRegUserInfo.push_back(regUserInfo);
	lock.Unlock();
	if(WAIT_OBJECT_0 == WaitForSingleObject(g_hRegUserInfo, 0) || NULL == g_hRegUserInfo)
	{	
		// 线程没有运行
		g_hRegUserInfo = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RegUserThread, NULL, 0, 0);
		CloseHandle(g_hRegUserInfo);
	}

}

void CRealNameBase::ExeSfreg(LPCTSTR szNation, LPCTSTR szDepart, LPCTSTR szAddress, LPCTSTR szTel)
{
	// -A证件号码	-B证件类别(文字)   -C姓名		-D性别
	// -E地址		-F电话号码		-G国籍		-H会员号
	// -I证件类别编码 -J民族			-K发证单位	-L卡类型 
	// -M印刷号 -R是否只读
	// 2014-5-8 - qsc
	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
	CString strParam, strIDType, strSex;

	// 性别
	if(!Balance.GetSex())
	{
		strSex.LoadString(IDS_MAN);
	}
	else
	{
		strSex.LoadString(IDS_WOMAN);
	}

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); i++)
	{
		if(Balance.GetIdType() == CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeID())
		{
			strIDType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeName();
			break;
		}
	}
	
	strParam.Format(_T("-A%s -B%s -C%s -D%s -E%s -F%s -G%s -H%d -I%d -J%s -K%s -L%d -M%s -R%d"), 
		Balance.GetPersonalId(), strIDType, Balance.GetUserName(), strSex,
		szAddress, szTel, _T("CN"), Balance.GetMemberId(), 
		Balance.GetIdType(), szNation, szDepart, Balance.GetUserClass(),
		Balance.GetSerailNum(), FALSE);

	IBA_LOG(_T("传递给SFREG的信息:%s"),strParam);
	const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
	const CString strDest	  = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\imgID.jpg");
	
	if(PathFileExists(strSource))		// 拷贝一个做备份，方便查看
	{
		if(!CopyFile(strSource, theApp.GetWorkPath() + _T("\\IBATemp\\ID1.jpg"), FALSE))
		{
			IBA_LOG(_T("拷贝%s失败1!，%d"), strSource, GetLastError());
		}
	}

	if(PathFileExists(strDest))
	{
		if(!DeleteFile(strDest))
		{
			IBA_LOG(_T("删除%s失败!,%d"), strDest,GetLastError());
		}
	}

	if(PathFileExists(strSource))
	{
		if(!CopyFile(strSource, strDest, FALSE))
		{
			IBA_LOG(_T("拷贝%s失败!,%d"), strSource,GetLastError());
		}
	}
	else
	{
		IBA_LOG(_T("%s不存在！"), strSource);
	}

	if(PathFileExists(strSource))		// 拷贝完删除
	{
		if(!DeleteFile(strSource))
		{
			IBA_LOG(_T("删除%s失败!,"), strSource, GetLastError());
		}
	}
	
	CString strSFREGPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\sfreg.exe");
	if(CIBAHelpper::ExePath(strSFREGPath, strParam))
	{
		IBA_LOG0(_T("调用sfreg成功"));
	}
	else
	{
		IBA_LOG0(_T("调用sfreg失败"));
	}
	//}	
}

//
INT CRealNameBase::OnOldUser(CNetBarUser & NetBarUser)
{
	INT nRes = 0;
	IBA_LOG(_T("CRealNameBase::OnOldUser"));

	// QSCADD 2013-7-25 11:10:54
	// 任子行实名加复制照片处理
	if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
	{
		CString strPhotoPath;
		if(CRZXRealName::GetPhotoPath(strPhotoPath, NetBarUser.GetCertificateID()))
		{	
			CString strPath, strNewFile;
			strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
			CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
			strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(NetBarUser.GetNetId()));
			IBA_LOG(_T("获取任子行照片路径%s, 拷贝路径%s"), strPhotoPath, strNewFile);
			if(!CopyFile(strPhotoPath, strNewFile,FALSE))
			{
				IBA_LOG(_T("拷贝任子行照片失败"));
			}
		}	
		else
		{
			IBA_LOG(_T("获取任子行照片失败"));
		}
	}
	
	CActiveMember ActiveMember;

	BOOL bExists =  CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(NetBarUser.GetMemberId(), ActiveMember);

	//激活用户
	if (ActivationMember(NetBarUser))
	{
		nRes = 1;
	}

	// 播放生日祝福语
	BOOL bBirthday = SpeakBirthday(NetBarUser);


	//// 网盾要求的，激活（也即老用户刷卡）也要上传照片-2013-6-8 16:24:00
	//if (CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameWD)
	//{
	//	NS_REALNAME::CWDRealName* pWd = (NS_REALNAME::CWDRealName*)theApp.GetRealName()->m_lpRealName;
	//	if (pWd)
	//	{
	//		IBA_LOG0(_T("上传网盾证件照"));
	//		if (pWd->m_bNeedUpload)
	//		{
	//			if (pWd->m_NetBarUser.GetCertificateType() == 99)
	//			{
	//				if (pWd->m_NetBarUser.GetCertificateID().IsEmpty())
	//				{
	//					pWd->m_NetBarUser.SetCertificateID(NetBarUser.GetCertificateID());
	//				}
	//				if (pWd->m_NetBarUser.GetNetId().IsEmpty())
	//				{
	//					pWd->m_NetBarUser.SetNetId(NetBarUser.GetNetId());
	//				}
	//				if (pWd->m_NetBarUser.GetUserName().IsEmpty())
	//				{
	//					pWd->m_NetBarUser.SetUserName(NetBarUser.GetUserName());
	//				}
	//			}
	//			pWd->Start();
	//			pWd->m_bNeedUpload = FALSE;
	//		}
	//	}
	//	else
	//	{
	//		IBA_LOG0(_T("pointer is NULL"));
	//	}
	//}
	if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 0) //不弹出任何对话框
	{
		return nRes;
	}

	if (AfxGetMainWnd()->IsIconic())//判断当前是最小窗口
	{
		AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);//最大化
	}

	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetForegroundWindow();

	CCurrentNetBarUser::GetInstance()->SetNetId( NetBarUser.GetNetId());

	//弹出充值退款
	if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 1)
	{
		if (CCreditDlg::m_StaticPrehWnd != NULL) //窗口已经弹出来了
		{
			BOOL bRet = SendMessage(CCreditDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)(LPCTSTR)NetBarUser.GetNetId());

			if (!bRet) //表示正在充值
			{
		
			}
			else
			{
				SetForegroundWindow(CCreditDlg::m_StaticPrehWnd);
				SetFocus(CCreditDlg::m_StaticPrehWnd);
			}
		}
		else
		{
			CCreditDlg dlg;
			dlg.SetNetId(NetBarUser.GetNetId(), TRUE);
			dlg.SetFormCard();
			dlg.DoModal();	
		}
	}
	else if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 2) //使用新的充值/结账对话框
	{
		if (AfxGetMainWnd()->IsWindowEnabled())
		{
			//CString strNetId = NetBarUser.GetNetId();

			if (CCurrentNetBarUser::GetInstance()->Balance.IsNetbarMember() == 1 
				&& !bExists) //会员且不在用户视图
			{
				CNDCreditDlg dlg;
				if (bBirthday)
				{
					dlg.m_strBirthday = LOAD_STRING(IDS_BIRTHDAY);
				}
				dlg.SetNetId(NetBarUser.GetNetId());
				dlg.DoModal();
			}
			else
			{
				CNDReturnDlg dlg;
				dlg.SetNetId(NetBarUser.GetNetId());
				dlg.DoModal();
			}
		}
	}
	else
	{
		//...不显示任何对话框
	}

	return nRes;
}

/*************************************************
Function:       // OnOldUserInLocalMode
Description:    // 本地模式下，为已激活用户或未激活会员用户进行弹窗或激活弹窗动作.
Calls:          // DoModal
Table Updated:  // Member
Input:          // NetBaruUser - 提供NetId和CertificateID
Return:         // m_NetBarUser.GetCertificateID为空,返回0
				// 从Member表中找不到对应的用户,返回0
				// 其他返回1
Others:         // 只有本地模式下新激活的会员，才会被设置余额为0，重新创建localActiveMember。原本就激活的，不需要重新创建，只需要设置为localActiveMember
*************************************************/
INT CRealNameBase::OnOldUserInLocalMode(CNetBarUser & NetBarUser)
{
	CIBALog::GetInstance()->Write(_T("CRealNameBase::OnOldUserInLocalMode"));

	// QSCADD 2013-7-25 11:10:54
	// 任子行实名加复制照片处理
	if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
	{
		CString strPhotoPath;
		if(CRZXRealName::GetPhotoPath(strPhotoPath, NetBarUser.GetCertificateID()))
		{	
			CString strPath, strNewFile;
			strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
			CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
			strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(NetBarUser.GetNetId()));
			IBA_LOG(_T("获取任子行照片路径%s, 拷贝路径%s"), strPhotoPath, strNewFile);
			if(!CopyFile(strPhotoPath, strNewFile,FALSE))
			{
				IBA_LOG(_T("拷贝任子行照片失败"));
			}
		}	
		else
		{
			IBA_LOG(_T("获取任子行照片失败"));
		}
	}

	CActiveMember ActiveMemberActivated;

	//已经激活的，动作如下
	BOOL bExists =  CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(NetBarUser.GetMemberId(), ActiveMemberActivated);

	if(bExists)
	{
		if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 0) //不弹出任何对话框
		{
			return 1;
		}

		if (AfxGetMainWnd()->IsIconic())//判断当前是最小窗口
		{
			AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);//最大化
		}

		AfxGetMainWnd()->BringWindowToTop();
		AfxGetMainWnd()->SetForegroundWindow();

		CCurrentNetBarUser::GetInstance()->SetNetId(NetBarUser.GetNetId());

		//弹出充值退款
		if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 1)
		{
			if (CCreditDlg::m_StaticPrehWnd != NULL) //窗口已经弹出来了
			{
				BOOL bRet = SendMessage(CCreditDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)(LPCTSTR)NetBarUser.GetNetId());
				if (!bRet) //表示正在充值
				{
					//DoNothing
				}
				else
				{
					SetForegroundWindow(CCreditDlg::m_StaticPrehWnd);
					SetFocus(CCreditDlg::m_StaticPrehWnd);
				}
			}
			else
			{
				CCreditDlg dlg;
				//dlg.SetLocalActiveMember(ActiveMemberActivated);
				dlg.SetNetId(NetBarUser.GetNetId());
				dlg.DoModal();	
			}
		}
		else if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 2) //使用新的充值/结账对话框
		{
			if (AfxGetMainWnd()->IsWindowEnabled())
			{
				//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值，可以先跳过。
				BOOL bIsNDCredit(FALSE);
				LPTSTR lpszWndCaption = new TCHAR[30];
				memset(lpszWndCaption,0,30);
				HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
				if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
				{
					CString strWndCation(lpszWndCaption);
					strWndCation.Trim();
					bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
				}
				if(bIsNDCredit)  //充值窗口已经弹出
				{
					//DoNothing
				}
				else
				{
					CNDReturnDlg dlg;
					//dlg.SetLocalActiveMember(ActiveMemberActivated);
					dlg.SetNetId(NetBarUser.GetNetId());
					dlg.DoModal();
				}
			}
		}
		else
		{
			//...不显示任何对话框则DoNothing
		}
		return 1;
	}

	//激活用户 - 即添加m_localActiveMember
	
	//激活用户
	IBA_LOG0 (_T ("Temp.CRealNameBase::OnLocalRealName: This is a VIP user. Will Create temp ActiveMember for him."));

	if (m_NetBarUser.GetNetId(). IsEmpty ())
	{
		IBA_LOG0 (_T ("Abnormal.CRealNameBase::OnLocalRealName:m_NetBarUser.NetId is empty! Will return TRUE directly!"));
		return 0 ;
	}

	// 使用CertificateID从member表中取出数据来，初始余额为0，写入localMember, 并要弹出充值界面
	CString sql ;
	UINT uNetBarId = CNetBarConfig::GetInstance()->GetNetBarId();

	sql.Format ( _T("select * from member where  serialNum='%s'and netBarId = %d and (available = 0 or available = 1)"), m_NetBarUser.GetNetId(), uNetBarId); 

	GxxDictionaryPtr memberPtr = CIBADAL:: GetInstance ()->ReadRecordFromDB (sql);

	//重新再检查一遍，确认数据未被篡改
	if (memberPtr == NULL)
	{
		IBA_LOG (_T ("Abnormal.CRealNameBase::OnLocalRealName: No record found in member table.Data has been changed in Member table! Will return TRUE directly!"));
		return 0 ;
	}
	
	CActiveMember ActiveMember;
	//为会员用户创建临时激活对象，用于在本地模式下上机。初始余额设置为0.
	IBA_TRACE ("%s", memberPtr -> describe_d(0, false ).c_str ());
	ActiveMember .SetNetbarID ( memberPtr-> valueForKey ("netBarId")->intValue ());                       //NetbarID
	ActiveMember .SetMemberID ( memberPtr-> valueForKey ("memberId")->intValue ());                       //MemberID
	ActiveMember .SetClassId ( memberPtr-> valueForKey ("classId")->intValue ());                         //ClassID
/*	std ::wstring name = G2XUtility ::CG2XCommonTool :: Utf8_to_WideChar( std ::string ( memberPtr-> valueForKey ("name")->stringValue ()));
	ActiveMember .SetUserName ( CString( name .c_str ()));        */                                        //UserName
	CString strUserName;
	if(CIBADAL::GetInstance()->GetUserNameFromMember(m_NetBarUser.GetNetId(),strUserName))
	{
		ActiveMember.SetUserName(strUserName);
	}
	else
	{
		std ::wstring name = G2XUtility ::CG2XCommonTool :: Utf8_to_WideChar( std ::string ( memberPtr-> valueForKey ("name")->stringValue ()));
		ActiveMember .SetUserName ( CString( name .c_str ()));                                       //UserName
	}
	ActiveMember .SetNetId ( CString( CA2T (memberPtr -> valueForKey("serialNum")-> stringValue ())));       //NetId
	ActiveMember .SetIdType ( memberPtr-> valueForKey ("idType")->intValue ());                           //idType
	ActiveMember .SetPersonalID ( CString( CA2T (memberPtr -> valueForKey("idNumber")-> stringValue ())));   //PersonalID
	ActiveMember .SetSex ( memberPtr-> valueForKey ("sex")->intValue ());                                  //Sex

	ActiveMember .SetAvailavleBalance (0);                                                            //AvailavleBalance
	ActiveMember .SetAdvancePayment (0);                                                              //AdvancePayment
	ActiveMember .SetAmount (0);                                                                       //Amount
	ActiveMember .SetCreditProcess ( ActiveMember. GetAdvancePaymentAsString ());                        //CreditProcess start with AdvancePayment
	ActiveMember .SetCasherID ( CIBAConfig:: GetInstance ()->GetCashRegisterID ());                       //CashierID
	CString strNetId = ActiveMember. GetNetId ();
	strNetId = strNetId . Right(8);
	strNetId .TrimLeft ( _T('0')); // 过滤掉左边 的0
	ActiveMember .SetNetIdAlias ( strNetId);                                                           //AliasNetId
	ActiveMember .SetActivationTime ( COleDateTime:: GetCurrentTime ());                                 //ActivationTime = now
	ActiveMember .SetActive ( TRUE);                                                                   //Active=TRUE
	ActiveMember .SetStatus ( CActiveMember:: EStatus_Waiting );                                         //Status = Waiting待上机
	ActiveMember .SetIsOnLine(FALSE);                                                                          //IsOnline
	//ActiveMember.SetRemark(_T("LocalActivated"));                                                                 //Remark:标记是本地激活的
	//ActiveMember.SetTerminalID(_T("''"));                                                                          //TerminalId是CheckoutInfo表的主键，不能为NULL(activeMember表默认为NULL，所以这里要赋值)

	SendMessage (CMainFrame :: m_hOperationLog, WM_OPERATIONLOG , 1, (LPARAM)&ActiveMember ); //显示刷卡记录

	//添加到激活列表
	CLocalServer::GetInstance()->ActiveMemberList.AddActiveMember(ActiveMember);

	//未赋值成员
	//ActiveMember.SetRefNo();         //引用号
	//ActiveMember.SetRemark();              //Remark
	//ActiveMember.SetIsRegisterBT();  //是否开户包时
	//ActiveMember.SetPayType();       //PayType
	//ActiveMember.SetPCClassID();     //PCClassID
	//ActiveMember.SetCheckInTime();   //CheckInTime - COleDateTime
	//ActiveMember.SetUpdateDataTime(); //UpdateDataTime - COleDateTime
	//ActiveMember.SetNextCheckinTime(); //NextCheckinTime - UINT
	//ActiveMember.SetDynamicMaxAmount(); //累计封顶额
	//ActiveMember.SetIsLocalUser();      //IsLocalUser
	//ActiveMember.SetPassword();         //password
	//ActiveMember.SetPolicyInfo();       //costRate
	//ActiveMember.SetUserClassName();    //UserClassName
	//ActiveMember.SetRoomMemberId();     //包房房主的用户号
	//ActiveMember.SetScanType();         //扫描类型

	if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 0) //不弹出任何对话框
	{
		return 1;
	}

	if (AfxGetMainWnd()->IsIconic())//判断当前是最小窗口
	{
		AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);//最大化
	}

	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetForegroundWindow();

	CCurrentNetBarUser::GetInstance()->SetNetId( NetBarUser.GetNetId());

	if (AfxGetMainWnd()->IsWindowEnabled())
	{
		//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值
		BOOL bIsNDCredit(FALSE);
		LPTSTR lpszWndCaption = new TCHAR[30];
		memset(lpszWndCaption,0,30);
		HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
		if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
		{
			CString strWndCation(lpszWndCaption);
			strWndCation.Trim();
			bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
		}
		if(bIsNDCredit)
		{
			//DoNothing
		}
		else
		{
			CNDCreditDlg dlg;
			//if (bBirthday)
			//{
			//	dlg.m_strBirthday = LOAD_STRING(IDS_BIRTHDAY);
			//}
			//dlg.SetLocalActiveMember(ActiveMember); //设置为LocalActiveMember
			dlg.SetNetId(NetBarUser.GetNetId());
			dlg.DoModal();
		}
	}
	return 1;

	////弹出充值退款
	//if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 1)  
	//{
	//	if (CCreditDlg::m_StaticPrehWnd != NULL) //窗口已经弹出来了
	//	{
	//		BOOL bRet = SendMessage(CCreditDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)(LPCTSTR)NetBarUser.GetNetId());
	//		if (!bRet) //表示正在充值
	//		{
	//			//DoNothing
	//		}
	//		else
	//		{
	//			SetForegroundWindow(CCreditDlg::m_StaticPrehWnd);
	//			SetFocus(CCreditDlg::m_StaticPrehWnd);
	//		}
	//	}
	//	else
	//	{
	//		CCreditDlg dlg;
	//		dlg.SetLocalActiveMember(ActiveMember);
	//		dlg.DoModal();	
	//	}
	//}
	//else if (CIBAConfig::GetInstance()->GetOperationAfterSwipingCard() == 2) //使用新的充值/结账对话框 
	//{
	//	if (AfxGetMainWnd()->IsWindowEnabled())
	//	{
	//		//先判断充值或充值退款窗口未弹出，如果弹出说明正在充值
	//		BOOL bIsNDCredit(FALSE);
	//		LPTSTR lpszWndCaption = new TCHAR[30];
	//		memset(lpszWndCaption,0,30);
	//		HWND hwnd = AfxGetApp()->GetMainWnd()->GetForegroundWindow()->GetSafeHwnd();
	//		if(::GetWindowText(hwnd,lpszWndCaption,30) > 0)
	//		{
	//			CString strWndCation(lpszWndCaption);
	//			strWndCation.Trim();
	//			bIsNDCredit = !strWndCation.CompareNoCase(_T("充值"));
	//		}
	//		if(bIsNDCredit)
	//		{
	//			//DoNothing
	//		}
	//		else
	//		{
	//			CNDCreditDlg dlg;
	//			//if (bBirthday)
	//			//{
	//			//	dlg.m_strBirthday = LOAD_STRING(IDS_BIRTHDAY);
	//			//}
	//			dlg.SetLocalActiveMember(ActiveMember); //设置为LocalActiveMember
	//			dlg.DoModal();
	//		}
	//	}
	//}
	//else
	//{
	//	//...不显示任何对话框
	//}

	//return 1;
}

INT CRealNameBase::OnNewUser(CNetBarUser & NetBarUser)
{
	//if (CRegisterDlg::m_StaticPrehWnd != NULL) //注册窗口已经弹出
	//{
	//	SendMessage(CRegisterDlg::m_StaticPrehWnd, WM_RECEIVECARD, NULL, (LPARAM)&NetBarUser);
	//	return;
	//}

	//2015-0910 liyajun 断网情况下禁止开户
	IBA_LOG(_T("Temp.CRealNameBase::OnNewUser - Start"));
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{	
		MessageBox(NULL,_T("断网情况下禁止开户！"),NULL,MB_OK);
		return 0;
	}


	if (AfxGetMainWnd()->IsIconic())
	{
		AfxGetMainWnd()->ShowWindow(SW_MAXIMIZE);
		AfxGetMainWnd()->BringWindowToTop();
	}
	
	CRegisterDlg dlg;
	dlg.SetUserInfo(NetBarUser);
	if (dlg.DoModal() == IDOK)
	{
		IBA_LOG(_T("Temp.CRealNameBase::OnNewUser - Result = 1"));
		return 1;
	}
	IBA_LOG(_T("Temp.CRealNameBase::OnNewUser - Result = 0"));
	return 0;
}

/*************************************************
Function:       // ReadUserInfo
Description:    // 从注册表 HKEY_CURRENT_USER下RealName\\Base\\NewUser 中读取用户信息保存到NetBarUser中。(注册表中的值在ID2Reader.exe刷卡时候被设置)
Calls:          // GetProfileString,GetProfileInt
Table Accessed: // None
Table Updated: //  None
Input:          // None
Output:         // NetBarUser对象，包含当前用户的所有信息
Return:         // 返回TRUE,除非注册表中 HKEY_CURRENT_USER下RealName\\Base\\NewUser 的IDNo为空，返回FALSE
Others:         // 如果后台设置了允许自动获取账号，NetBarUser.NetId为读取到的账号的后12位。
*************************************************/
BOOL CRealNameBase::ReadUserInfo(CNetBarUser & NetBarUser)
{
	IBA_LOG(_T("CRealNameBase::ReadUserInfo Start.Key = HKEY_CURRENT_USER下RealName\\Base\\NewUser"));
	CString strKey(_T("RealName\\Base\\NewUser"));

	CString strTmp = theApp.GetProfileString(strKey, _T("IDNo")); //从注册表HKEY_CURRENT_USER路径下读取IDNo
	IBA_LOG(_T("Read IDNo:%s"),strTmp);

	strTmp.Trim();

	if (strTmp.IsEmpty())
	{
		IBA_LOG(_T("Abnormal. CRealNameBase::ReadUserInfo: HKEY_CURRENT_USER下RealName\\Base\\NewUser 的IDNo为空"));
		return FALSE;
	}

	NetBarUser.SetNetId(strTmp);                                         //NetId设置为IDNo

	strTmp = theApp.GetProfileString(strKey, _T("Name"));
	strTmp.Trim();
	strTmp.Remove(' ');
	NetBarUser.SetUserName(strTmp);                                     //UserName设置为Name

	strTmp = theApp.GetProfileString(strKey, _T("CertificateID"));      //CertificateID
	NetBarUser.SetCertificateID(strTmp);

	strTmp = theApp.GetProfileString(strKey, _T("Password"));           //Password (一般为空？)
	NetBarUser.SetPassword(strTmp);

	INT nTmp = theApp.GetProfileInt(strKey, _T("CertificateType"), 11); //CertificateType
	NetBarUser.SetCertificateType(nTmp);
	
	// 2013-12-18 18:18:55-qsc
	strTmp = theApp.GetProfileString(strKey, _T("Nation"));//民族       //Nation
	NetBarUser.SetNation(strTmp);

	CString strMan;
	strMan.LoadString(IDS_MAN);
	strTmp = theApp.GetProfileString(strKey, _T("Sex"));                //Sex
	if (strTmp.CompareNoCase(strMan) != 0)
	{
		NetBarUser.SetSex(1);	 //女的
	}
	else
	{
		NetBarUser.SetSex(0);	// 男的
	}
	
	strTmp = theApp.GetProfileString(strKey, _T("Address"));            //地址
	NetBarUser.SetAddr(strTmp);
	strTmp = theApp.GetProfileString(strKey, _T("Department"));         //发证机关
	NetBarUser.SetDepartment(strTmp);

	//strTmp = theApp.GetProfileString(strKey, _T("Signdate"));//签发日期
	//strTmp = theApp.GetProfileString(strKey, _T("OutOfDate"));	 //有效期
	//strTmp = theApp.GetProfileString(strKey, _T("Birthday"));	//出生日期

	NetBarUser.SetDataFrom(1);                                  //数据来自 1 - 刷卡    (其他 2 - 读卡器 0 - 输入)                

	//是否现在删除整个注册表键，如果不删除，则设置SetDeleteKey(TRUE)
	if (NetBarUser.GetDeleteKey())
	{
		theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\Base\\NewUser"));
	}
	else
	{
		NetBarUser.SetDeleteKey(TRUE);
	}
	
	// 8257 (guoxuxing 2013-1-28): 生成证件照图片
	if (CNetBarConfig::GetInstance()->GetEnableID2Reader())
	{
		CString strPath = theApp.GetWorkPath();
		CString strImgPath = strPath + _T("\\IDImages\\");
		if (!CIBAHelpper::MakeSurePathExists(strImgPath))
			IBA_LOG(_T("创建证件照路径失败 %s"), strPath);
		CString strID2ReaderPath = strPath + _T("\\IBABin\\");
		CString strIDPic = strID2ReaderPath + _T("temp.jpg");
		CString strNewPicPath = strImgPath;
		strNewPicPath.Append(NetBarUser.GetCertificateID());
		strNewPicPath.Append(_T(".jpg"));
		if (!PathFileExists(strIDPic))
		{
			IBA_LOG(_T("ID2Reader.exe未生成证件照图片 %s"), strIDPic);
		}
		else
		{
			CopyFile(strIDPic, strNewPicPath, TRUE);

			// 2013-12-18 14:52:37-qsc
			const CString strDest	= theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
			if(!CopyFile(strIDPic, strDest, FALSE))
			{
				IBA_LOG(_T("拷贝%s失败!,%d"), strIDPic, GetLastError());
			}
		}
	}
	
	//中心后台设置：是否自动获取账号，0-不启用，1-截取证件号(一代身份证或二代身份证)的后12位为上网账号
	if(1 == CNetBarConfig::GetInstance()->GetEnableAccount12()) 
	{
		CString strCID = NetBarUser.GetCertificateID();//取证件号

		if(12 < strCID.GetLength() &&                  //如证件号长度>12，以及证件类型为11或12(一代身份证或二代身份证)，截取后12位
			(NetBarUser.GetCertificateType() == 11 ||
			NetBarUser.GetCertificateType() == 12))
		{
			NetBarUser.SetNetId(strCID.Right(12));
		}
		else
		{
			//NetBarUser的NetId直接使用注册表中获取的IDN
		}

	}
	return TRUE;
}

/*************************************************
Function:       // OnLocalRealName
Description:    // 本地模式校验NetId,从Member表查找对应NetId和NebBarId,且可用的账户,并设置m_nCardId和m_nMemberId,设置m_NetBarUser
				// 若校验成功为已激活用户或未激活会员用户进行弹窗或本地激活和弹窗动作..
Calls:          // CheckCardInLocalMode,OnOldUserInLocalMode
Table Accessed: // Member表 - 查询未激活VIP用户的信息，以赋值给为其临时创建的ActiveMember对象
Input:          // nMsgType - 未使用
                // strNetID - 待查询NetID
Return:         // Member表中查找不到对应用户,返回FALSE
				// Member表中查找不到对应用户,或网络忽然恢复正常,返回FALSE
Others:         // 如果未激活，只为VIP会员用户创建临时对象。
*************************************************/
BOOL NS_REALNAME::CRealNameBase::OnLocalRealName(UINT nMsgType, CString strNetID)
{
	if(CheckCardInLocalMode(strNetID)) //本地模式下校验NetId.从Member表查找对应NetId和NebBarId,且可用的账户,并设置m_nCardId和m_nMemberId,设置m_NetBarUser
	{
		if(!m_NetBarUser.GetNetId().IsEmpty())
		{
			OnOldUserInLocalMode(m_NetBarUser);//本地模式下，为已激活用户或未激活会员用户进行弹窗或激活弹窗动作.
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************
Function:       // DoHasLocalRealName
Description:    // 根据消息来源(即被调用的方式)设置m_NebBarUser对象.本地模式时,进行本地模式的激活或弹窗动作.
				// 正常模式:便捷通道 - 无
							刷卡 - 无
					本地模式: 便捷通道 - 取输入
							  刷卡 - 从注册表取用户数据到m_NetBarUser
Calls:          // OnLocalRealName
Table Accessed: // None
Table Updated:  // None
Input:          // nMsgType - 表示消息来源。999-刷卡，0-???，CIBAGlobal::emRealnameZD-智多实名，其他-未知
                // lpData - 便捷通道时的输入信息，dwLen - 便捷通道的输入长度
Output:         // None. 
Return:         // 本地模式进行本地操作后返回TRUE,刷卡读取注册表失败时返回TRUE.其他情况返回FALSE.
Others:         // 返回TRUE时,会直接退出整个消息响应,相当于什么都不做
*************************************************/
BOOL NS_REALNAME::CRealNameBase::DoHasLocalRealName(UINT nMsgType,LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	CString strMsgType = _T("nMsgType=0");
	if(nMsgType != 0)
	{
		strMsgType = (nMsgType == 999)?_T("Sweep card"):_T("Convinient channel");
	}
	IBA_LOG(_T("CRealNameBase::DoHasLocalRealName, Start - %s"),strMsgType);

	m_NetBarUser.SetDeleteKey(FALSE);//不删除HKEY_LOCAL_USER下Software\\Sunlike\\IBA\\RealName\\Base\\NewUser的键及所有字级

	CString strNetId;

	switch(nMsgType)
	{
	//case 99: //便捷通道下-取输入
	//	strNetId = *((CString*)lpData);
	//	break;

	case 999: //实名刷卡-取m_NetBarUser.NetId
		if (!ReadUserInfo(m_NetBarUser))  //从注册表 HKEY_CURRENT_USER下RealName\\Base\\NewUser 中读取用户信息保存到NetBarUser中.若IDNo为空,返回FALSE
		{
			IBA_LOG0(_T("Abnormal.CRealNameBase::DoHasLocalRealName:ReadUserInfo failed after sweeping card. IDNo is empty in registry.Will return directly!"));
			return TRUE; //此处要返回TRUE,既不会进入本地激活流程,又不会进入正常激活流程.相当于什么都不做.
		}
		strNetId = m_NetBarUser.GetNetId();
		break;

	case 0: //为0 -取m_NetBarUser.NetId
		if (!ReadUserInfo(m_NetBarUser))
		{
			IBA_LOG0(_T("Abnormal.CRealNameBase::DoHasLocalRealName:ReadUserInfo failed when  nMsgType=0. IDNo is empty in registry.Will return directly!"));
			return TRUE;
		}
		strNetId = m_NetBarUser.GetNetId();
		break;

	case CIBAGlobal::emRealnameZD:		// 2014-8-28 - qsc 为CIBAGlobal::emRealnameZD，取输入
		//strNetId = (LPCTSTR)lpData;
		strNetId = *((CString*)lpData);
		break;

	default:                            //其他，直接退出
		IBA_ASSERT2(0,"未知位置实名消息类型");
		IBA_LOG(_T("Abnormal.CRealNameBase::DoHasLocalRealName: nMsgType = %d , Will return FALSE directly!"), nMsgType);  //此处并没有与正常网络状态区但消息有效区分开来？
		return TRUE;
		break;
	}

	//只有断网才进行本地模式的操作
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		if(OnLocalRealName(nMsgType, strNetId))    //本地模式校验NetId,从Member表查找对应NetId和NebBarId,且可用的账户,并设置m_nCardId和m_nMemberId,设置m_NetBarUser
												// 若校验成功为已激活用户或未激活会员用户进行弹窗或本地激活和弹窗动作..		
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************
Function:       // OnRealNameMsg
Description:    // 响应刷卡或便捷通道输入(输入开头不为\\,?,+,且长度>=6,且后台设置允许输入激活)，
				// 根据消息来源(即被调用的方式)设置m_NebBarUser对象.本地模式时,进行本地模式的激活或弹窗动作.
				// 网络正常 - 
				// 便捷通道：校验NetId. 并设置m_nCardId和m_nMemberId(到NetBarUser) -》查余额 -》显示SeriaNum在工具栏 -》激活并弹出对话框(根据后台设置)  -- 没有注册新用户这一功能
				// 实名刷卡：实名刷卡 - 注册表读用户信息到NetBarUser，校验NetId.并设置m_nCardId和m_nMemberId -》m_nMemberId=0新用户就开户激活 -》否则旧用户激活并弹出对话框(根据后台设置) 
Calls:          // ...
Input:          // nMstType = 刷卡时为：999
                // lpData = 便捷通道输入时为：输入的数字，刷卡时为：NULL
				// dwLen = 便捷通道输入时为：输入的数字长度，刷卡时为：NULL
Return:         // 本地模式或用户状态为emUserStateLocal返回-1，其他返回0
*************************************************/
INT CRealNameBase::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_TRACE("CRealNameBase::OnRealNameMsg(...)");
	IBA_LOG(_T("CRealNameBase::OnRealNameMsg  Start -刷卡"));

	//根据消息来源(即被调用的方式)设置m_NebBarUser对象.本地模式时,进行本地模式的激活或弹窗动作.
		// 正常模式:便捷通道 - 无
	//	刷卡 - 无
	//	本地模式: 便捷通道 - 取输入
	//	刷卡 - 从注册表取用户数据到m_NetBarUser
	if (DoHasLocalRealName(nMsgType, lpData, dwLen))
	{
		IBA_LOG(_T("CRealNameBase::OnRealNameMsg  Result - TRUE , LocalMode Operate or Register key is empty for sweep card."));
		return 0;
	}

	//正常模式入口 - 网络正常模式下CheckCard,Dobalance,设置m_NetBarUser，OnOldUser都到下面激活成员
	//实名刷卡 - 从注册表读用户信息到NetBarUser，校验NetId. 并设置m_nCardId和m_nMemberId -》m_nMemberId=0新用户就开户激活 -》否则旧用户激活并弹出对话框(根据后台设置) 
	if (999 == nMsgType)  //通用模拟刷卡消息
	{
		ReadUserInfo(m_NetBarUser); 
		m_NetBarUser.GetNetId();
		if (!CheckCard(m_NetBarUser.GetNetId()))   //IDCheckCard失败则直接返回
		{
			IBA_LOG(_T("Temp.CRealNameBase::OnRealNameMsg: CheckCard failed. will return without doing anything"));
			return 0;
		}
		//如果CheckCard成功，m_nMemberId !=0 ，则为已注册用户
		if (m_nMemberId > 0)                       
		{
			m_NetBarUser.SetMemberId(m_nMemberId);
			QueryBalance();   //查余额
			OnOldUser(m_NetBarUser);   //激活，根据后台设置弹出对应对话框(充值退款或其他)
		}
		//如果CheckCard成功，m_nMemberId =0 ， m_nCardId!=0 ，则为未注册用户
		else                                      
		{
			// 2013-12-6 15:31:50 qsc
			if (AutoMatchIDCard(m_NetBarUser))    //如果找到了匹配的身份证号，那就申请换卡，按旧用户算？
			{
				AfxGetMainWnd()->BringWindowToTop();
				AfxGetMainWnd()->SetForegroundWindow();
				OnOldUser(m_NetBarUser);
			}
			else
			{
				OnNewUser(m_NetBarUser);          //注册为新用户
			}
		}
	}

	return 0;
}

//读卡器刷卡
void CRealNameBase::OnCardIn()
{
	IBA_LOG(_T("进入OnCardIn刷卡器刷卡！"));
	char szCardID[20] = {0};

	wReadIdCard(szCardID, TRUE);

	CString strCardID = CA2T(szCardID);//

	if (!CheckCard(strCardID))
	{
		return;
	}

	CNetBarUser NetBarUser;
	NetBarUser.SetNetId(strCardID);
	NetBarUser.SetDataFrom(1);

	if (m_nMemberId > 0) //老卡，先激活
	{
		NetBarUser.SetMemberId(m_nMemberId);
		QueryBalance();
		OnOldUser(NetBarUser);
	}
	else //新卡，计费未注册
	{
		NetBarUser.SetCertificateID(strCardID);
		NetBarUser.SetCertificateType(99);
		NetBarUser.SetUserName(_T("临时用户"));
		NetBarUser.SetSex(0);
	
		// 2013-12-6 15:31:50 qsc
		if (AutoMatchIDCard(NetBarUser))
		{
			AfxGetMainWnd()->BringWindowToTop();
			AfxGetMainWnd()->SetForegroundWindow();

			OnOldUser(NetBarUser);
		}
		else
		{
			OnNewUser(NetBarUser);
		}
	}
}

INT CRealNameBase::InvokeRealName(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	return 0;
}

BOOL CRealNameBase::InitRealName()
{
	if (CIBAConfig::GetInstance()->GetUseOurReader())
	{
		ULONG lRet = wInit(0);//打开串口并初始化

		if (MSG_OK == lRet)
		{
			//设置接收消息窗体，
			//注意，读卡库中有一个线程在不断的进行扫描，并且是与自定义消息宏对应
			wSetRecvWnd(AfxGetMainWnd()->GetSafeHwnd());
			
			m_bOpenReader = TRUE;			
		}
		else
		{
			IBA_LOG(_T("Can't open the reader! wInit Return = %d"), lRet);
		}
	}

	if (!m_DataXReceiver.Create(_T("FC96DB2B-5188-4419-883F-DAB7D412555B")))
	{
		IBA_LOG(_T("Create DataXReceiver failed"));
	}

	m_DataXSender.Create(1, _T("3EA34B24-A23C-4c78-9B68-F66545913299"));

	ExecuteRNS(TRUE);

	return TRUE;
}

BOOL CRealNameBase::UnInitRealName()
{
	if (m_bOpenReader)
	{
		wClose();

		m_bOpenReader = FALSE;
	}
	ExecuteRNS(FALSE);

	
	return TRUE;
}

BOOL CRealNameBase::CheckRealName()
{
	ExecuteProxy();

	return TRUE;
}

BOOL CRealNameBase::IsScanEnabled()
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		return (CIBAConfig::GetInstance()->GetScanId() != 0) &&
			CNetBarConfig::GetInstance()->GetScanEnabled() == 1;
	}

	return FALSE;
}

BOOL CRealNameBase::ScanUserInfo(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	IBA_LOG(_T("读取类型%d"), nType);
	CString strTmpPath = theApp.GetWorkPath() + _T("\\IBATemp\\");
	CIBAHelpper::MakeSurePathExists(strTmpPath);
	
	if(CIBAGlobal::SRT_SCAN2_BACK != nType)
	{
		strTmpPath.Append(_T("ID.jpg"));
	}
	else
	{
		strTmpPath.Append(_T("ID2_BACK.jpg"));
	}
	//IBA_LOG(_T("设置地址%s"), strTmpPath);

	if(PathFileExists(strTmpPath))
	{
		if(!DeleteFile(strTmpPath))
		{
			IBA_LOG(_T("删除%s失败！,%d"), strTmpPath, GetLastError());
		}
	}
	
	//static CIBAScanReco ScanReco;
	CIBAScanReco ScanReco;

	if (ScanReco.Open() == 1)
	{
		INT nRet = 0, nIdType = 11;

		if (CIBAGlobal::SRT_SCAN1 == nType)
		{
			if (ScanReco.ScanImage(IDENTITY_ID1, strTmpPath) == 1)
			{
				nRet = ScanReco.Recognise(IDENTITY_ID1, _T(""));
			}
		}
		else if (CIBAGlobal::SRT_SCAN2 == nType)
		{
			if (ScanReco.ScanImage(IDENTITY_ID2, strTmpPath) == 1)
			{
				nRet = ScanReco.Recognise(IDENTITY_ID2, _T(""));
			}

			nIdType = 12;
		}
		else if(CIBAGlobal::SRT_SCAN2_BACK == nType)
		{
			if (ScanReco.ScanImage(IDENTITY_ID2_BACK, strTmpPath) == 1)
			{
				nRet = ScanReco.Recognise(IDENTITY_ID2_BACK, _T(""));
			}

			nIdType = 12;
		}
		else if (CIBAGlobal::SRT_READ2 == nType)
		{
			nRet = ScanReco.ReadCard(strTmpPath);

			nIdType = 12;
		}
		else if(CIBAGlobal::SRT_SCAN_PASSPORT == nType)
		{
			if (ScanReco.ScanImage(IDENTITY_PASSPORT, strTmpPath) == 1)
			{
				nRet = ScanReco.Recognise(IDENTITY_PASSPORT, _T(""));
			}

			nIdType = 94;
		}
		else
		{
			//其他的失败
			nRet= 0; 
		}

		if (nRet == 1)
		{	
			NetBarUser.SetCertificateID(ScanReco.GetIDNumber());
			NetBarUser.SetCertificateType(nIdType);
			NetBarUser.SetUserName(ScanReco.GetName().Trim());
			NetBarUser.SetNation(ScanReco.GetNation());
			NetBarUser.SetDepartment(ScanReco.GetDepartment());
			NetBarUser.SetSex(ScanReco.GetSex());
			NetBarUser.SetNetId(NetBarUser.GetCertificateID());
			NetBarUser.SetDataFrom(2);
			NetBarUser.SetCanEditUserInfo(TRUE);
			NetBarUser.SetAddr(ScanReco.GetAddress());
			// 2014-3-25 - qsc
			NetBarUser.SetSignDate(ScanReco.GetSignDate());
			// qscadd 2013-10-25
			if(1 == CNetBarConfig::GetInstance()->GetEnableAccount12())
			{
				CString strCID = NetBarUser.GetCertificateID();
				if(12 < strCID.GetLength() && 
					(NetBarUser.GetCertificateType() == 11 ||
					NetBarUser.GetCertificateType() == 12))
				{
					NetBarUser.SetNetId(strCID.Right(12));
				}
			}

			return TRUE;
		}

		ScanReco.Close();
	}
	else
	{
		IBA_LOG0(_T("扫描仪打开失败！"));
	}
	return FALSE;
}

BOOL CRealNameBase::UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	return FALSE;
}

BOOL CRealNameBase::IsRegisterEnabled()
{
	return TRUE;
}

BOOL CRealNameBase::IsCheckInEnabled()
{
	return TRUE;
}

BOOL CRealNameBase::IsMenuEnabled()
{
	return FALSE;
}

BOOL CRealNameBase::IsRealNameExtendMenuEnabled()
{
	return FALSE;
}

INT CRealNameBase::OnExecuteMsg(UINT nExecuteType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	return 0;
}

BOOL CRealNameBase::NeedToCheckRegisterData()
{
	return FALSE; //默认不需要
}

BOOL CRealNameBase::DoScan(UINT nType)
{
	CNetBarUser NetBarUser;

	if (ScanUserInfo(nType, NetBarUser, 0))
	{
		CheckCard(NetBarUser.GetNetId());

		if (m_nMemberId > 0)
		{
			NetBarUser.SetMemberId(m_nMemberId);
			QueryBalance();
			OnOldUser(NetBarUser);
		}
		else
		{
			// 2013-12-6 14:56:54 - qsc
			if (AutoMatchIDCard(NetBarUser))
			{
				AfxGetMainWnd()->BringWindowToTop();
				AfxGetMainWnd()->SetForegroundWindow();

				OnOldUser(NetBarUser);
			}
			else
			{
				OnNewUser(NetBarUser);
			}
			//OnNewUser(NetBarUser);
		}

		return TRUE;
	}

	return FALSE;
}

/*************************************************
Function:       // CheckCard
Description:    // 校验NetId. 并设置m_nCardId和m_nMemberId.
Calls:          // theApp.GetCurCashier()->DoCheckCard(IDCheckCard);
Table Accessed: // None
Table Updated:  // None
Input:          // lpszNetId - 用于向中心申请DoCheckCard
Output:         // Implicitly save result of IDCheckCard to m_nCardId and m_nMemberId.
Return:         // 网络状态为Broken,直接返回FALSE. IDCheckCard成功，返回TRUE.否则返回FALSE.
Others:         // 
*************************************************/
BOOL CRealNameBase::CheckCard(LPCTSTR lpszNetId)
{
	//本地模式不需要CheckCard?
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("Abnormal.CRealNameBase::CheckCard:NetworkStatus suddenly changed to broken"));
		return FALSE;
	}

	// qscadd 2013-10-25 如果后台设置了自动获取账号，则当证件类型为11或12时(一二代身份证)，截取证件号码后12位为strNetId。中心同样可以辨认
	CString strNetId(lpszNetId);
	if(1 == CNetBarConfig::GetInstance()->GetEnableAccount12())
	{
		CString strCID = m_NetBarUser.GetCertificateID();
		if(12 < strCID.GetLength() && 
			(m_NetBarUser.GetCertificateType() == 11 ||
			m_NetBarUser.GetCertificateType() == 12))
		{
			m_NetBarUser.SetNetId(strCID.Right(12));
			strNetId = strCID.Right(12);
		}
	}

	//向中心CheckCard，将返回的CardId和MemberId保存到m_nCardId和m_nMemberId中
	CIDCheckCardMethod IDCheckCard;
	
	IDCheckCard.SetSerialNum(strNetId);
	IDCheckCard.SetSerialNumType(1);

	theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

	if (IDCheckCard.GetStatusCode() == 0)
	{
		m_nCardId = IDCheckCard.GetCardId();		
		m_nMemberId = IDCheckCard.GetMemberId();
		return TRUE;
	}
	//如卡号不存在或做废卡，则m_nCardId及m_nMemberId设置为0.返回FALSE
	else if (IDCheckCard.GetStatusCode() == 41) //卡号不存在
	{
		IBA_LOG(_T("Abnormal.CRealNameBase::CheckCard: IDCheckCard - CardId not exist"));
	}
	else
	{
		if (IDCheckCard.GetStatusCode() != -1)
		{
			//注意,作废卡没有用户号
			theApp.IBAMsgBox(IDCheckCard.GetStatusMessage());
		}	
		else
		{
			IBA_LOG(_T("Abnormal.CRealNameBase::CheckCard: IDCheckCard - Failed, return value(-1)"));
		}
	}

	m_nCardId = 0;
	m_nMemberId = 0;

	return FALSE;
}

/*************************************************
Function:       // CheckCardInLocalMode
Description:    // 本地模式下校验NetId.从Member表查找对应NetId和NebBarId,且可用的账户,并设置m_nCardId和m_nMemberId,设置m_NetBarUser
Calls:          // None
Table Accessed: // Member
Table Updated:  // None
Input:          // lpszNetId - 用于从Member表中查询记录
Output:         // Implicitly save result to m_nCardId and m_nMemberId.
Return:         // 正常模式返回FALSE. Member表中查询不到对应记录,返回FALSE.
Others:         // 
*************************************************/
BOOL CRealNameBase::CheckCardInLocalMode(LPCTSTR lpszNetId)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
	{
		return FALSE;
	}

	//从Member表中校验NetId,未注册账户或临时账号均不允许注册
	CString sql ;
	UINT uNetBarId = CNetBarConfig::GetInstance()->GetNetBarId();
	sql .Format ( _T("select * from member where serialNum='%s' and netBarId = %d and (available = 0 or available = 1)"), lpszNetId,uNetBarId);

	IBA_LOG(_T("Temp.CRealNameBase::CheckCardInLocalMode: Find userinfomation with NebBarId = %d and serialNum = %s and (available = 0 or available = 1)"),uNetBarId,lpszNetId);

	GxxDictionaryPtr memberPtr = CIBADAL:: GetInstance ()->ReadRecordFromDB ( sql);
	if (memberPtr == NULL)
	{
		m_nCardId = 0;
		m_nMemberId = 0;
		return FALSE ;
	}

	if (!CNetBarConfig :: GetInstance()-> GetIsMember ( memberPtr ->intValueForKey ("classId")) )
	{
		m_nCardId = 0;
		m_nMemberId = 0;
		return FALSE ;
	}

	m_nCardId = memberPtr->intValueForKey("cardId");
	m_nMemberId = memberPtr->intValueForKey("memberId");

	m_NetBarUser.SetMemberId(m_nMemberId);

	CString strTmp;
	m_NetBarUser.SetNetId(StringValueForKey("serialNum",memberPtr));               //NetId设置为idNumber
	m_NetBarUser.SetCertificateID(StringValueForKey("idNumber",memberPtr));     //CertificatedID
	
	strTmp = StringValueForKey("name",memberPtr);                                  //UserName设置为Name
	strTmp.Trim();
	strTmp.Remove(' ');
	m_NetBarUser.SetUserName(strTmp);

	m_NetBarUser.SetPassword(_T(""));                                             //Password (一般为空？)
	m_NetBarUser.SetCertificateType(memberPtr->intValueForKey("idType"));         //CertificateType
	m_NetBarUser.SetNation(StringValueForKey("job",memberPtr));                  //民族       //Nation

	strTmp.Format(_T("%s"),StringValueForKey("sex",memberPtr));                  //Sex
	m_NetBarUser.SetSex(_ttoi(strTmp));
	m_NetBarUser.SetAddr(StringValueForKey("address",memberPtr));               //地址
	m_NetBarUser.SetDepartment(StringValueForKey("department",memberPtr));       //发证机关
	m_NetBarUser.SetDataFrom(0);                                  //数据来自 1 - 刷卡    (其他 2 - 读卡器 0 - 输入) 

	////strTmp = theApp.GetProfileString(strKey, _T("Signdate"));//签发日期
	////strTmp = theApp.GetProfileString(strKey, _T("OutOfDate"));	 //有效期
	////strTmp = theApp.GetProfileString(strKey, _T("Birthday"));	//出生日期             

	return TRUE;
}
/*************************************************
Function:       // QueryBalance
Description:    // 使用m_nMemberId和m_nCardId向中心查余额，使用返回的SerialNum显示在工具栏中。
Calls:          // theApp.GetCurCashier()->DoBalance
Input:          // Implicitly use m_nMemberId,m_nCardId to DoBalance
Return:         // DoBalance成功，返回TRUE. 否则返回FALSE.
*************************************************/
BOOL CRealNameBase::QueryBalance()
{
	UNIT_AUTO_TRACE("CRealNameBase::QueryBalance()--");

	CCurrentNetBarUser::GetInstance()->Balance.SetMemberId(m_nMemberId);
	CCurrentNetBarUser::GetInstance()->Balance.SetCardId(m_nCardId);
	CCurrentNetBarUser::GetInstance()->Balance.SetRound(0);//取整

	theApp.GetCurCashier()->DoBalance(CCurrentNetBarUser::GetInstance()->Balance);

	if (CCurrentNetBarUser::GetInstance()->Balance.GetStatusCode() == 0)
	{
		//theApp.UpdateToolbarLastID( CCurrentNetBarUser::GetInstance()->Balance.GetSerailNum()); //Temporarily comment out

		return TRUE;
	}

	return FALSE;
}

BOOL CRealNameBase::AutoModifyMemberInfo()
{
	if (CNetBarConfig::GetInstance()->GetNoModifyMember() == 0)
	{
		return FALSE;
	}

	if (m_NetBarUser.GetUserName() != CCurrentNetBarUser::GetInstance()->Balance.GetUserName() || //姓名
		m_NetBarUser.GetCertificateID() != CCurrentNetBarUser::GetInstance()->Balance.GetPersonalId() || //证件号
		m_NetBarUser.GetCertificateType() != CCurrentNetBarUser::GetInstance()->Balance.GetIdType()) //证件类型
	{
		CCheckMemberDlg CheckMemberDlg;
		CheckMemberDlg.SetCardID(m_NetBarUser.GetNetId());
		CheckMemberDlg.SetRealName(m_NetBarUser);
		CheckMemberDlg.SetServlet(CCurrentNetBarUser::GetInstance()->Balance);
		CheckMemberDlg.DoModal();

		return TRUE;
	}

	return FALSE;
}

/*************************************************
Function:       // AutoMatchIDCard  ？？？？？？？？？？？？
Description:    // 检查后台设置是否为自动匹配身份证号，检查账号长度是否符合身份证长度，不符合则退出FALSE
				// 向中心请求查找身份证号对应的卡号(CardId)，NetBarUser.NetId = 中心返回的账号(NetId)，查找不到则返回FALSE
				// CheckCard,校验上面返回的账号，如果nMemberId>0说明曾经已经存在对应的会员，那么要申请换卡 
				// CCardReplaceMethod向中心申请换卡，NetBarUser.NetId = 中心返回的新卡号
				// 将该账号保存到NetBarUser中，重新校验卡号和查余额
Calls:          // ...
Table Accessed: // None
Table Updated:  // None
Input:          // NetBarUser - 提供账号信息
Output:         // NetBarUser - 将从中心转换成的账号再保存到NetBarUser
Return:         // 所有都成功返回TRUE，否则返回FASLE
Others:         // 由于中心逻辑未得到，具体功能仍不确认！
*************************************************/
BOOL CRealNameBase::AutoMatchIDCard(CNetBarUser & NetBarUser)
{   
	//后台设置为不自动匹配身份证号，直接返回FALSE
	if (CNetBarConfig::GetInstance()->GetAutoMatchIDCard() == 0) return FALSE;

	//身份证号码长度<15，直接返回FALSE
	if (NetBarUser.GetNetId().GetLength() < 15) return FALSE;

	//用户查询接口 - 通过PersonalId看是否能查询到对应用户
	CMemberInfoMethod MemberInfo;
	MemberInfo.SetPersonalId(NetBarUser.GetNetId());
	MemberInfo.SetAvailable(1);

	theApp.GetCurCashier()->DoMemberInfo(MemberInfo);
	if (MemberInfo.GetStatusCode() != 0) return FALSE;   //中心查找失败，直接返回FALSE
	if (MemberInfo.GetMemberCount() == 0) return FALSE;  //中心找不到，直接返回FALSE


	CheckCard(MemberInfo.GetSerailNum(0)); //向中心校验卡号，从NetId得到CardId和MemberId
	if (m_nMemberId > 0)
	{
		CString strNewNetId = NetBarUser.GetNetId();//保存上网帐号
		NetBarUser.SetMemberId(m_nMemberId);
		NetBarUser.SetNetId(MemberInfo.GetSerailNum(0));  //重新设置NetBarUser.NetId = 中心返回的卡序列号

		if (QueryBalance())  //向中心查询余额
		{
			//换卡接口
			if (ChangeNetIdToPersonalId(strNewNetId)) 
			{
				NetBarUser.SetNetId(strNewNetId);//重新设置NetBarUser.NetId = 换卡后的卡序列号

				//TODO:再查一次用户信息
				CheckCard(strNewNetId);
				QueryBalance();
			}
			return TRUE;
		}
	}
	return FALSE;
}

//把二代证转换为上网号
/*************************************************
Function:       // ChangeNetIdToPersonalId
Description:    // 
Calls:          // 
Table Accessed: // 
Table Updated: // 
Input:          // 
                // 
Output:         // 
Return:         // 
Others:         // 
*************************************************/
BOOL CRealNameBase::ChangeNetIdToPersonalId(CString strNewNetId)
{	
	//功能没启动 - gintautomatchidcard =2才算启动
	if (CNetBarConfig::GetInstance()->GetAutoMatchIDCard() != 2)
	{
		return FALSE;
	}

	//身份证验证
	CIdentityNumber IdentityNumber(strNewNetId);

	if (!IdentityNumber.IsValid())
	{
		CIBALog::GetInstance()->Write(_T("PersonalId is not valid!"));
		return FALSE;
	}

	//校验卡信息，通过输入参数作为卡印刷号向中心取得CardId
	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strNewNetId);
	theApp.GetCurCashier()->DoCheckCard(checkCard);

	if (checkCard.GetStatusCode() == 0)
	{
		//修改账号
		CCardReplaceMethod CardReplace;
		CardReplace.SetPersonalId(CCurrentNetBarUser::GetInstance()->Balance.GetPersonalId());//证件号码  ？？Balance有值？？
		CardReplace.SetMemberName(CCurrentNetBarUser::GetInstance()->Balance.GetMemberName());
		CardReplace.SetOpenNetbarId(CCurrentNetBarUser::GetInstance()->Balance.GetOpenNetbarId());
		CardReplace.SetCardId(checkCard.GetCardId());	//使用生成的CardId
		CardReplace.SetPassword(_T("PASSWORDPASSWORD"));//不验证密码

		theApp.GetCurCashier()->DoCardReplace(CardReplace);

		if (CardReplace.GetStatusCode() == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRealNameBase::ExecuteProxy()
{
	BOOL bRes = TRUE;

	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("39F59AC6-0F51-4939-B91A-80012AC93AB6"));

	if (hMutex != NULL)
	{
		CloseHandle(hMutex);
	}
	else
	{
		CString strProxy(theApp.GetWorkPath());
		strProxy.Append(_T("\\IBABin\\IReal.exe"));
		
		CL2Process SunlikeAuditProxy(strProxy);
		if (!SunlikeAuditProxy.Execute())
		{
			bRes = FALSE;
		}
	}

	// 2011/11/16-8210-gxx: 

	hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("_SUNLIKEAUDITPROXY_MUTEX_"));

	if (hMutex != NULL)
	{
		CloseHandle(hMutex);
	}
	else
	{
		CString strProxy(theApp.GetWorkPath());
		strProxy.Append(_T("\\IBABin\\SunlikeAuditProxy.exe"));

		CL2Process SunlikeAuditProxy(strProxy);
		if (!SunlikeAuditProxy.Execute())
		{
			bRes = FALSE;
		}

		// 2015-4-9
		strProxy = theApp.GetWorkPath();
		strProxy.Append(_T("\\IBABin\\rzxsurename.exe"));

		CL2Process RzxSureName(strProxy);
		if (!RzxSureName.Execute())
		{
			bRes = FALSE;
		}
	}

	//

	return bRes;
}

BOOL CRealNameBase::RobFocusAfterSwipingCard()
{
	return FALSE;
}

void CRealNameBase::OnCashierCheckIn()
{
	m_DataXSender.PostData(DataX_DDN2RN_CASHIERCHECKIN, 0, 0);
}

INT CRealNameBase::OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
								 LPVOID lpBuf, size_t cbLen)
{
	DataX_UserInfo dxUserInfo = {0};

	if (DataX_RN2DDN_CARDINFO == nMsg)
	{
		memcpy(&dxUserInfo, lpBuf, sizeof DataX_UserInfo);
		
		return OnCardInfo(dxUserInfo);
	}
	else if (DataX_RN2DDN_FORCECHECKOUT == nMsg)
	{
		memcpy(&dxUserInfo, lpBuf, sizeof DataX_UserInfo);

		return OnForceCheckOut(dxUserInfo);
	}
	else if (DataX_RN2DDN_ADDMEMBER == nMsg)
	{
		memcpy(&dxUserInfo, lpBuf, sizeof DataX_UserInfo);

		return OnAddMember(dxUserInfo);
	}

	return 0;
}

BOOL CRealNameBase::OnCardInfo(DataX_UserInfo& dxUserInfo)
{
	return FALSE;
}

BOOL CRealNameBase::OnForceCheckOut(DataX_UserInfo& dxUserInfo)
{
	return FALSE;
}

BOOL CRealNameBase::OnAddMember(DataX_UserInfo& dxUserInfo)
{
	return FALSE;
}

BOOL CRealNameBase::SpeakBirthday(CNetBarUser & NetBarUser)
{
	CIdentityNumber dn(NetBarUser.GetCertificateID());
	if (dn.IsValid() && dn.IsBirthDay())
	{
		/*CString strSoundFile(theApp.GetWorkPath() + _T("\\IBAConfig\\Shengri.wav"));

		if (PathFileExists(strSoundFile))
		{
			sndPlaySound(strSoundFile, SND_ASYNC);
		}	*/

		return TRUE;
	}
	return FALSE;
}

void CRealNameBase::ExecuteRNS(BOOL bInit) 
{
	CString strRNS, strCMD;
	strRNS = theApp.GetWorkPath() + _T("\\IBABin\\RNS.EXE");

	if (PathFileExists(strRNS))
	{
		strCMD.Format(_T("-DID%d -RNT%d"), CNetBarConfig::GetInstance()->GetDomainId(),
			CNetBarConfig::GetInstance()->GetNetBarId());

		strCMD.AppendChar(' ');
		strCMD.Append(bInit ? _T("-I") : _T("-U"));

		CL2Process RNS(strRNS);
		RNS.Execute(strCMD);
	}
}

//******************************************************************************

CNoneRealName::CNoneRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CNoneRealName"));
}

CNoneRealName::~CNoneRealName(void)
{

}

BOOL CNoneRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	return TRUE;
}

BOOL CNoneRealName::CheckRealName()
{
	return TRUE;
}

BOOL CNoneRealName::IsScanEnabled()
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		return (CIBAConfig::GetInstance()->GetScanId() != 0);
	}

	return FALSE;
}

