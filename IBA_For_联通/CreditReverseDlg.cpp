// CreditReverseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "dal\IBADAL.h"
#include "MainFrm.h"
#include ".\creditreversedlg.h"
#include "Socket\LocalServer.h"

#include "DlgCreditModify.h"

#include "BCGPLabalButton.h"
#include "BCGPLinkStatic.h"

using namespace NS_COMMON;
// CCreditReverseDlg 对话框

IMPLEMENT_DYNAMIC(CCreditReverseDlg, CNDDialog)
CCreditReverseDlg::CCreditReverseDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CCreditReverseDlg::IDD, pParent)
{
}

CCreditReverseDlg::~CCreditReverseDlg()
{
}

void CCreditReverseDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CREDIT, m_CreditList);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	
	
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);

	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);

	

}

BEGIN_MESSAGE_MAP(CCreditReverseDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CREDIT, OnNMClickListCredit)
	ON_BN_CLICKED(IDC_BUTTON_RECENTCREDIT, OnBnClickedButtonRecentcredit)
	ON_WM_TIMER()
	ON_NOTIFY(GLCN_CHILD_CONTROL, IDC_LIST_CREDIT, OnGLCNotify)
END_MESSAGE_MAP()


// CCreditReverseDlg 消息处理程序

void CCreditReverseDlg::OnBnClickedOk()
{
	if (m_strRefNo.IsEmpty() || m_strCreditTime.IsEmpty())
	{
		return;
	}

	CIBAString strTmp;
	strTmp.LoadString(IDS_CREDITREVERSE);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	SetToolTipPos(IDOK);

	CCreditReverseMethod CreditReverse;

	CreditReverse.SetMemberId(_ttoi(m_strMemberID));
	CreditReverse.SetLastRefNo(_ttoi(m_strRefNo));

	CString strCreditTime(m_strCreditTime);
	strCreditTime.Remove(':');
	strCreditTime.Remove('-');
	strCreditTime.Remove(' ');
	CreditReverse.SetTrantime(strCreditTime);

	theApp.GetCurCashier()->DoCreditReverse(CreditReverse);
	
	if (CreditReverse.GetStatusCode() == 0)
	{
		ShowToolTip(IDS_CREDITRESERVEROK);

		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
			m_nMemberId, 0, 0, _tstof(m_strCreditMoney) * 100);

		 //发送充值消息		
		CString strTmp;
		strTmp.Format(IDS_ONLINECREDITREVERSE, _tstof(m_strCreditMoney));
		CLocalServer::GetInstance()->LocalSendShortMessageByMemberID((UINT)_ttoi(m_strMemberID), strTmp);

		//添加冲正记录
		CCreditInfoDB CreditInfo;
		CreditInfo.SetMemberId(_ttoi(m_strMemberID));
		CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
		CreditInfo.SetRefNo(_ttoi(m_strRefNo));
		CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		CreditInfo.SetCreditDate(CIBAHelpper::GetCurrentTimeAsString(TRUE));
		
		CIBADAL::GetInstance()->AddCreditReverseInfo(CreditInfo);
		
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(9);
		OperationInfo.SetUserName(m_strName);
		OperationInfo.SetNetId(m_strNetId);
		OperationInfo.SetMemberId(m_nMemberId);
		OperationInfo.SetOperationAmount(_tstof(m_strCreditMoney) * 100);
		OperationInfo.SetPresentAmount(0);
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		//保存最后交易记录
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(_ttoi(m_strRefNo));
		TranInfo.SetLastTranTime(strCreditTime);
		TranInfo.SetLastTranType(6);
		TranInfo.Save();
		
		//重新查询
		
		m_strNetId = m_strLastUserID;
		
		ClearData();
		
		if (!m_strNetId.IsEmpty())
		{
			CheckCard(m_strNetId);
		}
		else
		{
			INT nIndex = m_CreditList.GetSelectionMark();
			m_CreditList.DeleteItem(nIndex);
		}
		
		UpdateData(FALSE);
	}
	else
	{
		ShowToolTip(CreditReverse.GetStatusMessage(), TRUE);
	}
}

