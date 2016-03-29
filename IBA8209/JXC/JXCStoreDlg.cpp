// JXCStoreDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\BossPwdDlg.h"
#include ".\jxcstoredlg.h"
#include "..\IBAHelpper.h"
#include "jxccommodityseldlg.h"
#include "..\Cashier.h"
#include "..\ServletHellper.h"

// CJXCStoreDlg 对话框

IMPLEMENT_DYNAMIC(CJXCStoreDlg, CIBADialog)
CJXCStoreDlg::CJXCStoreDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CJXCStoreDlg::IDD, pParent)
, m_nStoreCount(0), m_nPrice(0), m_nUnitMutiple(1)
, m_nStoreCount2(0)
, m_bAllowPretranslate(TRUE)
, m_strStoreCount2(_T(""))
, m_strStoreCount(_T(""))
{
}

CJXCStoreDlg::~CJXCStoreDlg()
{
}

void CJXCStoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_NAME, m_strCommodityName);
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strCommodityType);
	DDX_Text(pDX, IDC_STATIC_STORE, m_strCommodityRemainQuantity);
	DDX_Control(pDX, IDC_LIST_COMMODITY, m_CommodityList);
	DDX_Text(pDX, IDC_STATIC_UINT, m_strCommodityUnit);
	DDX_Text(pDX, IDC_EDIT_COMMODITYID, m_strCommodityId);
	DDX_Text(pDX, IDC_EDIT_PRICE, m_strCommodityPrice);
	DDX_Text(pDX, IDC_STATIC_TOTALAMOUNT, m_strCommodityAmount);
	DDX_Text(pDX, IDC_STATIC_UINT2, m_strCommodityUnit2);
	DDX_Text(pDX, IDC_STATIC_PRICEUNIT, m_strPriceUnit);
	DDX_Text(pDX, IDC_STATIC_UNITMUTIPLE, m_strUnitMutiple);
	//DDX_Text(pDX, IDC_EDIT_COUNT, m_nStoreCount);
	//DDX_Text(pDX, IDC_EDIT_COUNT2, m_nStoreCount2);
	DDX_Control(pDX, IDC_EDIT_PRICE, m_edtPrice);
	DDX_Control(pDX, IDC_EDIT_COMMODITYID, m_edtCommodityId);
	//DDX_Control(pDX, IDC_EDIT_COUNT2, m_edtStoreCount2);
	//DDX_Control(pDX, IDC_EDIT_COUNT, m_edtStoreCount);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strResult);
	DDX_Text(pDX, IDC_EDIT_COUNT2, m_strStoreCount2);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_strStoreCount);
}

BEGIN_MESSAGE_MAP(CJXCStoreDlg, CIBADialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_COMMODITY, OnBnClickedButtonCommodity)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_PRICE, OnEnChangeEditPrice)
	ON_BN_CLICKED(IDC_BUTTON_SYNCSERVER, OnBnClickedButtonSyncserver)
	ON_EN_KILLFOCUS(IDC_EDIT_COUNT, OnEnKillfocusEditCount)
	ON_EN_KILLFOCUS(IDC_EDIT_COUNT2, OnEnKillfocusEditCount2)
	ON_WM_TIMER()
	ON_EN_UPDATE(IDC_EDIT_COUNT2, OnEnUpdateEditCount2)
	ON_EN_UPDATE(IDC_EDIT_COUNT, OnEnUpdateEditCount)
END_MESSAGE_MAP()

// CJXCStoreDlg 消息处理程序

BOOL CJXCStoreDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetEditFont();
	CIBAHelpper::CreateFont(m_BigFont, 20, 600);

	m_edtPrice.SetWindowText(_T("0.00"));
	m_edtPrice.SetFont(&m_BigFont);

	GetDlgItem(IDC_EDIT_COUNT)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_COUNT)->SetFont(&m_BigFont);

	GetDlgItem(IDC_EDIT_COUNT2)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_COUNT2)->SetFont(&m_BigFont);

	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_BigFont);

	GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(SW_HIDE);

	if (!CheckPopedom(_T("IBAP_STOREIN"))) //显示权限提示
	{
		GetDlgItem(IDC_STATIC_TIP)->ShowWindow(SW_NORMAL);
	}

	InitListView();

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

	return FALSE;  
}

