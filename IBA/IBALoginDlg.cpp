// IBALoginDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "BossPwdDlg.h"
#include "NetBarInfoPage.h"
#include ".\ibalogindlg.h"
#include "dal\IBADAL.h"
#include "AllCheckOut.h"
#include ".\RealName\IBARealName.h"

using namespace NS_DAL;

// CIBALoginDlg 对话框

IMPLEMENT_DYNAMIC(CIBALoginDlg, CDialog)
CIBALoginDlg::CIBALoginDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CIBALoginDlg::IDD, pParent)
, m_nLoginType(0)
{
	EnableVisualManagerStyle(FALSE);
}

CIBALoginDlg::~CIBALoginDlg()
{
}

void CIBALoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPassword);
	DDX_Control(pDX, IDC_COMBO_CASHIER, m_cboOperators);

	DDX_Control(pDX, IDOK, m_btnLogin);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_SET, m_btnSet);
	DDX_Control(pDX, IDC_BUTTON_ENMERGENCY, m_btnEnmergency);
}

BEGIN_MESSAGE_MAP(CIBALoginDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnBnClickedButtonSet)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ENMERGENCY, OnBnClickedButtonEnmergency)
END_MESSAGE_MAP()

// CIBALoginDlg 消息处理程序

void CIBALoginDlg::OnBnClickedOk()
{
	UpdateData();

	CString strOperater, strMsg;
	m_cboOperators.GetWindowText(strOperater);
	strOperater.Trim();//去除首尾空格

#ifdef DEBUG
	if( strOperater==_T("gxx") )
	{
		//m_strPassword = _T("123");
	}
#endif

	if (!CheckInput(strOperater))
	{
		return;
	}
	//登入方法
	theApp.GetCurCashier()->SetName(strOperater);
	theApp.GetCurCashier()->SetPassword(m_strPassword);

	//Normal或Restore模式，进行收银台登陆，因为中间可能会弹出窗口或等待，因此要将按钮设为无效
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);//暂时使按钮无效，退出前再使按钮有效

		CCashRegisterLoginMethod_atLogin crLogin;

		//theApp.GetCurCashier()->CheckIn(crLogin, strOperater);
		theApp.GetCurCashier()->CheckIn_atLogin(crLogin, strOperater);

		//中心返回码
		if (crLogin.GetStatusCode() > 0) 
		{	
			SetToolTipPos(IDC_COMBO_CASHIER);

			switch (crLogin.GetStatusCode())
			{
			case NS_SERVLET::ESC_MACJYC30:     // MAC校验错
				{
					strMsg.Format(LOAD_STRING(IDS_MACERROR), CIBAConfig::GetInstance()->GetCashRegisterID(), crLogin.GetPSK().Left(12));
					CIBAHelpper::PutTextToClipboard(crLogin.GetPSK().Left(12));

					GetDlgItem(IDC_BUTTON_SET)->ShowWindow(SW_SHOW);
				}
				break;

			case NS_SERVLET::ESC_WXSYTID50:    // 无效收银台ID
				{
					strMsg.Format(LOAD_STRING(IDS_CASHREGISTERID), CIBAConfig::GetInstance()->GetCashRegisterID(), crLogin.GetPSK().Left(12));
					CIBAHelpper::PutTextToClipboard(crLogin.GetPSK().Left(12));

					GetDlgItem(IDC_BUTTON_SET)->ShowWindow(SW_SHOW);
				}
				break;

			case NS_SERVLET::ESC_WXMM15:       // 无效密码
				{
					strMsg = LOAD_STRING(IDS_LOGINERROR);
				}	
				break;

			default:
				{
					IBA_TRACE("登录时，中心返回码默认处理");

					strMsg = crLogin.GetStatusMessage();
				}		
			}

			m_strPassword.Empty();
			UpdateData(FALSE);

			ShowToolTip(strMsg);

			GetDlgItem(IDOK)->EnableWindow(TRUE);//恢复窗口
			return;
		}
		else if (crLogin.GetStatusCode() < 0) 
		{
			SetToolTipPos(IDOK);
			//ShowToolTip(strMsg);

			GetDlgItem(IDC_BUTTON_SET)->ShowWindow(SW_SHOW);

			if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
			{
				// 2011/07/13-8201-gxx: 进行本地登录
				if (!theApp.GetCurCashier()->LocalModeLogin())
				{
					MessageBox(_T("网络连接中断,进行本地登录时验证收银员和密码失败,请使用前一次正常登录的账号和密码!"),
						NULL,MB_OK|MB_ICONINFORMATION);

					GetDlgItem(IDOK)->EnableWindow(TRUE); //按钮恢复
					return ;
				}
			}
			
			//已经在theApp.GetCurCashier()->CheckIn_atLogin中进入本地模式了
			//CBrokenNetwork::GetInstance()->DisableNetwork();
			//IBA_LOG(L"m_threadTestResume => Start");
			//CBrokenNetwork::GetInstance()->m_threadTestResume->Start();//开启线程。网络恢复后，为不在线用户补传本地记录线程函数，网络确定恢复以后启动。只有在网络状态为正常后才启动。恢复完后从localMember删除记录。

			//return;
		}

		#ifndef _DEBUG // release 版本

		// 2011/07/14-8201-gxx: 添加断网的判断
		if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
		{
			//弹出通知
			if (!crLogin.GetNoticeInfo().IsEmpty())
			{
				MsgBox(crLogin.GetNoticeInfo());
			}

			//弹出通知网络连接
			if (!crLogin.GetPopupURL().IsEmpty())
			{
				ShellExecute(NULL, _T("open"), crLogin.GetPopupURL(), NULL, NULL, SW_NORMAL);
			}

			//同步本地时间
			CIBAHelpper::SyncLocalTime(CIBAHelpper::CenterTimeToOleDateTime(crLogin.GetServerTime()));
		}

		#endif
	}
	else
	{
		// 2011/07/13-8201-gxx: 进行本地登录
		if (!theApp.GetCurCashier()->LocalModeLogin())
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);

			MessageBox(_T("网络连接中断,进行本地登录时验证收银员和密码失败,请使用前一次正常登录的账号和密码!"),
				NULL,MB_OK|MB_ICONINFORMATION);

			GetDlgItem(IDOK)->EnableWindow(TRUE);
			return ;
		}
		
		//CBrokenNetwork::DisableNetwork();
	}

	theApp.GetRealName()->OnCashierCheckIn();

	CBCGPDialog::OnOK();
}

