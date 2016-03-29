// jxc\JXCSellDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include "..\Cashier.h"
#include "..\IBAHelpper.h"
#include "..\CurrentNetBarUser.h"
#include "JXCCommodityDlg.h"
#include ".\JXCSellDlg.h"
#include "..\Socket\LocalServer.h"
#include "..\IBAPrint.h"

// CJXCSellDlg 对话框

IMPLEMENT_DYNAMIC(CJXCSellDlg, CIBADialog)
CJXCSellDlg::CJXCSellDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CJXCSellDlg::IDD, pParent)
, m_nCommodityCount(1), m_nMemberId(0), m_nCardId(0)
, m_CommodityDlg(this, 0)
, m_bAllowPretranslate(TRUE)
{
	m_strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
}

CJXCSellDlg::~CJXCSellDlg()
{ 
}

void CJXCSellDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_COMMODITYID, m_strCommodityId);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strCommodityName);
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strCommodityType);
	DDX_Text(pDX, IDC_STATIC_PRICE, m_strCommodityPrice);
	DDX_Text(pDX, IDC_STATIC_STORE, m_strCommodityRemainQuantity);

	//DDX_Text(pDX, IDC_EDIT_COUNT, m_nCommodityCount);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_edtCount);

	DDX_Text(pDX, IDC_EDIT_NETID, m_strNetId);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_STATIC_MONEY, m_strAllMoney);

	DDX_Control(pDX, IDC_COMBO_PAYTYPE, m_cboPayType);
	DDX_Control(pDX, IDC_LIST_COMMODITY, m_CommodityList);
}


BEGIN_MESSAGE_MAP(CJXCSellDlg, CIBADialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_COMMODITY, OnBnClickedButtonCommodity)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_CBN_SELCHANGE(IDC_COMBO_PAYTYPE, OnCbnSelchangeComboPaytype)
	ON_EN_KILLFOCUS(IDC_EDIT_PASSWORD, OnEnKillfocusEditPassword)
	//ON_EN_KILLFOCUS(IDC_EDIT_COUNT, OnEnKillfocusEditCount)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CJXCSellDlg 消息处理程序

BOOL CJXCSellDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitListView();

#ifndef __USING_INTER_RESOURCE
	CIBAString strCob;
	strCob.LoadString(IDS_PAYTYPE_COMBO);
	CStringArray strArr;
	CIBAHelpper::SplitLine(strCob,strArr);
	if (strArr.GetCount() != 2)
	{
		strArr.RemoveAll();
		strArr.Add(_T("现金购物"));
		strArr.Add(_T("账户购物"));
	}
	m_cboPayType.ResetContent();
	m_cboPayType.AddString(strArr.GetAt(0));
	m_cboPayType.AddString(strArr.GetAt(1));
