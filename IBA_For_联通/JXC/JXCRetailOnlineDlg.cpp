// RetailOnline.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\Cashier.h"
#include "..\IBAHelpper.h"
#include ".\jxcretailonlinedlg.h"
#include "..\Socket\LocalServer.h"

// CJXCRetailOnlineDlg 对话框

IMPLEMENT_DYNAMIC(CJXCRetailOnlineDlg, CNDDialog)
CJXCRetailOnlineDlg::CJXCRetailOnlineDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CJXCRetailOnlineDlg::IDD, pParent)
, m_bAllowPretranslate(TRUE)
, m_nCommodityCount(1)
, m_CommodityDlg(this, 3), m_bOnline(2)
{//接受其他窗体的账号
	m_strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
}

CJXCRetailOnlineDlg::~CJXCRetailOnlineDlg()
{
}

void CJXCRetailOnlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_COMMODITYID, m_strCommodityId);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strCommodityName);
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strCommodityType);
	DDX_Text(pDX, IDC_STATIC_PRICE, m_strCommodityPrice);
	DDX_Text(pDX, IDC_STATIC_STORE, m_strCommodityRemainQuantity);

	DDX_Control(pDX, IDC_EDIT_COUNT, m_edtCount);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);

	DDX_Text(pDX, IDC_STATIC_MONEY, m_strAllMoney);
	DDX_Control(pDX, IDC_LIST_COMMODITY, m_CommodityList);

	DDX_Text(pDX, IDC_EDIT_TERMID, m_strTermId);
}

BEGIN_MESSAGE_MAP(CJXCRetailOnlineDlg, CNDDialog)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_COMMODITY, OnBnClickedButtonCommodity)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CJXCRetailOnlineDlg 消息处理程序

BOOL CJXCRetailOnlineDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();
	//未查询，初始不在线
	m_bOnline = 2;

	InitListView();

	SetEditFont();

	CIBAHelpper::CreateFont(m_BigFont, 23, 700);
	GetDlgItem(IDC_STATIC_MONEY)->SetFont(&m_BigFont);
	//编辑框的各种限制
	m_edtCount.DisableMask();
	m_edtCount.SetValidChars(_T("1234567890"));
	m_edtCount.SetLimitText(3);
	m_edtCount.SetWindowText(_T("1"));

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	//捆绑快捷键
	AddFocusEnter(IDC_EDIT_IDCARDSNO, IDC_BUTTON_QUERY);
	AddFocusEnter(IDC_EDIT_TERMID, IDC_BUTTON_QUERY);
	AddFocusEnter(IDC_EDIT_COUNT, IDC_BUTTON_ADD);
	//首次焦点位置
	GetDlgItem(IDC_EDIT_TERMID)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCRetailOnlineDlg::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_ONLINE_HEAD);
	//strHeader = L"商品名称|商品编码|价格|数量|金额"; //|商品类别|商品条码

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {250, 140, 70, 70, 70};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_CommodityList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_CommodityList.SetFont(&m_Font);
	m_CommodityList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CJXCRetailOnlineDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_nCardId = nCardId;
	m_nMemberId = nMemberId;

	QueryBalance(m_nMemberId, nCardId);

	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

	GetDlgItem(IDOK)->EnableWindow(m_SellRecordArray.GetCount() > 0 && m_nMemberId > 0);

	if (m_bOnline != 1)
	{
		GetDlgItem(IDC_EDIT_TERMID)->SetFocus();
		SetToolTipPos(IDC_BUTTON_QUERY);
		ShowToolTip(_T("用户离线，请上线后再操作！"));
	}
}

void CJXCRetailOnlineDlg::OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance)
{
	m_strNetId = Balance.GetSerailNum();
	m_strName = Balance.GetUserName();

	m_nUserClassID = Balance.GetUserClass();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
	m_strAllBalance.Format(_T("%.2f"), nAllMoney / 100.0);

	m_bOnline = Balance.GetOnline();
}

