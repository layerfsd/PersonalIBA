#include "stdafx.h"
#include "..\IBA.h"
#include "..\Cashier.h"
#include "..\..\GCardID_RZX\RealName.h"
#include "..\ServletHellper.h"
#include ".\rzxrealname.h"
#include "..\CurrentNetBarUser.h"

using namespace NS_REALNAME;

CRZXRealName::CRZXRealName(void)
{
	CIBALog::GetInstance()->Write(_T("CRZXRealName"));

	m_hIdLogUpload = NULL;
	m_pfnRzxCheckOut = NULL;
	
	//m_hRzxCard = NULL;
	//m_pfnCheckCardActive = NULL;

	Load();
}

CRZXRealName::~CRZXRealName(void)
{
	UnLoad();
}

BOOL CRZXRealName::InitRealName()
{
	CRealNameBase::InitRealName();

	ExecuteProxy();

	return TRUE;
}

BOOL CRZXRealName::UnInitRealName()
{
	CRealNameBase::UnInitRealName();

	//{ 2011/05/13-gxx: 退出时，结束掉 任子行 实名的进程
	CL2Process rxzProcess;	rxzProcess.Attach(_T("rzxsurename.exe"));	rxzProcess.Terminate();
	//}
	
	
	return TRUE;
}

void CRZXRealName::Load()
{
	//CIBALog::GetInstance()->Write(_T("Begin loading rzx library!"));

	//m_hIdLogUpload = LoadLibrary(_T("idlogupload.dll"));

	//if (m_hIdLogUpload == NULL)
	//{	
	//	CIBALog::GetInstance()->WriteFormat(_T("LoadLibrary idlogupload.dll Error = %d"), GetLastError());
	//	AfxMessageBox(_T("加载任子行动态库(idlogupload.dll)失败！"));
	//}
	//else
	//{
	//	m_pfnRzxCheckOut = (pfnRzx2008RzxCheckOut)::GetProcAddress(m_hIdLogUpload, "RzxCheckOut");
	//}

	//CIBALog::GetInstance()->Write(_T("Load idlogupload.dll completed!"));

}

void CRZXRealName::UnLoad()
{
	if (m_hIdLogUpload != NULL)
	{	
		FreeLibrary(m_hIdLogUpload);
		m_hIdLogUpload = NULL;
	}
}

INT CRZXRealName::OnRealNameMsg(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	UNIT_AUTO_TRACE("CRZXRealName::OnRealNameMsg(UINT nMsgType=%d, LPVOID lpData=%x , DWORD dwLen=%d)"
		,nMsgType,lpData,dwLen);

	CRealNameBase::OnRealNameMsg(nMsgType, lpData, dwLen);

	if (0 == nMsgType)
	{
		ReplyMessage(0);

		CString strKey(_T("RealName\\RZX\\IDUser"));

		CString strIdCardNumber = theApp.GetProfileString(strKey, _T("IDNo"));

		if (strIdCardNumber.IsEmpty())
		{
			return 0;
		}

		UINT nValidate = theApp.GetProfileInt(strKey, _T("Validate"), 0);

		if (nValidate == 0 || nValidate == 1)
		{
			//
		}
		else
		{
			CIBALog::GetInstance()->WriteFormat(_T("RZX IDCard Validate = %d"), nValidate);

			return 0;
		}

		m_NetBarUser.SetNetId(strIdCardNumber);
		CString strTmp = theApp.GetProfileString(strKey, _T("Name"));
		m_NetBarUser.SetUserName(strTmp);
		strTmp = theApp.GetProfileString(strKey, _T("CertificateID"));
		m_NetBarUser.SetCertificateID(strTmp);
		strTmp = theApp.GetProfileString(strKey, _T("CertificateType"));
		m_NetBarUser.SetCertificateType(_ttoi(strTmp));
		strTmp = theApp.GetProfileString(strKey, _T("Password"));
		m_NetBarUser.SetPassword(strTmp);
		m_NetBarUser.SetDataFrom(1);

		//卡类型；1 - 普通卡、计费， 3 - 临时卡、计费
		m_NetBarUser.SetIsMember(1 == theApp.GetProfileInt(strKey, _T("IDCardType"), 3));

		// 2013/5/3-guoxuxing
		CNetBarConfig::GetInstance()->m_bRZXCardMember = m_NetBarUser.GetIsMember();

		if (!CheckCard(m_NetBarUser.GetNetId()))
		{
			return 0;
		}

		if (m_nMemberId > 0) //老卡，先激活
		{
			m_NetBarUser.SetMemberId(m_nMemberId);

			QueryBalance();

			//密码改为实名密码
			AutoChangeRealNamePwd(m_NetBarUser);

			if (AutoModifyMemberInfo())
			{
				QueryBalance();
			}


			//AfxGetMainWnd()->BringWindowToTop();
			//AfxGetMainWnd()->SetForegroundWindow();

			OnOldUser(m_NetBarUser);
		}
		else //新卡，计费未注册
		{
			IBA_LOG (_T("判断是否有必要把上网账号转换成证件号码"));
			if (AutoMatchIDCard(m_NetBarUser))
			{
				IBA_LOG (_T("需要转换"));
				AfxGetMainWnd()->BringWindowToTop();
				AfxGetMainWnd()->SetForegroundWindow();

				OnOldUser(m_NetBarUser);
			}
			else
			{
				IBA_LOG (_T("不需要转换"));
				OnNewUser(m_NetBarUser);
			}
		}
	}

	//theApp.DelRegTree(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\RZX\\IDUser"));

	return 0;
}