#endif
	

	SetEditFont();
	m_cboPayType.SetCurSel(0);
	m_cboPayType.SetFont(&m_Font);
	
	CIBAHelpper::CreateFont(m_BigFont, 23, 700);
	GetDlgItem(IDC_STATIC_MONEY)->SetFont(&m_BigFont);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_NETID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);

	((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->LimitText(8);

	m_edtCount.DisableMask();
	m_edtCount.SetValidChars(_T("1234567890"));
	m_edtCount.SetLimitText(3);
	m_edtCount.SetWindowText(_T("1"));

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCSellDlg::OnEnKillfocusEditPassword()
{
	RedrawWindow(); //这行很重要
}

void CJXCSellDlg::OnEnKillfocusEditCount()
{
	RedrawWindow(); //这行很重要
}

void CJXCSellDlg::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_JXC_SELL_HEAD);
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

void CJXCSellDlg::AddSellRecord(NS_SERVLET::CSellRecord& SellRecord)
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

void CJXCSellDlg::OnBnClickedButtonDel()
{
	INT nIdx = m_CommodityList.GetSelectionMark();
	
	if (nIdx < 0)
	{
		return;
	}

	m_SellRecordArray.RemoveAt(nIdx);

	ShowSellRecordInfo();
}

void CJXCSellDlg::OnBnClickedButtonClear()
{
	if (IDOK == MsgBox(L"取消本次销售吗？", MB_OKCANCEL | MB_ICONQUESTION))
	{
		ClearData();
		UpdateData(FALSE);
	}
}

void CJXCSellDlg::OnBnClickedButtonCommodity()
{
	m_CommodityDlg.m_CommodityArray.RemoveAll();
	m_CommodityDlg.DoModal();
	m_CommodityList.SetFocus();
}

void CJXCSellDlg::OnBnClickedButtonAdd()
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

void CJXCSellDlg::OnBnClickedOk()
{
	if (m_SellRecordArray.GetCount() <= 0)
	{
		SetToolTipPos(IDOK);
		ShowToolTip(L"请先选择用户要购买的商品！");
		return;
	}

	CWaitCursor Wait;

	CRetailMethod Retail;

	if (m_cboPayType.GetCurSel() == 1)
	{
		if (!GetCardIdFromCenter())
		{
			return;
		}

		Retail.SetPayType(2);
		Retail.SetCardId(m_nCardId);
		Retail.SetMemberId(m_nMemberId);
		Retail.SetPassword(m_strPassword);
	}
	else
	{
		Retail.SetPayType(1);
	}

	Retail.SetRetailType(1);
	Retail.SetSellRecords(m_SellRecordArray);
	theApp.GetCurCashier()->DoRetail(Retail);

	SetToolTipPos(IDOK);

	if (Retail.GetStatusCode() == 0)
	{
		// 2011/09/02-8201-gxx:保存最后交易记录 
		//
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(Retail.GetRefNo());
		TranInfo.SetLastTranTime(Retail.GetTranTime());
		TranInfo.SetLastTranType(7);
		TranInfo.Save();

		if (m_cboPayType.GetCurSel() == 1) //账号购物
		{
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
				m_nMemberId, 0, 0, _tstof(m_strAllMoney) * 100);
		}

		SaveRetailData(Retail);

		ShowToolTip(L"购物成功！");

		if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ISPRINT))
		{
			TCHAR szBuf[MAX_PATH] = {0};
			GetDlgItemText(IDC_EDIT_NETID, szBuf, MAX_PATH);

			CString strAccount(szBuf);
			PrintTicket(strAccount);
			PrintTicket(strAccount);
		}

		ClearData();
		UpdateData(FALSE);
	}
	else
	{
		CString strTmp;
		strTmp.Format(L"购物失败！原因：%s - %s", Retail.GetStatusMessage(), Retail.GetExtInfo());

		ShowToolTip(strTmp, TRUE);
	}

	GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();
}

BOOL CJXCSellDlg::GetCardIdFromCenter()
{
	if (!CheckInput())
	{
		return FALSE;
	}

	CString strTmp = m_strNetId;

	BOOL bRet = FALSE;

	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strTmp);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	if (checkCard.GetStatusCode() == 0)
	{
		if (checkCard.GetMemberId() != 0) //已经开过的卡,充值退卡
		{
			m_nCardId = checkCard.GetCardId();
			m_nMemberId = checkCard.GetMemberId();

			bRet = TRUE;
		}
		else //新卡注册
		{
			SetToolTipPos(IDC_EDIT_NETID);
			ShowToolTip(IDS_HASNOTUSED);
		}
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(checkCard.GetStatusMessage(), TRUE);
	}

	return bRet;
}

void CJXCSellDlg::OnCbnSelchangeComboPaytype()
{
	BOOL bRet = FALSE;

	if (m_cboPayType.GetCurSel() == 1)
	{
		GetDlgItem(IDC_EDIT_NETID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_NETID)->SetFocus();
	}
	else
	{
		GetDlgItem(IDC_EDIT_NETID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);

		m_strNetId.Empty();
		m_strPassword.Empty();

		UpdateData(FALSE);
	}
}

