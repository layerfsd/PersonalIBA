// OffDutyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "IBAPrint.h"
#include "IBAShiftReport.h"
#include "Servlet\LastTranInfo.h"
#include ".\offdutydlg.h"
#include "storedlg.h"
#include ".\Socket\LocalServer.h"
#include "DdnPcpInc.h"

using namespace NS_DAL;

// COffDutyDlg 对话框

IMPLEMENT_DYNAMIC(CDlgIBADuty, CIBADialog)
CDlgIBADuty::CDlgIBADuty(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgIBADuty::IDD, pParent)
, m_nAllMoney(0), m_nNextReserve(0)
{
	m_nDutyIncome = 0;
	m_nLocalCreditMoney = 0;
}

CDlgIBADuty::~CDlgIBADuty()
{
}

void CDlgIBADuty::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_RESERVE, m_edtReverveAmount);//备用金数目（动静态）
	DDX_Control(pDX, IDC_COMBO_SHIFT, m_cboShift);//收银员转换

	DDX_Control(pDX, IDC_EDIT_PAYOUT, m_edtPayout);//支出
	DDX_Text(pDX, IDC_EDIT_MEMO, m_strDescription);//描述
	DDX_Control(pDX, IDC_EDIT_NEXTREVERSE, m_edtNextReverse);
	DDX_Text(pDX, IDC_EDIT_NEXTREVERSE, m_strNextReverse);
}

BEGIN_MESSAGE_MAP(CDlgIBADuty, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_STAT, OnBnClickedButtonStat)
	ON_EN_CHANGE(IDC_EDIT_PAYOUT, OnEnChangeEditPayout)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnBnClickedButtonPrint)
	ON_BN_CLICKED(IDC_BUTTON_JXCPRINT, OnBnClickedButtonJxcprint)
	ON_EN_CHANGE(IDC_EDIT_NEXTREVERSE, OnEnChangeEditNextreverse)
	ON_BN_CLICKED(IDC_BUTTON_STORECHECK, OnBnClickedButtonStorecheck)
END_MESSAGE_MAP()


// COffDutyDlg 消息处理程序

void CDlgIBADuty::OnBnClickedOk()
{

	//UNIT_AUTO_TRACE("上下班控制");
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	//CWaitCursor Wait;

	UpdateData();

	//在线与否
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		if (IDOK != MsgBoxFormat(MB_OKCANCEL | MB_ICONQUESTION, IDS_OFFDUTYQUESTION, 
				theApp.GetCurCashier()->GetName()))
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			return;
		}

		CArray<CLocalConsumeInfo,CLocalConsumeInfo&> ConsumeInfoArray;
		CIBADAL::GetInstance()->GetLocalConsumes(ConsumeInfoArray);
		if (ConsumeInfoArray.GetCount() > 0)
		{
			theApp.IBAMsgBox(IDS_LOCALOFFDUTYERROR);  // 本地消费信息没有完全上传到中心，不允许下班
			CBCGPDialog::OnCancel();
			return;
		}

		//下班
		DoOffDuty();
	}
	else
	{//上班
		DoOnDuty();		
	}
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}
//统计数据,显示所有数据
void CDlgIBADuty::OnBnClickedButtonStat()
{
	CWaitCursor Wait;

	m_nAllMoney = 0;
	m_nNextReserve = 0;

	SetToolTipPos(IDC_BUTTON_STAT);

	if (theApp.GetCurCashier()->IsOnDuty()) //上班
	{
		m_strOnDutyTime = theApp.GetCurCashier()->GetOnDutyTime();
		
		CLastTranInfo LastTranInfo;//最后交易方法
		LastTranInfo.Load();//载入注册表信息

		m_ShiftTotalMethod.SetLastRefNo(LastTranInfo.GetLastRefNo());//最后一次交易的操作号
		m_ShiftTotalMethod.SetLastTranTime(LastTranInfo.GetLastTranTime());//最后一次交易的时间
		m_ShiftTotalMethod.SetLastTranType(LastTranInfo.GetLastTranType());//最后一次交易的类型

		theApp.GetCurCashier()->DoShiftTotal(m_ShiftTotalMethod);//

		if (m_ShiftTotalMethod.GetStatusCode() == 0)
		{			
			ShowShiftTotalInfo();//取得，显示换班数据

			FillGird();//填充库存数据

			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_MEMO)->SetFocus();
			GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(CIBAConfig::GetInstance()->GetShowShiftTotalInfo());
		}
		else
		{
			ShowToolTip(m_ShiftTotalMethod.GetStatusMessage(), TRUE);
		}
	}
	else//下班
	{//盘点库存
		GetDlgItem(IDOK)->EnableWindow(TRUE);

		//清空上一班次入库记录
		//CIBADAL::GetInstance()->ZeroAllCommodityStoreInQuantity();

		CStoreQuantityMethod StoreQuantity;//库存盘点统计方法

		theApp.GetCurCashier()->DoStoreQuantity(StoreQuantity);

		if (StoreQuantity.GetStatusCode() == 0)
		{	// 2011/04/27-gxx: 注释掉此行代码://清空所有交易保留的数量
			//CIBADAL::GetInstance()->ZeroAllCommodityRemainQuantity();

			for (UINT i = 0; i < StoreQuantity.GetRecordCount(); i++)
			{//更新库存记录（商品编号+ 库存数量）
				CIBADAL::GetInstance()->UpdateCommodityRemainQuantity(StoreQuantity.GetCommodityCode(i), StoreQuantity.GetRemainQuantity(i));	
			}
			//填充列表（从数据库里取得数据）
			FillGird();

			m_edtReverveAmount.SetFocus();
		}
		else
		{
			ShowToolTip(StoreQuantity.GetStatusMessage(), TRUE);
		}
	}
}

