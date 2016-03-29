// NDBundleTime2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAPrint.h"
#include "IBAHelpper.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include "MainFrm.h"
#include "ComputerListView.h"
#include ".\NDBundleTime2Dlg.h"


// CNDBundleTime2Dlg 对话框

IMPLEMENT_DYNAMIC(CNDBundleTime2Dlg, CNDDialog)
CNDBundleTime2Dlg::CNDBundleTime2Dlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDBundleTime2Dlg::IDD, pParent)
, m_nSelBundleTimeId(0), m_bOnLine(FALSE)
, m_bAfterBundleTime(FALSE)
{
}

CNDBundleTime2Dlg::~CNDBundleTime2Dlg()
{
}

void CNDBundleTime2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BUNDLETIME, m_BundleTimeList);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberID);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_WSCZ, m_strBalanceW);
	DDX_Text(pDX, IDC_STATIC_YSCZ, m_strBalance);
	DDX_Text(pDX, IDC_STATIC_BTAREA, m_strBundleTimeArea);
	DDX_Text(pDX, IDC_STATIC_BTENDTIME, m_strBundleTimeEndTime);
	DDX_Text(pDX, IDC_STATIC_BTID, m_strBundleTimeID);
	DDX_Text(pDX, IDC_STATIC_BTMONEY, m_strBundleTimeMoney);
	DDX_Text(pDX, IDC_STATIC_BTMONEY2, m_strBundleTimeMoney2);
	DDX_Text(pDX, IDC_STATIC_BTSTARTTIME, m_strBundleTimeStartTime);
	DDX_Text(pDX, IDC_STATIC_BTYPE, m_strBundleTimeType);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
}


BEGIN_MESSAGE_MAP(CNDBundleTime2Dlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BUNDLETIME, OnNMClickListBundletime)
	ON_BN_CLICKED(IDC_BUTTON_CANCELBT, OnBnClickedButtonCancelbt)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CNDBundleTime2Dlg 消息处理程序

BOOL CNDBundleTime2Dlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	InitListView();
	SetEditFont();

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CNDBundleTime2Dlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);

	QueryBalance(nMemberId, nCardId);

	GetBundleTimeAction();

	if (m_strMemberID != m_strLastMemberID)
	{
		GetBundleTimeList();
		m_strLastMemberID = m_strMemberID;
	}	
}

void CNDBundleTime2Dlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_bOnLine = (Balance.GetOnline() == 1);

	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

	double dblTmp = 0.0, dblTmp1 = 0.0, dblTmp2 = 0.0;

	//无送
	dblTmp = Balance.GetCashBalance();
	dblTmp2 = dblTmp / 100;
	m_strBalanceW.Format(_T("%.2f"), dblTmp2);

	//有送
	dblTmp = Balance.GetUsePresent();
	dblTmp2 = dblTmp / 100;
	m_strBalance.Format(_T("%.2f"), dblTmp2);

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT m_nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
	m_strAllBalance.Format(_T("%.2f"), m_nAllMoney / 100.0);
}

void CNDBundleTime2Dlg::GetBundleTimeList()
{
	m_BundleTimeList.DeleteAllItems();

	theApp.GetCurCashier()->DoBundleTimeList2(BundleTimeList2);

	if (BundleTimeList2.GetStatusCode() == 0)
	{
		for (INT i = 0; i < (INT)BundleTimeList2.GetRecordCount(); i++)
		{
			CString strTmp;

			strTmp.Format(_T("%d"), BundleTimeList2.GetBundleId(i));
			m_BundleTimeList.InsertItem(i, strTmp);

			strTmp = BundleTimeList2.GetStartTime(i);
			m_BundleTimeList.SetItemText(i, 1, strTmp);

			strTmp = BundleTimeList2.GetEndTime(i);
			m_BundleTimeList.SetItemText(i, 2, strTmp);

			strTmp = BundleTimePayType(BundleTimeList2.GetMode(i));
			m_BundleTimeList.SetItemText(i, 3, strTmp);

			strTmp.Format(_T("%d"), BundleTimeList2.GetDays(i));
			m_BundleTimeList.SetItemText(i, 4, strTmp);

			strTmp.Format(_T("%.2f"), BundleTimeList2.GetPrice(i) / 100.0);
			m_BundleTimeList.SetItemText(i, 5, strTmp);

			strTmp = BundleTimeList2.GetDescription(i);
			m_BundleTimeList.SetItemText(i, 6, strTmp);

			m_BundleTimeList.SetItemData(i, i);
		}
	}
}

