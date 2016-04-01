// RegisterDlg.cpp : 实现文件

#include "stdafx.h"
#include "IBA.h"
#include "PwdDlg.h"
#include "Encrypt.h"
#include "IBAPrint.h"
#include "dal\IBADAL.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include "RealName\IBARealName.h"
#include "MainFrm.h"
#include "IBAHelpper.h"
#include "BundleTimeListDlg.h"
#include "IdentityNumber.h"
#include "memberinfodlg.h"
#include "CurrentNetBarUser.h"
#include "ActiveMemberView.h"
#include ".\registerdlg.h"
#include "BossPwdDlg.h"
#include "DlgInputMobile.h"
#include "RealName\WDRealName.h"
#include "RealName\RealNameBase.h"
#include "NetBarUser.h"
#include "RealName\RZXRealName.h"
#include "DlgCapPhoto.h"
#include "ChinaUnicomConfig.h"
#include "RealName\AuditInterface.h"
#include "DlgCapture.h"
#include "DlgInputUserPwd.h"
#include "LocalCenter/LocalCenter.h"

// CRegisterDlg 对话框

HWND CRegisterDlg::m_StaticPrehWnd = NULL;

BOOL CRegisterDlg::m_StaticTmpMember = FALSE;

IMPLEMENT_DYNAMIC(CRegisterDlg, CNDDialog)
CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CRegisterDlg::IDD, pParent)
, m_bIsTempUser(TRUE), m_bNoPresent(FALSE)
, m_bWaitReg(FALSE), m_bClickRnReg(FALSE)
{
	InitData();

	m_bPhoneNumberOpen = FALSE;
	IBA_LOG0(_T("Register dialog object created!"));
}

CRegisterDlg::~CRegisterDlg()
{
#ifndef _DEBUG
	HWND hWnd = theApp.GetMainWnd()->GetSafeHwnd();
	if(NULL != hWnd)
	{
		// 先把主窗口设为最顶层
		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );  
		//::SetWindowPos(GetSafeHwnd(), hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		//// 这段代码将当前窗口带到带到最顶层，并设置为活动窗口
		//HWND hForegdWnd = ::GetForegroundWindow();
		//DWORD dwCurID = ::GetCurrentThreadId();
		//DWORD dwForeID = ::GetWindowThreadProcessId(hForegdWnd, NULL);
		//::AttachThreadInput(dwCurID, dwForeID, TRUE);
		//::SetForegroundWindow(GetSafeHwnd());
		//::AttachThreadInput(dwCurID, dwForeID, FALSE);


		//// 设置金额输入框为输入焦点
		//m_edtMoney.SetFocus();

	}
#endif
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strNetId);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PERSONALID, m_strPersonalID);
	DDX_Check(pDX, IDC_CHECK_USERHASCARD, m_bUserHasCard);
	DDX_Check(pDX, IDC_CHECK_PRESENT, m_bNoPresent);
	DDX_Check(pDX, IDC_CHECK_PRINT, m_bPrintTicket);
	DDX_Control(pDX, IDC_COMBO_SEX, m_cboSex);
	DDX_Control(pDX, IDC_COMBO_USERCLASS, m_cboUserClass);
	DDX_Control(pDX, IDC_COMBO_IDTYPE, m_cboIDType);
	DDX_Control(pDX, IDC_EDIT_MONEY, m_edtMoney);
	DDX_Control(pDX, IDC_CHECK_TEMPMEMBER, m_chkTempMember);
	DDX_Control(pDX, IDC_COMBO_BTSEL, m_cboBundTimeSel);
	DDX_Control(pDX, IDC_EDIT_PERSONALID, m_edtPersonalID);
	DDX_Control(pDX, IDC_COMBO_TEMPUSERCLASS, m_cboTempUserClass);
	DDX_Control(pDX, IDC_RADIO_TEMPUSER, m_btnTempUserClass);
	DDX_Control(pDX, IDC_RADIO_MEMBER, m_btnMemberUserClass);
	DDX_Control(pDX, IDC_COMBO_COMPUTER, m_cboComputer);

	DDX_Control(pDX, IDC_BTN_CUSTOM_BOUNDLETIME, m_btnSefDefine);
	DDX_Text(pDX, IDC_EDIT_TELNUM, m_strTelNum);
	DDV_MaxChars(pDX, m_strTelNum, 20);
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CNDDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BUNDLETIME, OnBnClickedButtonBundletime)
	ON_BN_CLICKED(IDC_BUTTON_READID2, OnBnClickedButtonReadid2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_TEMPMEMBER, OnBnClickedCheckTempmember)
	ON_BN_CLICKED(IDC_BUTTON_REPRINT, OnBnClickedButtonReprint)
	ON_BN_CLICKED(IDC_CHECK_PRINT, OnBnClickedCheckPrint)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_MESSAGE(WM_RECEIVECARD, OnRealName)
	ON_COMMAND(IDM_SCAN2_BACK, OnScan2Back)
	ON_COMMAND(IDM_SCAN2, OnScan2)
	ON_COMMAND(IDM_SCAN1, OnScan1)
	ON_COMMAND(IDM_READ2, OnRead2)
	ON_COMMAND(IDC_BUTTON_READCARD, OnRead2)
	ON_COMMAND(IDM_SCAN_PASSPORT, OnScanPassport)
	ON_COMMAND(ID_SCAN_OTHER, OnScanOther)
	ON_EN_SETFOCUS(IDC_EDIT_IDCARDSNO, OnEnSetfocusEditIdcardsno)
	ON_CBN_SELCHANGE(IDC_COMBO_BTSEL, OnCbnSelchangeComboBtsel)
	ON_EN_CHANGE(IDC_EDIT_PERSONALID, OnEnChangeEditPersonalid)
	ON_CBN_SELCHANGE(IDC_COMBO_USERCLASS, OnCbnSelchangeComboUserclass)
	ON_BN_CLICKED(IDC_RADIO_TEMPUSER, OnBnClickedRadioTempuser)
	ON_BN_CLICKED(IDC_RADIO_MEMBER, OnBnClickedRadioMember)
	ON_CBN_SELCHANGE(IDC_COMBO_TEMPUSERCLASS, OnCbnSelchangeComboTempuserclass)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_BOUNDLETIME, OnBnClickedBtnCustomBoundletime)
	ON_BN_CLICKED(IDC_BTN_INPUT_NUMBER, OnBnClickedBtnInputNumber)
	ON_BN_CLICKED(IDC_BTN_JUBAO, OnBnClickedBtnCapphtoto)
	ON_BN_CLICKED(IDC_BTN_RN_REG, OnBnClickedBtnRnReg)
	ON_MESSAGE(C_CANCLOSE, OnCanClose)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_TEL, OnBnClickedButtonQueryTel)
	ON_BN_CLICKED(IDC_BTN_PHOTO, OnBnClickedBtnPhoto)
END_MESSAGE_MAP()

// CRegisterDlg 消息处理程序

void CRegisterDlg::OnBnClickedOk()
{
	UpdateData();
	
	if (!theApp.GetRealName()->IsRegisterEnabled())
	{
		theApp.IBAMsgBox(_T("实名不允许开户!"));
		return;//实名是否允许开户
	}

	if (!CheckRealNameData()) return; //校验实名开户数据
	
	GetRegisterInfo();//从控件检验注册信息
	
	if (!CheckInput()) return; //输入有误

	if (!CheckMoney()) return;//检验输入金额

	MakeUserPwd();//自动生成密码

	if(2 == CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		if(!CIBAHelpper::TakePhoto(CIBAHelpper::GetUserZcImgPath(m_nIdType, m_strPersonalID), m_strPersonalID))
		{
			return ;
		}
	}

	// 2014-6-26 - qsc
	if(0 != CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		CString strDest = CIBAHelpper::GetUserZjImgFilePath(m_nIdType, m_strPersonalID);
		// 2014-8-13 - qsc 每次都覆盖 if(!PathFileExists(strDest))
		{
			const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
			CopyFile(strSource, strDest, FALSE);

			CString strBak = theApp.GetWorkPath() + _T("\\IBATemp\\ID_bak.jpg");
			CopyFile(strSource, strBak, FALSE);
			DeleteFile(strSource);
		}		
	}

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		//2015-0918 liyajun 严禁注册本地用户
		//RegisterLocalUser();
		return;
	}
	else
	{	
		//{
		//	// QSCT1
		//	// 2014-4-25 qsc  取消调用sfreg注册
		//	// 对应的OnCanClose这些也没用了，不过还不能确定深圳网监真正的态度，代码留着
		//	//// 2013-10-12 qsc
		//	if(2 <= CNetBarConfig::GetInstance()->GetSoftSfreg()	||
		//		m_bClickRnReg)		// 自动注册和通过实名注册打开的都要打开SFREG
		//	{
		//		m_bClickRnReg = FALSE;
		//		m_bWaitReg = TRUE;
		//		ExeSfreg();
		//		return ;

		//	}
		//	else
		//	{
		//		RegisterCenterUser();
		//	}
		//}
		//
		/*if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameJQ)
		{
		try
		{
		SendRegDataToProxy();
		}
		//catch (...)
		{
		IBA_LOG(_T("向实名接口发送注册信息出错! %d"), GetLastError());
		}
		RegisterCenterUser();
		}*/
		//{
		// 2014-4-28 - qsc 深圳地区 QSCT1
		//if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameRZX)
		if(1 == CNetBarConfig::GetInstance()->GetDomainId() ||	CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameJQ)
		{
			try
			{
			SendRegDataToProxy();
			}
			catch (...)
			{
				IBA_LOG(_T("向实名接口发送注册信息出错! %d"), GetLastError());
			}

		}

		RegisterCenterUser();
		//}		

		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			ResetNetworkBrokenControl();
		}
	}

	if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
	{
		CString strPhotoPath;
		if(NS_REALNAME::CRZXRealName::GetPhotoPath(strPhotoPath, m_strPersonalID))
		{	
			CString strPath, strNewFile;
			strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
			CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
			strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(m_strNetId));
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
	
}