void CJXCRetailOnlineDlg::OnBnClickedButtonDel()
{
	INT nIdx = m_CommodityList.GetSelectionMark();

	if (nIdx < 0)
	{
		return;
	}

	m_SellRecordArray.RemoveAt(nIdx);

	ShowSellRecordInfo();
}

void CJXCRetailOnlineDlg::OnBnClickedButtonClear()
{
	if (IDOK == MsgBox(L"取消本次销售吗？", MB_OKCANCEL | MB_ICONQUESTION))
	{
		m_strAllMoney.Empty();
		ClearData();
		UpdateData(FALSE);
	}
}
//执行兑换
void CJXCRetailOnlineDlg::OnBnClickedOk()
{
	if (m_SellRecordArray.GetCount() <= 0)
	{
		GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();
		SetToolTipPos(IDOK);
		ShowToolTip(L"请先选择用户要购买的商品！");
		return;
	}

	if (m_bOnline != 1)
	{
		GetDlgItem(IDC_EDIT_TERMID)->SetFocus();
		SetToolTipPos(IDOK);
		ShowToolTip(L"离线用户不能执行在线购物！");
		return;
	}

	if (_tstof(m_strAllBalance) - _tstof(m_strAllMoney) < 0)
	{
		GetDlgItem(IDC_EDIT_TERMID)->SetFocus();
		SetToolTipPos(IDOK);
		ShowToolTip(IDS_NEEDMOREMONEY);

		CIBAString strMsgContent;
		strMsgContent.LoadString(IDS_NEEDMOREMONEY);
		CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(m_nMemberId, strMsgContent);

		return;
	}

	CWaitCursor Wait;

	CRetailOnlineMethod RetailOnline;

	RetailOnline.SetPayType(2);
	RetailOnline.SetMemberId(m_nMemberId);
	RetailOnline.SetRetailType(1);
	RetailOnline.SetSellRecords(m_SellRecordArray);
	theApp.GetCurCashier()->DoRetailOnline(RetailOnline);

	SetToolTipPos(IDOK);

	if (RetailOnline.GetStatusCode() == 0)
	{
		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
			m_nMemberId, 0, 0, _tstof(m_strAllMoney) * 100);
		
		SaveRetailData(RetailOnline);

		ShowToolTip(L"购物成功！");
		CString strMsgContent;
		strMsgContent.Format(_T("在线购物成功, 您本次在线消费金额为 %s 元"), m_strAllMoney);
		CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(m_nMemberId, strMsgContent);

		ClearData();
		m_strAllMoney.Empty();
		m_strNetId.Empty();

		// 2011/09/02-8201-gxx:保存最后交易记录 
		//
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(RetailOnline.GetRefNo());
		TranInfo.SetLastTranTime(RetailOnline.GetTranTime());
		TranInfo.SetLastTranType(7);
		TranInfo.Save();

		
		UpdateData(FALSE);
	}
	else
	{
		CString strTmp;
		strTmp.Format(L"购物失败！原因：%s - %s", RetailOnline.GetStatusMessage(), RetailOnline.GetExtInfo());

		ShowToolTip(strTmp, TRUE);
	}

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();
}

void CJXCRetailOnlineDlg::OnBnClickedButtonCommodity()
{
	m_CommodityDlg.m_CommodityArray.RemoveAll();
	m_CommodityDlg.DoModal();
	m_CommodityList.SetFocus();
}

