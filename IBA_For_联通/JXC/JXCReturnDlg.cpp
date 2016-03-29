// JXC\JXCReturnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"
#include "JXCVoucherNoDlg.h"
#include ".\jxcreturndlg.h"
#include "..\Dal\RightsManagement.h"


// CJXCReturnDlg 对话框

IMPLEMENT_DYNAMIC(CJXCReturnDlg, CIBADialog)
CJXCReturnDlg::CJXCReturnDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CJXCReturnDlg::IDD, pParent)
, m_nCommodityCount(1)
{
}

CJXCReturnDlg::~CJXCReturnDlg()
{
}

void CJXCReturnDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_COMMODITYID, m_strCommodityId);
	DDX_Text(pDX, IDC_EDIT_COMMODITYTD, m_strCommodityTd);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strCommodityName);
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strCommodityType);
	DDX_Text(pDX, IDC_STATIC_PRICE, m_strCommodityPrice);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nCommodityCount);

	DDX_Text(pDX, IDC_STATIC_MONEY, m_strAllMomey);
	DDX_Text(pDX, IDC_EDIT_VOUCHERNO, m_strVoucherNo);

	DDX_Control(pDX, IDC_LIST_COMMODITY, m_CommodityList);
}


BEGIN_MESSAGE_MAP(CJXCReturnDlg, CIBADialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_VOUCHERNO, OnBnClickedButtonVoucherno)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_WM_SHOWWINDOW()
//	ON_BN_CLICKED(IDC_BUTTON_COMMODITY, OnBnClickedButtonCommodity)
//ON_BN_CLICKED(IDC_BUTTON_COMMODITY, OnBnClickedButtonCommodity)
END_MESSAGE_MAP()


// CJXCReturnDlg 消息处理程序

HBRUSH CJXCReturnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_MONEY)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}	
	}

	return hbr;
}

void CJXCReturnDlg::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_RETURN_HEAD);
	//strHeader = L"商品名称|商品编码|价格|数量|金额|时间"; //

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {230, 120, 60, 60, 70, 170};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_CommodityList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_CommodityList.SetFont(&m_Font);
	m_CommodityList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

BOOL CJXCReturnDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitListView();

	SetEditFont();

	CIBAHelpper::CreateFont(m_BigFont, 23, 700);
	GetDlgItem(IDC_STATIC_MONEY)->SetFont(&m_BigFont);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_COMMODITY)->EnableWindow(FALSE);

	((CEdit*)GetDlgItem(IDC_EDIT_COUNT))->SetLimitText(3);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCReturnDlg::OnBnClickedButtonVoucherno()
{
	CJXCVoucherNoDlg dlg(this);
	
	if (IDOK == dlg.DoModal())
	{
		m_RetailVoucher = dlg.GetRetailVoucher();
		m_strVoucherNo = m_RetailVoucher.GetVoucherNo();

		m_SellRecordArray.RemoveAll();
		dlg.GetSellRecordArray(m_SellRecordArray);

		m_CommodityList.DeleteAllItems();

		for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
		{
			CSellRecord & SellRecord = m_SellRecordArray.GetAt(i);

			m_CommodityList.InsertItem(i, SellRecord.GetCommodityName());

			CString strTmp;
			
			m_CommodityList.SetItemText(i, 1, SellRecord.GetCommodityCode());
			
			strTmp.Format(_T("%.2f"), SellRecord.GetPrice() / 100.0);
			m_CommodityList.SetItemText(i, 2, strTmp);

			strTmp.Format(_T("%d"), SellRecord.GetQuantity());
			m_CommodityList.SetItemText(i, 3, strTmp);

			strTmp.Format(_T("%.2f"), SellRecord.GetAmount() / 100.0);
			m_CommodityList.SetItemText(i, 4, strTmp);

			m_CommodityList.SetItemText(i, 5, m_RetailVoucher.GetVoucherTime());

			m_CommodityList.SetItemData(i, i);
		}

		ShowMoney();
	}
}

void CJXCReturnDlg::OnBnClickedOk()
{
	if (m_SellRecordArray.GetCount() <= 0)
	{
		SetToolTipPos(IDOK);
		ShowToolTip(L"请先选择销售订单！");
		return;
	}

	if (!CRightsManagement::GetInstance()->Take_IBAP_RETAILRETURN(TRUE))
	{
		return;
	}

	CWaitCursor Wait;

	CRetailMethod Retail;

	Retail.SetPayType(m_RetailVoucher.GetPayType());
	Retail.SetMemberId(m_RetailVoucher.GetMemberId());
	Retail.SetPassword(_T(""));
	Retail.SetRetailType(2);
	Retail.SetSellRecords(m_SellRecordArray);
	Retail.SetVoucherNo(m_RetailVoucher.GetVoucherNo());

	theApp.GetCurCashier()->DoRetail(Retail);

	SetToolTipPos(IDOK);

	if (Retail.GetStatusCode() == 0)
	{
		ShowToolTip(L"退货成功！");

		SaveRetailData(Retail);

		CBalanceMethod Balance;
		Balance.SetMemberId(m_RetailVoucher.GetMemberId());
		theApp.GetCurCashier()->DoBalance(Balance);

		ClearData();
		UpdateData(FALSE);
	}
	else
	{
		CString strTmp;
		strTmp.Format(L"退货失败！原因：%s - %s", Retail.GetStatusMessage(), Retail.GetExtInfo());

		ShowToolTip(strTmp, TRUE);
	}
}

