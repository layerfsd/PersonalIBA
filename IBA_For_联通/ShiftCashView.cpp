// ShiftCashView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "dal\IBADAL.h"
#include ".\shiftcashview.h"


// CShiftCashView

IMPLEMENT_DYNCREATE(CShiftCashView, CIBAFormView)

CShiftCashView::CShiftCashView()
:CIBAFormView(CShiftCashView::IDD)
{
	m_bUseDeposit = TRUE;
	InitData();
}

CShiftCashView::~CShiftCashView()
{
}

void CShiftCashView::DoDataExchange(CDataExchange* pDX)
{
	CIBAFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strResult);
	DDX_Control(pDX, IDC_LIST_COMMON, m_List);
}

BEGIN_MESSAGE_MAP(CShiftCashView, CIBAFormView)
	//ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CShiftCashView 消息处理程序

void CShiftCashView::OnInitialUpdate()
{
	CIBAFormView::OnInitialUpdate();

	InitListView();
	SetEditFont();
	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC_SHIFTCASH)->SetFont(&m_Font);
}

void CShiftCashView::OnClose()
{
	INT nIdx = theApp.GetIBAView()->FindTab(GetSafeHwnd());
	theApp.GetIBAView()->GetTabControl().ShowTab(nIdx, FALSE);

	m_List.DeleteAllItems();

	m_strOperator.Empty();
	m_strResult.Empty();
	UpdateData(FALSE);
}

void CShiftCashView::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(0, 0, cx, cy, FALSE);

	if (m_List.GetSafeHwnd() != NULL)
	{
		m_List.MoveWindow(10, 90, cx - 15, cy - 95, TRUE);
	}
}

void CShiftCashView::OnBnClickedButtonQuery()
{
	SetToolTipPos(IDC_BUTTON_QUERY);

	UpdateData();

	InitData();

	m_List.DeleteAllItems();

	QueryCenter();

	UpdateData(FALSE);
}

void CShiftCashView::InitData()
{
	m_nCreditMoney = 0;
	m_nCashSoldAmount = 0;
	m_nPayout = 0;
	m_nReturnedMoney = 0;
	m_nTotalAmount = 0;

	m_strResult.Empty();

	m_nDiankaSold = 0;
}

enum{
	ShiftBeginTime,  // 班次开始时间
	ShiftEndTime,    // 班次结束时间
	ShiftType,       // 班别
	Casher,          // 交款人
	HandInMoney,     // 上交金额
	NextReserve,     // 留给下一班
	CreditMoney,     // 充值金额
	ReturnMoney,     // 退款金额
	CreditCardMoney, // 充值卡金额
	CashConsume,     // 现金购物
	AccountConsume,  // 账户购物
	StorePayout,     // 入库支出
	OtherPayout,     // 其他支出
	Remark,          // 备注
	Deposit,         // 押金
	DiankaSold       // 点卡销售
};
//static const UINT Width[] = {115, 115, 50, 70, 70, 70, 70, 70, 60, 85, 85, 85, 70, 100};
static const UINT nStrIndex[] = {
	ShiftBeginTime, ShiftEndTime, ShiftType,       Casher,      HandInMoney,    NextReserve,
		CreditMoney,    ReturnMoney,  CreditCardMoney, CashConsume, AccountConsume, DiankaSold,
		StorePayout,    OtherPayout,  Remark, 
};
static const UINT Width[] = {
	115,            115,          50,              70,           70,            70,
		70,             70,           60,              85,           85,            85,
		90,             100,          100
};

void CShiftCashView::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_SHIFTCASHHEADER);

	//strHeader = L"班次开始时间|班次结束时间|班别|交款人|上交金额|留给下一班|充值金额|退款金额|充值卡金额|现金购物|账户购物|入库支出|其他支出|备注|押金|点卡销售";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);


	ASSERT(sizeof(Width) == sizeof(nStrIndex));

	int nLen = sizeof(Width) / sizeof(UINT);
	for (int i = 0; i < nLen; i++)
	{
		m_List.InsertColumn(i, strArray.GetAt( nStrIndex[i] ), LVCFMT_LEFT, Width[i]);
	}

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

HBRUSH CShiftCashView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBAFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	return hbr;
}