//实名控制消息调用
INT CRZXRealName::InvokeRealName(UINT nMsgType, LPVOID lpData /*= NULL*/, DWORD dwLen /*= 0*/)
{
	if (!theApp.GetCurCashier()->IsOnDuty())//收银员不在线，及下班，则不退
	{
		return FALSE;
	}

	CIBALog::GetInstance()->WriteFormat(_T("InvokeRealName: nMsgType = %d"), nMsgType);//消息写日志

	switch (nMsgType)
	{
	case 1:
		return SendMsgToRZX(IDCARD_MANAGE_ADD);//开户

	case 2:
		return SendMsgToRZX(IDCARD_MANAGE_LOSD);//

	case 3:
		return SendMsgToRZX(IDCARD_MANAGE_MODIFY);//修改

	case 4:
		return SendMsgToRZX(IDCARD_MANAGE_MEMBER);//

	case 100:
		return CheckOutTempUser((LPCTSTR)lpData);//实名退临时卡

	}

	return FALSE;	
}

BOOL CRZXRealName::UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption)
{
	return FALSE;
}

BOOL CRZXRealName::IsMenuEnabled()
{
	return TRUE;
}

BOOL CRZXRealName::IsRealNameExtendMenuEnabled()
{
	return TRUE;
}

//实名卡退款登出
//INT CRZXRealName::CheckOutTempUser(LPCTSTR lpCardNum)
//{
//	INT nRet = 0;
//
//	if (m_pfnRzxCheckOut != NULL)
//	{	
//		CString strTmp(lpCardNum);
//
//		if (strTmp.GetLength() > 10) //任子行只支持10位
//		{
//			strTmp = strTmp.Right(10);
//		}
//
//		CStringA strCardNum = CT2CA(strTmp);
//		nRet = m_pfnRzxCheckOut(strCardNum);
//
//		CIBALog::GetInstance()->WriteFormat(_T("m_pfnRzxCheckOut: CardNum = %s, Ret = %d"), strTmp, nRet);
//
//		//返回0成功，1失败
//		if (1 == nRet)
//		{
//			//AfxMessageBox(_T("任子行注销卡资料失败！"));
//			CIBALog::GetInstance()->Write(_T("任子行注销卡资料失败！"));
//		}
//	}
//	else
//	{		
//		CIBALog::GetInstance()->WriteFormat(_T("Can't get address 0f RzxCheckOut!"));
//	}
//	
//	return nRet;
//} 

