// StoreDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\storedlg.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "Dal\DALJXC.h"
#include "Dal\IBADAL.h"
#include "Socket\LocalServer.h"
#include "ComputerListView.h"

// CStoreDlg 对话框

IMPLEMENT_DYNAMIC(CStoreDlg, CIBADialog)
CStoreDlg::CStoreDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CStoreDlg::IDD, pParent)
{
	m_lpCommodityGrid = NULL;
}

CStoreDlg::~CStoreDlg()
{
}

void CStoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SORT, m_cboSortType);
}

BEGIN_MESSAGE_MAP(CStoreDlg, CIBADialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SORT, OnCbnSelchangeComboSort)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_PRINTCOMMODITY, OnBnClickedButtonPrintcommodity)
END_MESSAGE_MAP()


// CStoreDlg 消息处理程序

BOOL CStoreDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 初始化控件
BOOL CStoreDlg::InitCtrl(void)
{
	//同步数据库存
	CStoreQuantityMethod StoreQuantity;//库存盘点统计方法

	theApp.GetCurCashier()->DoStoreQuantity(StoreQuantity);

	if (StoreQuantity.GetStatusCode() == 0)
	{	
		// 2011/04/27-gxx: 注释掉此行代码://清空所有交易保留的数量
		//CIBADAL::GetInstance()->ZeroAllCommodityRemainQuantity();

		for (UINT i = 0; i < StoreQuantity.GetRecordCount(); i++)
		{//更新库存记录（商品编号+ 库存数量）
			CIBADAL::GetInstance()->UpdateCommodityRemainQuantity(StoreQuantity.GetCommodityCode(i), StoreQuantity.GetRemainQuantity(i));	
		}
	}
	else
	{
		ShowToolTip(StoreQuantity.GetStatusMessage(), TRUE);
	}

	int index = m_cboSortType.SetCurSel(0);

	if (!SetGrid(index))
	{
		return FALSE;
	}

	return TRUE;
}

// 初始化GRID
BOOL CStoreDlg::SetGrid(UINT nType)
{
	CRect rectGrid;
	GetDlgItem(IDC_STATIC_COMMODITY)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_STATIC_COMMODITY)->MapWindowPoints(this, &rectGrid);
	
	m_CommodityGrid.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	
	m_lpCommodityGrid = &m_CommodityGrid;

	switch(nType)
	{
	case 0:
		{
			InitSortGrid();
			FillSortGird();
			break;
		}
	case 1:
		{
			InitPriceGrid();
			FillPriceGird();
			break;
		}
	case 2:
		{
			InitSortPriceGrid();
			FillSortPriceGird();
			break;
		}
	}
	return TRUE;
}