void CJXCReturnDlg::ShowMoney()
{
	UINT nTmp = 0;

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		nTmp += m_SellRecordArray.GetAt(i).GetAmount();
	}

	if (nTmp > 0)
	{
		m_strAllMomey.Format(_T("%.2f"), nTmp / 100.0);	
	}
	else
	{
		m_strAllMomey.Empty();
	}

	if (m_SellRecordArray.GetCount() == 0)
	{
		m_strVoucherNo.Empty();
	}

	UpdateData(FALSE);

	GetDlgItem(IDOK)->EnableWindow(m_SellRecordArray.GetCount() > 0);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(m_SellRecordArray.GetCount() > 0);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(m_SellRecordArray.GetCount() > 0);
}

void CJXCReturnDlg::OnBnClickedButtonDel()
{
	INT nIdx = m_CommodityList.GetSelectionMark();

	if (nIdx < 0)
	{
		return;
	}

	CString strTmp;

	m_SellRecordArray.RemoveAt(nIdx);

	m_CommodityList.DeleteAllItems();

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		CSellRecord & SellRecord = m_SellRecordArray.GetAt(i);

		m_CommodityList.InsertItem(i, SellRecord.GetCommodityName());

		strTmp.Format(_T("%.2f"), SellRecord.GetPrice() / 100.0);
		m_CommodityList.SetItemText(i, 1, strTmp);

		strTmp.Format(_T("%d"), SellRecord.GetQuantity());
		m_CommodityList.SetItemText(i, 2, strTmp);

		strTmp.Format(_T("%.2f"), SellRecord.GetAmount() / 100.0);
		m_CommodityList.SetItemText(i, 3, strTmp);

		m_CommodityList.SetItemText(i, 4, SellRecord.GetCommodityBarCode());

		m_CommodityList.SetItemData(i, i);
	}

	ShowMoney();
}

void CJXCReturnDlg::OnBnClickedButtonClear()
{
	if (IDOK == MsgBox(L"取消本次退货？", MB_OKCANCEL | MB_ICONQUESTION))
	{
		ClearData();
		UpdateData(FALSE);
	}
}

void CJXCReturnDlg::ClearData()
{
	m_CommodityList.DeleteAllItems();
	m_SellRecordArray.RemoveAll();

	m_strVoucherNo.Empty();
	m_strAllMomey.Empty();
	m_strCommodityName.Empty();
	m_strCommodityPrice.Empty();
	m_strCommodityTd.Empty();
	m_strCommodityType.Empty();
	m_nCommodityCount = 1;
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_COMMODITY)->EnableWindow(FALSE);
}

void CJXCReturnDlg::SaveRetailData(CRetailMethod &Retail)
{
	CRetailVoucher RetailVoucher;

	RetailVoucher.SetVoucherNo(Retail.GetVoucherNo());
	RetailVoucher.SetOldVoucherNo(m_RetailVoucher.GetVoucherNo());
	RetailVoucher.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	RetailVoucher.SetVoucherTime(CIBAHelpper::FormatCenterTime(Retail.GetTranTime()));
	RetailVoucher.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());
	RetailVoucher.SetMemberId(m_RetailVoucher.GetMemberId());
	RetailVoucher.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	RetailVoucher.SetPayType(m_RetailVoucher.GetPayType());
	RetailVoucher.SetUserId(theApp.GetCurCashier()->GetName());
	RetailVoucher.SetAccountYear(COleDateTime::GetCurrentTime().Format(_T("%Y")));
	RetailVoucher.SetAccountMonth(COleDateTime::GetCurrentTime().Format(_T("%m")));
	RetailVoucher.SetRefNo(Retail.GetRefNo());
	RetailVoucher.SetRetailType(2);

	CIBADAL::GetInstance()->AddRetailVoucher(RetailVoucher);

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		INT nTmp = 0;

		CRetailDetail RetailDetail;

		RetailDetail.SetVoucherNo(Retail.GetVoucherNo());

		nTmp = 0 - m_SellRecordArray.GetAt(i).GetAmount();
		RetailDetail.SetAmount(nTmp);

		RetailDetail.SetPrice(m_SellRecordArray.GetAt(i).GetPrice());

		nTmp = 0 - m_SellRecordArray.GetAt(i).GetQuantity();
		RetailDetail.SetQuantity(nTmp);
		RetailDetail.SetReturnQuantity(RetailDetail.GetQuantity());

		RetailDetail.SetCommodityCode(m_SellRecordArray.GetAt(i).GetCommodityCode());
		RetailDetail.SetPoint(m_SellRecordArray.GetAt(i).GetPoint());
		
		CIBADAL::GetInstance()->AddRetailDetail(RetailDetail);
	}
}

void CJXCReturnDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		m_ToolTip.HideTooltip();
	}

	CIBADialog::OnShowWindow(bShow, nStatus);
}

BOOL CJXCReturnDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam )
	{
		GetParent()->GetParent()->PostMessage(WM_CLOSE);
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}