BOOL CDlgIBADuty::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitContrls();

	InitGridTree();

	OnBnClickedButtonStat();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgIBADuty::InitContrls()
{	
	SetEditFont();
	SetStaticFont();
	//清空
	GetDlgItem(IDC_STATIC_DEPOSIT)->SetWindowText(_T(""));

	m_cboShift.AddString(CIBAConfig::GetInstance()->GetShiftName1());
	m_cboShift.AddString(CIBAConfig::GetInstance()->GetShiftName2());
	m_cboShift.AddString(CIBAConfig::GetInstance()->GetShiftName3());

	m_cboShift.SetCurSel(GetShiftType() - 1);
	m_cboShift.SetFont(&m_Font);

	GetDlgItem(IDC_STATIC_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_HANDLEIN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC2_HANDLEIN)->SetFont(&m_FontPlus1Bold);
	GetDlgItem(IDC_STATIC_SELFDUTY)->SetFont(&m_FontPlus1Bold);
	GetDlgItem(IDC_STATIC_HELP)->SetFont(&m_FontSmall);

	m_edtReverveAmount.DisableMask();
	m_edtReverveAmount.SetValidChars(_T("0123456789."));
	m_edtReverveAmount.SetLimitText(7);

	m_edtNextReverse.SetLimitText(7);
	m_edtPayout.SetLimitText(7);

	GetDlgItem(IDC_BUTTON_JXCPRINT)->EnableWindow(FALSE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	// 2011/05/19-gxx: 是否可编辑"留给下一班金额"
	GetDlgItem(IDC_EDIT_NEXTREVERSE)->EnableWindow( CIBAConfig::GetInstance()->GetAllowNextDutyMoney() );


	//隐藏某些项
	GetDlgItem(IDC_STATIC_HANDLEIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2_HANDLEIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_NEXTREVERSE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2_NEXTRESERVE)->ShowWindow(SW_HIDE);

	if (!CNetBarConfig::GetInstance()->IsUseDeposit())
	{
		GetDlgItem(IDC_STATIC_DEPOSIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC2_DEPOSIT)->ShowWindow(SW_HIDE);

		// 2011/06/22-gxx: 公式中没有押金
		CIBAString str;
		str.LoadString(IDS_DUTY_REMARK);
		GetDlgItem(IDC_STATIC_HELP)->SetWindowText(str);
	}
	else
	{
		// 2011/06/22-gxx: 公式中有押金
		CIBAString str;
		str.LoadString(IDS_DUTY_REMARK2);
		GetDlgItem(IDC_STATIC_HELP)->SetWindowText(str);
	}

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		if (CIBAConfig::GetInstance()->GetDynamicReserve())
		{
			GetDlgItem(IDC_STATIC_HANDLEIN)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_STATIC2_HANDLEIN)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_EDIT_NEXTREVERSE)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_STATIC2_NEXTRESERVE)->ShowWindow(SW_NORMAL);
		} 
	
		m_cboShift.EnableWindow(FALSE);
		m_edtReverveAmount.EnableWindow(FALSE);

		//允许编辑”其他支出“的判断
		GetDlgItem(IDC_EDIT_PAYOUT)->EnableWindow(CIBAConfig::GetInstance()->GetEditPayOut());

		GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(FALSE);
	}
	else
	{
		m_edtPayout.EnableWindow(FALSE);
		m_edtNextReverse.EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_MEMO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PRINT)->ShowWindow(SW_HIDE);

		//显示上一次数据库数值
		if (CIBAConfig::GetInstance()->GetDynamicReserve())
 		{	
			m_nNextReserve = CIBAConfig::GetInstance()->GetDynamicReserveMoney();

			//{ 2011/09/05-8201-gxx: 
			INT nNextReserve = 0;
			if (CIBADAL::GetInstance()->GetShiftCashRecord(nNextReserve))
			{
				if (m_nNextReserve != nNextReserve)
				{
					CString strOut;
					//上一班预留金出现异常,参考值为:%.2f , %.2f\r\n选择"是"表示选择第一个值为上一班预留\r\n选择"否"表示选择第二值为上一班预留
					//strOut.Format(LOAD_STRING(IDS_DUTY_NEXTRESERVE_ERROR),m_nNextReserve/100.0);
					strOut.LoadString(IDS_DUTY_NEXTRESERVE_ERROR);
					GetDlgItem(IDC_STATIC_BYJ_HIT)->SetWindowText(strOut);
					GetDlgItem(IDC_STATIC_BYJ_HIT)->SetFont(&m_FontSmall);
					GetDlgItem(IDC_STATIC_BYJ_HIT)->ShowWindow(TRUE);
				}
			}
			
			//}
			
			
			////如果读取文件失败，则读数据库
			//if (m_nNextReserve == 0)
			//{
			//	CIBADAL::GetInstance()->GetShiftCashRecord(m_nNextReserve);//读取数据库
			//}

 			CString strTmp;
 			strTmp.Format(_T("%.2f"), m_nNextReserve / 100.0);
 			GetDlgItem(IDC_EDIT_RESERVE)->SetWindowText(strTmp);
 			m_edtReverveAmount.EnableWindow(FALSE);

			GetDlgItem(IDC_STATIC_HANDLEIN)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_STATIC2_HANDLEIN)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_EDIT_NEXTREVERSE)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_STATIC2_NEXTRESERVE)->ShowWindow(SW_NORMAL);
 		} 
 		else
 		{
 			GetDlgItem(IDC_EDIT_RESERVE)->SetWindowText(_T("0.0"));
 			m_edtReverveAmount.EnableWindow(TRUE);
 			m_edtReverveAmount.SetFocus();
 		}

		CIBAString strTmp;
		strTmp.LoadString(IDS_DOONDUTY);
		GetDlgItem(IDOK)->SetWindowText(strTmp);
	}

	//不显示交班信息
	if (!CIBAConfig::GetInstance()->GetShowShiftTotalInfo())
	{
		HideShiftTotalInfo();
	}
}