BOOL CRegisterDlg::CheckInput()
{
	//上网账号

	//guoxuxing-2013-6-25: 如果强制使用手机号开户
	if (CNetBarConfig::GetInstance()->GetMobileOpen())
	{
		if (m_strNetId.GetLength() != 11)
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO);
			ShowToolTip(IDS_INVALID_MOBILE);
			GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
			return FALSE;
		}
	}
	else
	{
		if (m_strNetId.GetLength() < 6 || m_strNetId.GetLength() > 20)
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO);
			ShowToolTip(IDS_SNNOLONG);
			GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
			return FALSE;
		}
	}
	

	if (m_chkTempMember.GetCheck() != BST_UNCHECKED)
	{
		return TRUE;
	}

	if(!m_strTelNum.IsEmpty())
	{
		for(int i = 0; i < m_strTelNum.GetLength(); i++)
		{
			if(
				!isdigit(m_strTelNum.GetAt(i)) &&
				!(m_strTelNum.GetAt(i) != '_') &&
				!(m_strTelNum.GetAt(i) != '-')
			  )
			{
				SetToolTipPos(IDC_EDIT_TELNUM);
				ShowToolTip(IDS_ERROR_TELNUM_TIPS);
				GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
				return FALSE;
			}
		}

		// 不能大于13位
		if(m_strTelNum.GetLength() > 13)
		{
			SetToolTipPos(IDC_EDIT_TELNUM);
			ShowToolTip(IDS_ERROR_TELNUM_COUNT);
			GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
			return FALSE;
		}
	}
	else
	{
		if(0 != CNetBarConfig::GetInstance()->GetTelVerify())
		{
			CString strTips;
			strTips.LoadString(IDS_ERROR_TELNUM_EMPTY);
			GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
			SetToolTipPos(IDC_EDIT_TELNUM);
			ShowToolTip(strTips, TRUE);
			return FALSE;
		}
	}

	//姓名
	if (m_strUserName.GetLength() < 2)
	{
		SetToolTipPos(IDC_EDIT_USERNAME);
		ShowToolTip(IDS_ERRORUSERNAME);
		GetDlgItem(IDC_EDIT_USERNAME)->SetFocus();

		return FALSE;
	}

	if (m_strUserName.GetLength() > 10)
	{
		m_strUserName = m_strUserName.Left(10);
	}

	//证件号码
	if (m_strPersonalID.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_PERSONALID);
		ShowToolTip(IDS_INPUTCERTIFICATENUM);
		GetDlgItem(IDC_EDIT_PERSONALID)->SetFocus();
		return FALSE;
	}
	else
	{
		// 2012/06/28-8246-gxx: 强制扫描也验证.
		//if (CNetBarConfig::GetInstance()->GetMustScan() == 0) //如果强制扫描就不验证身份证
		{
			if (11 == m_nIdType || 12 == m_nIdType) //身份证要做验证
			{	
				CIdentityNumber IdentityNumber(m_strPersonalID);
				
				if (!IdentityNumber.IsValid()) //无效身份证
				{
					if (CIBAConfig::GetInstance()->GetCheckIDCardNumber()) //设置了需要检验身份证
					{
						SetToolTipPos(IDC_EDIT_PERSONALID);
						ShowToolTip(IDS_IDNUMERROR);
						GetDlgItem(IDC_EDIT_PERSONALID)->SetFocus();

						return FALSE;
					}
				}
				else 
				{	
					// 网监要求开户对身份证做18岁验证， LimitAge =0不限制
					if(CNetBarConfig::GetInstance()->GetLimitAge() != 0)
					{
						if(IdentityNumber.GetRealAge() < CNetBarConfig::GetInstance()->GetLimitAge())
						{
							SetToolTipPos(IDC_EDIT_PERSONALID);
							ShowToolTip(_T("此用户未成年，禁止开户！"));
							GetDlgItem(IDC_EDIT_PERSONALID)->SetFocus();
							return FALSE;
						}
					}

					if (IdentityNumber.GetAge() < CIBAConfig::GetInstance()->GetRegisterAge())
					{
						CIBAString strTmp;
						strTmp.LoadString(IDS_NONAGEERROR);
						
						if (MsgBox(strTmp, MB_OKCANCEL) == IDCANCEL)
						{
							GetDlgItem(IDC_EDIT_PERSONALID)->SetFocus();

							return FALSE;
						} 
					}

					m_cboSex.SetCurSel(IdentityNumber.GetSex());

				} 
			}
		}
	}
	
	return TRUE;
}

BOOL CRegisterDlg::OnInitDialog()
{
	IBA_LOG(_T("Temp.CRegisterDlg::OnInitDialog - Start"));
	//CIBADialog::OnInitDialog();
	IBA_LOG(_T("Temp.CNDDialog::OnInitDialog - Start"));
	CNDDialog::OnInitDialog();
	IBA_LOG(_T("Temp.CNDDialog::OnInitDialog - End"));
	IBA_LOG(_T("Register dialog init start!"));

	if (!theApp.GetCurCashier()->MonitorAuditProxy())
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	CIBAGlobal::m_hCurRegisterDlg = GetSafeHwnd();

	InitContrls();

	if (m_strPersonalID.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	}
	else
	{
		m_edtMoney.SetFocus();
	}

	m_StaticPrehWnd = GetSafeHwnd();

	AddFocusEnter(IDC_EDIT_USERNAME, IDC_BUTTON_QUERY);
	AddFocusEnter(IDC_EDIT_PERSONALID, IDC_BUTTON_QUERY);

	UpdateData(FALSE);

	SetTimer(TimeIdOfAutoQuery,400,NULL);
	
	if (CNetBarConfig::GetInstance()->GetMobileOpen())
	{
		SetDlgItemText(IDC_EDIT_IDCARDSNO, _T(""));			// 如果设置了手机账号，不论什么情况下都清空账号框
	}	
	IBA_LOG0(_T("Register dialog init end!"));

	return FALSE;  
}

HBRUSH CRegisterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_EDIT == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_MONEY)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}	
	}
	else if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
		else if (pWnd->GetDlgCtrlID() == IDC_STATIC_DEPOSIT)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	return hbr;
}

void CRegisterDlg::InitContrls()
{
	// 性别
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SEX);
		pCombo->AddString(LOAD_STRING(IDS_MAN));
		pCombo->AddString(LOAD_STRING(IDS_WOMAN));
	}
	
	//性别
	m_cboSex.SetCurSel(0);

	//证件类型
	InitIdTypeComboBox();

	//用户级别
	InitUserClassComboBox();

	InitComputerComboBox();

	CIBAHelpper::CreateFont(m_MoneyFont, 28, 900);

	m_edtMoney.DisableMask();
	m_edtMoney.SetValidChars(_T("0123456789."));
	m_edtMoney.SetLimitText(6);
	m_edtMoney.SetFont(&m_MoneyFont);

	m_btnSefDefine.SizeToContent();

	GetDlgItem(IDC_STATIC0)->SetFont(&m_Font);

	//强制扫描
	if (CNetBarConfig::GetInstance()->GetMustScan() == 1)
	{
		GetDlgItem(IDC_BUTTON_QUERY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_IDCARDSNO))->SetReadOnly(TRUE);

		EnableUserInfoControls(m_bCanEditUserInfo);

		SetTimer(TimerIdOfDisableInput, 300, NULL);
	}

#ifndef _DEBUG
	
	// 2011/09/28-8201-gxx: 
	if (m_cboTempUserClass.GetCount() == 0)
	{
		GetDlgItem(IDC_CHECK_TEMPMEMBER)->ShowWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_TEMPMEMBER)->ShowWindow(CNetBarConfig::GetInstance()->GetTempMember());
	}

#endif

	CWnd* pScanBtn = GetDlgItem(IDC_BUTTON_READID2);

	if (pScanBtn != NULL)
	{
	//QSC	pScanBtn->ShowWindow(theApp.GetRealName()->IsScanEnabled());
	}

	//扫描证件功能，强制扫描= 2时隐藏
	if (CNetBarConfig::GetInstance()->GetMustScan() == 2)
	{
		GetDlgItem(IDC_BUTTON_READID2)->ShowWindow(SW_HIDE);
	}

	//重复打印小票按钮初始隐藏
	GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);
	
	//开户默认金额
	if (CIBAConfig::GetInstance()->GetRegisterMoney() > 0)
	{
		CString strTmpMoney;
		strTmpMoney.Format(_T("%.2f"), CIBAConfig::GetInstance()->GetRegisterMoney() / 100.0);
		m_edtMoney.SetWindowText(strTmpMoney);
	}

	//用户自带卡功能
	GetDlgItem(IDC_CHECK_USERHASCARD)->ShowWindow(CIBAConfig::GetInstance()->GetHasCard());
	
	//押金
	GetDlgItem(IDC_STATIC_DEPOSIT)->ShowWindow(CNetBarConfig::GetInstance()->IsUseDeposit());

	//自动生成账号
	if (m_StaticTmpMember)
	{
		m_chkTempMember.SetCheck(TRUE);
		OnBnClickedCheckTempmember();
	}

	//{ 2011/07/14-8201-gxx: 

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		ResetNetworkBrokenControl();
	}
	//}

	//许丹要求不显示该按钮
	/*if (CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameWD)
	{
		CWnd* pWnd = GetDlgItem(IDC_BTN_INPUT_NUMBER);
		if (pWnd)
		{
			pWnd->ShowWindow(TRUE);
		}
		else
		{
			IBA_ASSERT2(0,"界面异常，控件IDC_BTN_INPUT_NUMBER已不存在");
			IBA_LOG0(_T("CRegisterDlg 界面异常，控件IDC_BTN_INPUT_NUMBER已不存在"));
		}
	}*/
	
	if (CNetBarConfig::GetInstance()->GetMobileOpen())
	{
		GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(TRUE);
		((CEdit*)GetDlgItem(IDC_EDIT_IDCARDSNO))->SetReadOnly(FALSE);
	}

	if(1 == CNetBarConfig::GetInstance()->GetEnableJuBao() 
		&& !m_bFromCard && !m_bFromScan)
	{
		GetDlgItem(IDC_BTN_JUBAO)->ShowWindow(SW_SHOW);	
		((CEdit*)GetDlgItem(IDC_EDIT_IDCARDSNO))->SetReadOnly(TRUE);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(FALSE);
		EnableUserInfoControls(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else 
	{
		GetDlgItem(IDC_BTN_JUBAO)->ShowWindow(SW_HIDE);	
	}


	if(1 != CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		GetDlgItem(IDC_BTN_RN_REG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_PHOTO)->ShowWindow(SW_HIDE);
	}
	// 2014-4-25 - qsc 后面确认不需要实名注册按钮后可以删除按钮
	// QSCT1 
	GetDlgItem(IDC_BTN_RN_REG)->ShowWindow(SW_HIDE);

	if(0 != CNetBarConfig::GetInstance()->GetCannotSelfBundleTime())
	{
		GetDlgItem(IDC_BTN_CUSTOM_BOUNDLETIME)->EnableWindow(FALSE);
	}

	if((0 != CNetBarConfig::GetInstance()->GetTelVerify()) && (0 != CNetBarConfig::GetInstance()->GetHideTelInput()))	// 电话验证
	{
		GetDlgItem(IDC_EDIT_TELNUM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_8)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_QUERY_TEL)->ShowWindow(SW_SHOW);
	}
	else
	{
		if(0 == CNetBarConfig::GetInstance()->GetHideTelInput())
		{
			GetDlgItem(IDC_EDIT_TELNUM)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_8)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_QUERY_TEL)->ShowWindow(SW_HIDE);
		}
	}

}

void CRegisterDlg::MakeUserPwd()
{
	//使用任何实名类型(尤其过滤王)
	m_strPassword.Trim();

	if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameZD)
	{	
		// 2014-10-8 - qsc 智多实名使用空密码
		m_strPassword.Empty();
		return ;
	}

	if (m_bFromCard)//刷卡得到的信息
	{
		//坚决采用过滤王密码
		if (CNetBarConfig::GetInstance()->GetRealNamePassword() == 2)
		{
			return;
		}
		
		// 2014-8-28 - qsc 金桥实名使用金桥密码
		if (CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameJQ)
		{
			return;
		}

		//使用实名密码，不强制，如果为空则使用自定义密码
		if (CNetBarConfig::GetInstance()->GetRealNamePassword() > 0)
		{
			if (!m_strPassword.IsEmpty())
			{
				return;
			}
		}
	}

	INT randNum = 0;

	INT nPwdType = CIBAConfig::GetInstance()->GetTempPasswordType();
	
	CString strTmp = CIBAConfig::GetInstance()->GetTempDefPassword();

	INT nTmp = m_cboUserClass.GetCurSel();

	if (!m_bIsTempUser) //会员
	{
		nPwdType = CIBAConfig::GetInstance()->GetPasswordType();
		strTmp = CIBAConfig::GetInstance()->GetDefPassword();
	}

	switch (nPwdType)
	{
	case CIBAGlobal::emPwdTypeRandom: //随机

		srand(time(NULL));
		randNum = rand() % 1000;	
		strTmp.Format(_T("%.3d"), randNum);

		break;

	case CIBAGlobal::emPwdTypeIDTail6: //证件后6
		
		if (m_strPersonalID.GetLength() < 6)
		{
			strTmp.Format(_T("%.6s"), m_strPersonalID);
		}
		else
		{
			strTmp = m_strPersonalID.Right(6);
		}
		
		break;

	case CIBAGlobal::emPwdTypeFixed: //固定
		
		//strTmp = CIBAConfig::GetInstance()->GetDefPassword();

		break;

	case CIBAGlobal::emPwdTypeInput: // 用户输入
		{
			CDlgPwd dlg;
			dlg.DoModal();
			strTmp = dlg.GetPwd();
		}		
		break;

	case CIBAGlobal::emPwdTypeEmpty: // 空密码---只支持临时用户
		//if (m_bIsTempUser)
		{
			strTmp = _T(""); 
		}
		
		
		break;
	default:
		IBA_ASSERT2(FALSE, "无法识别的密码类型");
		break;
	}

	m_strPassword = strTmp;

	if (m_strPassword.IsEmpty() && nPwdType != CIBAGlobal::emPwdTypeEmpty )
	{
		if (m_bFromScan || m_bFromCard)//!<信息是否来自扫描|自刷卡
		{
			//密码为空，不允许开户
			MsgBox(IDS_PSWEMPTY);
			return;
		}
	}
}