BOOL CJXCStoreDlg::PreTranslateMessage(MSG* pMsg)
{
	//设定时间内不允许处理消息
	if (!m_bAllowPretranslate)
	{
		return CBCGPDialog::PreTranslateMessage(pMsg);
	}

	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam) //按回车
	{
		CWnd* pWnd = GetFocus(); //焦点何处

		if (pWnd != NULL)
		{
			if (pWnd->GetDlgCtrlID() == IDC_EDIT_COMMODITYID)
			{
				if (QueryCommodity())
				{
					m_bAllowPretranslate = FALSE;
					SetTimer(1001, 1000, NULL);
					return TRUE;
				}				
			}
			else if (pWnd->GetDlgCtrlID() == IDC_EDIT_COUNT)
			{
				if (GetDlgItem(IDC_EDIT_COUNT2)->IsWindowVisible())
				{	
					GetDlgItem(IDC_EDIT_COUNT2)->SetFocus();
				}
				else
				{
					OnBnClickedButtonAdd();
				}
				
				return TRUE;
			}
			else if (pWnd->GetDlgCtrlID() == IDC_EDIT_COUNT2)
			{
				OnBnClickedButtonAdd();
				return TRUE;
			}
		}
	}
	//ESC消息拦截
	if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam )
	{
		GetParent()->GetParent()->PostMessage(WM_CLOSE);
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}

HBRUSH CJXCStoreDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_EDIT == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_COUNT 
			|| pWnd->GetDlgCtrlID() == IDC_EDIT_PRICE
			|| pWnd->GetDlgCtrlID() == IDC_EDIT_COUNT2)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	return hbr;
}

void CJXCStoreDlg::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_STORE_IN_HEAD);
	//strHeader = L"商品名称|商品编码|单价|单位|入库数量|合计总额|库存量";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {250, 140, 50, 50, 90, 100, 90};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_CommodityList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_CommodityList.SetFont(&m_Font);
	m_CommodityList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//整行选中风格+网格行风格
}

void CJXCStoreDlg::OnBnClickedButtonCommodity()
{
	//商品选择对话框
	CJXCCommoditySelDlg CommodityDlg(this);
	
	if (CommodityDlg.DoModal() == IDOK)
	{
		//从对话框取得信息
		GetDataFromDlg(CommodityDlg);

		UpdateData(FALSE);
	}
}

void CJXCStoreDlg::OnBnClickedButtonAdd()
{
	UpdateData(TRUE);
	m_nStoreCount = _tstoi(m_strStoreCount);
	m_nStoreCount2 = _tstoi(m_strStoreCount2);

	if (m_strCommodityId.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_COMMODITYID);
		ShowToolTip(_T("商品编号不能为空！"));
		return ;
	}

	if (m_nPrice < 0)
	{
		SetToolTipPos(IDC_EDIT_PRICE);
		ShowToolTip(_T("商品单价不能为0！"));
		return ;
	}

	if (m_nUnitMutiple > 1)
	{
		if (m_nStoreCount < 1 && m_nStoreCount2 < 1)
		{
			SetToolTipPos(IDC_EDIT_COUNT);
			ShowToolTip(_T("商品数量不能为0！"));
			return ;
		}
	}
	else
	{
		if (m_nStoreCount < 1 )
		{
			SetToolTipPos(IDC_EDIT_COUNT);
			ShowToolTip(_T("商品数量不能为0！"));
			return ;
		}
	}

	CString strTmp;
	m_CommodityList.InsertItem(0, m_strCommodityName);
	m_CommodityList.SetItemText(0, 1, m_strCommodityId);

	strTmp.Empty();
	//进行一次转换，防止出现错误,因为可输入方式没有进行格式限制
	strTmp.Format(_T("%.2f"), m_nPrice / 100.0);
	m_CommodityList.SetItemText(0, 2, strTmp);

	if (m_nUnitMutiple > 1)
	{
		m_CommodityList.SetItemText(0, 3, m_strCommodityUnit2);
	} 
	else
	{
		m_CommodityList.SetItemText(0, 3, m_strCommodityUnit);
	}

	strTmp.Format(_T("%d"), m_nStoreCount * m_nUnitMutiple + m_nStoreCount2);
	m_CommodityList.SetItemText(0, 4, strTmp);
	
	strTmp.Format(_T("%.2f"), (m_nStoreCount * m_nUnitMutiple + m_nStoreCount2) * m_nPrice / 100.0);
	m_CommodityList.SetItemText(0, 5, strTmp);

	m_CommodityList.SetItemText(0, 6, m_strCommodityRemainQuantity);

	ClearData();
	m_strCommodityId.Empty();

	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();
}