// 初始化GRID
BOOL CStoreDlg::InitSortPriceGrid(void)
{
	m_lpCommodityGrid->RemoveAll();
	
	while(m_lpCommodityGrid->GetColumnCount() > 0)
	{
		m_lpCommodityGrid->DeleteColumn(0);
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_STORE_HEAD);
	//strHeader = L"商品类别+价格|商品名称|商品编码|库存|现金购物|金额|账户购物|赠送|兑换积分";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		const UINT Width[] = {120, 180, 90, 40, 60, 40, 60, 40, 60};

		for (INT i = 0; i < strArray.GetCount(); i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}
	else
	{
		m_lpCommodityGrid->SetFont(&m_Font);

		const UINT Width[] = {120, 250, 140, 90};

		for (INT i = 0; i < 4; i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}

	return TRUE;
}

// 初始化GRID
BOOL CStoreDlg::InitPriceGrid(void)
{
	m_lpCommodityGrid->RemoveAll();
	while(m_lpCommodityGrid->GetColumnCount() > 0)
	{
		m_lpCommodityGrid->DeleteColumn(0);
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_STORE_HEAD2);
	//strHeader = L"商品价格|商品名称|商品编码|商品类别|库存|现金购物|金额|账户购物|赠送|兑换积分";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		const UINT Width[] = {60, 180, 90, 90, 40, 60, 40, 60, 40, 60};

		for (INT i = 0; i < strArray.GetCount(); i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}
	else
	{
		m_lpCommodityGrid->SetFont(&m_Font);

		const UINT Width[] = {70, 250, 140, 90, 90};

		for (INT i = 0; i < 5; i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}

	return TRUE;
}

// 初始化GRID
BOOL CStoreDlg::InitSortGrid(void)
{
	m_lpCommodityGrid->RemoveAll();
	while(m_lpCommodityGrid->GetColumnCount() > 0)
	{
		m_lpCommodityGrid->DeleteColumn(0);
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_STORE_HEAD2);
	//strHeader = L"商品类别|商品名称|商品编码|价格|库存|现金购物|金额|账户购物|赠送|兑换积分";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		const UINT Width[] = {60, 180, 90, 40, 40, 60, 40, 60, 40, 60};

		for (INT i = 0; i < strArray.GetCount(); i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}
	else
	{
		m_lpCommodityGrid->SetFont(&m_Font);

		const UINT Width[] = {70, 250, 140, 90, 90};

		for (INT i = 0; i < 5; i++)
		{
			m_lpCommodityGrid->InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_lpCommodityGrid->SetColumnAlign(i, HDF_LEFT);
		}
	}

	return TRUE;
}

//填充按种类和价格分类数据
//strHeader = L"商品类别|商品名称|商品编码|价格|库存|现金购物|金额|账户购物|赠送|兑换积分";
void CStoreDlg::FillSortPriceGird()
{
	m_lpCommodityGrid->RemoveAll();

	CCommodityArray CommodityArray;//待售商品队列
	CCommodityQueryCondition CommodityQueryCondition;//商品查询条件，用来指定特定商品查询
	//从数据库取得库存数据
	if (!CIBADAL::GetInstance()->GetSortPriceCommodity(CommodityArray, CommodityQueryCondition)) return;

	CCommoditySoldInfoArray CommoditySoldInfoArray;
	if (!CIBADAL::GetInstance()->GetCommoditySoldInfo(CommoditySoldInfoArray)) return;

	//取得列数
	const INT nColumns = m_lpCommodityGrid->GetColumnCount();

	INT nStoreNum = 0; //库存数量

	CString strLastGroupId;//上一次的商品
	UINT nLastPrice = 0;
	CBCGPGridRow* pLastGroup = NULL;
	//商品记录
	for (INT i = 0; i < CommodityArray.GetCount(); i++)
	{	//商品信息
		CCommodity & Commodity = CommodityArray.GetAt(i);

		//商品类别ID
		CString strTmp = Commodity.GetGroupId();
		UINT nTmpPrice = Commodity.GetAdvicePrice();
		//此商品不是同一类
		if (strLastGroupId != strTmp || nLastPrice != nTmpPrice)
		{	
			if (nStoreNum > 0)//数量大于0
			{
				pLastGroup->GetItem(3)->SetValue(nStoreNum);
				nStoreNum = 0;
			}

			strLastGroupId = strTmp;
			nLastPrice = nTmpPrice;
			//创建记录
			pLastGroup = m_lpCommodityGrid->CreateRow(nColumns);
			pLastGroup->AllowSubItems();// 关联式子菜单
			strTmp = Commodity.GetGroupName();
			strTmp.AppendFormat(_T(" %.2f"), nTmpPrice / 100.0);
			pLastGroup->GetItem(0)->SetValue(strTmp.GetString());
			//添加记录
			m_lpCommodityGrid->AddRow(pLastGroup, FALSE);
		}

		//此商品出现过
		CBCGPGridRow* pCommodity = m_lpCommodityGrid->CreateRow(nColumns);
		ASSERT_VALID(pCommodity);
		//设置子项
		pCommodity->GetItem(1)->SetValue(Commodity.GetCommodityName().GetString());
		pCommodity->GetItem(2)->SetValue(Commodity.GetBarCode().GetString());
		pCommodity->GetItem(3)->SetValue(Commodity.GetRemainQuantity());

		if (theApp.GetCurCashier()->IsOnDuty())
		{
			for (INT j = 0; j < CommoditySoldInfoArray.GetCount(); j++)
			{
				CCommoditySoldInfo CommoditySoldInfo = CommoditySoldInfoArray.GetAt(j);

				if (CommoditySoldInfo.GetCommoditycode().CompareNoCase(Commodity.GetBarCode()) == 0)
				{
					//现金合计数量
					INT nCount = CommoditySoldInfo.GetSoldCount1() + CommoditySoldInfo.GetReturnCount1();

					if (nCount > 0)
					{
						pCommodity->GetItem(4)->SetValue(nCount);
						strTmp.Format(_T("%.2f"), (CommoditySoldInfo.GetSoldAmout1() + CommoditySoldInfo.GetReturnAmout1()) / 100.0);
						pCommodity->GetItem(5)->SetValue(strTmp.GetString());
					}
					//账户合计数量
					nCount = CommoditySoldInfo.GetSoldCount2() + CommoditySoldInfo.GetReturnCount2();

					if (nCount > 0)
					{
						pCommodity->GetItem(6)->SetValue(nCount);
					}
					//赠送合计数量
					if (CommoditySoldInfo.GetPresentCount() > 0)
					{
						pCommodity->GetItem(7)->SetValue(CommoditySoldInfo.GetPresentCount());
					}

					if (CommoditySoldInfo.GetExchangePoint())
					{
						pCommodity->GetItem(8)->SetValue(CommoditySoldInfo.GetExchangePoint());
					}
				}
			}
		}

		nStoreNum += Commodity.GetRemainQuantity();
		pLastGroup->AddSubItem(pCommodity, FALSE);//添加子列表

	}

	if (pLastGroup != NULL) //有商品数据
	{
		pLastGroup->GetItem(3)->SetValue(nStoreNum);
	}

	m_lpCommodityGrid->AdjustLayout();
}

//更新商品数据
//strHeader = L"商品类别|商品名称|商品编码|价格|库存|现金购物|金额|账户购物|赠送|兑换积分";
void CStoreDlg::FillPriceGird()
{
	m_lpCommodityGrid->RemoveAll();

	CCommodityArray CommodityArray;//待售商品队列
	CCommodityQueryCondition CommodityQueryCondition;//商品查询条件
	//从数据库取得库存数据
	if (!CIBADAL::GetInstance()->GetPriceCommodity(CommodityArray, CommodityQueryCondition)) return;

	CCommoditySoldInfoArray CommoditySoldInfoArray;
	if (!CIBADAL::GetInstance()->GetCommoditySoldInfo(CommoditySoldInfoArray)) return;

	//取得列数
	const INT nColumns = m_lpCommodityGrid->GetColumnCount();

	UINT nStoreNum = 0; //库存数量

	UINT nPrePrice = 0;//先前的价格

	CString strTmp;//临时变量
	CBCGPGridRow* pLastGroup = NULL;
	//商品记录
	for (INT i = 0; i < CommodityArray.GetCount(); i++)
	{	//商品信息
		CCommodity & Commodity = CommodityArray.GetAt(i);
		//分类头设置
		UINT nPrice = Commodity.GetAdvicePrice();
		if (nPrice != nPrePrice)
		{
			if (nStoreNum > 0)//数量大于0
			{
				pLastGroup->GetItem(4)->SetValue(nStoreNum);
				nStoreNum = 0;
			}

			nPrePrice = nPrice;
			//创建记录
			pLastGroup = m_lpCommodityGrid->CreateRow(nColumns);
			pLastGroup->AllowSubItems();// 关联式子菜单
			strTmp.Format(_T("%.2f"), Commodity.GetAdvicePrice() / 100.0);
			pLastGroup->GetItem(0)->SetValue(strTmp.GetString());
			//添加记录
			m_lpCommodityGrid->AddRow(pLastGroup, FALSE);
		}

		//此商品出现过
		CBCGPGridRow* pCommodity = m_lpCommodityGrid->CreateRow(nColumns);
		ASSERT_VALID(pCommodity);
		//设置子项
		pCommodity->GetItem(1)->SetValue(Commodity.GetCommodityName().GetString());
		pCommodity->GetItem(2)->SetValue(Commodity.GetBarCode().GetString());
		strTmp.Format(_T("%.2f"), Commodity.GetAdvicePrice() / 100.0);
		pCommodity->GetItem(3)->SetValue(Commodity.GetGroupName().GetString());
		pCommodity->GetItem(4)->SetValue(Commodity.GetRemainQuantity());

		if (theApp.GetCurCashier()->IsOnDuty())
		{
			for (INT j = 0; j < CommoditySoldInfoArray.GetCount(); j++)
			{
				CCommoditySoldInfo CommoditySoldInfo = CommoditySoldInfoArray.GetAt(j);

				if (CommoditySoldInfo.GetCommoditycode().CompareNoCase(Commodity.GetBarCode()) == 0)
				{
					//现金合计数量
					INT nCount = CommoditySoldInfo.GetSoldCount1() + CommoditySoldInfo.GetReturnCount1();

					if (nCount > 0)
					{
						pCommodity->GetItem(5)->SetValue(nCount);
						strTmp.Format(_T("%.2f"), (CommoditySoldInfo.GetSoldAmout1() + CommoditySoldInfo.GetReturnAmout1()) / 100.0);
						pCommodity->GetItem(6)->SetValue(strTmp.GetString());
					}
					//账户合计数量
					nCount = CommoditySoldInfo.GetSoldCount2() + CommoditySoldInfo.GetReturnCount2();

					if (nCount > 0)
					{
						pCommodity->GetItem(7)->SetValue(nCount);
					}
					//赠送合计数量
					if (CommoditySoldInfo.GetPresentCount() > 0)
					{
						pCommodity->GetItem(8)->SetValue(CommoditySoldInfo.GetPresentCount());
					}

					if (CommoditySoldInfo.GetExchangePoint())
					{
						pCommodity->GetItem(9)->SetValue(CommoditySoldInfo.GetExchangePoint());
					}
				}
			}

		}

		nStoreNum += Commodity.GetRemainQuantity();
		pLastGroup->AddSubItem(pCommodity, FALSE);//添加子列表
	}

	if (pLastGroup != NULL) //有商品数据
	{
		pLastGroup->GetItem(4)->SetValue(nStoreNum);
	}

	m_lpCommodityGrid->AdjustLayout();

}

//更新商品数据
//strHeader = L"商品类别|商品名称|商品编码|价格|库存|现金购物|金额|账户购物|赠送|兑换积分";
void CStoreDlg::FillSortGird()
{	
	m_lpCommodityGrid->RemoveAll();

	CCommodityArray CommodityArray;//商品队列
	CCommodityQueryCondition CommodityQueryCondition;//查询条件
	
	//取得商品库存数据和商品查询条件
	if (!CIBADAL::GetInstance()->GetStoreCommodity(CommodityArray, CommodityQueryCondition)) return;

	CCommoditySoldInfoArray CommoditySoldInfoArray;
	if (!CIBADAL::GetInstance()->GetCommoditySoldInfo(CommoditySoldInfoArray)) return;

	const INT nColumns = m_lpCommodityGrid->GetColumnCount();

	INT nTmp = 0; 

	CString strLastGroupId;
	CBCGPGridRow* pLastGroup = NULL;

	for (INT i = 0; i < CommodityArray.GetCount(); i++)
	{	
		CCommodity & CommodityInfo = CommodityArray.GetAt(i);

		CString strTmp = CommodityInfo.GetGroupId();

		if (strLastGroupId != strTmp)
		{	
			if (nTmp > 0)
			{
				pLastGroup->GetItem(4)->SetValue(nTmp);
				nTmp = 0;
			}

			strLastGroupId = strTmp;
			pLastGroup = m_lpCommodityGrid->CreateRow(nColumns);
			pLastGroup->AllowSubItems();
			pLastGroup->GetItem(0)->SetValue(CommodityInfo.GetGroupName().GetString());
			m_lpCommodityGrid->AddRow(pLastGroup, FALSE);
		}

		CBCGPGridRow* pCommodity = m_lpCommodityGrid->CreateRow(nColumns);
		ASSERT_VALID(pCommodity);

		pCommodity->GetItem(1)->SetValue(CommodityInfo.GetCommodityName().GetString());
		pCommodity->GetItem(2)->SetValue(CommodityInfo.GetBarCode().GetString());
		strTmp.Format(_T("%.2f"), CommodityInfo.GetAdvicePrice() / 100.0);
		pCommodity->GetItem(3)->SetValue(strTmp.GetString());
		pCommodity->GetItem(4)->SetValue(CommodityInfo.GetRemainQuantity());

		if (theApp.GetCurCashier()->IsOnDuty())
		{
			for (INT j = 0; j < CommoditySoldInfoArray.GetCount(); j++)
			{
				CCommoditySoldInfo CommoditySoldInfo = CommoditySoldInfoArray.GetAt(j);

				if (CommoditySoldInfo.GetCommoditycode().CompareNoCase(CommodityInfo.GetBarCode()) == 0)
				{
					//现金合计数量
					INT nCount = CommoditySoldInfo.GetSoldCount1() + CommoditySoldInfo.GetReturnCount1();

					if (nCount > 0)
					{
						pCommodity->GetItem(5)->SetValue(nCount);
						strTmp.Format(_T("%.2f"), (CommoditySoldInfo.GetSoldAmout1() + CommoditySoldInfo.GetReturnAmout1()) / 100.0);
						pCommodity->GetItem(6)->SetValue(strTmp.GetString());
					}
					//账户合计数量
					nCount = CommoditySoldInfo.GetSoldCount2() + CommoditySoldInfo.GetReturnCount2();

					if (nCount > 0)
					{
						pCommodity->GetItem(7)->SetValue(nCount);
					}
					//赠送合计数量
					if (CommoditySoldInfo.GetPresentCount() > 0)
					{
						pCommodity->GetItem(8)->SetValue(CommoditySoldInfo.GetPresentCount());
					}

					if (CommoditySoldInfo.GetExchangePoint())
					{
						pCommodity->GetItem(9)->SetValue(CommoditySoldInfo.GetExchangePoint());
					}
				}
			}
		}

		nTmp += CommodityInfo.GetRemainQuantity();

		pLastGroup->AddSubItem(pCommodity, FALSE);
	}

	if (pLastGroup != NULL) //有商品数据
	{
		pLastGroup->GetItem(4)->SetValue(nTmp);
	}

	m_lpCommodityGrid->AdjustLayout();

}

void CStoreDlg::OnCbnSelchangeComboSort()
{
	UINT nIndex = m_cboSortType.GetCurSel();

	switch(nIndex)
	{
	case 0:
		{
			InitSortGrid();
			FillSortGird();
			break;
		}
	case 1:
		{
			InitPriceGrid();
			FillPriceGird();
			break;
		}
	case 2:
		{
			InitSortPriceGrid();
			FillSortPriceGird();
			break;
		}
	default:
		break;
	}
}

void CStoreDlg::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
}

void CStoreDlg::OnBnClickedButtonPrintcommodity()
{
	//CStoreDlg::OnBnClickedOk();

	//BCGPPrintPreview(&m_CommodityGridView);

	m_lpCommodityGrid->ExpandAll();
	m_lpCommodityGrid->Print();

}

//void CStoreDlg::OnDestroy()
//{
//	//PostQuitMessage(0);
//
//	CIBADialog::OnDestroy();
//
//	// TODO: 在此处添加消息处理程序代码
//}
