// JXCCommodityDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "JXCSellDlg.h"
#include "JXCExchangeDlg.h"
#include "JXCPresentDlg.h"
#include ".\JXCCommodityDlg.h"
#include "JXCRetailOnlineDlg.h"

// CJXCCommodityDlg 对话框

IMPLEMENT_DYNAMIC(CJXCCommodityDlg, CIBADialog)
CJXCCommodityDlg::CJXCCommodityDlg(CWnd* pParent, UINT nType)
: CIBADialog(CJXCCommodityDlg::IDD, pParent)
, m_nCount(1), m_nCurSel(INVALID_COMMODITYID)
{
	m_lpJXCSellDlg = NULL;
	m_lpJXCExchangeDlg = NULL;
	m_lpJXCPresentDlg = NULL;
	m_lpJXCRetailOnlineDlg = NULL;

	switch(nType)
	{
	case 0:
		{
			m_lpJXCSellDlg = (CJXCSellDlg*)pParent;
			break;
		}
	case 1:
		{
			m_lpJXCExchangeDlg = (CJXCExchangeDlg*)pParent;
			break;
		}
	case 2:
		{
			m_lpJXCPresentDlg = (CJXCPresentDlg*)pParent;
			break;
		}
	case 3:
		{
			m_lpJXCRetailOnlineDlg = (CJXCRetailOnlineDlg*)pParent;
			break;
		}
	}

}

CJXCCommodityDlg::~CJXCCommodityDlg()
{
}

void CJXCCommodityDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_EDIT_COUNT, m_nCount);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_edtCount);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strCommodityName);
}


BEGIN_MESSAGE_MAP(CJXCCommodityDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, OnGridItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, OnGridItemChanged)
END_MESSAGE_MAP()


BOOL CJXCCommodityDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitGridTree();
	FillGird();
	
	SetEditFont();

	((CEdit*)GetDlgItem(IDC_EDIT_COUNT))->SetLimitText(3);

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	m_edtCount.DisableMask();
	m_edtCount.SetValidChars(_T("1234567890"));
	m_edtCount.SetLimitText(3);
	m_edtCount.SetWindowText(_T("1"));

	m_wndGridTree.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCCommodityDlg::InitGridTree()
{
	CRect rectGrid;
	GetDlgItem(IDC_LIST_COMMODITY)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_LIST_COMMODITY)->MapWindowPoints (this, &rectGrid);

	m_wndGridTree.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	//m_wndGridTree.EnableGroupByBox(FALSE);
	
	m_wndGridTree.SetFont(&m_Font);

	CIBAString strHeader;
	if (m_lpJXCExchangeDlg != NULL)
	{//积分方式
		//strHeader = L"商品类别|商品名称|商品编码|所需积分|库存数量";
		strHeader.LoadString(IDS_JXC_COMM_HEAD1);
	}
	else
	{//赠送和销售都采用价格方式
		//strHeader = L"商品类别|商品名称|商品编码|价格|库存数量";
		strHeader.LoadString(IDS_JXC_COMM_HEAD2);
	}
	
	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {80, 240, 120, 70, 80};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_wndGridTree.InsertColumn(i, strArray.GetAt(i), Width[i]);
		m_wndGridTree.SetColumnAlign(i, HDF_LEFT);
	}

}

void CJXCCommodityDlg::QueryCommodity(CCommodityQueryCondition & CommodityQueryCondition)
{
	CIBADAL::GetInstance()->GetStoreCommodity(m_CommodityArray, CommodityQueryCondition);
}

void CJXCCommodityDlg::FillGird()
{	
	CString strLastGroupId;
	CBCGPGridRow* pLastGroup = NULL;

	const INT nColumns = m_wndGridTree.GetColumnCount();

	if (m_CommodityArray.IsEmpty())
	{
		CCommodityQueryCondition CommodityQueryCondition;

		if (m_lpJXCExchangeDlg != NULL)
		{
			CommodityQueryCondition.m_bHasPoint = TRUE;
		}

		if (!CIBADAL::GetInstance()->GetStoreCommodity(m_CommodityArray, CommodityQueryCondition)) return;
	}
	
	for (INT i = 0; i < m_CommodityArray.GetCount(); i++)
	{	
		CCommodity & Commodity = m_CommodityArray.GetAt(i);
		
		INT nTmp = 0; 
		
		CString strTmp = Commodity.GetGroupId();

		if (strLastGroupId != strTmp)
		{	
			strLastGroupId = strTmp;
			pLastGroup = m_wndGridTree.CreateRow(nColumns);
			pLastGroup->AllowSubItems();
			pLastGroup->GetItem(0)->SetValue(Commodity.GetGroupName().GetString());
			pLastGroup->GetItem(0)->SetData(INVALID_COMMODITYID);
			m_wndGridTree.AddRow(pLastGroup, FALSE);
		}

		CBCGPGridRow* pCommodity = m_wndGridTree.CreateRow(nColumns);
		ASSERT_VALID(pCommodity);

		pCommodity->GetItem(0)->SetData(i);
		pCommodity->GetItem(1)->SetValue(Commodity.GetCommodityName().GetString());
		pCommodity->GetItem(2)->SetValue(Commodity.GetBarCode().GetString());
		
		if (m_lpJXCExchangeDlg != NULL)
		{
			strTmp.Format(_T("%d"), Commodity.GetPoint());
		}
		else
		{
			strTmp.Format(_T("%.2f"), Commodity.GetAdvicePrice() / 100.0);
		}

		pCommodity->GetItem(3)->SetValue(strTmp.GetString());

		pCommodity->GetItem(4)->SetValue(Commodity.GetRemainQuantity());

		pLastGroup->AddSubItem(pCommodity, FALSE);
	}

	m_wndGridTree.AdjustLayout();
}