void CJXCStoreDlg::OnBnClickedButtonDel()
{
	INT nCurSelIndex = m_CommodityList.GetSelectionMark();
	//越界
	if (nCurSelIndex < 0 && nCurSelIndex >= m_CommodityList.GetItemCount())
	{
		return;
	}
	//移除
	m_CommodityList.DeleteItem(nCurSelIndex);

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

	CRect rect;
	m_CommodityList.GetWindowRect(&rect);
	RedrawWindow(&rect);
}

BOOL CJXCStoreDlg::CheckPopedom(LPCTSTR lpszModuleName)
{
	BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 

	if (bRet)
	{
		bRet = theApp.GetCurCashier()->HasPopedomOf(lpszModuleName);
	}

	return bRet;
}

void CJXCStoreDlg::OnBnClickedOk()
{
	if (!CheckPopedom(_T("IBAP_STOREIN")))
	{
		CDlgBossPwd PwdDlg(this);
		
		if (PwdDlg.DoModal() != IDOK)
		{
			return;
		}
	}

	UpdateData(TRUE);

	NS_SERVLET::CStoreInRecordArray StoreInRecordArray;
	MakeStoreInRecord(StoreInRecordArray);

	if (StoreInRecordArray.GetCount() <= 0)
	{
		return;
	}

	SetToolTipPos(IDOK);

	NS_SERVLET::CStoreInMethod StoreInMethod;
	StoreInMethod.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());
	StoreInMethod.SetCount(StoreInRecordArray.GetCount());
	StoreInMethod.SetStoreInRecords(StoreInRecordArray);
	StoreInMethod.SetPayType( CIBAConfig::GetInstance()->GetJXCPayCash() );
	theApp.GetCurCashier()->DoStoreIn(StoreInMethod);

	if (StoreInMethod.GetStatusCode() == 0)
	{
		m_CommodityList.DeleteAllItems();

		UINT nCurAmount = 0;
		
		for (INT i = 0; i < StoreInRecordArray.GetCount(); i++)
		{
			nCurAmount += StoreInRecordArray.GetAt(i).GetAmount();

			//更新库存
			if (StoreInRecordArray.GetAt(i).GetRemainQuantity() == 0) //之前没有库存则插入
			{
				COleDateTime dtNow = COleDateTime::GetCurrentTime();

				CString strSQL;

				strSQL.Format(_T("insert into storetotal(accountYear, accountMonth, netBarId, storeId, commodityCode, remainQuantity) values(%d,%d,%d,%d,'%s',0)"),
					dtNow.GetYear(), dtNow.GetMonth(), CNetBarConfig::GetInstance()->GetNetBarId(), 
					CIBAConfig::GetInstance()->GetStoreID(), StoreInRecordArray.GetAt(i).GetCommodityCode());

				CIBADAL::GetInstance()->ExecuteSQL(strSQL);
			}

			//CIBADAL::GetInstance()->UpdateCommodityStoreInQuantity(StoreInRecordArray.GetAt(i).GetCommodityCode(),
			//	StoreInRecordArray.GetAt(i).GetQuantity(), StoreInRecordArray.GetAt(i).GetAmount());

			CIBADAL::GetInstance()->UpdateCommodityRemainQuantity(StoreInRecordArray.GetAt(i).GetCommodityCode(),
				StoreInRecordArray.GetAt(i).GetQuantity() + StoreInRecordArray.GetAt(i).GetRemainQuantity());
		}

		ClearData();

		m_strResult.Format(_T("入库金额共计： %.2f 元！"), nCurAmount / 100.0);
		GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

		UpdateData(FALSE);

		ShowToolTip(StoreInMethod.GetStatusMessage());
	} 
	else
	{
		CString strTmp;
		strTmp.Format(L"入库失败！原因：%s - %s", StoreInMethod.GetStatusMessage(), StoreInMethod.GetExtInfo());
		ShowToolTip(strTmp, TRUE);
	}
}