void CJXCSellDlg::ShowSellRecordInfo()
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

	GetDlgItem(IDOK)->EnableWindow(m_SellRecordArray.GetCount() > 0);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(m_SellRecordArray.GetCount() > 0);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(m_SellRecordArray.GetCount() > 0);
}

HBRUSH CJXCSellDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CJXCSellDlg::ClearData()
{
	m_CommodityList.DeleteAllItems();
	m_SellRecordArray.RemoveAll();

	m_strPassword.Empty();
	m_strAllMoney.Empty();
	m_strNetId.Empty();
	
	m_strCommodityName.Empty();
	m_strCommodityPrice.Empty();
	m_strCommodityId.Empty();
	m_strCommodityType.Empty();
	m_strCommodityRemainQuantity.Empty();
	
	m_nMemberId = 0;
	m_nCardId = 0;
	m_nCommodityCount = 1;
	
	m_edtCount.SetWindowText(_T("1"));

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
}

BOOL CJXCSellDlg::CheckInput()
{
	UpdateData();

	BOOL bRet = TRUE;

	if (m_strNetId.IsEmpty())
	{
		((CEdit*)GetDlgItem(IDC_EDIT_NETID))->SetFocus();
		SetToolTipPos(IDC_EDIT_NETID);
		ShowToolTip(IDS_INPUTNETID);
		
		return FALSE;
	}

	if (m_strPassword.IsEmpty())
	{
		((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetFocus();
		SetToolTipPos(IDC_EDIT_PASSWORD);
		ShowToolTip(IDS_INPUTERROR);
		return FALSE;
	}

	return TRUE;
}

void CJXCSellDlg::SaveRetailData(CRetailMethod &Retail)
{
	CRetailVoucher RetailVoucher;

	RetailVoucher.SetVoucherNo(Retail.GetVoucherNo());
	RetailVoucher.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	RetailVoucher.SetVoucherTime(CIBAHelpper::FormatCenterTime(Retail.GetTranTime()));
	RetailVoucher.SetStoreId(CIBAConfig::GetInstance()->GetStoreID());
	RetailVoucher.SetMemberId(m_nMemberId);
	RetailVoucher.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	RetailVoucher.SetPayType(m_cboPayType.GetCurSel() + 1);
	RetailVoucher.SetUserId(theApp.GetCurCashier()->GetName());
	RetailVoucher.SetAccountYear(COleDateTime::GetCurrentTime().Format(_T("%Y")));
	RetailVoucher.SetAccountMonth(COleDateTime::GetCurrentTime().Format(_T("%m")));
	RetailVoucher.SetRefNo(Retail.GetRefNo());
	RetailVoucher.SetRetailType(1);

	CIBADAL::GetInstance()->AddRetailVoucher(RetailVoucher);


	for (INT i = 0; i < m_SellRecordArray.GetCount(); i++)
	{
		CRetailDetail RetailDetail;
	
		RetailDetail.SetVoucherNo(Retail.GetVoucherNo());
		RetailDetail.SetAmount(m_SellRecordArray.GetAt(i).GetAmount());
		RetailDetail.SetPrice(m_SellRecordArray.GetAt(i).GetPrice());
		RetailDetail.SetQuantity(m_SellRecordArray.GetAt(i).GetQuantity());
		RetailDetail.SetCommodityCode(m_SellRecordArray.GetAt(i).GetCommodityCode());
		RetailDetail.SetPoint(m_SellRecordArray.GetAt(i).GetPoint());

		CIBADAL::GetInstance()->AddRetailDetail(RetailDetail);
	}
}

BOOL CJXCSellDlg::QueryCommodity()
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
	GetDlgItem(IDC_EDIT_COUNT)->SetFocus();
	CString txt;
	GetDlgItem(IDC_EDIT_COMMODITYID)->GetWindowText(txt);
	((CEdit*)GetDlgItem(IDC_EDIT_COMMODITYID))->SetSel(0, txt.GetLength());

	return TRUE;
}

BOOL CJXCSellDlg::PreTranslateMessage(MSG* pMsg)
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
					SetTimer(1001, 500, NULL);
					return TRUE;
				}				
			}
			else if (pWnd->GetDlgCtrlID() == IDC_EDIT_COUNT)
			{
				if (GetDlgItem(IDC_BUTTON_ADD)->IsWindowEnabled())
				{
					return PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD, BN_CLICKED)); //模拟按钮单击事件
				}
				else
				{
					return TRUE;
				}				
			}	
		}
	}

	if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam )
	{
		GetParent()->GetParent()->PostMessage(WM_CLOSE);
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}


void CJXCSellDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		m_ToolTip.HideTooltip();
	}

	CIBADialog::OnShowWindow(bShow, nStatus);
}
void CJXCSellDlg::OnTimer(UINT nIDEvent)
{
	//延时处理
	if (nIDEvent == 1001)
	{
		m_bAllowPretranslate = TRUE;
	}

	CIBADialog::OnTimer(nIDEvent);
}

void CJXCSellDlg::PrintTicket(CString strAccount)
{	
	CString strTicket, strPrint;

	BOOL bIsCredit = TRUE;
	if (!strAccount.IsEmpty())
		bIsCredit = FALSE;
	CIBAPrint::GetShoppingTicket(strTicket, bIsCredit);

	// 账号
	CString strTmp(_T(""));
	strTmp.Format(_T("%d"), CNetBarConfig::GetInstance()->GetNetBarId());
	strTicket.Replace(_T("[cardserialnumber]"), strTmp);

	// 用户账号
	if (!bIsCredit)
		strTicket.Replace(_T("[usernumber]"), strAccount);

	CString strBegin(_T("[tagBegin]")), strEnd(_T("[tagEnd]"));
	int nBegin = strTicket.Find(strBegin);
	int nBeginLen = strBegin.GetLength();
	int nEnd = strTicket.Find(strEnd);
	int nEndLength = strEnd.GetLength();

	// 主模板内容
	CString strTemplate = strTicket.Mid(nBegin + nBeginLen, nEnd - nBegin - nBeginLen);
	strPrint = strTicket.Left(nBegin);

	// 定义各数值
	float fPrice = 0.0f;										// 单价
	float fTotalMoney = 0.0f;									// 总消费金额
	float fMoney = 0.0f;										// 某种商品总消费金额
	int nCount = 0;												// 数量

	// 循环添加商品
	for(int i = 0; i < m_CommodityList.GetItemCount(); i++)
	{
		CString strTempTmp = strTemplate;
		strTmp = m_CommodityList.GetItemText(i, 0);
		strTempTmp.Replace(_T("[productname]"), strTmp);		// 商品名称

		strTmp = m_CommodityList.GetItemText(i, 1);
		strTempTmp.Replace(_T("[productencode]"), strTmp);		// 商品编码

		strTmp = m_CommodityList.GetItemText(i, 2);
		fPrice = atof(CT2A(strTmp.GetBuffer()));
		strTmp.ReleaseBuffer();
		strTempTmp.Replace(_T("[productprice]"), strTmp);		// 商品单价

		strTmp = m_CommodityList.GetItemText(i, 3);	
		nCount = atoi(CT2A(strTmp.GetBuffer()));
		strTmp.ReleaseBuffer();
		strTempTmp.Replace(_T("[count]"), strTmp);				// 数量

		fMoney = fPrice * (float)nCount;
		strTmp.Format(_T("%.2f"), fMoney);
		strTempTmp.Replace(_T("[productmoney]"), strTmp);		// 金额 = 商品单价 x 数量

		strPrint += strTempTmp;

		fTotalMoney += fMoney;
	}

	strTmp.Format(_T("%.2f"), fTotalMoney);
	strPrint += strTicket.Right(strTicket.GetLength() - nEnd - nEndLength);
	strPrint.Replace(_T("[allconsume]"), strTmp);				// 总金额

	// 打印
	CIBAPrint::Print(strPrint);
}