//设置统计的字体
void CDlgIBADuty::SetStaticFont()
{
	CWnd* pChildWnd = GetWindow(GW_CHILD);   

	while (pChildWnd != NULL)   
	{
		TCHAR szClassName[MAX_PATH] = {0};
		GetClassName(pChildWnd->GetSafeHwnd(), szClassName, MAX_PATH);

		if (lstrcmp(szClassName, _T("Static")) == 0)
		{
			switch(pChildWnd->GetDlgCtrlID())
			{
			case IDC_STATIC_CREDIT:
			case IDC_STATIC_RETURNED:
			case IDC_STATIC_CASHPAY:
			case IDC_STATIC_DIANKA:
			case IDC_STATIC_STOREIN:
			case IDC_STATIC_DEPOSIT:
			case IDC_STATIC_NETCASH:
			case IDC_STATIC_RESERVE:
			case IDC_STATIC_SELFDUTY:
			case IDC_STATIC_CREDITCARD:
			case IDC_STATIC_NETBARPAY:
			case IDC_STATIC_ZSB_CONSUME:
			case IDC_STATIC_ZSB_BALANCE:
				GetDlgItem(pChildWnd->GetDlgCtrlID())->SetFont(&m_Font);
				break;

			}
			if (pChildWnd->GetDlgCtrlID() != IDC_STATIC && 
				pChildWnd->GetDlgCtrlID() != IDC_STATIC2_HANDLEIN && 
				pChildWnd->GetDlgCtrlID() != IDC_STATIC2_NEXTRESERVE)
			{
				//GetDlgItem(pChildWnd->GetDlgCtrlID())->SetFont(&m_Font);
			}
		}
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);  
	}
}
//
void CDlgIBADuty::OnEnChangeEditPayout()
{
	UpdateData();
	INT nPayout = 0, nRemainAmount = 0, nFetchAmount = 0;

	GetEachMoney(nPayout, nRemainAmount, nFetchAmount);

	CString strTmp;

	INT nAllMoney = m_nAllMoney;//从上次读取中得到数据，保存成全局变量
	nAllMoney -= nPayout;

	strTmp.Format(_T("%.2f"), nAllMoney / 100.0);
	GetDlgItem(IDC_STATIC_ALL)->SetWindowText(strTmp);

	////////////////////////动态更新上缴金额.本班保留金////////////////////////
	//下班保留金
	m_nNextReserve = nRemainAmount;//下一班保留金额

	//上交金额
	nAllMoney -= nRemainAmount;//上交金额
	strTmp.Empty();
	strTmp.Format(_T("%.2f"), nAllMoney / 100.0);
	GetDlgItem(IDC_STATIC_HANDLEIN)->SetWindowText(strTmp);

}

void CDlgIBADuty::OnEnChangeEditNextreserve()
{
	OnEnChangeEditPayout();
}

void CDlgIBADuty::OnEnChangeEditAll()
{
	INT nPayout = 0, nRemainAmount = 0, nFetchAmount = 0;

	GetEachMoney(nPayout, nRemainAmount, nFetchAmount);

	CString strTmp;

	INT nAllMoney = m_nAllMoney;

	nAllMoney -= nPayout;
	nAllMoney -= nFetchAmount;
}

void CDlgIBADuty::GetEachMoney(INT& nPayout, INT& nRemainAmount, INT& nFetchAmount)
{
	//其他支出
	m_edtPayout.GetWindowText(m_strPayout);
	double dblPayout = _tstof(m_strPayout);
	nPayout = Round(dblPayout * 100);
	
	//预留金额
	m_edtNextReverse.GetWindowText(m_strNextReverse);
	double dblRemainAmount = _tstof(m_strNextReverse);
	nRemainAmount = Round(dblRemainAmount * 100);
	
	//上缴
	GetDlgItem(IDC_STATIC_HANDLEIN)->GetWindowText(m_strFetchAmount);
	double dblFetchAmount = _tstof(m_strFetchAmount);
	nFetchAmount = Round(dblFetchAmount * 100);

	//更新班次收入
	//GetDlgCtrlID(IDC_STATIC_NETBARPAY)->GetWindowText(strValue);
	int nDutyIncome = m_nDutyIncome - nPayout;
	CString str;
	str.Format(_T("%.2f"),nDutyIncome/100.0);
	GetDlgItem(IDC_STATIC_NETCASH)->SetWindowText(str);

}