void CJXCStoreDlg::MakeStoreInRecord(NS_SERVLET::CStoreInRecordArray &StoreInRecordArray)
{
	for (INT i = 0; i < m_CommodityList.GetItemCount(); i++)
	{
		AddRecord(StoreInRecordArray, i);
	}
}

BOOL CJXCStoreDlg::AddRecord(NS_SERVLET::CStoreInRecordArray &StoreInRecordArray, INT i)
{
	BOOL bFind = FALSE;

	INT j = 0;
	for (j = 0;j < StoreInRecordArray.GetCount(); j++)
	{
		CString strCode = m_CommodityList.GetItemText(i, 1);
		if (strCode.CompareNoCase(StoreInRecordArray.GetAt(j).GetCommodityCode()) == 0)
		{
			bFind = TRUE;
			break;
		}
	}

	if (bFind)
	{
		long nQuantity = _tstoi(m_CommodityList.GetItemText(i, 4)) + StoreInRecordArray.GetAt(j).GetQuantity();
		long nAmount = nQuantity * StoreInRecordArray.GetAt(j).GetPrice();
		StoreInRecordArray.GetAt(j).SetQuantity(nQuantity);
		StoreInRecordArray.GetAt(j).SetAmount(nAmount);
	}
	else
	{
		NS_SERVLET::CStoreInRecord StoreInRecord;
		StoreInRecord.SetCommodityCode(m_CommodityList.GetItemText(i, 1));
		long nQuantity = _tstoi(m_CommodityList.GetItemText(i, 4));
		long nPrice = _tstof(m_CommodityList.GetItemText(i, 2)) * 100;
		long nAmount = nQuantity * nPrice;
		StoreInRecord.SetQuantity(nQuantity);
		StoreInRecord.SetRemainQuantity(_tstoi(m_CommodityList.GetItemText(i, 6)));
		StoreInRecord.SetPrice(nPrice);
		StoreInRecord.SetAmount(nAmount);

		StoreInRecordArray.Add(StoreInRecord);
	}	

	return bFind;
}

void CJXCStoreDlg::OnEnChangeEditPrice()
{
	UpdateData();
	m_nStoreCount = _tstoi(m_strStoreCount);
	m_nStoreCount2 = _tstoi(m_strStoreCount2);

	CString strTmp;
	GetDlgItem(IDC_EDIT_PRICE)->GetWindowText(strTmp);
	m_nPrice = _tstof(m_strCommodityPrice) * 100;
	
	if (m_nUnitMutiple > 1)
	{
		UINT nTmp = m_nPrice * (m_nStoreCount * m_nUnitMutiple + m_nStoreCount2);
		m_strCommodityAmount.Format(_T("%.2f元"), nTmp / 100.0);
	} 
	else
	{
		m_strCommodityAmount.Format(_T("%.2f元"), m_nPrice * m_nStoreCount / 100.0);
	}
	
	UpdateData(FALSE);
}

