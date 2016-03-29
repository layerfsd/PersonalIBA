// JXC\JXCVoucherNoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"
#include ".\jxcvouchernodlg.h"


// CJXCVoucherNoDlg 对话框

IMPLEMENT_DYNAMIC(CJXCVoucherNoDlg, CIBADialog)
CJXCVoucherNoDlg::CJXCVoucherNoDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CJXCVoucherNoDlg::IDD, pParent)
{
}

CJXCVoucherNoDlg::~CJXCVoucherNoDlg()
{
}

void CJXCVoucherNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strVoucherNo);
}


BEGIN_MESSAGE_MAP(CJXCVoucherNoDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CJXCVoucherNoDlg::InitGridTree()
{
	CRect rectGrid;
	GetDlgItem(IDC_LIST_COMMODITY)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_LIST_COMMODITY)->MapWindowPoints(this, &rectGrid);

	m_wndGridTree.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	m_wndGridTree.SetReadOnly(FALSE);

	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_VOUCHER_HEAD);
	//strHeader = L"销售单号|商品名称|商品编码|价格|数量|实收金额";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {110, 220, 120, 60, 55, 70};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_wndGridTree.InsertColumn(i, strArray.GetAt(i), Width[i]);
		m_wndGridTree.SetColumnAlign(i, HDF_LEFT);
	}

	m_wndGridTree.SetFont(&m_Font);
}

void CJXCVoucherNoDlg::FillGird()
{	
	CString strLastGroupId;
	CBCGPGridRow* pLastGroup = NULL;

	const INT nColumns = m_wndGridTree.GetColumnCount();

	CRetailQueryCondition RetailQueryCondition;
	
	RetailQueryCondition.m_strCashier = _T("")/*theApp.GetCurCashier()->GetName()*/;

	COleDateTime timeCurrent = COleDateTime::GetCurrentTime();
	COleDateTimeSpan oneDayGap(1,0,0,0);
	timeCurrent -= oneDayGap;
	

	RetailQueryCondition.m_strStartTime = timeCurrent.Format(_T("%Y-%m-%d %H:%M:%S"));
	RetailQueryCondition.m_strEndTime = COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	RetailQueryCondition.m_nPayType = 0;
	RetailQueryCondition.m_nRetailType = 1;

	if (!CIBADAL::GetInstance()->GetRetailInfo(m_RetailInfoArray, RetailQueryCondition)) return;

	for (INT i = 0; i < m_RetailInfoArray.GetCount(); i++)
	{	
		CRetailInfo & RetailInfo = m_RetailInfoArray.GetAt(i);

		INT nTmp = 0; 

		CString strTmp = RetailInfo.RetailVoucher.GetVoucherNo();

		if (strLastGroupId != strTmp)
		{	
			strLastGroupId = strTmp;
			pLastGroup = m_wndGridTree.CreateRow(nColumns);

			for (INT j = 0; j < nColumns; j++)
			{
				pLastGroup->GetItem(j)->AllowEdit(FALSE);
			}

			pLastGroup->AllowSubItems();

			pLastGroup->GetItem(0)->SetValue(strTmp.GetString());
			pLastGroup->GetItem(1)->SetValue(RetailInfo.RetailVoucher.GetVoucherTime().GetString());

			m_wndGridTree.AddRow(pLastGroup, FALSE);
		}

		CBCGPGridRow* pCommodity = m_wndGridTree.CreateRow(nColumns);
		
		for (INT j = 0; j < nColumns; j++)
		{
			pCommodity->GetItem(j)->AllowEdit(FALSE);
		}

		pCommodity->ReplaceItem(0, new CBCGPGridCheckItem(true));
		pCommodity->SetData(i);

		pCommodity->GetItem(1)->SetValue(RetailInfo.Commodity.GetCommodityName().GetString());

		pCommodity->GetItem(2)->SetValue(RetailInfo.RetailDetail.GetCommodityCode().GetString());

		strTmp.Format(_T("%.2f"), RetailInfo.RetailDetail.GetPrice() / 100.0);
		pCommodity->GetItem(3)->SetValue(strTmp.GetString());

		if (RetailInfo.RetailDetail.GetQuantity() <= 1)
		{
			pCommodity->GetItem(4)->SetValue(RetailInfo.RetailDetail.GetQuantity());
		}
		else
		{
			CBCGPGridItem* pSpinItem = new CBCGPGridItem(RetailInfo.RetailDetail.GetQuantity()); 
			pSpinItem->EnableSpinControl(TRUE, 1, RetailInfo.RetailDetail.GetQuantity());
			pSpinItem->Enable(FALSE);
			pCommodity->ReplaceItem(4, pSpinItem);
		}

		strTmp.Format(_T("%.2f"), RetailInfo.RetailDetail.GetAmount() / 100.0);
		pCommodity->GetItem(5)->SetValue(strTmp.GetString());
		
		pLastGroup->AddSubItem(pCommodity, FALSE);
	}

	m_wndGridTree.AdjustLayout();
}