void CRegisterDlg::PrintTicket()
{	
	if(0 == CNetBarConfig::GetInstance()->GetIsPrintA4())	// 没有配置参数
	{
		PrintTicketPos();
	}
	else
	{
		PrintTicketA4();
	}
}

void CRegisterDlg::PrintTicket2()
{	
	CString strTicket, strTmp;

	CIBAPrint::GetRegisterAndBundleTimeTicket(strTicket);

	strTicket.Replace(_T("[name]"), m_strUserName);
	strTicket.Replace(_T("[idnumber]"), m_strPersonalID);

	if (CNetBarConfig::GetInstance()->GetRealNamePassword() >= 2)
	{
		strTicket.Replace(_T("[password]"), _T("*****"));
	}
	else
	{
		strTicket.Replace(_T("[password]"), m_strPassword);
	}
	strTicket.Replace(_T("[memberclass]"), m_strUserClassName);

	strTmp.Format(_T("%.2f"), m_nCreditMoney / 100.0);
	strTicket.Replace(_T("[money]"), strTmp);
	strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
	strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
	strTmp.Format(_T("%d"), m_nMemberId);
	strTicket.Replace(_T("[memberid]"), strTmp);

	strTicket.Replace(_T("[pcclass]"), m_strBundleTimeArea);
	strTmp.Format(_T("%.2f"), m_nBundTimeMoney / 100.0);
	strTicket.Replace(_T("[bundletimemoney]"), strTmp);
	strTicket.Replace(_T("[starttime]"), m_strBundleTimeStartTime.Mid(11, 5));
	strTicket.Replace(_T("[endtime]"), m_strBundleTimeEndTime.Mid(11, 5));
	strTicket.Replace(_T("[totalmoney]"), m_strBalanceAfterBundleTime);

	CIBAPrint::Print(strTicket);
}


void CRegisterDlg::PrintTicketPos()
{
	CString strTicket, strTmp;

	CIBAPrint::GetRegisterTicket(strTicket);

	strTicket.Replace(_T("[name]"), m_strUserName);
	strTicket.Replace(_T("[idnumber]"), m_strPersonalID);

	if (CNetBarConfig::GetInstance()->GetRealNamePassword() >= 2)
	{
		strTicket.Replace(_T("[password]"), _T("*****"));
	}
	else
	{
		strTicket.Replace(_T("[password]"), m_strPassword);
	}

	strTicket.Replace(_T("[memberclass]"), m_strUserClassName);

	strTmp.Format(_T("%.2f"), m_nCreditMoney / 100.0);
	strTicket.Replace(_T("[money]"), strTmp);
	strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
	strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));

	strTmp.Format(_T("%d"), m_nMemberId);
	strTicket.Replace(_T("[memberid]"), strTmp);

	CIBAPrint::Print(strTicket);
}

void CRegisterDlg::PrintTicketA4()
{
	CIBAPrintA4::GetInstance()->SetOwner(theApp.m_pMainWnd);
	CIBAPrintA4::GetInstance()->m_strName = m_strUserName;
	CIBAPrintA4::GetInstance()->m_strNumber = m_strNetId;
	CIBAPrintA4::GetInstance()->m_strID = m_strPersonalID;
	CIBAPrintA4::GetInstance()->m_strAddress = m_userInfo.GetAddr();

	CChinaUnicomConfig::ClassIDItem item;
	if (CChinaUnicomConfig::GetInstance()->GetClassIDItem(m_nUserClassID, item))
	{
		CIBAPrintA4::GetInstance()->m_strContent[0] = _T("产品名称：") + item.strName;
		CIBAPrintA4::GetInstance()->m_strContent[1] = _T("产品特征：") + item.strDetail;
		CIBAPrintA4::GetInstance()->m_strContent[2] = _T("免费时间：") + item.strTime;
	}

#ifdef DEBUG
	CIBAPrintA4::GetInstance()->DoPrintPriview();
#else
	CIBAPrintA4::GetInstance()->DoPrint();
#endif
}

INT_PTR CRegisterDlg::DoModal()
{
	if(m_StaticPrehWnd && ::IsWindow(m_StaticPrehWnd))
	{
		// 已经创建过开户窗口了，不再创建, 直接返回
		return IDCANCEL;
	}
	INT_PTR nRet = CIBADialog::DoModal();
	m_StaticPrehWnd = NULL;
	return nRet;
}

void CRegisterDlg::ClearData()
{
	m_strNetId.Empty();
	m_strNetId2.Empty();
	m_strPassword.Empty();
	m_strPersonalID.Empty();
	m_strUserName.Empty();
}

BOOL CRegisterDlg::GetCardIdFromCenter()
{
	CString strTmp = m_strNetId;
	//在账号前段插入0
	if (strTmp.GetLength() < 10)
	{
		strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
	}

	BOOL bRet = FALSE;

	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strTmp);


	theApp.GetCurCashier()->DoCheckCard(checkCard);
	
	if (checkCard.GetStatusCode() == 0)
	{
		if (checkCard.GetMemberId() != 0) //已经开过的卡,充值退卡
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO);
			ShowToolTip(IDS_HASUSED);
		}
		else //新卡注册
		{
			SetCardId(checkCard.GetCardIdAsString());
			bRet = TRUE;
		}
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(checkCard.GetStatusMessage(), TRUE);
	}

	return bRet;
}

void CRegisterDlg::OnBnClickedButtonBundletime() 
{
	CDlgBundleTimeList dlg;

	GetRegisterInfo();

	dlg.SetUserClassId(m_nUserClassID);

	if (dlg.DoModal() == IDOK)
	{
		//根据对话框选择内容选中

		for (INT i = 0; i < m_cboBundTimeSel.GetCount(); i++)
		{
			if (m_cboBundTimeSel.GetItemData(i) == dlg.GetBundleTimeId())
			{
				m_cboBundTimeSel.SetCurSel(i);
				break;
			}
		}

		OnCbnSelchangeComboBtsel();
	}
}

BOOL CRegisterDlg::IsBundleTime()
{
	if (0 == m_nBundleTimeId)
	{
		return FALSE;
	}

	CBundleTimeActionMethod BundleTimeAction;
	BundleTimeAction.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBundleTimeAction(BundleTimeAction);

	if (BundleTimeAction.GetStatusCode() == 0 && BundleTimeAction.GetNetBarId() > 0)
	{
		CIBATTS::GetInstance()->SpeakBundleTimeRegisterResult(BundleTimeAction.GetAmount1(), m_nCreditMoney + m_nCreditPresent - BundleTimeAction.GetAmount1());

		m_strBundleTimeStartTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetStartTime());
		m_strBundleTimeEndTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetEndTime());
		//m_strBundleTimeMoney.Format(_T("%.2f"), BundleTimeAction.GetAmount1() / 100.0);
		m_nBundTimeMoney = BundleTimeAction.GetAmount1();
		m_strBundleTimeArea = CNetBarConfig::GetInstance()->GetPCClassName(BundleTimeAction.GetPcClass());
		m_strBalanceAfterBundleTime.Format(_T("%.2f"), (m_nCreditMoney + m_nCreditPresent -  BundleTimeAction.GetAmount1()) / 100.0);

		if (m_strBundleTimeArea.IsEmpty())
		{
			m_strBundleTimeArea = LOAD_STRING(IDS_NOLIMITAREA);
		}

		ShowRegisterBundleTimeResult();
		if (m_bPrintTicket)
		{
			PrintTicket2();
		}

		return TRUE;
	}

	return FALSE;
}

void CRegisterDlg::OnBnClickedButtonReadid2()
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_SCAN);
	CMenu* pPopup = menu.GetSubMenu(0);

	CRect rect;
	GetDlgItem(IDC_BUTTON_READID2)->GetWindowRect(rect);
	
	//// 2014-6-26 - qsc 移除“读取卡”，放到按钮单独实现
	//pPopup->RemoveMenu(IDM_READ2, MF_BYCOMMAND);
	// 2013-8-2 qsc 如果是网盾实名，则删除"扫描其他证件"菜单
	if(CIBAGlobal::emRealnameWD == CNetBarConfig::GetInstance()->GetAuditType())
	{
		pPopup->RemoveMenu(ID_SCAN_OTHER, MF_BYCOMMAND);
	}
	
	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->Detach(), rect.CenterPoint().x, rect.CenterPoint().y, this, TRUE);
}

void CRegisterDlg::OnScan1()
{
	ScanReco(CIBAGlobal::SRT_SCAN1);
}

void CRegisterDlg::OnScan2()
{
	ScanReco(CIBAGlobal::SRT_SCAN2);
}

void CRegisterDlg::OnScanPassport()
{
	ScanReco(CIBAGlobal::SRT_SCAN_PASSPORT);
}

void CRegisterDlg::OnScan2Back()
{
	CWaitCursor Wait;

	CNetBarUser NetBarUser;

	if (theApp.GetRealName()->ScanUserInfo(CIBAGlobal::SRT_SCAN2_BACK, NetBarUser, TRUE))
	{
		m_strOfficer = NetBarUser.GetDepartment();
		//NetBarUser.GetSignDate();
		m_strOfficer =NetBarUser.GetDepartment();

		CString strTable, strShow;
		m_strOfficer.Trim();
		if(m_strOfficer.IsEmpty())
		{
			strShow.LoadString(IDS_SCAN2_BACK_FAILED);
		}
		else
		{
			strTable.LoadString(IDS_SCAN2_BACK_SUCCESS);
			strShow.Format(strTable, m_strOfficer);
		}
		

		SetToolTipPos(IDC_BUTTON_READID2);
		ShowToolTip(strShow);
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_READID2);
		ShowToolTip(IDS_SCANFAILED);
	}
}

void CRegisterDlg::OnRead2()
{
	ScanReco(CIBAGlobal::SRT_READ2);
}

void CRegisterDlg::OnScanOther()
{
	ScanReco(CIBAGlobal::SRT_SCAN_OTHER);
}