BOOL CCreditReverseDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	//{ 2011/05/24-gxx: 自动查询在刷卡及扫描证件的情况下采用
	if (m_bAutoQuery)
	{
		SetTimer(TimeIdOfAutoQuery, 100, NULL);//只查询一次
	}
	else
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_IDCARDSNO);
		ASSERT(pWnd);
		if (pWnd)
		{
			pWnd->SetFocus();
		}
	}
	//}

	InitControls();

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCreditReverseDlg::InitControls()
{
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	CIBAString strHeader;
	//strHeader = _T("充值时间|充值金额|赠送金额|修改|删除");
	strHeader.LoadString(IDS_CREDITRLISTHEAD);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT nWidth[] = {166, 70, 70, 94, 94};
	const UINT nFormat[] = {LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_CENTER, LVCFMT_CENTER};

	m_CreditList.InsertColumn(eCreditTime, strArray.GetAt(eCreditTime), 
		nFormat[eCreditTime],nWidth[eCreditTime]);

	
	m_CreditList.InsertColumn(eCreditMoney, strArray.GetAt(eCreditMoney), 
		nFormat[eCreditMoney],nWidth[eCreditMoney]);

	m_CreditList.InsertColumn(ePresentMoney, strArray.GetAt(ePresentMoney), 
		nFormat[ePresentMoney],nWidth[ePresentMoney]);

	m_CreditList.InsertColumn(eModify, _T(""), 
		nFormat[eModify], nWidth[eModify], TRUE, FALSE, TRUE);
	m_CreditList.InsertColumn(eDelete, _T(""), 
		nFormat[eDelete], nWidth[eDelete], TRUE, FALSE, TRUE);

	m_CreditList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_NOSORTHEADER);
	
	m_CreditList.SetItemsHeight(20);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
}


void CCreditReverseDlg::OnNMClickListCredit(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT nIndex = m_CreditList.GetSelectionMark();

	if (nIndex >= 0)
	{
		DWORD dwIndx = m_CreditList.GetItemData(nIndex);

		m_strCreditTime = CIBAHelpper::FormatCenterTime(ShiftCredit.GetCreditTime(dwIndx));
		m_strRefNo.Format(_T("%d"), ShiftCredit.GetRefNo(dwIndx));
		m_strCreditMoney.Format(_T("%.2f"), ShiftCredit.GetCreditAmount(dwIndx) / 100.0);
		m_strMemberID.Format(_T("%d"), ShiftCredit.GetMemberId(dwIndx));

		UpdateData(FALSE);

		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}

	*pResult = 0;
}

void CCreditReverseDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);

	m_strLastUserID = m_strNetId;

	QueryBalance(nMemberId, nCardId);

	GetCreditInfos(nMemberId);	
}

void CCreditReverseDlg::OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance)
{
	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());

	double dblTmp = 0.0, dblTmp1 = 0.0, dblTmp2 = 0.0;

	//充值账户
	dblTmp = Balance.GetBalance();
	dblTmp2 = dblTmp / 100;
	m_strCreditBalance.Format(_T("%.2f"), dblTmp2);

	//无送
	dblTmp = Balance.GetCashBalance();
	dblTmp2 = dblTmp / 100.0;
	m_strBalanceW.Format(_T("%.2f"), dblTmp2);

	dblTmp1 = dblTmp;

	//有送
	dblTmp = Balance.GetUsePresent();
	dblTmp2 = dblTmp / 100.0;
	m_strBalanceY.Format(_T("%.2f"), dblTmp2);

	dblTmp1 += dblTmp;

	//赠送
	dblTmp = Balance.GetCreditPresent();
	dblTmp2 = dblTmp / 100.0;
	m_strBalanceZ.Format(_T("%.2f"), dblTmp2);

	dblTmp1 += dblTmp;

	//上网预留费
	dblTmp = Balance.GetOnlineAmount();
	dblTmp2 = dblTmp / 100.0;

	dblTmp1 += dblTmp;

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	dblTmp2 = dblTmp1 / 100.0;
	m_strAllBalance.Format(_T("%.2f"), dblTmp2);

	//押金
	dblTmp = Balance.GetGuarantyMoney();
	dblTmp2 = dblTmp / 100.0;
	m_strDeposit.Format(_T("%.2f"), dblTmp2);

	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());
}