INT CRZXRealName::CheckOutTempUser(LPCTSTR lpCardNum)
{
	#define UM_ID_TEMPCARD_RZX  WM_USER + 1010

	CString strTmp(lpCardNum);

	if (strTmp.GetLength() > 10) //任子行只支持10位
	{
		strTmp = strTmp.Right(10);
	}

	CStringA strCardNum = CT2CA(strTmp);

	COPYDATASTRUCT cds = {0};

	cds.dwData = UM_ID_TEMPCARD_RZX;
	cds.cbData = strCardNum.GetLength();
	cds.lpData = (PVOID)strCardNum.GetString();

	HWND hSureName = ::FindWindow(NULL, _T("任子行实名登记管理系统"));

	if (hSureName == NULL)
	{
		//AfxMessageBox(_T("找不到任子行实名登记管理系统！"));

		CIBALog::GetInstance()->Write(_T("找不到任子行实名登记管理系统！"));

		return FALSE;
	}

	::SendMessage(hSureName, WM_COPYDATA, (WPARAM)hSureName, (LPARAM)&cds);
	
	return TRUE;
} 


BOOL CRZXRealName::SendMsgToRZX(UINT nMsgType)
{
	HWND hSureName = ::FindWindow(NULL, _T("任子行实名登记管理系统"));

	if (hSureName == NULL)
	{
		AfxMessageBox(_T("找不到任子行实名登记管理系统！"));

		return FALSE;
	}

	BOOL bSend = ::PostMessage(hSureName, WM_USER_IDCARD_MANAGE, nMsgType, NULL);

	return bSend;
} 


BOOL CRZXRealName::RobFocusAfterSwipingCard()
{
	return TRUE;
}