//打印小票
void CDlgIBADuty::PrintTicket(CShiftCash &ShiftCash)
{
	CString strTicket;
	CIBAPrint::GetShiftCashTicket(strTicket);

	CString strTmp;

	strTicket.Replace(_T("[ondutytime]"), ShiftCash.GetStartDate());
	strTicket.Replace(_T("[offdutytime]"), ShiftCash.GetEndDate());

	strTmp.Format(_T("%.2f"), ShiftCash.GetRemainAmount() / 100.0);
	strTicket.Replace(_T("[remain]"), strTmp);

	strTmp.Format(_T("-%.2f"), ShiftCash.GetReturnAmount() / 100.0);
	strTicket.Replace(_T("[return]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetCreditAmount() / 100.0);
	strTicket.Replace(_T("[credit]"), strTmp);

	// 2011/12/20-8237-gxx: 
	//strTmp.Format(_T("%.2f"), (ShiftCash.GetRemainAmount() + ShiftCash.GetCreditAmount() - ShiftCash.GetReturnAmount()) / 100.0);
	strTmp.Format(_T("%.2f"), ShiftCash.GetDutyIncome() / 100.0);
	strTicket.Replace(_T("[netcash]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetCashSoldAmount() / 100.0);
	strTicket.Replace(_T("[cashpay]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetPayout() / 100.0);
	strTicket.Replace(_T("[payout]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetTotalAmount() / 100.0);
	strTicket.Replace(_T("[allcash]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetNextRemainAmount() / 100.0);
	strTicket.Replace(_T("[nextremain]"), strTmp);

	strTmp.Format(_T("%.2f"), ShiftCash.GetFetchAmount() / 100.0);
	strTicket.Replace(_T("[fetchmoney]"), strTmp);

	strTicket.Replace(_T("[description]"), ShiftCash.GetDescription());


	// 2011/08/26-8201-gxx: 添加点卡
	strTmp.Format(_T("%.2f"),ShiftCash.GetDianka() / 100.0);
	strTicket.Replace(_T("[paycard]"), strTmp);

	strTmp.Format(_T("%.2f"),ShiftCash.GetStoreInAmount() / 100.0);
	strTicket.Replace(_T("[storein]"), strTmp);

	strTmp.Format(_T("%.2f"),ShiftCash.GetAccountPay() / 100.0);
	strTicket.Replace(_T("[accountpay]"), strTmp);

	strTmp.Format(_T("%.2f"),ShiftCash.GetCardCreditedAmount() / 100.0);
	strTicket.Replace(_T("[creditcard]"), strTmp);

	strTmp.Format(_T("%.2f"),ShiftCash.GetZSBConsume() / 100.0);
	strTicket.Replace(_T("[ddnpay]"), strTmp);

	strTmp.Format(_T("%.2f"),ShiftCash.GetZSBBalance() / 100.0);
	strTicket.Replace(_T("[ddnbalance]"), strTmp);





	CIBAPrint::Print(strTicket);
}

HBRUSH CDlgIBADuty::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		UINT nID = pWnd->GetDlgCtrlID();
		switch(nID)
		{
		
		case IDC_STATIC_CREDIT:
		case IDC_STATIC_RETURNED:
		case IDC_STATIC_CASHPAY:
		case IDC_STATIC_DIANKA:
		case IDC_STATIC_STOREIN:
		case IDC_STATIC_DEPOSIT:
		case IDC_STATIC_NETCASH:
		case IDC_STATIC_RESERVE:
		case IDC_STATIC_ALL:
		case IDC_STATIC_SELFDUTY:
		case IDC_STATIC2_HANDLEIN:
		case IDC_STATIC_HANDLEIN:
			pDC->SetTextColor(RGB(0,0,255));
			break;
		
		case IDC_STATIC_HELP:
			pDC->SetTextColor(RGB(120,120,120));
			break;

		case IDC_STATIC_BYJ_HIT:
			pDC->SetTextColor(RGB(220,0,0));
			break;
		

		}
	}
	else if (CTLCOLOR_EDIT == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_PAYOUT || 
			pWnd->GetDlgCtrlID() == IDC_EDIT_NEXTREVERSE)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}

	return hbr;
}
//打印交班表
void CDlgIBADuty::OnBnClickedButtonPrint()
{
	UpdateData();

	INT nPayout = 0, nRemainAmount = 0, nFetchAmount = 0;
	GetEachMoney(nPayout, nRemainAmount, nFetchAmount);//取支出，为下班的保留金，上缴金

	if (CIBAConfig::GetInstance()->GetShiftReportType() == 0)
	{
		CIBAShiftReport sr(FALSE);

		if (sr.Create())
		{
			//sr.SetNetBarName(CNetBarConfig::GetInstance()->GetNetBarName());
			//sr.SetOperator(theApp.GetCurCashier()->GetName());
			//sr.SetOnDutyTime(m_strOnDutyTime);
			//sr.SetCardCreditAmount(m_ShiftTotalMethod.GetCreditCard());
			//sr.SetCashSoldAmount(m_ShiftTotalMethod.GetCashPay());
			//sr.SetCostExpense(m_ShiftTotalMethod.GetCostExpense());
			//sr.SetCreditAmount(m_ShiftTotalMethod.GetCreditAmount());
			//sr.SetDeposit(m_ShiftTotalMethod.GetDeposit());
			//sr.SetDescription(m_strDescription);
			//sr.SetPayout(nPayout);
			//sr.SetRemainAmount(m_ShiftTotalMethod.GetReserveAmount());
			//sr.SetReturnAmount(m_ShiftTotalMethod.GetReturnedAmount());
			//sr.SetTotalAmount(nFetchAmount + nRemainAmount);
			//sr.SetNextRemain(m_nNextReserve);//下一班备用金
			//sr.SetFetchAmount(nFetchAmount);

			//strXML = "<ShiftInfo NetbarName=\"NetbarName\" Cashier=\"Cashier\" OnDutyTime=\"OnDutyTime\" 
			//OffDutyTime=\"OffDutyTime\" CreditMoney=\"123\" ReturnMoney=\"234\" CashSold=\"332\" 
			// PayCard=\"555\" StoreIn=\"3323\" Payout=\"233\" UseDeposit=\"1\" Deposit=\"455\" 
			// ShiftIn=\"344\" LastRemain=\"434\" AllCash=\"43434\" NextRemain=\"4343\" FetchMoney=\"434\" 
			// CreditCard=\"434\" CardSold=\"434\" Dodonew=\"43434\" Description=\"Description\" />";

			CString strReport(_T("<ShiftInfo "));
			strReport.AppendFormat(_T("NetbarName=\"%s\" "), CNetBarConfig::GetInstance()->GetNetBarName());
			strReport.AppendFormat(_T("Cashier=\"%s\" "), theApp.GetCurCashier()->GetName());
			strReport.AppendFormat(_T("OnDutyTime=\"%s\" "), m_strOnDutyTime);
			strReport.AppendFormat(_T("OffDutyTime=\"%s\" "), CIBAHelpper::GetCurrentTimeAsString(TRUE));
			strReport.AppendFormat(_T("CreditMoney=\"%d\" "), m_ShiftTotalMethod.GetCreditAmount());
			strReport.AppendFormat(_T("ReturnMoney=\"%d\" "), m_ShiftTotalMethod.GetReturnedAmount());
			strReport.AppendFormat(_T("CashSold=\"%d\" "), m_ShiftTotalMethod.GetCashPay());
			strReport.AppendFormat(_T("PayCard=\"%d\" "), m_ShiftTotalMethod.GetDiankaAmount());
			strReport.AppendFormat(_T("StoreIn=\"%d\" "), m_ShiftTotalMethod.GetStoreInAmount());
			strReport.AppendFormat(_T("Payout=\"%d\" "), nPayout);
			strReport.AppendFormat(_T("UseDeposit=\"%d\" "), 0);
			strReport.AppendFormat(_T("Deposit=\"%d\" "), m_ShiftTotalMethod.GetDeposit());
			strReport.AppendFormat(_T("ShiftIn=\"%d\" "), m_nDutyIncome - nPayout);
			strReport.AppendFormat(_T("LastRemain=\"%d\" "), m_ShiftTotalMethod.GetReserveAmount());
			strReport.AppendFormat(_T("AllCash=\"%d\" "), nFetchAmount + nRemainAmount);
			strReport.AppendFormat(_T("NextRemain=\"%d\" "), nRemainAmount);
			strReport.AppendFormat(_T("FetchMoney=\"%d\" "), nFetchAmount);
			strReport.AppendFormat(_T("CreditCard=\"%d\" "), m_ShiftTotalMethod.GetCreditCard());
			strReport.AppendFormat(_T("CardSold=\"%d\" "), m_ShiftTotalMethod.GetNetBarPay());
			strReport.AppendFormat(_T("Dodonew=\"%d\" "), m_ShiftTotalMethod.GetZSBConsume());
			strReport.AppendFormat(_T("Description=\"%s\" "), m_strDescription);
			
			strReport += _T("/>");
			sr.ShowReport(1,strReport);
		}
		else
		{
			CIBALog::GetInstance()->Write(_T("Can't create IBAShiftReport!"));
		}
	}
	else
	{
		CShiftCash ShiftCash;
		ShiftCash.SetStartDate(m_strOnDutyTime);
		ShiftCash.SetOperator(theApp.GetCurCashier()->GetName());
		ShiftCash.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
		ShiftCash.SetCardCreditedAmount(m_ShiftTotalMethod.GetCreditCard());
		ShiftCash.SetCostExpense(m_ShiftTotalMethod.GetCostExpense());
		ShiftCash.SetCreditAmount(m_ShiftTotalMethod.GetCreditAmount());
		ShiftCash.SetDeposit(m_ShiftTotalMethod.GetDeposit());
		ShiftCash.SetReturnAmount(m_ShiftTotalMethod.GetReturnedAmount());
		ShiftCash.SetDescription(m_strDescription);
		CString strRemain;
		GetDlgItem(IDC_STATIC_RESERVE)->GetWindowText(strRemain);
		ShiftCash.SetRemainAmount((INT)(_tstof(strRemain) * 100));
		ShiftCash.SetNextRemainAmount(m_nNextReserve);//下一班备用金
		ShiftCash.SetCashSoldAmount(m_ShiftTotalMethod.GetCashPay());
		ShiftCash.SetPayout(nPayout);
		ShiftCash.SetTotalAmount(nFetchAmount + nRemainAmount);
		ShiftCash.SetFetchAmount(nFetchAmount);
		ShiftCash.SetEndDate(CIBAHelpper::GetCurrentTimeAsString(TRUE));
		ShiftCash.SetDianka(m_ShiftTotalMethod.GetDiankaAmount());
		ShiftCash.SetAccountPay(m_ShiftTotalMethod.GetNetBarPay());
		ShiftCash.SetZSBConsume(m_ShiftTotalMethod.GetZSBConsume());
		ShiftCash.SetZSBBalance(m_ShiftTotalMethod.GetZSBBalance());
		ShiftCash.SetStoreInAmount(m_ShiftTotalMethod.GetStoreInAmount());

		CString strDutyIncome;
		GetDlgItem(IDC_STATIC_NETCASH)->GetWindowText(strDutyIncome);
		ShiftCash.SetDutyIncome((INT)(_tstof(strDutyIncome) * 100));

		PrintTicket(ShiftCash);
	}
}

void CDlgIBADuty::ShowShiftTotalInfo()
{
	CString strTmp;
	
	//充值金额
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetCreditAmount() / 100.0);
	GetDlgItem(IDC_STATIC_CREDIT)->SetWindowText(strTmp);
	
	//现金购物
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetCashPay() / 100.0);
	GetDlgItem(IDC_STATIC_CASHPAY)->SetWindowText(strTmp);
	
	//退款金额
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetReturnedAmount() / -100.0); //显示为负的
	GetDlgItem(IDC_STATIC_RETURNED)->SetWindowText(strTmp);
	
	//充值卡金额，不算在现金总额
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetCreditCard() / 100.0);
	GetDlgItem(IDC_STATIC_CREDITCARD)->SetWindowText(strTmp);

	//{ 2011/05/10-gxx: 增收宝 （消费金额，余额)
	
	INT nZSBCom = m_ShiftTotalMethod.GetZSBConsume();
	strTmp.Format(_T("%.2f"), nZSBCom / 100.0);
	GetDlgItem(IDC_STATIC_ZSB_CONSUME)->SetWindowText(strTmp);

	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetZSBBalance() / 100.0);
	GetDlgItem(IDC_STATIC_ZSB_BALANCE)->SetWindowText(strTmp);

	//}

	// 2011/07/26-8201-gxx: 点卡销售
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetDiankaAmount() / 100.0);
	GetDlgItem(IDC_STATIC_DIANKA)->SetWindowText(strTmp);
	

	//账户购物，不算在现金总额
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetNetBarPay() / 100.0);
	GetDlgItem(IDC_STATIC_NETBARPAY)->SetWindowText(strTmp);

	//商品采购支出
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetStoreInAmount() / -100.0);
	GetDlgItem(IDC_STATIC_STOREIN)->SetWindowText(strTmp);

	//押金
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetDeposit() / 100.0);
	GetDlgItem(IDC_STATIC_DEPOSIT)->SetWindowText(strTmp);

	//上一班留下
	strTmp.Format(_T("%.2f"), m_ShiftTotalMethod.GetReserveAmount() / 100.0);
	GetDlgItem(IDC_STATIC_RESERVE)->SetWindowText(strTmp);
	m_edtReverveAmount.SetWindowText(strTmp);
	
	UINT nCasherID = CIBAConfig::GetInstance()->GetCashRegisterID();
	
	// 班次收入
	INT nAllMoney = 0;
	nAllMoney += m_ShiftTotalMethod.GetCreditAmount();
	nAllMoney -= m_ShiftTotalMethod.GetReturnedAmount();
	nAllMoney += m_ShiftTotalMethod.GetCashPay();
	nAllMoney += m_ShiftTotalMethod.GetDeposit();
	nAllMoney -= m_ShiftTotalMethod.GetStoreInAmount();
	nAllMoney -= 0;
	nAllMoney += m_ShiftTotalMethod.GetCostExpense(); // +工本费
	nAllMoney += m_ShiftTotalMethod.GetDiankaAmount(); // 2011/07/26-8201-gxx: +点卡销售
	m_nDutyIncome = nAllMoney;
	strTmp.Format(_T("%.2f"), m_nDutyIncome / 100.0);
	GetDlgItem(IDC_STATIC_NETCASH)->SetWindowText(strTmp);

	

	//本班现金总额=班次收入+上一班预留
	nAllMoney += m_ShiftTotalMethod.GetReserveAmount();
	m_nAllMoney = nAllMoney;
	strTmp.Format(_T("%.2f"), m_nAllMoney / 100.0);
	GetDlgItem(IDC_STATIC_ALL)->SetWindowText(strTmp);

	//其他支出
	m_edtPayout.SetWindowText(_T("0.0"));


	m_nNextReserve = 0;
	//启用动态备用金
	if (CIBAConfig::GetInstance()->GetDynamicReserve())
	{
		//留给下一班的动态备用金，由中心计算，交班保存
		//m_nNextReserve = m_ShiftTotalMethod.GetTempBalance();

		//2011-03-21-gxx-修改预留金的获取
		
		int nBlaSum = CLocalServer::GetInstance()->ActiveMemberList.GetDynamicReserve(nCasherID);
		m_nNextReserve = nBlaSum;

	} 

	//留给下一班
	strTmp.Format(_T("%.2f"), m_nNextReserve / 100.0);
	GetDlgItem(IDC_EDIT_NEXTREVERSE)->SetWindowText(strTmp);

	//上缴金额
	strTmp.Format(_T("%.2f"), (m_nAllMoney - m_nNextReserve) / 100.0);
	GetDlgItem(IDC_STATIC_HANDLEIN)->SetWindowText(strTmp);
	
	
	//上班时间
	strTmp = GetShiftName(theApp.GetCurCashier()->GetShiftType());
	strTmp.Append(_T(" "));
	strTmp.Append(theApp.GetCurCashier()->GetOnDutyTime());
	CIBAString strCaption;
	strCaption.LoadString(IDS_THIS_DUTY_INFO);
	strCaption += _T("  ");
	strCaption += strTmp;
	GetDlgItem(IDC_STATIC_GROUP_TEXT)->SetWindowText(strCaption);
}