BOOL CIBALoginDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	LoadImage();
	
	SetEditFont();

	InitOperatorsCombo();
	GetDlgItem(IDC_COMBO_CASHIER)->SetFont(&m_Font);
	//设置登入框的位置
	if (m_nLoginType == 0)
	{//被覆盖窗体依然处于最顶端，但失去激活
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 318, 242, SWP_NOMOVE);
		GetDlgItem(IDC_EDIT_PWD)->SetFocus();
	}
	else
	{//不做原设置，只移动新窗体
		SetWindowPos(NULL, 0, 0, 318, 242, SWP_NOMOVE);
		m_cboOperators.SetFocus();
	}

	CRgn MyRgn;
	MyRgn.CreateRoundRectRgn(0, 0, 318, 242, 12, 12);
	SetWindowRgn(MyRgn, FALSE);

	//{ 2011/09/05-8201-gxx: 
	m_btnEnmergency.ShowWindow(FALSE);
	WINDOWPLACEMENT wp;
	m_btnEnmergency.GetWindowPlacement(&wp);
	CString strAllCheckOut;
	m_btnEnmergency.GetWindowText(strAllCheckOut);
	m_btnAllCheckOut.Create( strAllCheckOut, WS_VISIBLE|WS_CHILD|SS_NOTIFY,wp.rcNormalPosition,this,IDC_BUTTON_ENMERGENCY);
	m_btnAllCheckOut.SetFont(&m_FontSmallBold);
	m_btnAllCheckOut.SetBkColor(RGB(245, 236, 216));
	m_btnAllCheckOut.SetLinkColor(RGB(218,186,129));
	m_btnAllCheckOut.SetHoverColor(RGB(235, 207, 156));
	m_btnAllCheckOut.SetVisitedColor(RGB(218, 186, 129));
	m_btnAllCheckOut.SetTransparent(FALSE);
	m_btnAllCheckOut.ShowWindow(CIBAConfig::GetInstance()->GetEnmergencyVisible());
	
	//}
	
	//m_btnEnmergency.ShowWindow(CIBAConfig::GetInstance()->GetEnmergencyVisible());

	
	//如果没有设置网吧ID或者识别码
	if (CNetBarConfig::GetInstance()->GetNetBarId() == 0 
		|| CNetBarConfig::GetInstance()->GetNetBarKey().GetLength() < 16)
	{
		CDlgNetbarConfig dlg;
		if (dlg.DoModal() == IDOK)
		{
			CNetBarConfig::GetInstance()->LoadFromDB();
		}
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIBALoginDlg::OnCancel()
{
	// 2015-0828 liyajun
	CRegKey regKey;
	LONG lRet = regKey.Open(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA"));

	if (ERROR_SUCCESS == lRet)
	{
		DWORD dwValue = 0;
		lRet = regKey.QueryDWORDValue(_T("IsFN"), *(DWORD*)&dwValue);
		if (ERROR_SUCCESS == lRet)
		{
			regKey.DeleteValue(_T("IsFN"));
			regKey.DeleteSubKey(_T("IsFN"));
		}

		regKey.Close();
	}
	CIBADialog::OnCancel();
}

void CIBALoginDlg::InitOperatorsCombo()
{
	COperatorArray OperatorArray;
	CIBADAL::GetInstance()->GetOperatorRecord(OperatorArray);
	if (OperatorArray.GetCount() > 0)
	{//从数据库添加
		for (INT i = 0; i < OperatorArray.GetCount(); i++)
		{
			m_cboOperators.AddString(OperatorArray.GetAt(i));
		}
	}
	else
	{//数据库无数据，从注册表添加
		CString strTmp = theApp.GetProfileString(_T("NetBarInfo"), _T("Operators"));

		CStringArray strArray;

		CIBAHelpper::SplitLine(strTmp, strArray);

		for (INT i = 0; i < strArray.GetCount(); i++)
		{
			m_cboOperators.AddString(strArray.GetAt(i));
		}
	}
	m_cboOperators.SetWindowText(theApp.GetProfileString(_T("NetBarInfo"), _T("LastOperator"), 0));

}

BOOL CIBALoginDlg::CheckInput(CString &strOperater)
{
	if (strOperater.IsEmpty())
	{	
		SetToolTipPos(IDC_COMBO_CASHIER);
		ShowToolTip(IDS_INPUTACCOUNT);
		m_cboOperators.SetFocus();
		return FALSE;
	}

	m_strPassword.Trim();

	if (m_strPassword.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

		return FALSE;
	}

	return TRUE;
}

void CIBALoginDlg::OnBnClickedButtonSet()
{
	CDlgBossPwd dlg;

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CDlgNetbarConfig dlg2;
	dlg2.DoModal();
}

BOOL CIBALoginDlg::OnEraseBkgnd(CDC* pDC)
{
	if (m_BKImage.IsNull())
	{
		CIBADialog::OnEraseBkgnd(pDC);
	}
	else
	{
		m_BKImage.Draw(pDC->GetSafeHdc(), 0, 0);
	}

	m_btnLogin.SetBkGnd(pDC);
	m_btnCancel.SetBkGnd(pDC);
	m_btnSet.SetBkGnd(pDC);
	m_btnEnmergency.SetBkGnd(pDC);

	return TRUE;
}

void CIBALoginDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ReleaseCapture();//取消焦点
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); //发送消息

	CIBADialog::OnLButtonDown(nFlags, point);
}

void CIBALoginDlg::LoadImage()
{
	m_BKImage.Load(theApp.GetWorkPath() + _T("\\IBAImage\\LoginBK.png"));//背景
	m_btnLogin.LoadStdImageFile(theApp.GetWorkPath() + _T("\\IBAImage\\LoginBtn.png"));//按钮
	m_btnCancel.LoadStdImageFile(theApp.GetWorkPath() + _T("\\IBAImage\\CancelBtn.png"));//取消
	m_btnSet.LoadStdImageFile(theApp.GetWorkPath() + _T("\\IBAImage\\SetBtn.png"));//设置

	m_btnEnmergency.LoadStdImage(IDR_PNG_ENMERGENCY, _T("PNG"));
}

void CIBALoginDlg::OnBnClickedButtonEnmergency()
{
	CDlgAllCheckOut dlg;
	dlg.DoModal();
}

void CIBALoginDlg::SetLoginType(UINT newVal)
{

	m_nLoginType = newVal;
}

LRESULT CIBALoginDlg::OnShowToolTip(WPARAM wParam, LPARAM lParam)
{
	SetToolTipPos(IDOK);
	CString strShow(_T("原中心无法连接，正在切换中心登录"));
	ShowToolTip(strShow);
	return 0;
}