void CCreditReverseDlg::ClearData()
{
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strNetBar.Empty();
	m_strPersonalID.Empty();
	m_strUserClass.Empty();
	m_strAllBalance.Empty();
	m_strBalanceY.Empty();
	m_strBalanceW.Empty();
	m_strBalanceZ.Empty();
	m_strDeposit.Empty();
	m_strCreditMoney.Empty();
	m_strCreditTime.Empty();
	m_strRefNo.Empty();
	UpdateData(FALSE);
}

void CCreditReverseDlg::GetCreditInfos(UINT nMemberId)
{
	m_CreditList.DeleteAllItems();

	ShiftCredit.SetMemberId(nMemberId);
	
	theApp.GetCurCashier()->DoShiftCredit(ShiftCredit);

	if (ShiftCredit.GetStatusCode() == 0)
	{
		UINT nCount = min(ShiftCredit.GetRecordCount(), 20); //最多显示20条

		CIBAString strModify;
		CIBAString strDelete;
		strModify.LoadString(IDS_MODIFYMONEY);
		strDelete.LoadString(IDS_DELETEMONEY);
		for (UINT i = 0; i < nCount; i++)
		{
			IBA_TRACE("-------------------RefNO=%d", ShiftCredit.GetRefNo(i));

			m_CreditList.InsertItem(i, CIBAHelpper::FormatCenterTime(ShiftCredit.GetCreditTime(i)));
			
			CString strTmp;
			strTmp.Format(_T("%.2f"), ShiftCredit.GetCreditAmount(i) / 100.0);
			m_CreditList.SetItemText(i, eCreditMoney, strTmp);

			strTmp.Format(_T("%.2f"), ShiftCredit.GetCreditPresent(i) / 100.0);
			m_CreditList.SetItemText(i, ePresentMoney, strTmp);

			CBCGPLinkStatic* pLS = new CBCGPLinkStatic;
			pLS->Create(_T(""), WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_CENTER, CRect(0,0,0,0), 
				&m_CreditList, m_CreditList.RegeisterCtrlID());
			pLS->SetLinkColor(RGB(0,0,255));
			pLS->SetHoverColor(RGB(255,0,0));

			m_CreditList.SetItem(i, eModify, strModify, CGxxListCtrl::GLC_CUSTOM, pLS, &CSize(45,18));

			pLS = new CBCGPLinkStatic;
			pLS->Create(_T(""), WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_CENTER, CRect(0,0,0,0), 
				&m_CreditList, m_CreditList.RegeisterCtrlID());
			pLS->SetLinkColor(RGB(0,0,255));
			pLS->SetHoverColor(RGB(255,0,0));
			
			m_CreditList.SetItem(i, eDelete, strDelete, CGxxListCtrl::GLC_CUSTOM, pLS, &CSize(45,18));

			m_CreditList.SetItemData(i, i);
		}
		m_CreditList.ShowAllSubWindows();
	}
	else
	{
		if (nMemberId == 0)
		{
			SetToolTipPos(IDC_BUTTON_RECENTCREDIT);
		}
		else
		{
			SetToolTipPos(IDC_BUTTON_QUERY);
		}
		
		ShowToolTip(ShiftCredit.GetStatusMessage(), TRUE);
	}
}

void CCreditReverseDlg::OnBnClickedButtonRecentcredit()
{
	GetCreditInfos(0);
}

void CCreditReverseDlg::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfAutoQuery == nIDEvent)
	{
		KillTimer(nIDEvent);

		if (m_bAutoQuery)
		{
			m_bAutoQuery = FALSE;
			CNDDialog::OnBnClickedButtonQuery();
		}
	}

	CNDDialog::OnTimer(nIDEvent);
}