void CNDBundleTime2Dlg::GetBundleTimeAction()
{
	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);

	CBundleTimeActionMethod BundleTimeAction;
	BundleTimeAction.SetMemberId(_ttoi(m_strMemberID));

	theApp.GetCurCashier()->DoBundleTimeAction(BundleTimeAction);

	if (BundleTimeAction.GetStatusCode() == 0 && BundleTimeAction.GetNetBarId() > 0)
	{
		m_strBundleTimeID.Format(_T("%d"), BundleTimeAction.GetTimeId());
		m_strBundleTimeStartTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetStartTime());
		m_strBundleTimeEndTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetEndTime());
		
		//m_strBundleTimeType = BundleTimePayType(BundleTimeAction.GetPayType());
		
		m_strBundleTimeMoney.Format(_T("%.2f"), BundleTimeAction.GetAmount1() / 100.0);
		m_strBundleTimeMoney2.Format(_T("%.2f"), BundleTimeAction.GetAmount2() / 100.0);
		
		m_strBundleTimeArea = CNetBarConfig::GetInstance()->GetPCClassName(BundleTimeAction.GetPcClass());

		if (m_strBundleTimeArea.IsEmpty())
		{
			m_strBundleTimeArea = LOAD_STRING(IDS_NOLIMITAREA);
		}

		GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(TRUE);
	}
}	

CString CNDBundleTime2Dlg::BundleTimePayType(UINT nBundleType)
{	
	CString strTmp;

	/**
	*1:包月 2:包周 3:自定义天数
	*/

	if (1 == nBundleType)
	{
		strTmp = L"包月";
	}
	else if (2 == nBundleType)
	{
		strTmp = L"包周";
	}
	else
	{
		strTmp = L"自定义天数";
	}

	return strTmp;
}


void CNDBundleTime2Dlg::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_BUNDLETIMELISTHEADER);
	
	//strHeader = L"编号|开始时间|结束时间|类型|天数|金额|其他信息";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {40, 80, 80, 85, 55, 80, 120};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_BundleTimeList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	//m_BundleTimeList.SetFont(&m_Font);
	m_BundleTimeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CNDBundleTime2Dlg::OnNMClickListBundletime(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT nIndex = m_BundleTimeList.GetSelectionMark();

	if (nIndex >= 0)
	{
		INT nBtIndex = m_BundleTimeList.GetItemData(nIndex);

		m_nSelBundleTimeId = BundleTimeList2.GetBundleId(nBtIndex);

		if (!GetDlgItem(IDC_BUTTON_CANCELBT)->IsWindowEnabled())
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
	}

	*pResult = 0;
}

void CNDBundleTime2Dlg::OnBnClickedButtonCancelbt()
{
	CString strTmp;
	strTmp.Format(IDS_CANCELBUNDLETIME, m_strName);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	CWaitCursor Wait;

	CCancelBundleTimeMethod CancelBundleTime;

	CancelBundleTime.SetMemberId(_ttoi(m_strMemberID));

	theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);

	CIBAString strRet;

	if (CancelBundleTime.GetStatusCode() == 0)
	{
		OnBnClickedButtonQuery();

		strRet.LoadString(IDS_BTCANCELOK);

		m_bAfterBundleTime = FALSE;
	}
	else
	{
		strRet = CancelBundleTime.GetStatusMessage();
	}

	SetToolTipPos(IDC_BUTTON_CANCELBT);
	ShowToolTip(strRet, m_bAfterBundleTime);
}

void CNDBundleTime2Dlg::ClearData()
{
	m_strBalance.Empty();
	m_strBalanceW.Empty();
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strUserClass.Empty();
	m_strPersonalID.Empty();
	m_strNetBar.Empty();
	m_strBundleTimeID.Empty();
	m_strBundleTimeStartTime.Empty();
	m_strBundleTimeEndTime.Empty();
	m_strBundleTimeType.Empty();
	m_strBundleTimeMoney.Empty();
	m_strBundleTimeMoney2.Empty();
	m_strBundleTimeArea.Empty();
	m_nSelBundleTimeId = 0;

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//查询，重新清标志
	m_bAfterBundleTime = FALSE;

}

void CNDBundleTime2Dlg::OnBnClickedOk()
{
	DoBundleTime(0);
}

void CNDBundleTime2Dlg::DoBundleTime(UINT nType)
{
	CString strTmp;
	
	strTmp.Format(IDS_ASKBUNDLETIME);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	CWaitCursor Wait;

	CBundleTime2Method BundleTime;

	BundleTime.SetMemberId(_ttoi(m_strMemberID));
	BundleTime.SetTimeId(m_nSelBundleTimeId);

	DWORD length = MAX_PATH;
	TCHAR szComputerName[MAX_PATH] = {0};
	GetComputerName(szComputerName, &length);

	BundleTime.SetTermId(szComputerName);

	theApp.GetCurCashier()->DoBundleTime2(BundleTime);

	CIBAString strRet;

	if (BundleTime.GetStatusCode() == 0)
	{
		OnBnClickedButtonQuery();
		
		strRet.LoadString(IDS_BTSUCCESSED);

		//包时成功标志置位
		m_bAfterBundleTime = TRUE;
	}
	else
	{
		m_bAfterBundleTime = FALSE;
		strRet = BundleTime.GetStatusMessage();
	}

	SetToolTipPos(IDOK);

	ShowToolTip(strRet, !m_bAfterBundleTime);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
}