UINT CDlgIBADuty::GetShiftType()
{
	if (CIBAConfig::GetInstance()->GetShiftCount() == 1)
	{
		return 1;
	}

	COleDateTime dtNow = COleDateTime::GetCurrentTime();

	CStringArray strArray;

	CIBAHelpper::SplitLine(CIBAConfig::GetInstance()->GetShift1(), strArray, '-');

	INT nStart = _ttoi(strArray.GetAt(0));
	INT nEnd = _ttoi(strArray.GetAt(1));

	if (nEnd > nStart)
	{
		if (dtNow.GetHour() >= nStart && dtNow.GetHour() < nEnd)
		{
			return 1;
		}
	}
	else
	{
		if (dtNow.GetHour() >= nStart || dtNow.GetHour() < nEnd)
		{
			return 1;
		}
	}

	if (CIBAConfig::GetInstance()->GetShiftCount() == 3)
	{
		CIBAHelpper::SplitLine(CIBAConfig::GetInstance()->GetShift2(), strArray, '-');

		nStart = _ttoi(strArray.GetAt(0));
		nEnd = _ttoi(strArray.GetAt(1));

		if (nEnd > nStart)
		{
			if (dtNow.GetHour() >= nStart && dtNow.GetHour() < nEnd)
			{
				return 2;
			}
		}
		else
		{
			if (dtNow.GetHour() >= nStart || dtNow.GetHour() < nEnd)
			{
				return 2;
			}
		}
	}

	return 3;
}