//检查中心和实名的密码是否相同
BOOL CRZXRealName::AutoChangeRealNamePwd(CNetBarUser& NetBarUser)
{	
	if (CNetBarConfig::GetInstance()->GetRealNamePassword() < 2)	//小于2，不处理
	{
		return TRUE;
	}

	//任子行密码为空,不处理
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

BOOL NS_REALNAME::CRZXRealName::GetPhotoPath( CString& strFile, const CString& strID )
{
	CRegKey RegKey;
	ULONG uLen = 256;

	if(ERROR_SUCCESS == RegKey.Open(HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\RzxCorp\\SureName")))
	{
		RegKey.QueryValue( strFile.GetBuffer(256), _T("ExePath"), &uLen);
		strFile.ReleaseBuffer();
		if(!strFile.IsEmpty())
		{
			strFile += _T("\\temp\\");
			strFile += strID;
			strFile += _T(".jpg");
			return TRUE;
		}	
	}
	return FALSE;
}


CString NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ( const CString& sfzh )
{
	CString strBirthday;
	if (sfzh.GetLength() == 15) 
	{
		CString year, month, day;
		year = sfzh.Mid(6, 2);
		month = sfzh.Mid(8, 2);
		day = sfzh.Mid(10, 2);

		if (_ttoi(year) > 50) { 
			year = "19" + year;
		}
		else
		{
			year = "20" + year;
		}

		strBirthday = year+"-" + month+ "-" +day;
	}	
	else if (sfzh.GetLength() == 18) 
	{
		CString year, month, day;
		year = sfzh.Mid(6, 4);
		month = sfzh.Mid(10, 2);
		day = sfzh.Mid(12, 2);
		strBirthday = year+"-" + month+ "-" +day;
	}
	return strBirthday;
}

int NS_REALNAME::CRZXRealName::GetNationCode(const CString& strNation)
{
	if(strNation == _T("汉")) return 1;
	else if(strNation == _T("蒙古")) return 2;
	else if(strNation == _T("回")) return 3;
	else if(strNation == _T("藏")) return 4;
	else if(strNation == _T("维吾尔")) return 5;
	else if(strNation == _T("苗")) return 6;
	else if(strNation == _T("彝")) return 7;
	else if(strNation == _T("壮")) return 8;
	else if(strNation == _T("布依")) return 9;
	else if(strNation == _T("朝鲜")) return 10;
	else if(strNation == _T("满")) return 11;
	else if(strNation == _T("侗")) return 12;
	else if(strNation == _T("瑶")) return 13;
	else if(strNation == _T("白")) return 14;
	else if(strNation == _T("土家")) return 15;
	else if(strNation == _T("哈尼")) return 16;
	else if(strNation == _T("哈萨克")) return 17;
	else if(strNation == _T("傣")) return 18;
	else if(strNation == _T("黎")) return 19;
	else if(strNation == _T("傈僳")) return 20;
	else if(strNation == _T("佤")) return 21;
	else if(strNation == _T("畲")) return 22;
	else if(strNation == _T("高山")) return 23;
	else if(strNation == _T("拉祜")) return 24;
	else if(strNation == _T("水")) return 25;
	else if(strNation == _T("东乡")) return 26;
	else if(strNation == _T("纳西")) return 27;
	else if(strNation == _T("景颇")) return 28;
	else if(strNation == _T("柯尔克孜")) return 29;
	else if(strNation == _T("土")) return 30;
	else if(strNation == _T("达斡尔")) return 31;
	else if(strNation == _T("仫佬")) return 32;
	else if(strNation == _T("羌")) return 33;
	else if(strNation == _T("布朗")) return 34;
	else if(strNation == _T("撒拉")) return 35;
	else if(strNation == _T("毛南")) return 36;
	else if(strNation == _T("仡佬")) return 37;
	else if(strNation == _T("锡伯")) return 38;
	else if(strNation == _T("阿昌")) return 39;
	else if(strNation == _T("普米")) return 40;
	else if(strNation == _T("塔吉克")) return 41;
	else if(strNation == _T("怒")) return 42;
	else if(strNation == _T("乌孜别克")) return 43;
	else if(strNation == _T("俄罗斯")) return 44;
	else if(strNation == _T("鄂温克")) return 45;
	else if(strNation == _T("德昂")) return 46;
	else if(strNation == _T("保安")) return 47;
	else if(strNation == _T("裕固")) return 48;
	else if(strNation == _T("京")) return 49;
	else if(strNation == _T("塔塔尔")) return 50;
	else if(strNation == _T("独龙")) return 51;
	else if(strNation == _T("鄂伦春")) return 52;
	else if(strNation == _T("赫哲")) return 53;
	else if(strNation == _T("门巴")) return 54;
	else if(strNation == _T("珞巴")) return 55;
	else if(strNation == _T("基诺")) return 56;

	return 99;
}

BOOL NS_REALNAME::CRZXRealName::AutoMatchIDCard( CNetBarUser & NetBarUser )
{
	//普通模式
	if (CNetBarConfig::GetInstance()->GetAutoMatchIDCard() == 0) return FALSE;

	CString strIdNumber = NetBarUser.GetCertificateID();
	//非身份证
	if (strIdNumber.GetLength() < 15) return FALSE;

	//查找身份证对应的账号
	CMemberInfoMethod MemberInfo;

	MemberInfo.SetPersonalId(strIdNumber);
	MemberInfo.SetAvailable(1);

	theApp.GetCurCashier()->DoMemberInfo(MemberInfo);

	if (MemberInfo.GetStatusCode() != 0) return FALSE;

	if (MemberInfo.GetMemberCount() == 0) return FALSE;

	CheckCard(MemberInfo.GetSerailNum(0));

	if (m_nMemberId > 0)
	{
		CString strNewNetId = strIdNumber;
		NetBarUser.SetMemberId(m_nMemberId);
		NetBarUser.SetNetId(MemberInfo.GetSerailNum(0));

		if (QueryBalance())
		{
			//从身份证转账号
			if (ChangeNetIdToPersonalId(strNewNetId))
			{
				NetBarUser.SetNetId(strNewNetId);

				//TODO:再查一次用户信息

				CheckCard(strNewNetId);
				QueryBalance();
			}

			return TRUE;
		}
	}

	return FALSE;
}