void CJXCCommodityDlg::OnBnClickedButtonAdd()
{
	CString strTmp;
	m_edtCount.GetWindowText(strTmp);
	m_nCount = _ttoi(strTmp);

	if (m_nCount <= 0)
	{
		SetToolTipPos(IDC_EDIT_COUNT);
		ShowToolTip(IDS_INPUTERROR);
		m_edtCount.SetFocus();
		return;
	}

	if (m_nCurSel != INVALID_COMMODITYID)
	{
		AddSellRecord();
	}
}

void CJXCCommodityDlg::OnBnClickedOk()
{
	CString strTmp;
	m_edtCount.GetWindowText(strTmp);
	m_nCount = _ttoi(strTmp);

	if (m_nCount <= 0)
	{
		SetToolTipPos(IDC_EDIT_COUNT);
		ShowToolTip(IDS_INPUTERROR);
		m_edtCount.SetFocus();
		return;
	}

	if (m_nCurSel != INVALID_COMMODITYID)
	{
		AddSellRecord();

		CBCGPDialog::OnOK();
	}
}

void CJXCCommodityDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (0 == nFlags && point == CPoint(0, 0)) 
	{
		OnBnClickedOk();
	}
	else
	{
		CIBADialog::OnLButtonDblClk(nFlags, point);
	}
}

void CJXCCommodityDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (0 == nFlags && point == CPoint(0, 0)) 
	{
		CBCGPGridRow* pCommodity = m_wndGridTree.GetCurSel();

		if (pCommodity != NULL)
		{
			m_strCommodityName =  pCommodity->GetItem(1)->GetValue();
			m_nCurSel = pCommodity->GetItem(0)->GetData();
		}
		else
		{
			m_nCurSel = INVALID_COMMODITYID;
			m_strCommodityName.Empty();
		}

		UpdateData(FALSE);
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(!m_strCommodityName.IsEmpty());
		GetDlgItem(IDOK)->EnableWindow(!m_strCommodityName.IsEmpty());
	}
	else
	{
		CIBADialog::OnLButtonUp(nFlags, point);
	}
}

void CJXCCommodityDlg::AddSellRecord()
{
	NS_SERVLET::CSellRecord SellRecord;

	SellRecord.SetQuantity(m_nCount);
	SellRecord.SetCommodityCode(m_CommodityArray.GetAt(m_nCurSel).GetCommodityCode());
	SellRecord.SetPrice(m_CommodityArray.GetAt(m_nCurSel).GetAdvicePrice());
	SellRecord.SetAmount(m_nCount * m_CommodityArray.GetAt(m_nCurSel).GetAdvicePrice());
	SellRecord.SetCommodityBarCode(m_CommodityArray.GetAt(m_nCurSel).GetBarCode());
	SellRecord.SetCommodityName(m_CommodityArray.GetAt(m_nCurSel).GetCommodityName());
	SellRecord.SetPoint(m_nCount * m_CommodityArray.GetAt(m_nCurSel).GetPoint());

	if (m_lpJXCSellDlg != NULL) m_lpJXCSellDlg->AddSellRecord(SellRecord);
	if (m_lpJXCExchangeDlg != NULL) m_lpJXCExchangeDlg->AddSellRecord(SellRecord);
	if (m_lpJXCPresentDlg != NULL)
	{
		SellRecord.SetPoint(0);
		m_lpJXCPresentDlg->AddSellRecord(SellRecord);
	}
	if (m_lpJXCRetailOnlineDlg != NULL) m_lpJXCRetailOnlineDlg->AddSellRecord(SellRecord);

	m_nCurSel = INVALID_COMMODITYID;
	m_strCommodityName.Empty();
	UpdateData(FALSE);

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(!m_strCommodityName.IsEmpty());
	GetDlgItem(IDOK)->EnableWindow(!m_strCommodityName.IsEmpty());

}

void CJXCCommodityDlg::OnDestroy()
{
	CIBADialog::OnDestroy();

	m_wndGridTree.DeleteAllColumns();
	m_wndGridTree.DestroyWindow();
}

LRESULT CJXCCommodityDlg::OnGridItemChanged(WPARAM wParam, LPARAM lParam)
{
	CBCGPGridRow* pCommodity = m_wndGridTree.GetCurSel();

	if (pCommodity != NULL)
	{
		m_strCommodityName =  pCommodity->GetItem(1)->GetValue();
		m_nCurSel = pCommodity->GetItem(0)->GetData();
	}
	else
	{
		m_nCurSel = INVALID_COMMODITYID;
		m_strCommodityName.Empty();
	}

	UpdateData(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(!m_strCommodityName.IsEmpty());
	GetDlgItem(IDOK)->EnableWindow(!m_strCommodityName.IsEmpty());
	return 0;
}