void CDlgIBADuty::DoOnDuty()
{
	//取得用户输入的备用金
	UpdateData();
	
	//启用动态备用金
	if (CIBAConfig::GetInstance()->GetDynamicReserve())
	{
		GetDlgItem(IDC_EDIT_RESERVE)->EnableWindow(FALSE);
	} 

	//从输入取得备用金
	CString strTmp;
	m_edtReverveAmount.GetWindowText(strTmp);
	double dblReverveAmount = _tstof(strTmp);
	m_nNextReserve = dblReverveAmount * 100;
	
	//上班方法
	COnDutyMethod OnDuty;

	OnDuty.SetShiftType(m_cboShift.GetCurSel() + 1);

	OnDuty.SetReserveAmount(m_nNextReserve);

	theApp.GetCurCashier()->DoOnDuty(OnDuty);

	if (OnDuty.GetStatusCode() == 0)
	{			
		MsgBoxFormat(MB_OK, IDS_ONDUTYOK, theApp.GetCurCashier()->GetName());

		CBCGPDialog::OnOK();
	}
	else
	{
		if (OnDuty.GetStatusCode() == ESC_BCCS74)
		{
			ShowToolTip(IDS_ERRORBCCS, IDOK, TRUE);
		}
		else if (OnDuty.GetStatusCode() == ESC_YJSB72)
		{
			CString strLastUser, strTmp;
			
			theApp.GetCurCashier()->CheckLastDuty(strLastUser);

			strTmp.Format(IDS_ERRORYJSB, strLastUser);

			SetToolTipPos(IDOK);

			ShowToolTip(strTmp, TRUE);
		}
		else
		{
			SetToolTipPos(IDOK);

			ShowToolTip(OnDuty.GetStatusMessage(), TRUE);
		}
	}
}