void CJXCRetailOnlineDlg::OnBnClickedButtonAdd()
{
	CString strTmp;
	m_edtCount.GetWindowText(strTmp);
	m_nCommodityCount = _ttoi(strTmp);

	if (m_nCommodityCount <= 0)
	{
		SetToolTipPos(IDC_EDIT_COUNT);
		ShowToolTip(IDS_INPUTERROR);
		return;
	}

	NS_SERVLET::CSellRecord SellRecord;

	SellRecord.SetQuantity(m_nCommodityCount);
	SellRecord.SetCommodityCode(m_CommodityDlg.m_CommodityArray.GetAt(0).GetCommodityCode());
	SellRecord.SetPrice(m_CommodityDlg.m_CommodityArray.GetAt(0).GetAdvicePrice());
	SellRecord.SetAmount(m_nCommodityCount * m_CommodityDlg.m_CommodityArray.GetAt(0).GetAdvicePrice());
	SellRecord.SetCommodityBarCode(m_CommodityDlg.m_CommodityArray.GetAt(0).GetBarCode());
	SellRecord.SetCommodityName(m_CommodityDlg.m_CommodityArray.GetAt(0).GetCommodityName());
	SellRecord.SetPoint(m_nCommodityCount * m_CommodityDlg.m_CommodityArray.GetAt(0).GetPoint());

	AddSellRecord(SellRecord);

	//先清除数据
	m_strCommodityId.Empty();
	m_strCommodityName.Empty();
	m_strCommodityPrice.Empty();
	m_strCommodityType.Empty();
	m_strCommodityRemainQuantity.Empty();

	UpdateData(FALSE);

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();
}

void CJXCRetailOnlineDlg::ShowSellRecordInfo()
{
	UINT nTmp = 0; CString strTmp;

	m_CommodityList.DeleteAllItems();

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		CSellRecord & SellRecord = m_SellRecordArray.GetAt(i);

		m_CommodityList.InsertItem(i, SellRecord.GetCommodityName());

		m_CommodityList.SetItemText(i, 1, SellRecord.GetCommodityBarCode());

		strTmp.Format(_T("%.2f"), SellRecord.GetPrice() / 100.0);
		m_CommodityList.SetItemText(i, 2, strTmp);

		strTmp.Format(_T("%d"), SellRecord.GetQuantity());
		m_CommodityList.SetItemText(i, 3, strTmp);

		strTmp.Format(_T("%.2f"), SellRecord.GetAmount() / 100.0);
		m_CommodityList.SetItemText(i, 4, strTmp);

		m_CommodityList.SetItemData(i, i);

		nTmp += m_SellRecordArray.GetAt(i).GetAmount();
	}

	if (nTmp > 0)
	{
		m_strAllMoney.Format(_T("%.2f"), nTmp / 100.0);	
	}
	else
	{
		m_strAllMoney.Empty();
	}

	UpdateData(FALSE);

	GetDlgItem(IDOK)->EnableWindow(m_SellRecordArray.GetCount() > 0 && m_bOnline == 1);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(m_SellRecordArray.GetCount() > 0);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(m_SellRecordArray.GetCount() > 0);
}