void CRegisterDlg::ScanReco(UINT nType)
{
	CWaitCursor Wait;

	CNetBarUser NetBarUser;

	if (theApp.GetRealName()->ScanUserInfo(nType, NetBarUser, TRUE))
	{
		m_strPersonalID = NetBarUser.GetCertificateID();
		m_strUserName = NetBarUser.GetUserName();
		m_strNation = NetBarUser.GetNation();
		m_strOfficer = NetBarUser.GetDepartment();
		
		if (!m_strNetId.IsEmpty() && CNetBarConfig::GetInstance()->GetAuditType() == 0) 
		{
			//上网账号不为空且没有实名，则不改变上网账号
		}
		else
		{
			// qscadd 2013-10-25
			if(1 == CNetBarConfig::GetInstance()->GetEnableAccount12())
			{
				CString strCID = NetBarUser.GetCertificateID();
				if(12 < strCID.GetLength() && 
					(NetBarUser.GetCertificateType() == 11 ||
					NetBarUser.GetCertificateType() == 12))
				{
					NetBarUser.SetNetId(strCID.Right(12));
					m_strNetId = strCID.Right(12);
				}
			}
			else
			{
				m_strNetId = m_strPersonalID;
			}
		}

		m_cboSex.SetCurSel(NetBarUser.GetSex());

		if (NetBarUser.GetCertificateType() == 12)
		{
			if (CB_ERR == m_cboIDType.SelectString(0, _T("二代证")))
			{
				m_cboIDType.SelectString(0, _T("身份证"));
			}
		}
		else if (NetBarUser.GetCertificateType() == 99)
		{
			m_cboIDType.SelectString(0, _T("其他证件"));
			m_bNetIdFromPersonalId = TRUE;
		}
		else
		{
			m_cboIDType.SelectString(0, _T("身份证"));
		}

		m_bCanEditUserInfo = NetBarUser.GetCanEditUserInfo();
		
		EnableUserInfoControls(m_bCanEditUserInfo);

		UpdateData(FALSE);

		m_bFromScan = TRUE;
		m_bFromCard = FALSE;
		m_edtMoney.SetFocus();

		SetUserInfo(NetBarUser);
	}
	else
	{
		if(CIBAGlobal::SRT_READ2 == nType)
		{
			SetToolTipPos(IDC_BUTTON_READCARD);
			ShowToolTip(IDS_READCARD_FAILED);
			
		}
		else
		{
			SetToolTipPos(IDC_BUTTON_READID2);
			ShowToolTip(IDS_SCANFAILED);
		}	
	}
}

void CRegisterDlg::SetUserInfo(CNetBarUser& NetBarUser)
{
	m_bFromCard = FALSE;
	m_bFromScan = FALSE;

	if (NetBarUser.GetDataFrom() == 1)
	{
		m_bFromCard = TRUE;
	}
	else if (NetBarUser.GetDataFrom() == 2)
	{
		m_bFromScan = TRUE;
	}
	
	m_bCanEditUserInfo = NetBarUser.GetCanEditUserInfo();
	m_nUserSex = NetBarUser.GetSex();

	m_strUserName = NetBarUser.GetUserName();
	m_strPersonalID = NetBarUser.GetCertificateID();
	m_nIdType = NetBarUser.GetCertificateType();
	m_bIsMember = NetBarUser.GetIsMember();
	m_strPassword = NetBarUser.GetPassword();

	m_strNetId = NetBarUser.GetNetId();

	m_strNetId2 = m_strNetId;

	m_strOfficer = NetBarUser.GetDepartment();
	m_strNation = NetBarUser.GetNation();

	m_userInfo = NetBarUser;

	// 2014-10-29 - qsc
	// 身份证性别自己计算， 因为扫描出来的有可能不准确
	if (11 == m_nIdType || 12 == m_nIdType) //身份证要做验证
	{	
		CIdentityNumber IdentityNumber(m_strPersonalID);

		if (IdentityNumber.IsValid()) //无效身份证
		{
			m_nUserSex = IdentityNumber.GetSex();
		}
	}
}

//接收刷卡消息

LRESULT CRegisterDlg::OnRealName(WPARAM wp, LPARAM lp)
{
	CNetBarUser* pNetBarUser = (CNetBarUser*)lp;

	SetUserInfo(*pNetBarUser);

	SelectIDType();

	EnableUserInfoControls(m_bCanEditUserInfo);

	UpdateData(FALSE);

	return 0;
}

void CRegisterDlg::SelectIDType()
{
	for (INT i = 0; i < m_cboIDType.GetCount(); i++)
	{
		if (m_cboIDType.GetItemData(i) == m_nIdType)
		{
			m_cboIDType.SetCurSel(i);
			break;
		}
	}

	m_cboSex.SetCurSel(m_nUserSex);
}

void CRegisterDlg::OnEnSetfocusEditIdcardsno()
{
	if (m_bAfterRegister)
	{
		m_bAfterRegister = FALSE;

		m_strUserName.Empty();
		m_strPersonalID.Empty();
		m_strNetId.Empty();
		m_strOfficer.Empty();
		m_strNation.Empty();
		m_strTel.Empty();

		UpdateData(FALSE);
		
		m_nIdType = CIBAConfig::GetInstance()->GetDefIDTypeId();
		SelectIDType();

		m_cboUserClass.SetCurSel(0);
		m_cboTempUserClass.SetCurSel(0);

		//打印小票的状态控制
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);
	}
}

void CRegisterDlg::SaveRegisterInfo()
{
	CCardOpenInfo CardOpenInfo;

	CardOpenInfo.SetClassId(m_nUserClassID);
	CardOpenInfo.SetOperationDateTime(CIBAHelpper::FormatCenterTime(UserRegister.GetTrantime()));
	CardOpenInfo.SetRefNo(UserRegister.GetRefNo());
	CardOpenInfo.SetOperator(theApp.GetCurCashier()->GetName());
	CardOpenInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	CardOpenInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	CardOpenInfo.SetMemberId(UserRegister.GetMemberId());
	CardOpenInfo.SetSerialNo(UserRegister.GetSerialNo());
	CardOpenInfo.SetCostExpense(m_nCostExpense);
	CardOpenInfo.SetDeposit(m_nGuarantyMoney);
	CardOpenInfo.SetCardId(_ttoi(m_strCardId));
	CardOpenInfo.SetIdType(m_nIdType);
	CardOpenInfo.SetIdNumber(m_strPersonalID);
	CardOpenInfo.SetSerialNum(m_strNetId);
	CardOpenInfo.SetUserName(m_strUserName);
	CardOpenInfo.SetUserSex(m_cboSex.GetCurSel());

	CIBADAL::GetInstance()->AddCardOpenRecord(CardOpenInfo);

	//******************************************************************************

	CCreditInfoDB CreditInfo;

	CreditInfo.SetCardSerial(m_strNetId);
	CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(UserRegister.GetTrantime()));
	CreditInfo.SetRefNo(UserRegister.GetRefNo1());
	CreditInfo.SetCreditPresent(UserRegister.GetCreditPresent());
	CreditInfo.SetCreditAmount(m_nCreditMoney);
	CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
	CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	CreditInfo.SetMemberId(UserRegister.GetMemberId());
	CreditInfo.SetSerialNo(UserRegister.GetLocalSerialNo());
	CreditInfo.SetCassId(m_nUserClassID);

	CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);

	CIBADAL::GetInstance()->UpdateMemberInfo(UserRegister.GetMemberId(),
		m_strNation,
		m_strOfficer,
		m_userInfo.GetAddr(),
		m_strTelNum);
}

void CRegisterDlg::GetRegisterInfo()
{
	INT nTmp = m_cboIDType.GetCurSel();
	m_nIdType = m_cboIDType.GetItemData(nTmp);

	if (m_bIsTempUser)
	{
		nTmp = m_cboTempUserClass.GetCurSel();
		nTmp = m_cboTempUserClass.GetItemData(nTmp);
	}
	else
	{
		nTmp = m_cboUserClass.GetCurSel();
		nTmp = m_cboUserClass.GetItemData(nTmp);
	}

	m_strUserClassName = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(nTmp).GetClassName();
	m_nUserClassID = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(nTmp).GetClassID();
	m_nCostExpense = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(nTmp).GetCostExpense();
	m_nGuarantyMoney = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(nTmp).GetDeposit();
}

BOOL CRegisterDlg::CheckMoney()
{
	SetToolTipPos(IDC_EDIT_MONEY);

	CString strMoney;
	m_edtMoney.GetWindowText(strMoney);

	if (strMoney.IsEmpty())
	{
		ShowToolTip(IDS_MONEYERROR);
		m_edtMoney.SetFocus();
		return FALSE;
	}

	double dblMoney = _tstof(strMoney);

	//对小数后第3位进行舍入，防止出错
	INT nMoney = (dblMoney + 0.005) * 100; //转换成分
	//自带卡，为了方便处理，工本费和押金设置为0
	if (m_bUserHasCard)
	{
		m_nCostExpense = 0;
		m_nGuarantyMoney = 0;
	}

	// 2012/03/05-8238-gxx: 
	if (CIBAConfig::GetInstance()->GetMaxCreditMoney() != 0)
	{
		INT nMax = CIBAConfig::GetInstance()->GetMaxCreditMoney() * 100;
		if (nMoney > nMax)
		{
			ShowToolTip(IDS_OVERFLOW_MAXMONEY);
			m_edtMoney.SetFocus();
			m_edtMoney.SetSel(0, strMoney.GetLength());
			return FALSE;
		}
	}
	else
	{
		INT nMax = 10000*100;
		if (nMoney > nMax)
		{
			ShowToolTip(IDS_OVERFLOW_MAXMONEY2);
			m_edtMoney.SetFocus();
			m_edtMoney.SetSel(0, strMoney.GetLength());
			return FALSE;
		}
	}
	// --end

	///////////////////////////////////
	
	nMoney = nMoney - m_nCostExpense - m_nGuarantyMoney;

	m_nCreditMoney = nMoney;//充值金额

	if (m_nBundleTimeId > 0)
	{
		nMoney -= m_nBundTimeMoney;
	}

	if (nMoney < 0) // 钱不够
	{
		ShowToolTip(IDS_NOENOUGHMONEY);
		m_edtMoney.SetFocus();
		return FALSE;
	}

	return TRUE;
}

void CRegisterDlg::SetRegisterInfo()
{
	UserRegister.SetUserClass(m_nUserClassID);
	if (m_userInfo.GetCertificateType() == 98)
	{
		// 机读二代证也是二代证
		m_nIdType = 12; 
	}
	UserRegister.SetIdType(m_nIdType);
	UserRegister.SetPersonalId(m_strPersonalID);
	UserRegister.SetCountry(_T("CN"));
	UserRegister.SetUserName(m_strUserName);
	UserRegister.SetSex(m_cboSex.GetCurSel());
	UserRegister.SetCostExpense(m_nCostExpense);//不受自带卡影响，因为设为0
	UserRegister.SetGuarantyMoney(m_nGuarantyMoney);

	UserRegister.SetCardId(m_strCardId);
	UserRegister.SetNoCard(m_bUserHasCard);
	//UserRegister.SetNoCard(0); //固定收押金
	UserRegister.SetNoPresent(m_bNoPresent);
	UserRegister.SetPassword(m_strPassword);
	
	UserRegister.SetCreditAmount(m_nCreditMoney);
	UserRegister.SetAuditId(m_strNetId);

	//深圳特殊的几个字段
	UserRegister.SetTel(m_strTel);
	UserRegister.SetIdDepart(m_strOfficer);
	UserRegister.SetJob(m_strNation);
	UserRegister.SetAddress(m_userInfo.GetAddr());

	// 设置激活刷卡类型
	UserRegister.SetActivationType(m_userInfo.GetActivationType());

	if(!m_strTelNum.IsEmpty())
		UserRegister.SetTel(m_strTelNum);
	
	if (m_DefineBundleTimeDlg.m_BundleTimeInfo.bIsSelected)
	{
		UserRegister.SetTimeId( m_DefineBundleTimeDlg.m_BundleTimeInfo.TimeId);
		UserRegister.SetAccountType( m_DefineBundleTimeDlg.m_BundleTimeInfo.AccountType);
		UserRegister.SetPcClass( m_DefineBundleTimeDlg.m_BundleTimeInfo.PcClass );
		UserRegister.SetTimePass( m_DefineBundleTimeDlg.m_BundleTimeInfo.TimePass );
		UserRegister.SetBeginTime( m_DefineBundleTimeDlg.m_BundleTimeInfo.BeginTime );
		UserRegister.SetBundleAmount( m_DefineBundleTimeDlg.m_BundleTimeInfo.Amount);

		m_nBundleTimeId = m_DefineBundleTimeDlg.m_BundleTimeInfo.TimeId;
		m_nBundTimeMoney = m_DefineBundleTimeDlg.m_BundleTimeInfo.Amount;
	}
	else
	{
		UserRegister.SetTimeId(m_nBundleTimeId);
	}
}