void CCreditReverseDlg::OnGLCNotify(LPNMHDR lpHdr, LRESULT *pResult)
{
	LPNMHDR_GLC_CONTROL lpGLC = (LPNMHDR_GLC_CONTROL)lpHdr;
	*pResult = 0;

	if (lpGLC->pCtrlHdr->code != BN_CLICKED)
	{
		return ;

	}
	SetToolTipPos(IDC_STATIC_ALL);

	if ( lpGLC->nColID == eModify )
	{
		IBA_LOG(_T("开始修正金额"));

		// 修改金额

		INT nIndex = lpGLC->nRowID;
		ASSERT(nIndex>=0);

		m_CreditList.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		//m_CreditList.Invalidate();
		DWORD dwIndx = m_CreditList.GetItemData(nIndex);

		m_strCreditTime = CIBAHelpper::FormatCenterTime(ShiftCredit.GetCreditTime(dwIndx));
		m_strRefNo.Format(_T("%d"), ShiftCredit.GetRefNo(dwIndx));
		m_strCreditMoney.Format(_T("%.2f"), ShiftCredit.GetCreditAmount(dwIndx) / 100.0);
		m_strMemberID.Format(_T("%d"), ShiftCredit.GetMemberId(dwIndx));

		CDlgCreditModify dlgMod;
		dlgMod.m_strCreditMoney = m_strCreditMoney;

		CString strOldMoney = m_strCreditMoney;


		CString strNewMoney=_T("0.0");
		if (dlgMod.DoModal() == IDOK)
		{
			CIBAString strTmp;
			strTmp.LoadString(IDS_MODIFYCREDIT);

			if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
			{
				return ;
			}

			strNewMoney = dlgMod.m_strNewMoney;

			// 先充值
			CCreditMethod Credit;

			Credit.SetMemberId(m_nMemberId);
			Credit.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
			Credit.SetActivityId(0);
			Credit.SetNoPresent( ShiftCredit.GetCreditPresent(nIndex) == 0 );

			double dNewMoney = _tstof(strNewMoney);

			UINT nNewMoney = (UINT)(dNewMoney*100.0);

			Credit.SetAmount(nNewMoney);

			theApp.GetCurCashier()->DoCredit(Credit);

			if (Credit.GetStatusCode() != 0)
			{
				ShowToolTip(Credit.GetStatusMessage(), TRUE);
				IBA_LOG0(_T("修正金额失败"));
				return;
			}

			CIBALog::GetInstance()->WriteFormat(_T("充值修正->充值成功: netID=%s, oldMoney=%s, newMoney=%s"),
				m_strNetId,strOldMoney,strNewMoney);

			if (!DeleteCredit(FALSE))
			{
				Sleep(1000);
				CCreditReverseMethod CreditReverse;

				m_strRefNo.Format(_T("%d"),Credit.GetRefNo());
				m_strCreditTime = Credit.GetTrantime();
				m_strCreditMoney = strNewMoney;

				DeleteCredit(FALSE);

				CIBALog::GetInstance()->WriteFormat(_T("充值修正->充值成功,但冲正失败,取消刚才的充值: netID=%s, oldMoney=%s, newMoney=%s"),
					m_strNetId,strOldMoney,strNewMoney);
			}
			else
			{
				ShowToolTip(IDS_CREDITMODIFYOK);

				COperationInfo OperationInfo;
				OperationInfo.SetOperationType(CIBAGlobal::emOpTypeCreditModify);
				OperationInfo.SetUserName(m_strName);
				OperationInfo.SetNetId(m_strNetId);
				OperationInfo.SetMemberId(m_nMemberId);
				OperationInfo.SetOperationAmount( nNewMoney );
				//OperationInfo.SetPresentAmount(Credit.GetCreditPresent());

				CString strRemark;
				strRemark.Format(_T("修改前的充值金额为:%s"), strOldMoney );
				OperationInfo.SetDescription(strRemark);
				::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

				//发送充值消息		
				CString strTmp;
				strTmp.Format(_T("充值金额%s,修改后的金额为:%.2f"),strOldMoney, nNewMoney/100.0);
				CLocalServer::GetInstance()->LocalSendShortMessageByMemberID((UINT)_ttoi(m_strMemberID), strTmp);


				IBA_LOG0(_T("修正成功"));
			}
		}
	}

	else if( lpGLC->nColID == eDelete )
	{
		// 删除
		INT nIndex = lpGLC->nRowID;

		if (nIndex >= 0)
		{
			m_CreditList.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);

			CIBAString strTmp;
			strTmp.LoadString(IDS_CREDITREVERSE);

			if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
			{
				return ;
			}

			//m_CreditList.Invalidate();
			DWORD dwIndx = m_CreditList.GetItemData(nIndex);

			m_strCreditTime = CIBAHelpper::FormatCenterTime(ShiftCredit.GetCreditTime(dwIndx));
			m_strRefNo.Format(_T("%d"), ShiftCredit.GetRefNo(dwIndx));
			m_strCreditMoney.Format(_T("%.2f"), ShiftCredit.GetCreditAmount(dwIndx) / 100.0);
			m_strMemberID.Format(_T("%d"), ShiftCredit.GetMemberId(dwIndx));

			UINT nDeleteMoney = ShiftCredit.GetCreditAmount(dwIndx);

			UpdateData(FALSE);

			if (DeleteCredit())
			{
				ShowToolTip(IDS_CREDITRESERVEROK);

				COperationInfo OperationInfo;
				OperationInfo.SetOperationType(CIBAGlobal::emOpTypeCreditDelete);
				OperationInfo.SetUserName(m_strName);
				OperationInfo.SetNetId(m_strNetId);
				OperationInfo.SetMemberId(m_nMemberId);
				OperationInfo.SetOperationAmount(nDeleteMoney);
				OperationInfo.SetPresentAmount(0);
				::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
			}
		}

	}
}