void CJXCRetailOnlineDlg::SaveRetailData(CRetailOnlineMethod &RetailOnline)
{
	CRetailVoucher RetailVoucher;

	RetailVoucher.SetVoucherNo(RetailOnline.GetVoucherNo());
	RetailVoucher.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	RetailVoucher.SetVoucherTime(CIBAHelpper::FormatCenterTime(RetailOnline.GetTranTime()));
	RetailVoucher.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());
	RetailVoucher.SetMemberId(m_nMemberId);
	RetailVoucher.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	RetailVoucher.SetPayType(2); //必须设置为0，为了统计方便
	RetailVoucher.SetUserId(theApp.GetCurCashier()->GetName());
	RetailVoucher.SetAccountYear(COleDateTime::GetCurrentTime().Format(_T("%Y")));
	RetailVoucher.SetAccountMonth(COleDateTime::GetCurrentTime().Format(_T("%m")));
	RetailVoucher.SetRefNo(RetailOnline.GetRefNo());
	RetailVoucher.SetRetailType(1);

	CIBADAL::GetInstance()->AddRetailVoucher(RetailVoucher);

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		CRetailDetail RetailDetail;

		RetailDetail.SetVoucherNo(RetailOnline.GetVoucherNo());
		RetailDetail.SetQuantity(m_SellRecordArray.GetAt(i).GetQuantity());
		RetailDetail.SetPrice(m_SellRecordArray.GetAt(i).GetPrice());
		RetailDetail.SetCommodityCode(m_SellRecordArray.GetAt(i).GetCommodityCode());
		RetailDetail.SetPoint(m_SellRecordArray.GetAt(i).GetPoint());

		CIBADAL::GetInstance()->AddRetailDetail(RetailDetail);
	}
}
//查询商品
BOOL CJXCRetailOnlineDlg::QueryCommodity()
{
	UpdateData();

	if (m_strCommodityId.IsEmpty())
	{
		return FALSE;
	}

	//先清除数据
	m_strCommodityName.Empty();
	m_strCommodityPrice.Empty();
	m_strCommodityType.Empty();
	m_strCommodityRemainQuantity.Empty();

	UpdateData(FALSE);

	CCommodityQueryCondition CommodityQueryCondition;
	CommodityQueryCondition.m_strCommodityId = m_strCommodityId;

	m_CommodityDlg.m_CommodityArray.RemoveAll();
	m_CommodityDlg.QueryCommodity(CommodityQueryCondition);

	if (m_CommodityDlg.m_CommodityArray.GetCount() > 1)
	{
		m_CommodityDlg.DoModal();
	}
	else
	{
		if (m_CommodityDlg.m_CommodityArray.GetCount() > 0)
		{
			m_strCommodityId = m_CommodityDlg.m_CommodityArray.GetAt(0).GetBarCode();
			m_strCommodityName = m_CommodityDlg.m_CommodityArray.GetAt(0).GetCommodityName();
			m_strCommodityPrice.Format(_T("%.2f"), m_CommodityDlg.m_CommodityArray.GetAt(0).GetAdvicePrice() / 100.0);
			m_strCommodityType = m_CommodityDlg.m_CommodityArray.GetAt(0).GetGroupName();
			m_strCommodityRemainQuantity.Format(_T("%d"), m_CommodityDlg.m_CommodityArray.GetAt(0).GetRemainQuantity());

			UpdateData(FALSE);

			GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COUNT)->SetFocus();
		}
		else
		{
			SetToolTipPos(IDC_EDIT_COMMODITYID);
			ShowToolTip(IDS_NOCOMMODITY);
		}
	}

	//选中状态，防止不清除现象
	GetDlgItem(IDC_EDIT_COUNT)->SetFocus();;
	CString txt;
	GetDlgItem(IDC_EDIT_COMMODITYID)->GetWindowText(txt);
	((CEdit *)GetDlgItem(IDC_EDIT_COMMODITYID))->SetSel(0, txt.GetLength());

	return TRUE;
}

BOOL CJXCRetailOnlineDlg::PreTranslateMessage(MSG* pMsg)
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
				if (QueryCommodity())//商品查询
				{
					m_bAllowPretranslate = FALSE;
					SetTimer(1001, 1000, NULL);
					return TRUE;
				}				
			}	
		}
	}

	if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam )
	{
		GetParent()->GetParent()->PostMessage(WM_CLOSE);
	}

	return CNDDialog::PreTranslateMessage(pMsg);
}

void CJXCRetailOnlineDlg::AddSellRecord(NS_SERVLET::CSellRecord& SellRecord)
{
	BOOL bFind = FALSE;

	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		if (0 == m_SellRecordArray.GetAt(i).GetCommodityBarCode().CompareNoCase(SellRecord.GetCommodityBarCode()))
		{
			m_SellRecordArray.GetAt(i).SetQuantity(m_SellRecordArray.GetAt(i).GetQuantity() + SellRecord.GetQuantity());

			m_SellRecordArray.GetAt(i).SetAmount(SellRecord.GetPrice() * m_SellRecordArray.GetAt(i).GetQuantity());

			bFind = TRUE;

			break;
		}
	}

	if (!bFind)
	{
		m_SellRecordArray.Add(SellRecord);
	}

	ShowSellRecordInfo();
}

HBRUSH CJXCRetailOnlineDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_MONEY)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}	
	}

	return hbr;
}

void CJXCRetailOnlineDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		m_ToolTip.HideTooltip();
	}

	CNDDialog::OnShowWindow(bShow, nStatus);
}

void CJXCRetailOnlineDlg::ClearData()
{
	m_CommodityList.DeleteAllItems();
	m_SellRecordArray.RemoveAll();

	m_strName.Empty();
	m_strAllBalance.Empty();
	m_strUserClass.Empty();

	m_nMemberId = 0;
	m_nCardId = 0;
	m_bOnline = 2;
}

void CJXCRetailOnlineDlg::ClearData2()
{
	ClearData();

	m_CommodityList.DeleteAllItems();
	m_SellRecordArray.RemoveAll();

	m_strAllMoney.Empty();

	m_strCommodityName.Empty();
	m_strCommodityPrice.Empty();
	m_strCommodityId.Empty();
	m_strCommodityType.Empty();
	m_strCommodityRemainQuantity.Empty();

	m_nCommodityCount = 1;
	m_edtCount.SetWindowText(_T("1"));

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
}

void CJXCRetailOnlineDlg::OnBnClickedButtonQuery()
{
	CWaitCursor Wait;

	UpdateData();
	
	//输入保护
	if (m_strTermId.IsEmpty() || m_strNetId.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_TERMID);
		ShowToolTip(IDS_INPUTNETID);
		GetDlgItem(IDC_EDIT_TERMID)->SetFocus();
		return;
	}

	//终端优先，如果终端为空，默认采用账号
	//CString strFindNetId;
	//if (!m_strTermId.IsEmpty())
	//{
	//	for(INT_PTR i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	//	{
	//		CComputerInfo & ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(i);
	//		//仅抽取在线用户
	//		if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_ONLINE)
	//		{
	//			if (m_strTermId.CompareNoCase(ComputerInfo.GetTerminalID()) == 0)
	//			{
	//				strFindNetId = ComputerInfo.GetNetId();
	//			}
	//		}
	//	}

	//	//终端找到非空用户
	//	if (!strFindNetId.IsEmpty())
	//	{
	//		m_strNetId = strFindNetId;
	//	}
	//	else
	//	{
	//		SetToolTipPos(IDC_EDIT_TERMID);
	//		ShowToolTip(_T("终端没有用户在线，请重新输入！"));
	//		GetDlgItem(IDC_EDIT_TERMID)->SetFocus();

	//		m_strNetId.Empty();
	//		ClearData();

	//		UpdateData(FALSE);

	//		return;
	//	}
	//}
	//else
	//{
	//	for(INT_PTR i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	//	{
	//		CComputerInfo & ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(i);
	//		//仅抽取在线用户
	//		if (ComputerInfo.HasUser())
	//		{
	//			if (m_strNetId.CompareNoCase(ComputerInfo.GetNetId()) == 0)
	//			{
	//				m_strTermId = ComputerInfo.GetTerminalID();
	//			}
	//		}
	//	}
	//}
	
	//账号不在线
	//if (m_strTermId.IsEmpty())
	//{
	//	SetToolTipPos(IDC_EDIT_TERMID);
	//	ShowToolTip(_T("终端没有用户在线，请重新输入！"));
	//	GetDlgItem(IDC_EDIT_TERMID)->SetFocus();

	//	m_strNetId.Empty();
	//	ClearData();

	//	UpdateData(FALSE);

	//	return;
	//}

	//采用账号
	ClearData();

	CheckCard(m_strNetId);

	UpdateData(FALSE);
}

void CJXCRetailOnlineDlg::OnTimer(UINT nIDEvent)
{
	//延时处理
	if (nIDEvent == 1001)
	{
		m_bAllowPretranslate = TRUE;
	}

	CNDDialog::OnTimer(nIDEvent);
}

void CJXCRetailOnlineDlg::InitUserData(CString strTermId, CString strNetId)
{
	m_strNetId = strNetId;
	m_strTermId = strTermId;

	UpdateData(FALSE);

	OnBnClickedButtonQuery();
}