BOOL CJXCVoucherNoDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitGridTree();
	FillGird();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	m_wndGridTree.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCVoucherNoDlg::OnBnClickedOk()
{
	CBCGPGridRow* pCommodity = m_wndGridTree.GetRow(m_nCurSel);

	CList<CBCGPGridRow*, CBCGPGridRow*> MyRetailList;
	
	pCommodity->GetSubItems(MyRetailList);
	
	POSITION pos = MyRetailList.GetHeadPosition();
	
	for (INT i = 0; i < MyRetailList.GetCount(); i++)
	{
		CBCGPGridRow* pTmp = MyRetailList.GetNext(pos);
		
		if ((bool)pTmp->GetItem(0)->GetValue())
		{
			CRetailInfo& RetailInfo = m_RetailInfoArray.GetAt(pTmp->GetData());

			m_RetailVoucher = RetailInfo.RetailVoucher;

			NS_SERVLET::CSellRecord SellRecord;

			SellRecord.SetPrice(RetailInfo.RetailDetail.GetPrice());
			SellRecord.SetQuantity(pTmp->GetItem(4)->GetValue());
			SellRecord.SetAmount(SellRecord.GetPrice() * SellRecord.GetQuantity());
			SellRecord.SetCommodityName(RetailInfo.Commodity.GetCommodityName());
			SellRecord.SetCommodityCode(RetailInfo.RetailDetail.GetCommodityCode());

			m_SellRecordArray.Add(SellRecord);

		}
	}

	if (m_SellRecordArray.GetCount() > 0)
	{
		CBCGPDialog::OnOK();
	}
	else
	{
		MsgBox(L"对不起！您没有选择商品！");
	}
}

void CJXCVoucherNoDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (0 == nFlags && point == CPoint(0, 0)) 
	{
		CBCGPGridRow* pCommodity = m_wndGridTree.GetCurSel();

		if (pCommodity != NULL)
		{
			CString strTmp = pCommodity->GetItem(0)->GetValue();

			if (strTmp.GetLength() > 5) //单号
			{
				m_nCurSel = pCommodity->GetRowId();
				m_strVoucherNo = strTmp;
			}
			else
			{
				INT_PTR nIdx = pCommodity->GetData();

				strTmp = m_RetailInfoArray.GetAt(nIdx).RetailVoucher.GetVoucherNo();
				//同一个
				if (0 == m_strVoucherNo.CompareNoCase(strTmp))
				{
					
				}
				else
				{
					m_strVoucherNo = strTmp;
					m_nCurSel = pCommodity->GetParent()->GetRowId();
				}
			}
		}
		else
		{
			m_strVoucherNo.Empty();
		}

		UpdateData(FALSE);

		GetDlgItem(IDOK)->EnableWindow(!m_strVoucherNo.IsEmpty());
	}
	else
	{
		CIBADialog::OnLButtonUp(nFlags, point);
	}
}

void CJXCVoucherNoDlg::GetSellRecordArray(NS_SERVLET::CSellRecordArray& newVal)
{
	newVal.Copy(m_SellRecordArray);
}

CRetailVoucher CJXCVoucherNoDlg::GetRetailVoucher()
{
	return m_RetailVoucher;
}