void CDlgIBADuty::DoOffDuty()
{
	//舍入操作，防止浮点型出错
	m_nNextReserve = (_tstof(m_strNextReverse) + 0.005) * 100;
	
	if (m_nNextReserve < 0)
	{
		SetToolTipPos(IDC_EDIT_NEXTREVERSE);
		ShowToolTip(IDS_INPUTEERRO);
		return;
	}

	CShiftCash ShiftCash;//收银员换班方法
	ShiftCash.SetStartDate(theApp.GetCurCashier()->GetOnDutyTime());
	ShiftCash.SetOperator(theApp.GetCurCashier()->GetName());
	ShiftCash.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
	ShiftCash.SetCardCreditedAmount(m_ShiftTotalMethod.GetCreditCard());
	ShiftCash.SetCostExpense(m_ShiftTotalMethod.GetCostExpense());
	ShiftCash.SetCreditAmount(m_ShiftTotalMethod.GetCreditAmount());
	ShiftCash.SetDeposit(m_ShiftTotalMethod.GetDeposit());
	ShiftCash.SetReturnAmount(m_ShiftTotalMethod.GetReturnedAmount());
	ShiftCash.SetDescription(m_strDescription);
	ShiftCash.SetRemainAmount(m_nNextReserve );
	ShiftCash.SetCashSoldAmount(m_ShiftTotalMethod.GetCashPay());
	ShiftCash.SetNextOperator(GetShiftName(theApp.GetCurCashier()->GetShiftType()));

	COffDutyMethod OffDuty;//下班方法
	OffDuty.SetDeposit(m_ShiftTotalMethod.GetDeposit());
	OffDuty.SetCostExpense(m_ShiftTotalMethod.GetCostExpense());
	OffDuty.SetCreditAmount(m_ShiftTotalMethod.GetCreditAmount());
	OffDuty.SetCreditCard(m_ShiftTotalMethod.GetCreditCard());
	OffDuty.SetCashPay(m_ShiftTotalMethod.GetCashPay());
	OffDuty.SetRecordCount(m_ShiftTotalMethod.GetRecordCount());
	OffDuty.SetClassInfoRecords(m_ShiftTotalMethod.GetCardInfoRecords());
	OffDuty.SetReturnedAmount(m_ShiftTotalMethod.GetReturnedAmount());
	OffDuty.SetDescription(m_strDescription);
	OffDuty.SetNetBarPay(m_ShiftTotalMethod.GetNetBarPay());
	OffDuty.SetStoreInAmount(m_ShiftTotalMethod.GetStoreInAmount());

	INT nPayout = 0, nRemainAmount = 0, nFetchAmount = 0;

	GetEachMoney(nPayout, nRemainAmount, nFetchAmount);

	OffDuty.SetPayout(nPayout);
	OffDuty.SetRemainAmount(nRemainAmount);
	OffDuty.SetFetchAmount(nFetchAmount);

	ShiftCash.SetPayout(nPayout);
	ShiftCash.SetTotalAmount(nFetchAmount);

	theApp.GetCurCashier()->DoOffDuty(OffDuty);

	if (OffDuty.GetStatusCode() == 0)
	{
		ShiftCash.SetEndDate(CIBAHelpper::FormatCenterTime(OffDuty.GetOffDutyTime()));

		CIBADAL::GetInstance()->AddShiftCashRecord(ShiftCash);//写入数据库
		//写配置文件
		CIBAConfig::GetInstance()->SetDynamicReserveMoney(ShiftCash.GetRemainAmount());
		//自动交班打印
		if (CIBAConfig::GetInstance()->GetPrintShiftReport())
		{
			if (GetDlgItem(IDC_BUTTON_PRINT)->IsWindowEnabled())
			{
				OnBnClickedButtonPrint();
			}
		}

		MsgBoxFormat(MB_OK, IDS_OFFDUTYOK, theApp.GetCurCashier()->GetName());

		DeleteOffDutyOperationFile();

		// 2011/07/27-8201-gxx: 关闭点卡平台
		DestroyPayCardPlatform();

		CBCGPDialog::OnOK();
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(OffDuty.GetStatusMessage(), TRUE);
	}
}