void CRegisterDlg::ShowRegisterResult(BOOL bLocalUser)
{
	CIBATTS::GetInstance()->SpeakRegisterResult(m_nCreditMoney, m_nCreditPresent);
	COperationInfo OperationInfo;
	if (bLocalUser)
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeOpenLocal);
	}
	else
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeOpen);
	}
		
	OperationInfo.SetIsBundTime(FALSE);
	
	CString strTmp;
	m_cboSex.GetWindowText(strTmp);
	strTmp.Insert(0, '(');
	strTmp.AppendChar(')');
	strTmp.Insert(0, m_strUserName);

	OperationInfo.SetUserName(strTmp);
	OperationInfo.SetNetId(m_strNetId);
	OperationInfo.SetMemberId(UserRegister.GetMemberId());
	OperationInfo.SetOperationAmount(m_nCreditMoney);
	OperationInfo.SetPresentAmount(UserRegister.GetCreditPresent());
	OperationInfo.SetDeposit(m_nGuarantyMoney);

	
	
	if (m_bIsTempUser) //只显示临时用户的密码
	{
		OperationInfo.SetPassWord(m_strPassword);
	}
	
	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
}

void CRegisterDlg::ShowRegisterBundleTimeResult()
{
	COperationInfo OperationInfo;
	OperationInfo.SetOperationType(3);
	OperationInfo.SetIsBundTime(TRUE);
	
	CString strTmp;
	m_cboSex.GetWindowText(strTmp);
	strTmp.Insert(0, '(');
	strTmp.AppendChar(')');
	strTmp.Insert(0, m_strUserName);

	OperationInfo.SetUserName(strTmp);
	OperationInfo.SetNetId(m_strNetId);
	OperationInfo.SetMemberId(UserRegister.GetMemberId());
	OperationInfo.SetOperationAmount(m_nCreditMoney);
	OperationInfo.SetDeposit(m_nGuarantyMoney);
	OperationInfo.SetPresentAmount(UserRegister.GetCreditPresent());
	OperationInfo.SetBundTimeAmount(m_nBundTimeMoney);

	if (m_bIsTempUser) //只显示临时用户的密码
	{
		OperationInfo.SetPassWord(m_strPassword);
	}

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
}

void CRegisterDlg::InitData()
{
	//实名相关控制变量
	m_bCanEditUserInfo = (CNetBarConfig::GetInstance()->GetMustScan() == 0); //是否编辑用户信息
	m_bNetIdFromPersonalId = FALSE;
	m_bFromCard = FALSE;
	m_bFromScan = FALSE;

	//计费相关控制变量
	m_bUserHasCard = FALSE;
	m_bNoPresent = FALSE;
	m_nDefClassIndex = 0;
	m_nBundleTimeId = 0;
	m_nIdType = CIBAConfig::GetInstance()->GetDefIDTypeId();
	m_bIsMember = FALSE;
	m_nCreditPresent = 0;
	m_bAfterRegister = FALSE;
	m_nCreditMoney = 0;	
	m_nCreditPresent = 0;  
	m_nUserClassID = 0;	
	m_nCostExpense = 0;
	m_nGuarantyMoney = 0; 
	m_nMemberId = 0;
	m_nUserSex = 0;
	m_nBundleTimeType = 0;
	m_nBundTimeClass = 0;

	m_nBundTimeMoney = 0;

}

void CRegisterDlg::AutoUploadImage()
{
	CNetBarUser NetBarUser;
	NetBarUser.SetNetId(m_strPersonalID);
	NetBarUser.SetCertificateID(m_strPersonalID);
	NetBarUser.SetCertificateType(m_nIdType);
	NetBarUser.SetUserName(m_strUserName);

	theApp.GetRealName()->UploadImage(0, NetBarUser, m_bFromScan);

	m_bFromScan = FALSE;
}

void CRegisterDlg::OnBnClickedCheckTempmember()
{
	if (BST_UNCHECKED != m_chkTempMember.GetCheck())
	{
		MakeTmpMemberInfo();
		
		//GetDlgItem(IDC_EDIT_TELNUM)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		m_cboIDType.EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PERSONALID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(TRUE);
		//GetDlgItem(IDC_EDIT_TELNUM)->EnableWindow(TRUE);
	}
	
	//静态
	m_StaticTmpMember = (BST_UNCHECKED != m_chkTempMember.GetCheck());

	m_edtMoney.SetFocus();
}

void CRegisterDlg::OnBnClickedButtonReprint()
{
	if (m_nBundleTimeId == 0)
	{
		PrintTicket();
	}
	else
	{
		PrintTicket2();
	}
}

void CRegisterDlg::OnBnClickedCheckPrint()
{
	UpdateData(TRUE);

	if (m_bPrintTicket && m_bAfterRegister)
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_NORMAL);
	} 
	else
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);
	}
}

void CRegisterDlg::OnBnClickedButtonQuery()
{
	UpdateData();

	CDlgMemberInfo dlg;
	dlg.SetQueryStyle(0);
	dlg.SetNetId(m_strNetId);
	dlg.SetPersonalID(m_strPersonalID);
	dlg.SetUserName(m_strUserName);

	if (dlg.DoModal() == IDOK)
	{
		m_strUserName = dlg.GetUserName();
		m_strPersonalID = dlg.GetPersonalID();

		m_cboSex.SelectString(-1, dlg.GetSex());
		m_cboIDType.SelectString(-1, dlg.GetIDType());
		m_cboUserClass.SelectString(-1, dlg.GetUserClass());
	}
	UpdateData(FALSE);
}

void CRegisterDlg::OnCbnSelchangeComboBtsel()
{
	m_nBundleTimeId = m_cboBundTimeSel.GetItemData(m_cboBundTimeSel.GetCurSel());
	
	CFullBundleTimeInfo BundleTimeInfo;

	if ( theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CUSTOMBUNDLE")) &&
		m_cboBundTimeSel.GetCurSel() == 1 ) //自定义
	{
		// 2011-4-15-gxx-添加
		OnBnClickedBtnCustomBoundletime();
	}
	else if (m_nBundleTimeId > 0 &&
		CNetBarConfig::GetInstance()->m_NetbarBundleTime.GetBundleTimeInfo(
		m_nUserClassID, m_nBundleTimeId, BundleTimeInfo))
	{
		m_nBundTimeClass = BundleTimeInfo.IsNowEnabled() ? 0 : 1;

		//开始时间
		m_strBundleTimeStartTime = BundleTimeInfo.GetStartTime(TRUE).Left(5);
		//结束时间
		m_strBundleTimeEndTime = BundleTimeInfo.GetEndTime(TRUE).Left(5);
		//包时扣费类型
		m_nBundleTimeType = BundleTimeInfo.GetBundleTimeType();
		//包时金额
		m_nBundTimeMoney = BundleTimeInfo.GetBundleTimeMoney();

		CString strBundleTimeMoney;
		strBundleTimeMoney.Format(_T("%.2f"), (m_nBundTimeMoney + m_nGuarantyMoney) / 100.0);
		m_edtMoney.SetWindowText(strBundleTimeMoney);

		// 2011-4-15-gxx
		m_DefineBundleTimeDlg.m_BundleTimeInfo.bIsSelected = FALSE;
	}
	else //没有选包时
	{
		m_edtMoney.SetWindowText(_T(""));
		m_cboBundTimeSel.SetCurSel(0);

		m_nBundleTimeId = 0;
		m_nBundleTimeType = 0;
		m_nBundTimeClass = 0;
	
		m_strBundleTimeStartTime.Empty();
		m_strBundleTimeEndTime.Empty();
		m_nBundTimeMoney = 0;
		m_strBundleTimeArea.Empty();
		m_strBalanceAfterBundleTime.Empty();

		// 2011-4-5-gxx
		m_DefineBundleTimeDlg.m_BundleTimeInfo.bIsSelected = FALSE;
	} 
}

void CRegisterDlg::EnableUserInfoControls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_PERSONALID)->EnableWindow(bEnable);
	
	((CEdit*)GetDlgItem(IDC_EDIT_USERNAME))->SetReadOnly(!bEnable);
	((CEdit*)GetDlgItem(IDC_EDIT_PERSONALID))->SetReadOnly(!bEnable);

	m_cboIDType.EnableWindow(bEnable);
	m_cboSex.EnableWindow(bEnable);
}

void CRegisterDlg::OnTimer(UINT nIDEvent)
{
	CNDDialog::OnTimer(nIDEvent);

	if (TimerIdOfDisableInput == nIDEvent)
	{
		if (CNetBarConfig::GetInstance()->GetMobileOpen())
		{
			GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(TRUE);
			((CEdit*)GetDlgItem(IDC_EDIT_IDCARDSNO))->SetReadOnly(FALSE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(FALSE);
		}
		

		if (m_bCanEditUserInfo)
		{
			if (m_bNetIdFromPersonalId)
			{
				m_cboIDType.EnableWindow(FALSE);
				m_cboUserClass.SetCurSel(0);
				m_cboUserClass.EnableWindow(FALSE);
			}
		}
		else
		{
			EnableUserInfoControls(m_bCanEditUserInfo);
		}
	}
	if (TimeIdOfAutoQuery == nIDEvent)
	{
		KillTimer(nIDEvent);

		// 再次获取焦点
		if (theApp.GetRealName()->RobFocusAfterSwipingCard())
		{
			SimulateClickMe();	
		}

		// 2012/05/15-8243-gxx: 
		if (m_bPhoneNumberOpen)
		{
			OnBnClickedBtnInputNumber();
		}

#ifndef _DEBUG
		//SetWindowPos(&CWnd::wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

		HWND hWnd = theApp.GetMainWnd()->GetSafeHwnd();
		if(NULL != hWnd)
		{
			// 先把主窗口设为最顶层
			::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );  
			//::SetWindowPos(GetSafeHwnd(), hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			// 这段代码将当前窗口带到带到最顶层，并设置为活动窗口
			HWND hForegdWnd = ::GetForegroundWindow();
			DWORD dwCurID = ::GetCurrentThreadId();
			DWORD dwForeID = ::GetWindowThreadProcessId(hForegdWnd, NULL);
			::AttachThreadInput(dwCurID, dwForeID, TRUE);
			::SetForegroundWindow(GetSafeHwnd());
			::AttachThreadInput(dwCurID, dwForeID, FALSE);


			// 设置金额输入框为输入焦点
			m_edtMoney.SetFocus();

		}

#endif
	}
}