void CJXCStoreDlg::ClearData()
{
	//结果
	m_strResult.Empty();
	//不清编号
	//m_strCommodityId.Empty();
	m_strCommodityName.Empty();
	m_strCommodityType.Empty();
	m_strCommodityRemainQuantity.Empty();
	// 商品单位
	m_strCommodityUnit.Empty();
	m_strCommodityUnit2.Empty();
	m_nUnitMutiple = 1;
	m_strUnitMutiple.Empty();
	m_strPriceUnit.Empty();
	// 入库数量
	m_strStoreCount.Empty();
	m_strStoreCount2.Empty();;
	m_nStoreCount = 0;
	m_nStoreCount2 = 0;
	//采购单价（备份数据）
	m_nPrice = 0;
	// 采购单价（捆绑）
	m_strCommodityPrice.Empty();
	//总价
	m_strCommodityAmount.Empty();
	//隐藏必要的
	GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(FALSE);

}

void CJXCStoreDlg::OnBnClickedButtonSyncserver()
{
	CWaitCursor WaitCursor;

	GetDlgItem(IDC_BUTTON_SYNCSERVER)->EnableWindow(FALSE);

	SetToolTipPos(IDC_BUTTON_SYNCSERVER);

	if (CServletHellper::Synchronize())
	{
		ShowToolTip(_T("数据同步成功！"));
	}
	else
	{
		ShowToolTip(_T("数据同步失败！"), TRUE);
	}

	GetDlgItem(IDC_BUTTON_SYNCSERVER)->EnableWindow(TRUE);
}

BOOL CJXCStoreDlg::QueryCommodity()
{
	UpdateData();
	m_nStoreCount = _tstoi(m_strStoreCount);
	m_nStoreCount2 = _tstoi(m_strStoreCount2);

	if (m_strCommodityId.IsEmpty())
	{
		return FALSE;
	}

	//先清除数据
	ClearData();

	CCommodityQueryCondition CommodityQueryCondition;
	CommodityQueryCondition.m_strCommodityId = m_strCommodityId;

	CCommodityArray CommodityArray;
	CIBADAL::GetInstance()->GetCommodity(CommodityArray, CommodityQueryCondition);

	if (CommodityArray.GetCount() > 1) //多个商品
	{
		CJXCCommoditySelDlg JXCCommoditySelDlg(this);
		JXCCommoditySelDlg.SetCommodityCode(m_strCommodityId);
		
		if (JXCCommoditySelDlg.DoModal() == IDOK)
		{
			GetDataFromDlg(JXCCommoditySelDlg);
		}
	}
	else
	{
		if (CommodityArray.GetCount() > 0)
		{
			m_strCommodityId = CommodityArray.GetAt(0).GetCommodityCode();
			m_strCommodityName = CommodityArray.GetAt(0).GetCommodityName();
			m_strCommodityType = CommodityArray.GetAt(0).GetGroupName();
			m_strCommodityRemainQuantity.Format(_T("%d"), CommodityArray.GetAt(0).GetRemainQuantity());

			m_nUnitMutiple = CommodityArray.GetAt(0).GetUnitMultiple();
			m_strCommodityPrice.Format(_T("%.2f"), CommodityArray.GetAt(0).GetContractPrice() / m_nUnitMutiple / 100.0);
			
			m_nPrice = CommodityArray.GetAt(0).GetContractPrice() / m_nUnitMutiple;
			
			if (m_nUnitMutiple > 1)
			{
				CIBADAL::GetInstance()->GetCommodityUint(CommodityArray.GetAt(0).GetStoreUnit(), m_strCommodityUnit);
				CIBADAL::GetInstance()->GetCommodityUint(CommodityArray.GetAt(0).GetRetailUnit(), m_strCommodityUnit2);
				
				m_strUnitMutiple.Format(_T("(1%s = %d%s)"), 
					m_strCommodityUnit, m_nUnitMutiple, m_strCommodityUnit2);

				m_strPriceUnit.Format(_T("元/%s"), m_strCommodityUnit2);
				GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(SW_SHOW);
			}
			else
			{
				//商品单位
				CIBADAL::GetInstance()->GetCommodityUint(CommodityArray.GetAt(0).GetRetailUnit(), m_strCommodityUnit);
				m_strCommodityUnit2.Empty();
				m_strUnitMutiple.Empty();

				m_strPriceUnit.Format(_T("元/%s"), m_strCommodityUnit);

				GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(SW_HIDE);
			}
			//总价
			UINT nTmp = m_nPrice * (m_nStoreCount * m_nUnitMutiple + m_nStoreCount2);
			m_strCommodityAmount.Format(_T("%.2f元"), nTmp / 100.0);

			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COUNT)->SetFocus();
		}
		else
		{
			SetToolTipPos(IDC_EDIT_COMMODITYID);
			ShowToolTip(IDS_NOCOMMODITY);
		}
	}

	UpdateData(FALSE);

	return TRUE;
}