void CShiftCashView::QueryLocalDB()
{
	//COleDateTime dtTmp;
	//dtTmp = COleDateTime::GetCurrentTime();
	//dtTmp -= COleDateTimeSpan(3, 0, 0, 0);

	//CString strStartTime(dtTmp.Format(_T("%Y-%m-%d %H:%M")));

	//CString strEndTime(COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M")));

	//CShiftCashArray ShiftCashArray;

	//if (CIBADAL::GetInstance()->GetShiftCash(ShiftCashArray, strStartTime, strEndTime, m_strOperator))
	//{
	//	for (INT i = 0; i < ShiftCashArray.GetCount(); i++)
	//	{
	//		CString strTmp;

	//		m_List.InsertItem(i, ShiftCashArray.GetAt(i).GetStartDate());

	//		m_List.SetItemText(i, 1, ShiftCashArray.GetAt(i).GetEndDate());

	//		m_List.SetItemText(i, 2, ShiftCashArray.GetAt(i).GetNextOperator());

	//		m_List.SetItemText(i, 3, ShiftCashArray.GetAt(i).GetOperator());

	//		m_nTotalAmount += ShiftCashArray.GetAt(i).GetTotalAmount();
	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetTotalAmount() / 100.0);
	//		m_List.SetItemText(i, 4, strTmp);

	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetRemainAmount() / 100.0);
	//		m_List.SetItemText(i, 5, strTmp);

	//		m_nCreditMoney += ShiftCashArray.GetAt(i).GetCreditAmount();
	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetCreditAmount() / 100.0);
	//		m_List.SetItemText(i, 6, strTmp);

	//		m_nReturnedMoney += ShiftCashArray.GetAt(i).GetReturnAmount();
	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetReturnAmount() / 100.0);
	//		m_List.SetItemText(i, 7, strTmp);

	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetCardCreditedAmount() / 100.0);
	//		m_List.SetItemText(i, 8, strTmp);

	//		m_nCashSoldAmount += ShiftCashArray.GetAt(i).GetCashSoldAmount();
	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetCashSoldAmount() / 100.0);
	//		m_List.SetItemText(i, 9, strTmp);

	//		m_nPayout += ShiftCashArray.GetAt(i).GetPayout();
	//		strTmp.Format(_T("%.2f"), ShiftCashArray.GetAt(i).GetPayout() / 100.0);
	//		m_List.SetItemText(i, 10, strTmp);

	//		m_List.SetItemText(i, 11, ShiftCashArray.GetAt(i).GetDescription());
	//	}

	//	m_strResult.Format(IDS_SHIFTCASHQUERYRESULT,
	//		m_nCreditMoney / 100.0, m_nReturnedMoney / 100.0, m_nCashSoldAmount / 100.0,
	//		m_nPayout / 100.0, m_nTotalAmount / 100.0);
	//}
}

void CShiftCashView::QueryCenter()
{
	CShiftListMethod ShiftList;
	ShiftList.SetOperator(m_strOperator);
	theApp.GetCurCashier()->DoShiftList(ShiftList);

	if (ShiftList.GetStatusCode() != 0)
	{
		ShowToolTip(ShiftList.GetStatusMessage(), TRUE);
		return;
	}

	for (INT i = 0; i < ShiftList.GetRecordCount(); i++)
	{
		CString strTmp; INT nTmp = 0, nIdx = 0;

		m_List.InsertItem(i, _T(""));
		for(INT iCol = 0; iCol < m_List.GetHeaderCtrl().GetItemCount(); iCol++)
		{
			strTmp = _T("");

		switch(nStrIndex[iCol]){

		case ShiftBeginTime:
			strTmp = CIBAHelpper::FormatCenterTime(ShiftList.GetOnDutyTime(i));
			break;

		case ShiftEndTime:
			strTmp = CIBAHelpper::FormatCenterTime(ShiftList.GetOffDutyTime(i));
			break;

		case ShiftType:
			{
				switch (ShiftList.GetShiftType(i))
				{
				case 1:
					strTmp = CIBAConfig::GetInstance()->GetShiftName1();
					break;

				case 2:
					strTmp = CIBAConfig::GetInstance()->GetShiftName2();
					break;

				case 3:
					strTmp = CIBAConfig::GetInstance()->GetShiftName3();
					break;
				}
			}
			break;

		case Casher:
			strTmp = ShiftList.GetOperator(i);
			break;

		case  HandInMoney:
			{
				nTmp = ShiftList.GetFetchAmount(i);
				m_nTotalAmount += nTmp;

				//if (nTmp != 0)
				//{
				strTmp.Format(_T("%.2f"), nTmp / 100.0);
				//}	
			}
			break;

		case NextReserve:
			{
				nTmp = ShiftList.GetRemainAmount(i);
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case CreditMoney:
			{
				nTmp = ShiftList.GetCreditAmount(i);
				m_nCreditMoney += nTmp;
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case ReturnMoney:
			{
				nTmp = ShiftList.GetReturnAmount(i);
				m_nReturnedMoney += nTmp;
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case CreditCardMoney:
			{
				nTmp = ShiftList.GetCardCredit(i);
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case Deposit:
			{
				nTmp = ShiftList.GetDeposit(i);
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case CashConsume:
			{
				nTmp = ShiftList.GetCashPay(i);
				m_nCashSoldAmount += nTmp;
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case AccountConsume:
			{
				nTmp = ShiftList.GetNetBarPay(i);
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case StorePayout:
			{
				nTmp = ShiftList.GetStoreInAmount(i);
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case OtherPayout:
			{
				nTmp = ShiftList.GetPayout(i);
				m_nPayout += nTmp;
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;

		case Remark:
			strTmp = ShiftList.GetDescription(i);
			break;

		case DiankaSold:
			{
				nTmp = ShiftList.GetDiankaSold(i);
				m_nDiankaSold += nTmp;
				if (nTmp != 0)
				{
					strTmp.Format(_T("%.2f"), nTmp / 100.0);
				}
			}
			break;
		}

			m_List.SetItemText(i, iCol, strTmp);

		}// end-for-col

	}

	m_strResult.Format(IDS_SHIFTCASHQUERYRESULT,
		m_nCreditMoney / 100.0, 
		m_nReturnedMoney / 100.0, 
		m_nCashSoldAmount / 100.0,
		m_nDiankaSold / 100.0,
		m_nPayout / 100.0, 
		m_nTotalAmount / 100.0);

}