BOOL CRegisterDlg::CheckRealNameData()
{
	BOOL bRet = TRUE;

	if (CNetBarConfig::GetInstance()->GetMustScan() == 1 
		&& theApp.GetRealName()->NeedToCheckRegisterData())
	{
		// 2012/04/23-8242-gxx:
		if (CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameGLW &&
			
			(m_strPersonalID != m_userInfo.GetCertificateID() ||
			m_strUserName != m_userInfo.GetUserName()))
		{
			IBA_LOG0(_T("开户信息和实名刷卡信息不符"));
			theApp.IBAMsgBox(_T("非法的开户信息"));

			return FALSE;
		}
		//

		if (!(m_bFromCard || m_bFromScan))
		{
			CIBALog::GetInstance()->Write(_T("CheckScan failed!"));

			bRet = FALSE;
		}

		if (m_bFromScan) //扫描网盾要求证件号码和上网账号要一致
		{
			if (m_strPersonalID.GetAt(0) == '0' || m_strPersonalID.GetLength() <= 10)
			{
				MsgBox(_T("证件号码长度不能小于11且第一位不能为0！"));

				bRet = FALSE;

				return bRet;
			}

			if (m_strNetId.CollateNoCase(m_strPersonalID) != 0) //不一致
			{
				bRet = FALSE;
			}
		}

		if (m_bFromCard) //网盾刷卡的要校验卡号是否有变
		{
			if (m_strNetId.CollateNoCase(m_strNetId2) != 0) //不一致
			{
				bRet = FALSE;
			}
		}

		if (!bRet)
		{
			MsgBox(_T("请先刷实名卡或者扫描证件！"));
		}
	}

	return bRet;
}

void CRegisterDlg::OnEnChangeEditPersonalid()
{
	if (m_bNetIdFromPersonalId)
	{
		UpdateData();
		m_strNetId = m_strPersonalID;
		UpdateData(FALSE);
	}
}

void CRegisterDlg::MakeTmpMemberInfo()
{
	((CButton*)GetDlgItem(IDC_RADIO_TEMPUSER))->SetCheck(TRUE);
	OnBnClickedRadioTempuser();

	//使默认的项目永远为"其他证件"
	int nIndex(-1);
	for(int i=0;i< m_cboIDType.GetCount();++i)
	{
		if(99 == m_cboIDType.GetItemData(i))
		{
			nIndex = i;
		}
	}
	if(0 <= nIndex)
	{
		m_cboIDType.SetCurSel(nIndex);
	}
	else
	{
		m_cboIDType.SetCurSel(m_cboIDType.GetCount() - 1);
	}	
	m_cboIDType.EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PERSONALID)->EnableWindow(FALSE);
	//设置自动账号
	GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(FALSE);
	//从数据库中取得数据
	m_strNetId = CIBADAL::GetInstance()->GetAutoNetIdFromRecord();

	m_strUserName = _T("普通用户");
	m_strPersonalID = m_strNetId;
}

void CRegisterDlg::OnCbnSelchangeComboUserclass()
{
	OnCbnSelchangeComboTempuserclass();
}

void CRegisterDlg::OnCbnSelchangeComboTempuserclass()
{
	GetRegisterInfo();

	CString strTmp;

	if (m_nGuarantyMoney > 0)
	{
		strTmp.Format(LOAD_STRING(IDS_REGDIPOSIT), m_nGuarantyMoney / 100.0);
	}

	GetDlgItem(IDC_STATIC_DEPOSIT)->SetWindowText(strTmp);

	InitBundleTimeComboBox();
}


void CRegisterDlg::OnBnClickedRadioTempuser()
{
	m_bIsTempUser = TRUE;
	m_cboUserClass.EnableWindow(FALSE);
	m_cboTempUserClass.EnableWindow(TRUE);
	OnCbnSelchangeComboTempuserclass();

	m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintTicketOpenTemp();
	UpdateData(FALSE);
}

void CRegisterDlg::OnBnClickedRadioMember()
{
	m_bIsTempUser = FALSE;
	m_cboUserClass.EnableWindow(TRUE);
	m_cboTempUserClass.EnableWindow(FALSE);
	OnCbnSelchangeComboUserclass();

	m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintTicketOpenMember();
	UpdateData(FALSE);
}

void CRegisterDlg::InitBundleTimeComboBox()
{
	#define NetbarBundleTime CNetBarConfig::GetInstance()->m_NetbarBundleTime.m_BundleTimeInfoArray

	m_cboBundTimeSel.ResetContent();

	CIBAString strTmp;
	strTmp.LoadString(IDS_NOTHING);
	m_cboBundTimeSel.SetItemData(m_cboBundTimeSel.AddString(strTmp), 0);
	m_cboBundTimeSel.SetCurSel(0);

	if (theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CUSTOMBUNDLE")))
	{
		m_btnSefDefine.ShowWindow(SW_SHOW);

		strTmp.LoadString(IDS_SELFDEFINEBUNDLETIME);
		m_cboBundTimeSel.SetItemData(m_cboBundTimeSel.AddString(strTmp), 9999);
	}

	for (INT i = 0; i < NetbarBundleTime.GetCount(); i++)
	{
		//判断用户级别
		if (NetbarBundleTime.GetAt(i).GetClassId() != m_nUserClassID)
		{
			continue;
		}

		CString strBundleTimeInfo;

		strBundleTimeInfo.Format(LOAD_STRING(IDS_BTINFO),
			NetbarBundleTime.GetAt(i).GetBundleTimeId(),
			NetbarBundleTime.GetAt(i).GetStartTime(TRUE).Left(5),
			NetbarBundleTime.GetAt(i).GetEndTime(TRUE).Left(5),
			NetbarBundleTime.GetAt(i).GetTimePass(), 
			NetbarBundleTime.GetAt(i).GetBundleTimeMoney() / 100.0,
			NetbarBundleTime.GetAt(i).GetBundleTimeTypeString());

		m_cboBundTimeSel.SetItemData(m_cboBundTimeSel.AddString(strBundleTimeInfo), 
			NetbarBundleTime.GetAt(i).GetBundleTimeId());
	}
}

void CRegisterDlg::InitUserClassComboBox()
{
	BOOL bDefIsMember = FALSE;
	INT nDefIdx = 0;

	//添加用户级别

	INT nMemberCount = 0;
	INT nTempCount = 0;
	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_UserClassArray.GetCount(); i++)
	{
		INT nIdx = 0;

		if (CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetIsMember())
		{
			nIdx = m_cboUserClass.AddString(CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassName());
			m_cboUserClass.SetItemData(nIdx, i);
			nMemberCount++;
		}
		else
		{
			nIdx = m_cboTempUserClass.AddString(CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassName());
			m_cboTempUserClass.SetItemData(nIdx, i);
			nTempCount++;
		}

		//默认级别
		if (CIBAConfig::GetInstance()->GetDefUserClassID() == 
			CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassID())
		{
			nDefIdx = nIdx;
			bDefIsMember = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetIsMember();
		}
	}

	//临时用户
	BOOL bHasTempUser = TRUE;

	if (m_cboTempUserClass.GetCount() == 0) //没有设置临时用户隐藏组合框
	{
		bHasTempUser = FALSE;
		m_cboTempUserClass.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_TEMPUSER)->ShowWindow(SW_HIDE);
	}
	else if (m_cboTempUserClass.GetCount() == 1) //只有一个临时用户的话就隐藏组合框
	{
		m_cboTempUserClass.ShowWindow(SW_HIDE);
	}
	
	if (m_cboTempUserClass.GetCount() > 0)
	{
		m_cboTempUserClass.SetCurSel(0); //默认选择第一个
	}

	//会员用户

	if (m_cboUserClass.GetCount() == 0) //没有会员用户
	{
		GetDlgItem(IDC_RADIO_MEMBER)->ShowWindow(SW_HIDE);
		m_cboUserClass.ShowWindow(SW_HIDE);
	}
	else
	{
		m_cboUserClass.SetCurSel(0);//默认选择第一个
	}

	//设置开户默认的用户级别,并触发改变选择事件

	//{ 2011/09/28-8201-gxx: 
	if (nTempCount == 0 && nMemberCount > 0)
	{
		if (bDefIsMember)
		{
			m_cboUserClass.SetCurSel(nDefIdx);
		}
		((CBCGPButton*)GetDlgItem(IDC_RADIO_MEMBER))->SetCheck(TRUE);
		OnBnClickedRadioMember();

		m_chkTempMember.ShowWindow(FALSE);
	}
	else if(nTempCount > 0 && nMemberCount == 0)
	{
		if (nDefIdx >= nTempCount)
		{
			nDefIdx = 0;
		}
		m_cboTempUserClass.SetCurSel(nDefIdx);
		((CBCGPButton*)GetDlgItem(IDC_RADIO_TEMPUSER))->SetCheck(TRUE);
		OnBnClickedRadioTempuser();
	}
	else
	{
		if (bDefIsMember) //默认是会员
		{
			m_cboUserClass.SetCurSel(nDefIdx);
			((CBCGPButton*)GetDlgItem(IDC_RADIO_MEMBER))->SetCheck(TRUE);
			OnBnClickedRadioMember();
		}
		else
		{
			m_cboTempUserClass.SetCurSel(nDefIdx);
			((CBCGPButton*)GetDlgItem(IDC_RADIO_TEMPUSER))->SetCheck(TRUE);
			OnBnClickedRadioTempuser();
		}
	}
	//}

	//{ 2013/05/03-gxx: 
	if (!CNetBarConfig::GetInstance()->IsEnableOpenMember())
	{
		GetDlgItem(IDC_RADIO_MEMBER)->ShowWindow(SW_HIDE);
		m_cboUserClass.ShowWindow(SW_HIDE);
		if (nTempCount > 0)
		{
			if (!bDefIsMember)
			{
				m_cboTempUserClass.SetCurSel(nDefIdx);
			}
			((CBCGPButton*)GetDlgItem(IDC_RADIO_TEMPUSER))->SetCheck(TRUE);
			OnBnClickedRadioTempuser();
		}
	}
	
	//}
	
	

	

}

void CRegisterDlg::InitIdTypeComboBox()
{
	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); i++)
	{
		int nIdx = m_cboIDType.AddString(CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeName());
		UINT nTmp = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeID();
		m_cboIDType.SetItemData(nIdx, nTmp);
	}

	//设置开户默认证件类型

	SelectIDType();
}