void CJXCStoreDlg::GetDataFromDlg(CJXCCommoditySelDlg &CommodityDlg)
{
	ClearData();
	m_strCommodityId = CommodityDlg.GetCommodityCode();
	m_strCommodityName = CommodityDlg.GetCommodityName();
	m_strCommodityType = CommodityDlg.GetCommodityType();
	m_nStoreCount = _tstoi(m_strStoreCount);
	m_nStoreCount2 = _tstoi(m_strStoreCount2);
	//采购价
	m_strCommodityPrice.Format(_T("%.2f"), CommodityDlg.GetPrice() / 100.0);
	m_nPrice = CommodityDlg.GetPrice();

	m_strPriceUnit.Format(_T("元/%s"), CommodityDlg.GetCommodityRetailUint());
	//库存数目
	m_strCommodityRemainQuantity.Format(_T("%d"), CommodityDlg.GetDepositCount());
	//换算率
	m_nUnitMutiple = CommodityDlg.GetUnitMutiple();
	
	if (m_nUnitMutiple > 1)
	{
		m_strCommodityUnit = CommodityDlg.GetCommodityStoreUint();
		m_strCommodityUnit2 = CommodityDlg.GetCommodityRetailUint();
		m_strUnitMutiple.Format(_T("(1%s = %d%s)"), 
			m_strCommodityUnit, m_nUnitMutiple, m_strCommodityUnit2);
		GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(TRUE);

		//总价
		UINT nTmp = CommodityDlg.GetPrice() * (m_nStoreCount * m_nUnitMutiple + m_nStoreCount2);
		m_strCommodityAmount.Format(_T("%.2f元"), nTmp / 100.0);
	}
	else
	{
		//商品单位
		m_strCommodityUnit = CommodityDlg.GetCommodityRetailUint();
		m_strCommodityUnit2.Empty();
		m_strUnitMutiple.Empty();
		GetDlgItem(IDC_EDIT_COUNT2)->ShowWindow(FALSE);

		//总价
		UINT nTmp = CommodityDlg.GetPrice() * m_nStoreCount;
		m_strCommodityAmount.Format(_T("%.2f元"), nTmp / 100.0);
	}

	GetDlgItem(IDC_EDIT_COUNT)->SetFocus();
}

void CJXCStoreDlg::OnEnKillfocusEditCount()
{//BCGP的BUG，必须重绘
	RedrawWindow();
}

void CJXCStoreDlg::OnEnKillfocusEditCount2()
{
	RedrawWindow();
}

void CJXCStoreDlg::OnTimer(UINT nIDEvent)
{//延时处理
	if (nIDEvent == 1001)
	{
		m_bAllowPretranslate = TRUE;
	}

	CIBADialog::OnTimer(nIDEvent);
}

void CJXCStoreDlg::OnEnUpdateEditCount2()
{
	OnEnChangeEditPrice();
}

void CJXCStoreDlg::OnEnUpdateEditCount()
{
	OnEnChangeEditPrice();
}