BOOL CCreditReverseDlg::DeleteCredit(BOOL bSendMsgToClient)
{
	if (m_strRefNo.IsEmpty() || m_strCreditTime.IsEmpty())
	{
		return FALSE;
	}


	CCreditReverseMethod CreditReverse;

	CreditReverse.SetMemberId(_ttoi(m_strMemberID));
	CreditReverse.SetLastRefNo(_ttoi(m_strRefNo));

	CString strCreditTime(m_strCreditTime);
	strCreditTime.Remove(':');
	strCreditTime.Remove('-');
	strCreditTime.Remove(' ');
	CreditReverse.SetTrantime(strCreditTime);

	theApp.GetCurCashier()->DoCreditReverse(CreditReverse);

	if (CreditReverse.GetStatusCode() == 0)
	{
		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(
			m_nMemberId, 0, 0, _tstof(m_strCreditMoney) * 100 + CreditReverse.GetCreditPresent());

		if (bSendMsgToClient)
		{
			//发送充值消息		
			CString strTmp;
			strTmp.Format(IDS_ONLINECREDITREVERSE, _tstof(m_strCreditMoney));
			CLocalServer::GetInstance()->LocalSendShortMessageByMemberID((UINT)_ttoi(m_strMemberID), strTmp);
		}
		

		//添加冲正记录
		CCreditInfoDB CreditInfo;
		CreditInfo.SetMemberId(_ttoi(m_strMemberID));
		CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
		CreditInfo.SetRefNo(_ttoi(m_strRefNo));
		CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		CreditInfo.SetCreditDate(CIBAHelpper::GetCurrentTimeAsString(TRUE));

		CIBADAL::GetInstance()->AddCreditReverseInfo(CreditInfo);


		//保存最后交易记录
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(_ttoi(m_strRefNo));
		TranInfo.SetLastTranTime(strCreditTime);
		TranInfo.SetLastTranType(6);
		TranInfo.Save();

		//重新查询

		m_strNetId = m_strLastUserID;

		ClearData();

		if (!m_strNetId.IsEmpty())
		{
			CheckCard(m_strNetId);
		}
		else
		{
			INT nIndex = m_CreditList.GetSelectionMark();
			m_CreditList.DeleteItem(nIndex);
		}

		UpdateData(FALSE);

		return TRUE;
	}
	else
	{
		ShowToolTip(CreditReverse.GetStatusMessage(), TRUE);
	}

	return FALSE;
}

