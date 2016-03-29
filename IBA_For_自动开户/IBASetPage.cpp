// IBASetPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\ibasetpage.h"
#include "IBAHelpper.h"


// CSetPageOfCommon 对话框

IMPLEMENT_DYNAMIC(CSetPageOfCommon, CIBAPage)
CSetPageOfCommon::CSetPageOfCommon()
: CIBAPage(CSetPageOfCommon::IDD)
, m_nScan(0), m_nOperationAfterSwipingCard(2)
, m_nFontType(0)
{
	m_nOperationAferEnter = CIBAGlobal::emOperationAfterNull;
	m_nOperationAfterDClick = CIBAGlobal::emOperationAfterRefund;
	m_bForbidCheckIn = FALSE;
	m_bForbidCaptureScreen = FALSE;

	EnableVisualManagerStyle();
}

CSetPageOfCommon::~CSetPageOfCommon()
{
}

void CSetPageOfCommon::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_SHOWALL, m_bShowAllComputer);
	DDX_Check(pDX, IDC_CHECK_PAUSEREVMSG, m_bPauseRevMsg);
	DDX_Check(pDX, IDC_CHECK_OPEN, m_bOpenAfterSave);
	DDX_Check(pDX, IDC_CHECK_ENMERGENCY, m_bEnmergencyVisible);

	DDX_Check(pDX, IDC_CHECK_MSG_RB, m_bMsgLeftBottom);
	DDX_Check(pDX, IDC_CHECK_MSG_AUTOHIDE, m_bMsgAutoHide);
	
	DDX_Check(pDX, IDC_CHECK_LASTUSER, m_bShowLastUserNetId);
	DDX_Check(pDX, IDC_CHECK_USER_CHECKIN, m_bForbidCheckIn);

	DDX_Check(pDX, IDC_CHECK_FORBID_SCREEN, m_bForbidCaptureScreen);


	DDX_CBIndex(pDX, IDC_COMBO_FONTTYPE, m_nFontType);
	DDX_CBIndex(pDX, IDC_COMBO_SCAN, m_nScan);
	DDX_CBIndex(pDX, IDC_COMBO_OPERATIONAFTERSWIPINGCARD, m_nOperationAfterSwipingCard);
	DDX_CBIndex(pDX, IDC_COMBO_AFTER_ENTER, m_nOperationAferEnter);
	DDX_CBIndex(pDX, IDC_COMBO_AFTER_DCLICK, m_nOperationAfterDClick);

	
}


BEGIN_MESSAGE_MAP(CSetPageOfCommon, CIBAPage)
END_MESSAGE_MAP()


// CSetPageOfCommon 消息处理程序

void CSetPageOfCommon::OnOK()
{
	UpdateData();

	CIBAConfig::GetInstance()->SetShowAllComputer(m_bShowAllComputer);
	CIBAConfig::GetInstance()->SetOpenAfterSave(m_bOpenAfterSave);
	CIBAConfig::GetInstance()->SetEnmergencyVisible(m_bEnmergencyVisible);
	CIBAConfig::GetInstance()->SetPauseRevMsg(m_bPauseRevMsg);
	CIBAConfig::GetInstance()->SetFontType(m_nFontType);
	CIBAConfig::GetInstance()->SetOperationAfterSwipingCard(m_nOperationAfterSwipingCard);
	CIBAConfig::GetInstance()->SetShowLastUserNetId(m_bShowLastUserNetId);

	CIBAConfig::GetInstance()->SetMsgLeftBottom(m_bMsgLeftBottom);
	CIBAConfig::GetInstance()->SetMsgAutoHide(m_bMsgAutoHide);

	// 2011/10/20-8206-gxx: 
	CIBAConfig::GetInstance()->SetOperationAfterEnter(m_nOperationAferEnter);
	CIBAConfig::GetInstance()->SetOperationAfterDClick(m_nOperationAfterDClick);

	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBABin\\ScanReco.ini"));
	
	CString strTmp;
	strTmp.Format(_T("%d"), m_nScan);
	WritePrivateProfileString(_T("SCAN_LOAD"), _T("LOAD_ID"), strTmp, strFile);
	
	CNetBarConfig::GetInstance()->SetUserRCheckIn(m_bForbidCheckIn);

	CNetBarConfig::GetInstance()->SetForbidCaptureScreen(m_bForbidCaptureScreen);


	CIBAPage::OnOK();
}


BOOL CSetPageOfCommon::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	// 初始化组合框类容
	{
		CComboBox* pComb[2] = {
			(CComboBox*)GetDlgItem(IDC_COMBO_AFTER_ENTER),
				(CComboBox*)GetDlgItem(IDC_COMBO_AFTER_DCLICK)
		};
		CIBAString strData;
		strData.LoadString(IDS_OPERATION_AFTER);
		CStringArray strArr;
		CIBAHelpper::SplitLine(strData,strArr);
		if (strArr.GetCount() != 3)
		{
			IBA_LOG0(_T("回车后，用户视图弹出界面的ComboBox的标题文本个数不为3"));
			strData = _T("不弹出任何界面|弹出充值界面|弹出结账或退款界面");
			CIBAHelpper::SplitLine(strData,strArr);
		}
		for(int i=0; i<2; i++)
		{
			pComb[i]->AddString(strArr.GetAt(0));
			pComb[i]->AddString(strArr.GetAt(1));
			pComb[i]->AddString(strArr.GetAt(2));
		}

	}
	
	
	m_bShowAllComputer = CIBAConfig::GetInstance()->GetShowAllComputer();
	m_bEnmergencyVisible = CIBAConfig::GetInstance()->GetEnmergencyVisible();
	m_bOpenAfterSave = CIBAConfig::GetInstance()->GetOpenAfterSave();
	m_bPauseRevMsg = CIBAConfig::GetInstance()->GetPauseRevMsg();
	m_bShowLastUserNetId = CIBAConfig::GetInstance()->GetShowLastUserNetId();
	m_bMsgLeftBottom = CIBAConfig::GetInstance()->GetMsgLeftBottom();
	m_bMsgAutoHide = CIBAConfig::GetInstance()->GetMsgAutoHide();
	m_nOperationAferEnter = CIBAConfig::GetInstance()->GetOperationAfterEnter();
	m_nOperationAfterDClick = CIBAConfig::GetInstance()->GetOperationAfterDClick();

	m_nScan = CIBAConfig::GetInstance()->GetScanId();
	
	m_nOperationAfterSwipingCard = CIBAConfig::GetInstance()->GetOperationAfterSwipingCard();
	
	m_nFontType = CIBAConfig::GetInstance()->GetFontType();

	m_bForbidCheckIn = CNetBarConfig::GetInstance()->GetUserRCheckIn(); // 是否禁用用户视图右键上机

	m_bForbidCaptureScreen = CNetBarConfig::GetInstance()->GetForbidCaptureScreen();


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

