// NetBarInfoPage.cpp : 实现文件

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "NetBarOtherInfoPage.h"
#include "Socket\LocalServer.h"
#include "Servlet\WinHttpRequest.h"
#include "servlet\localmodemethod.h"
#include "Cashier.h"
#include "localcashier.h"
#include "AllCheckOut.h"
#include ".\netbarinfopage.h"

using namespace NS_SERVLET;
// CDlgNetbarConfig 对话框

IMPLEMENT_DYNAMIC(CDlgNetbarConfig, CIBADialog)
CDlgNetbarConfig::CDlgNetbarConfig()
: CIBADialog(CDlgNetbarConfig::IDD)
{
	
}

CDlgNetbarConfig::~CDlgNetbarConfig()
{
}

void CDlgNetbarConfig::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MAC, m_strMAC);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strBossPwd);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBarName);
	DDX_Text(pDX, IDC_EDIT_NETBARID, m_nNetBarID);
	//DDX_Text(pDX, IDC_STATIC_ADDR, m_strAddr);
	DDX_Text(pDX, IDC_STATIC_TEL, m_strTel);
	DDX_Text(pDX, IDC_EDIT_CENTERIP, m_strCenterIP);
	DDX_Text(pDX, IDC_EDIT_CENTERPORT, m_nCenterPort);
	//DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strPrincipal);
	DDX_Text(pDX, IDC_EDIT_NETBARKEY, m_strNetBarKey);
	DDX_Text(pDX, IDC_EDIT_CASHREGISTERID, m_strCashRegisterId);
}


BEGIN_MESSAGE_MAP(CDlgNetbarConfig, CIBADialog)
	ON_BN_CLICKED(IDC_BUTTON_SYNC, OnBnClickedButtonSync)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_BN_CLICKED(IDC_BUTTON_ALLCHECKOUT, OnBnClickedButtonAllcheckout)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
END_MESSAGE_MAP()


// CDlgNetbarConfig 消息处理程序

BOOL CDlgNetbarConfig::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetEditFont();
	InitData();

	UpdateData(FALSE);
	//定义热键，CTRL + F1
	if (!RegisterHotKey(m_hWnd, 0xC100, MOD_CONTROL, VK_F1))
	{
		CIBALog::GetInstance()->Write(_T("RegisterHotKey Failed!"));
	}
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgNetbarConfig::OnBnClickedButtonSync()
{
	SetToolTipPos(IDC_BUTTON_SYNC);
	UpdateData();
	//输入保护
	if (m_strNetBarKey.IsEmpty() || m_strCenterIP.IsEmpty() 
		|| m_nCenterPort == 0 || m_nNetBarID == 0)
	{
		ShowToolTip(IDS_INPUTERROR);
		return;
	}
	//保存到网吧设置
	SaveData();

	//mode 1同步 2基本数据备份 3基本数据恢复

	CString strURL;
	strURL.Format(_T("http://%s:8080/netbar/synchronize/cashregister.jsp?mode=%d&psk=%s&netBarId=%d&path=http://%s:%d/%s"),
		CNetBarConfig::GetInstance()->GetLocalServerIP(), 1, m_strNetBarKey,
		m_nNetBarID, m_strCenterIP, m_nCenterPort, m_strPath);
	
	CIBALog::GetInstance()->WriteFormat(_T("Synchronize URL = %s"), strURL);

	NS_SERVLET::CWinHttpRequest httpReq; 

	if (httpReq.GetReadyState() == 0)
	{
		ShowToolTip(IDS_REGWINHTTPDLL);
		return;
	}

	GetDlgItem(IDC_BUTTON_SYNC)->EnableWindow(FALSE);
	
	httpReq.SetTimeouts(5000, 5000, 5000, 60000);
	httpReq.Open(strURL);
	httpReq.Send();

	if (httpReq.GetReadyState() == 4) //执行成功
	{
		CString strTmp = httpReq.GetResponseText().Trim();

		CIBALog::GetInstance()->Write(strTmp);
		
		INT nIdx = strTmp.Find(_T("失败"));

		if (nIdx >= 0 && nIdx < 20)
		{
			ShowToolTip(IDS_SYNCFAILED, TRUE);
		}
		else
		{
			ShowToolTip(IDS_SYNCOK);
			NS_ISERVER::CLocalServer::GetInstance()->LocalRestartServer(); //重启一下ISERVER
		}

		CNetBarConfig::GetInstance()->LoadFromDB();
		InitData();
		UpdateData(FALSE);		
	}
	else
	{
		ShowToolTip(IDS_SYNCFAILED, TRUE);
	}

	GetDlgItem(IDC_BUTTON_SYNC)->EnableWindow(TRUE);
}