void CRegisterDlg::InitComputerComboBox()
{
	//采用临时的组合框先进行排序

	CBCGPComboBox* pTmpCbo = ((CBCGPComboBox*)GetDlgItem(IDC_COMBO_TEMPSORT));

	ASSERT(pTmpCbo);

	if (NULL == pTmpCbo)
	{
		return;
	}

	for (INT_PTR i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	{
		CComputerInfo & computer = CLocalServer::GetInstance()->ComputerList.GetComputer(i);

		if (computer.GetComputerStatus() == CComputerInfo::ECS_OFFLINE
			|| computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
		{
			INT nTmp = pTmpCbo->AddString(computer.GetTerminalID());

			pTmpCbo->SetItemData(nTmp, i);
		}
	}

	//实际的组合框不排序

	CIBAString strTmp;
	strTmp.LoadString(IDS_NOTHING);
	m_cboComputer.AddString(strTmp);

	for (INT i = 0; i < pTmpCbo->GetCount(); i++)
	{
		pTmpCbo->GetLBText(i, strTmp);
		
		INT nTmp = m_cboComputer.AddString(strTmp);
		m_cboComputer.SetItemData(nTmp, pTmpCbo->GetItemData(i));
	}

	//删除在线的

	CActiveMember ActiveMember;
	
	CSingleLock lock(&CLocalServer::GetInstance()->ActiveMemberList.ActiveMemberListLock, TRUE);
	
	CLocalServer::GetInstance()->ActiveMemberList.StartEnumActiveMember();

	while (CLocalServer::GetInstance()->ActiveMemberList.GetEachActiveMember(ActiveMember))
	{
		CString strFind;
		INT nTmp = m_cboComputer.SelectString(-1, ActiveMember.GetTerminalID());
		if (nTmp >= 0)
		{
			m_cboComputer.GetLBText(nTmp,strFind);
			if (strFind == ActiveMember.GetTerminalID())
			{
				m_cboComputer.DeleteString(nTmp);
			}
		}
	}

	m_cboComputer.SetCurSel(0);

	// 2012/03/21-8238-gxx: 
	if (CNetBarConfig::GetInstance()->GetRealNamePassword() == 4)
	{
		CWnd* pWnd = GetDlgItem(IDC_STATIC_6);
		if (pWnd)
		{
			pWnd->ShowWindow(FALSE);
		}
		m_cboComputer.ShowWindow(FALSE);
	}
}

/*************************************************
Function:       // AutoCheckIn
Description:    //
Calls:          //
Table Accessed: //
Table Updated:  //
Input:          //
                //
Output:         //
Return:         //
Others:         //
*************************************************/
void CRegisterDlg::AutoCheckIn()
{
	if (m_cboComputer.GetCurSel() > 0) //有没有选择电脑
	{
		INT_PTR nIdx = m_cboComputer.GetItemData(m_cboComputer.GetCurSel());

		//是不是有效的电脑

		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(nIdx))
		{
			CComputerInfo & computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIdx);
			if (computer.GetComputerStatus() == CComputerInfo::ECS_OFFLINE
				|| computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
			{
				{
					CActiveMember ActiveMember;
					if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
						m_nMemberId, ActiveMember))
					{
						ActiveMember.SetTerminalID(computer.GetTerminalID());
						ActiveMember.SetPCClassID(computer.GetPCClassID());

						CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
						CLocalServer::GetInstance()->ActiveMemberList.RefreshView();
					}
				}
				//添加到数据库
				CLastUserInfo LastUserInfo;
				LastUserInfo.SetNetId(m_strNetId);
				LastUserInfo.SetMemberId(m_nMemberId);
				LastUserInfo.SetTermId(computer.GetTerminalID());
				LastUserInfo.SetLoginStyle(1); //强制登录

				// 2014-10-27 - qsc 智多实名，手动输入密码
				CString strPwd;
				if(CIBAGlobal::emRealnameZD != CNetBarConfig::GetInstance()->GetAuditType())
				{
					strPwd = CIBAHelpper::EncodePassword(m_strPassword);
				}
				else
				{
					CDlgInputUserPwd dlg;
					if(IDOK == dlg.DoModal())
					{
						strPwd = CIBAHelpper::EncodePassword(dlg.GetPwd(), false);
					}
					else
					{
						return ;
					}
				}

				

				LastUserInfo.SetPassWord(strPwd);
				LastUserInfo.SetUpdateTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));

				CIBADAL::GetInstance()->DeleteLastUserInfo(LastUserInfo.GetTermId());
				CIBADAL::GetInstance()->AddLastUserInfo(LastUserInfo);

				//请求登录
				if (computer.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
				{
					CLocalServer::GetInstance()->LocalAutoCheckIn(nIdx);
				}
			}
			else
			{
				MsgBox(IDS_TERMTAKEUP);
			}
		}
	}
}
void CRegisterDlg::OnBnClickedBtnCustomBoundletime()
{
	if (m_DefineBundleTimeDlg.DoModal() == IDOK)
	{
		if ( m_DefineBundleTimeDlg.m_BundleTimeInfo.bIsSelected )
		{
			IBA_ASSERT(m_DefineBundleTimeDlg.m_BundleTimeInfo.Amount<=999999);

			CString str;
			str.Format(_T("%.2f"),
				(m_DefineBundleTimeDlg.m_BundleTimeInfo.Amount + m_nGuarantyMoney) / 100.0);

			m_nBundTimeMoney = m_DefineBundleTimeDlg.m_BundleTimeInfo.Amount;

			m_edtMoney.SetWindowText(str);
		}

		m_cboBundTimeSel.SetCurSel(1);
	}
	else
	{
		m_cboBundTimeSel.SetCurSel(0);
	}
}

void CRegisterDlg::RegisterLocalUser()
{
	// 2011/07/26-8201-gxx: 判断上网账号是否重复
	CString strSQL;
	strSQL.Format(_T("select * from ActiveMember where netId ='%s'"), m_strNetId);

	//try
	//{
	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) )
	{
		if (Rs.GetRecordCount() != 0)
		{
			theApp.IBAMsgBox(_T("开户失败,上网账号重复"));

			Rs.Close();
			pDb->Release();
			return;
		}
		Rs.Close();	
	}
	pDb->Release();
	//}
	//catch(...){}

	CLocalRegisterInfo RegisterInfo;
	RegisterInfo.SetNetbarId( CNetBarConfig::GetInstance()->GetNetBarId() );
	RegisterInfo.SetIdType(m_nIdType);
	RegisterInfo.SetUserClass( m_nUserClassID );
	RegisterInfo.SetPersonalId( m_strPersonalID );
	RegisterInfo.SetUserName( m_strUserName );
	RegisterInfo.SetSex(m_cboSex.GetCurSel());
	RegisterInfo.SetPassword(m_strPassword);
	RegisterInfo.SetMemberId( CBrokenNetwork::RegisterMemberID() );

	CString strTmp = m_strNetId;
	//在账号前段插入0
	if (strTmp.GetLength() < 10)
	{
		strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
	}

	RegisterInfo.SetNetId( strTmp );
	RegisterInfo.SetMoney( m_nCreditMoney );

	//增加到用户视图
	CCurrentNetBarUser::GetInstance()->SetNetId(strTmp);
	
	CLocalServer::GetInstance()->ActiveMemberList.AddActiveMember(RegisterInfo);

		//CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
	/*theApp.UpdateToolbarLastID(CCurrentNetBarUser::GetInstance()->
		Balance.GetSerailNum());*/   ////Temporarily comment out

	ShowRegisterResult(TRUE);

	m_nMemberId = RegisterInfo.GetMemberId();

	AutoCheckIn();

	// 2015/5/15 tmelody
	// 记录到本地
	// {
	// 构造本地记录信息
	USERINFO userInfo;
	userInfo._strAccount = m_strNetId;
	userInfo._strMemberID.Format(_T("%d"), m_nMemberId);
	userInfo._strPwd = m_strPassword;
	userInfo._strClassID.Format(_T("%d"), m_nUserClassID);
	userInfo._bLocalRegister = TRUE;
	
	// 记录注册信息
	CLocalCenter::GetInstance()->RecordRegisteredUser(userInfo);
	// }

	CDialog::OnOK();
}

void CRegisterDlg::RegisterCenterUser()
{
	IBA_LOG0(_T("Enter 注册中心用户..."));

	//if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		// 2011/07/26-8201-gxx: 判断上网账号是否重复，改为从Member表判断
		CString strSQL;
		strSQL.Format(_T("select * from ActiveMember where netId ='%s'"), m_strNetId);
		//strSQL.Format(_T("select * from member where serialNum ='%s'"), m_strNetId);

		//try
		//{
		CADODBConnInfo* pDb = NULL;

		if (CIBADAL::GetInstance()->GetDBConnInfo(pDb)) 
		{
			CADORecordset Rs(pDb->GetConn());

			if (Rs.Open(strSQL) )
			{
				if (Rs.GetRecordCount() != 0)
				{
					theApp.IBAMsgBox(_T("开户失败,上网账号重复"));
					IBA_LOG0(_T("Leave 注册中心用户 失败: 开户失败,上网账号重复"));

					Rs.Close();
					pDb->Release();
					return;
				}
				Rs.Close();	
			}
			pDb->Release();
		}

	//	}
	//	catch(...){}
	}

	if (!GetCardIdFromCenter())
	{
		IBA_LOG0(_T("Leave 注册中心用户 失败: 从中心检验卡号"));
		return;//从中心检验卡号
	}

	SetRegisterInfo();//设置注册方法信息

	theApp.GetCurCashier()->DoRegister(UserRegister);

	if (UserRegister.GetStatusCode() == 0) //成功
	{
		//增加到用户视图
		CCurrentNetBarUser::GetInstance()->Balance.SetCardId(_ttoi(m_strCardId));
		CCurrentNetBarUser::GetInstance()->Balance.SetMemberId(UserRegister.GetMemberId());
		theApp.GetCurCashier()->DoBalance(CCurrentNetBarUser::GetInstance()->Balance);
		if (CCurrentNetBarUser::GetInstance()->Balance.GetStatusCode() == 0)
		{
			CLocalServer::GetInstance()->ActiveMemberList.AddActiveMember(
				CCurrentNetBarUser::GetInstance()->Balance,
				m_nBundleTimeId!=0,
				m_nBundTimeMoney,
				CCurrentNetBarUser::GetInstance()->Balance.GetPayType(),
				m_userInfo.GetScanType()
				);

			CCurrentNetBarUser::GetInstance()->SetNetId(CCurrentNetBarUser::GetInstance()->Balance.GetSerailNum());
			//CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

			//theApp.UpdateToolbarLastID(CCurrentNetBarUser::GetInstance()->
			//	Balance.GetSerailNum());  ////Temporarily comment out
		}

		m_bNetIdFromPersonalId = FALSE;
		m_bCanEditUserInfo = (CNetBarConfig::GetInstance()->GetMustScan() == 0); //是否编辑用户信息

		m_bAfterRegister = TRUE;

		m_edtMoney.SetWindowText(_T(""));

		//显隐重复打印按钮
		OnBnClickedCheckPrint();

		m_nMemberId = UserRegister.GetMemberId();
		m_nCreditPresent = UserRegister.GetCreditPresent();
		
		SaveRegisterInfo();//写信息到数据库

		// 2015/5/15 tmelody
		// 记录到本地
		// {
		USERINFO userInfo;
		userInfo._strAccount = m_strNetId;
		userInfo._strMemberID.Format(_T("%d"), m_nMemberId);
		userInfo._strPwd = m_strPassword;
		userInfo._strClassID.Format(_T("%d"), m_nUserClassID);
		userInfo._bLocalRegister = FALSE;
		CIBALog::GetInstance()->WriteFormat(_T("中心注册成功------------"));
		CIBALog::GetInstance()->WriteFormat(_T("NetId - %s"),m_strNetId);
		CIBALog::GetInstance()->WriteFormat(_T("MemberId - %d"),m_nMemberId);		
		CLocalCenter::GetInstance()->RecordRegisteredUser(userInfo);

		if (!IsBundleTime())
		{
			ShowRegisterResult();//显示注册结果
			if (m_bPrintTicket)
			{
				PrintTicket();
			}
		}

		BOOL bLoad = TRUE;

		if (bLoad)
			AutoUploadImage();//上传照片

		IBA_LOG0(_T("注册中心用户 will end!"));
		// 2012/09/26-8249-gxx: 深圳网监专用
		// 2014-3-6 17:48:14 - qsc
		//OpenRServer();

		//try
		//{
		AutoCheckIn();
		//}
		//catch(...)
		//{
		//	IBA_LOG0(_T("自动登入出错"));
		//}

		/*try
		{*/
		AutoClose(IDOK);
		//}
	/*	catch(...)
		{
			IBA_LOG0(_T("自动关闭对话框出错"));
		}*/
		
		/*AutoCheckIn();
		AutoClose(IDOK);*/
		CIDCheckCardMethod IDCheckCard;
		IDCheckCard.SetSerialNum(m_strNetId);
		IDCheckCard.SetSerialNumType(1);

		theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

		if (IDCheckCard.GetStatusCode() == 0)
		{
			int nCardId = IDCheckCard.GetCardId();

			CBalanceMethod& Balance = CCurrentNetBarUser::GetInstance()->Balance;
			Balance.SetMemberId(m_nMemberId);
			Balance.SetCardId(nCardId);
			Balance.SetRound(0);

			theApp.GetCurCashier()->DoBalance(Balance);

			int nStatus = Balance.GetStatusCode();
			if (nStatus == 0)
			{
				int nTotal = Balance.GetTotalBalacne();
				int nRemain = Balance.GetBalance();
				int n = Balance.GetLastConsumeAmount();
				int m = 0;
			}
		}
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(UserRegister.GetStatusMessage(), TRUE);

		IBA_LOG(_T("注册结果: %s"), UserRegister.GetStatusMessage());
	}

	IBA_LOG0(_T("Leave 注册中心用户"));
}