void CDlgIBADuty::InitGridTree()
{
	CRect rectGrid;
	GetDlgItem(IDC_STATIC_JXC)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_STATIC_JXC)->MapWindowPoints(this, &rectGrid);

	m_wndGridTree.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);

	CIBAString strHeader;
	strHeader.LoadString(IDS_DUTYJXC);
	//strHeader = L"商品类别|商品名称|商品编码|价格|库存|现金购物|金额|账户购物|赠送|兑换积分|入库数量|入库金额";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		const UINT Width[] = {60, 180, 90, 40, 40, 60, 40, 60, 40, 60, 60, 60};

		for (INT i = 0; i < strArray.GetCount() - 2; i++)
		{
			m_wndGridTree.InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_wndGridTree.SetColumnAlign(i, HDF_LEFT);
		}
	}
	else
	{
		m_wndGridTree.SetFont(&m_Font);

		const UINT Width[] = {70, 250, 140, 90, 90};

		for (INT i = 0; i < 5; i++)
		{
			m_wndGridTree.InsertColumn(i, strArray.GetAt(i), Width[i]);
			m_wndGridTree.SetColumnAlign(i, HDF_LEFT);
		}
	}
}

//更新商品数据
void CDlgIBADuty::FillGird()
{	
	m_wndGridTree.RemoveAll();

	CCommodityArray CommodityArray;//待售商品队列
	CCommodityQueryCondition CommodityQueryCondition;//商品查询条件
	//从数据库取得库存数据
	if (!CIBADAL::GetInstance()->GetStoreCommodity(CommodityArray, CommodityQueryCondition)) return;

	CCommoditySoldInfoArray CommoditySoldInfoArray;
	if (!CIBADAL::GetInstance()->GetCommoditySoldInfo(CommoditySoldInfoArray)) 
	{
		//
	}

	//取得列数
	const INT nColumns = m_wndGridTree.GetColumnCount();

	INT nTmp = 0; //库存数量

	CString strLastGroupId;//上一次的商品
	CBCGPGridRow* pLastGroup = NULL;
	//商品记录
	for (INT i = 0; i < CommodityArray.GetCount(); i++)
	{	//商品信息
		CCommodity & Commodity = CommodityArray.GetAt(i);
		//商品ID
		CString strTmp = Commodity.GetGroupId();
		//此商品没有出现过
		if (strLastGroupId != strTmp)
		{	
			if (nTmp > 0)//数量大于0
			{
				pLastGroup->GetItem(4)->SetValue(nTmp);
				nTmp = 0;
			}

			strLastGroupId = strTmp;
			//创建记录
			pLastGroup = m_wndGridTree.CreateRow(nColumns);
			pLastGroup->AllowSubItems();// 关联式子菜单
			pLastGroup->GetItem(0)->SetValue(Commodity.GetGroupName().GetString());
			//添加记录
			m_wndGridTree.AddRow(pLastGroup, FALSE);
		}
		//此商品出现过
		CBCGPGridRow* pCommodity = m_wndGridTree.CreateRow(nColumns);
		ASSERT_VALID(pCommodity);
		//设置子项
		pCommodity->GetItem(1)->SetValue(Commodity.GetCommodityName().GetString());
		pCommodity->GetItem(2)->SetValue(Commodity.GetBarCode().GetString());
		strTmp.Format(_T("%.2f"), Commodity.GetAdvicePrice() / 100.0);
		pCommodity->GetItem(3)->SetValue(strTmp.GetString());
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

			if (Commodity.GetStoreInQuantity() > 0)
			{
				pCommodity->GetItem(10)->SetValue(Commodity.GetStoreInQuantity());
				pCommodity->GetItem(11)->SetValue(Commodity.GetStoreInAmount());
			}
		}

		nTmp += Commodity.GetRemainQuantity();

		pLastGroup->AddSubItem(pCommodity, FALSE);//添加子列表
	}

	if (pLastGroup != NULL) //有商品数据
	{
		pLastGroup->GetItem(4)->SetValue(nTmp);
		GetDlgItem(IDC_BUTTON_JXCPRINT)->EnableWindow(TRUE);
	}

	m_wndGridTree.AdjustLayout();
}
//打印商品
void CDlgIBADuty::OnBnClickedButtonJxcprint()
{
	m_wndGridTree.ExpandAll();
	m_wndGridTree.Print();
}

void CDlgIBADuty::OnEnChangeEditNextreverse()
{
	OnEnChangeEditPayout();
}

INT CDlgIBADuty::Round(double dsrc)
{
	if (dsrc >= 0)
	{
		return (INT)(dsrc + 0.5);
	}
	else
	{
		return (INT)(dsrc - 0.5);
	}
}

void CDlgIBADuty::OnBnClickedButtonStorecheck()
{
	CStoreDlg StoreDlg;
	StoreDlg.DoModal();
}

void CDlgIBADuty::HideShiftTotalInfo()
{
	GetDlgItem(IDC_STATIC_HANDLEIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2_HANDLEIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_NEXTREVERSE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2_NEXTRESERVE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RESERVE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CREDIT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RETURNED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_DEPOSIT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_NETCASH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CASHPAY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PAYOUT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_ALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_NEXTREVERSE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_HANDLEIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CREDITCARD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_NETBARPAY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_STOREIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(FALSE);
}


CString CDlgIBADuty::GetShiftName(UINT nShiftType)
{
	if (1 == nShiftType)
	{
		return CIBAConfig::GetInstance()->GetShiftName1();
	}
	else if (2 == nShiftType)
	{
		return CIBAConfig::GetInstance()->GetShiftName2();
	}
	else  if (3 == nShiftType)
	{
		return CIBAConfig::GetInstance()->GetShiftName3();
	}
	else
	{
		return _T("");
	}
}

void CDlgIBADuty::DeleteOffDutyOperationFile()
{
	static TCHAR* sStrOutDir  = _T("\\IBAConfig\\operateInfo\\");
	CString strFileName = theApp.GetWorkPath();
	strFileName += sStrOutDir;
	strFileName += theApp.GetCurCashier()->GetName();
	strFileName += L".dat";

	if (PathFileExists(strFileName))
	{
		DeleteFile(strFileName);
	}
}