void CDlgNetbarConfig::InitData()
{
	CString strTmp;
	CIBAHelpper::GetIPAndMAC(strTmp, m_strMAC);
	m_strMAC.Replace(_T("-"), _T(""));

	m_NetBarConfig2.LoadFromDB();
	
	m_IServerIP = CNetBarConfig::GetInstance()->GetLocalServerIP();

	m_strCashRegisterId.Format(_T("%d"), CIBAConfig::GetInstance()->GetCashRegisterID());
	m_strNetBarKey = CNetBarConfig::GetInstance()->GetNetBarKey();
	m_nNetBarID = CNetBarConfig::GetInstance()->GetNetBarId();
	m_strCenterIP = CNetBarConfig::GetInstance()->GetMainCenterIP();
	m_nCenterPort = CNetBarConfig::GetInstance()->GetMainCenterPort();
	m_strPath = CNetBarConfig::GetInstance()->GetMainCenterPath();
	m_strNetBarName = CNetBarConfig::GetInstance()->GetNetBarName();
	m_strBossPwd = CNetBarConfig::GetInstance()->GetBossPwd();

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_NetBarInfoArray.GetCount(); i++)
	{
		if (CNetBarConfig::GetInstance()->m_NetBarInfoArray.GetAt(i).GetNetBarID() == m_nNetBarID)
		{
			CNetBarInfo& NetBarInfo = CNetBarConfig::GetInstance()->m_NetBarInfoArray.GetAt(i);

			m_strAddr = NetBarInfo.GetAddress();
			m_strPrincipal = NetBarInfo.GetManager();
			m_strTel = NetBarInfo.GetTel();
		}
	}

	m_nLocalModeStartKind = CNetBarConfig::GetInstance()->GetLocalModeStartKind();

	DoChangeLocalStartKind(m_nLocalModeStartKind);
}

void CDlgNetbarConfig::DoChangeLocalStartKind(INT nNewKind)
{
	m_nLocalModeStartKind = nNewKind;

	CButton *pBtns[3] = {
		(CButton*)GetDlgItem(IDC_RADIO1),
		(CButton*)GetDlgItem(IDC_RADIO2),
		(CButton*)GetDlgItem(IDC_RADIO3)
	};
	if (nNewKind > 2 || nNewKind < 0)
	{
		nNewKind = CIBAGlobal::emLocalModeManual;
	}

	pBtns[nNewKind]->SetCheck(BST_CHECKED);
}

void CDlgNetbarConfig::SaveData()
{
	CIBAConfig::GetInstance()->SetCashRegisterID(_ttoi(m_strCashRegisterId));

	if (m_strNetBarKey != CNetBarConfig::GetInstance()->GetNetBarKey()
		|| m_nNetBarID != CNetBarConfig::GetInstance()->GetNetBarId())
	{
		CNetBarConfig::GetInstance()->SetNetBarInfo(m_nNetBarID, m_strNetBarKey);
	}

	CString strTmp;
	strTmp.Format(_T("%s:%d"), m_strCenterIP, m_nCenterPort);
	CNetBarConfig::GetInstance()->SetMainHost(strTmp, m_strPath.Trim());//主中心

	CNetBarConfig::GetInstance()->SetLocalModeStartKind(m_nLocalModeStartKind);
}

void CDlgNetbarConfig::OnBnClickedOk()
{
	UpdateData();

	SaveData();

	if (m_strBossPwd != CNetBarConfig::GetInstance()->GetBossPwd())
	{
		CNetBarConfig::GetInstance()->SetBossPwd(m_strBossPwd);
	}

	CBCGPDialog::OnOK(); 
}

HBRUSH CDlgNetbarConfig::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//if (CTLCOLOR_STATIC == nCtlColor)
	//{
	//	if (pWnd->GetDlgCtrlID() == IDC_STATIC1)
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 255));
	//	}
	//	else if (pWnd->GetDlgCtrlID() == IDC_STATIC2)
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 255));
	//	}
	//	else if (pWnd->GetDlgCtrlID() == IDC_STATIC3)
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 255));
	//	}
	//	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_NAME)
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 255));
	//	}
	//}

	return hbr;
}

LRESULT CDlgNetbarConfig::OnHotKey(WPARAM wParam, LPARAM lParam)
{	
	CConfigOfOther dlg;
	dlg.DoModal();

	return 0;
}

void CDlgNetbarConfig::OnBnClickedButtonAllcheckout()
{
	CDlgAllCheckOut dlg;
	dlg.DoModal();
}

void CDlgNetbarConfig::OnBnClickedRadio1()
{
	// 禁用
	DoChangeLocalStartKind(CIBAGlobal::emLocalModeForbid);
}

void CDlgNetbarConfig::OnBnClickedRadio2()
{
	// 手动启用
	DoChangeLocalStartKind(CIBAGlobal::emLocalModeManual);
}

void CDlgNetbarConfig::OnBnClickedRadio3()
{
	// 自动启用
	DoChangeLocalStartKind(CIBAGlobal::emLocalModeAuto);
}