void CRegisterDlg::ResetNetworkBrokenControl()
{
	GetDlgItem(IDC_RADIO_MEMBER)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_USERCLASS)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GROUP)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_BTSEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BUNDLETIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_BTN_CUSTOM_BOUNDLETIME)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_PRESENT)->ShowWindow(FALSE);

	CIBAString str;
	str.LoadString(IDS_OPENLOCALUSER);
	SetWindowText(str);

	CEdit* pEdit = NULL;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_IDCARDSNO);
	pEdit->EnableWindow(TRUE);
	pEdit->SetReadOnly(FALSE);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_USERNAME);
	pEdit->EnableWindow(TRUE);
	pEdit->SetReadOnly(FALSE);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PERSONALID);
	pEdit->EnableWindow(TRUE);
	pEdit->SetReadOnly(FALSE);
}

void CRegisterDlg::OnBnClickedBtnInputNumber()
{
	CDlgInputMobile dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_strNetId = dlg.m_strPhoneNumber;
		m_strPersonalID = dlg.m_strPhoneNumber;
		m_strUserName = _T("手机号码");
		UpdateData(FALSE);

		m_bFromScan = TRUE;
		m_cboIDType.SelectString(0, _T("其他证件"));

		GetDlgItem(IDC_EDIT_MONEY)->SetFocus();
	}
}

void CRegisterDlg::SetPhoneNumberOpen()
{
	m_bPhoneNumberOpen = TRUE;
}

void CRegisterDlg::OpenRServer()
{
	CString strIniFile = theApp.GetWorkPath();
	strIniFile.Append(_T("\\IBAConfig\\nde_server.ini")); // base, Filename
	if (!::PathFileExists(strIniFile))
	{
		IBA_LOG(_T("配置文件: %s 不存在"), strIniFile);
		return;
	}
	TCHAR tRes[512] = {0};
	::GetPrivateProfileString(_T("base"), _T("Filename"), NULL, tRes, 512, strIniFile);
	if (!::PathFileExists(tRes))
	{
		IBA_LOG(_T("执行文件: %s 不存在"), tRes);
		return;
	}/*
	添加用户:add|用户名|密码

		删除用户:del|用户名

*/
	CString strNetID = m_strNetId;
	if (strNetID.GetLength() < 10)
	{
		INT nFil = 10 - strNetID.GetLength();
		for (; nFil > 0; nFil --)
		{
			strNetID.Insert(0, _T('0'));
		}
	}
	INT nFil = 8 - m_strPassword.GetLength();
	CString strPwd = m_strPassword;
	for (; nFil > 0; nFil --)
	{
		strPwd.AppendChar(_T('0'));
	}

	CString strParam;
	strParam.Format(_T("add|%s|%s"), strNetID, strPwd);

	IBA_LOG(_T("执行: %s %s"), tRes, strParam);
	
	ShellExecute(NULL, _T("open"), tRes, strParam, NULL, SW_SHOWNORMAL);
}

void CRegisterDlg::OnBnClickedBtnCapphtoto()
{
	// TODO: 在此添加控件通知处理程序代码
	if(1 != CNetBarConfig::GetInstance()->GetEnableJuBao())
		return;
	// 验证在拍照对话框里面完成
	CDlgCapPhoto dlg;
	if(IDOK == dlg.DoModal())
	{
		dlg.GetNameID(m_strUserName, m_strPersonalID);
		m_strNetId = m_strPersonalID;
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetWindowText(m_strNetId);
		GetDlgItem(IDC_EDIT_USERNAME)->SetWindowText(m_strUserName);
		GetDlgItem(IDC_EDIT_PERSONALID)->SetWindowText(m_strNetId);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		UpdateData();
	}
}

void CRegisterDlg::OnBnClickedBtnRnReg()
{
	IBA_LOG0(_T("点击实名注册"));
	m_bClickRnReg = TRUE;
	OnBnClickedOk();
}

LRESULT CRegisterDlg::OnCanClose(WPARAM wP,LPARAM lP)
{
	IBA_LOG0(_T("Enter CRegisterDlg::OnCanClose"));
	if(1 <= CNetBarConfig::GetInstance()->GetSoftSfreg() 
		&& TRUE == m_bWaitReg)
	{
		m_bWaitReg = FALSE;
		RegisterCenterUser();

		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			ResetNetworkBrokenControl();
		}

		if(1 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
		{
			CString strPhotoPath;
			if(NS_REALNAME::CRZXRealName::GetPhotoPath(strPhotoPath, m_strPersonalID))
			{	
				CString strPath, strNewFile;
				strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("UserImages"));
				CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
				strNewFile.Format(_T("%s\\%s.jpg"), strPath, CIBAHelpper::FormatNetId(m_strNetId));
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
	}
	IBA_LOG0(_T("Leave CRegisterDlg::OnCanClose"));
	return 0;
}

void CRegisterDlg::SendRegDataToProxy()
{
	UpdateData();
	if(!PathFileExists(theApp.GetWorkPath() + _T("\\IBABin\\AuditInterface.dll")))
	{
		return ;
	}

	CAuditInterface auditInterface;
	if(!auditInterface.Load())
	{
		IBA_LOG0(_T("加载实名接口动态库AuditInterface.dll失败！请检查AuditInterface.dll的版本！"));
		return ;
	}
	auditInterface.uninit_();
	CStringA strIpA = CT2A(CNetBarConfig::GetInstance()->GetProxyIP());
	auditInterface.init_(strIpA, 7834);
	CString strSex;
	// 性别
	m_cboSex.GetLBText(m_cboSex.GetCurSel(), strSex);

	int isex;
	if (strSex == _T("男")) isex = 1;else isex  = 0;
	int nMemberId= 0;
	int nNation = NS_REALNAME::CRZXRealName::GetNationCode(m_strNation);
	UINT nNetbarId = CNetBarConfig::GetInstance()->GetNetBarId();
	COleDateTime dt;
	dt = COleDateTime::GetCurrentTime();
	CString strDatetime;
	strDatetime = dt.Format(_T("%Y-%m-%d"));

	CString validDate("2050-01-01");
	CString strBrithday = NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ(m_strPersonalID);

	CStringA strPersonalIDA = CT2A(m_strPersonalID);
	CStringA strNameA = CT2A(m_strUserName);
	CStringA strAddrA = CT2A(m_userInfo.GetAddr());
	CStringA strTelNumA = CT2A(m_strTelNum);

	CStringA strBrithdayA = CT2A(strBrithday);
	CStringA strDatetimeA = CT2A(strDatetime);
	CStringA strValidDateA = CT2A(validDate);
	CStringA strOfficerA = CT2A(m_strOfficer);
	CStringA strNetIdA = CT2A(m_strNetId);

	int retcode = -1;
	try
	{
	int ret = auditInterface.regUser_(nNetbarId, m_nIdType, strPersonalIDA, strNameA, 
		isex, "CN", strAddrA, strTelNumA, nMemberId, strBrithdayA,
		strDatetimeA, strValidDateA, nNation, strOfficerA, m_nUserClassID, strNetIdA, &retcode);

	if (ret == 0)
	{
		if (retcode == 1 || retcode == -6 )
		{
			IBA_LOG0(_T("向实名接口传注册信息成功（任子行实名）!"));
		}
	}
	else if (ret == ERROR_NETWORK_FAILED)
	{
		IBA_LOG0(_T("网络故障,上传用户信息失败!"));
	}
	else
	{
		IBA_LOG0(_T("其它异常!"));
	}
	}
	catch(...)
	{
		IBA_LOG(_T("向实名注册抛出异常:%d"), GetLastError());
	}
	
}

void CRegisterDlg::ExeSfreg()
{
	IBA_LOG0(_T("CRegisterDlg::ExeSfreg"));
	UpdateData();
	// -A证件号码	-B证件类别(文字)   -C姓名		-D性别
	// -E地址		-F电话号码		-G国籍		-H会员号
	// -I证件类别编码 -J民族			-K发证单位	-L卡类型 
	// -M印刷号 -R是否只读
	CString strParam, strIDType, strSex;

	// 获取证件类型
	m_cboIDType.GetLBText(m_cboIDType.GetCurSel(), strIDType);
	// 性别
	m_cboSex.GetLBText(m_cboSex.GetCurSel(), strSex);

	strParam.Format(_T("-A%s -B%s -C%s -D%s -E%s -F%s -G%s -H%d -I%d -J%s -K%s -L%d -M%s -R%d"), 
		m_strPersonalID, strIDType, m_strUserName, strSex,
		m_userInfo.GetAddr(), m_strTelNum, _T("CN"), 0, 
		m_nIdType, m_strNation, m_strOfficer, m_nUserClassID,
		m_strNetId, m_bCanEditUserInfo);
	CString str = m_userInfo.GetTel();
	IBA_LOG(_T("传递给SFREG的信息:%s"),strParam);
	const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
	const CString strDest	  = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\imgID.jpg");

	if(PathFileExists(strSource))		// 拷贝一个做备份，方便查看
	{
		if(!CopyFile(strSource, theApp.GetWorkPath() + _T("\\IBATemp\\ID1.jpg"), FALSE))
		{
			IBA_LOG(_T("拷贝%s失败1!,%d"), strSource, GetLastError());
		}
	}

	if(PathFileExists(strDest))
	{
		if(!DeleteFile(strDest))
		{
			IBA_LOG(_T("删除%s失败!, %d"), strDest, GetLastError());
		}
	}

	if(PathFileExists(strSource))
	{
		if(!CopyFile(strSource, strDest, FALSE))
		{
			IBA_LOG(_T("拷贝%s失败!,%d"), strSource, GetLastError());
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
			IBA_LOG(_T("删除%s失败!, %d"), strSource, GetLastError());
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
}


void CRegisterDlg::OnBnClickedButtonQueryTel()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	CMemberInfoMethod MemberInfo;
	
	CString strTmp = m_strNetId;
	// 2014-3-18 - qsc 补0
	//在账号前段插入0
	if (strTmp.GetLength() < 10)
	{
		strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
	}

	MemberInfo.SetSerialNum(strTmp);
	MemberInfo.SetPersonalId( m_strPersonalID );
	MemberInfo.SetUsername( m_strUserName );
	MemberInfo.SetAvailable(0);// 0全部查询, 1有效查询
	theApp.GetCurCashier()->DoMemberInfo(MemberInfo);

	if(0 == MemberInfo.GetStatusCode() &&
		0 < MemberInfo.GetMemberCount())
	{
		m_strTelNum = MemberInfo.GetTel(0);
		UpdateData(FALSE);
	}
}

void CRegisterDlg::OnBnClickedBtnPhoto()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CIBAHelpper::TakePhoto(CIBAHelpper::GetUserZcImgPath(m_nIdType, m_strPersonalID), m_strPersonalID);
}

void CRegisterDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
	{
		if(!m_userInfo.GetUserName().IsEmpty())
		{
			NS_REALNAME::CheckoutInfo coi;
			ZeroMemory(&coi, sizeof(coi));
			_tcsncpy(coi.szName, m_userInfo.GetUserName(), 100);
			_tcsncpy(coi.szNetId, m_userInfo.GetNetId(), 100);

			coi.nCheckoutType = CIBAGlobal::cot_TempLogout;

			theApp.GetRealName()->CheckOut(&coi);
		}	
	}
	CNDDialog::OnCancel();
}
