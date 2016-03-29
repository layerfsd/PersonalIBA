// BundleTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cashier.h"
#include "IBAPrint.h"
#include "IBAHelpper.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include ".\bundletimedlg.h"
#include "ComputerInfo.h"
#include "selcomputerdlg.h"
#include "MainFrm.h"
#include "NDCreditDlg.h"
#include "ComputerListView.h"
#include "selfdefinebundtimedlg.h"
#include "HelpFunc.h"
#include "DlgChoseForwardBundle.h"
#include "Dal\DALUSER.h"
#include "Dal\IBADAL.h"
#include "CashierHelper.h"

//2015-0825 liyajun 可排序的列表控件
int CALLBACK CSortListCtrl::ListCompare (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortListCtrl* pSortList = (CSortListCtrl*)lParamSort;
	CString strText1 = pSortList->GetItemText(lParam1,pSortList->m_nSortedCol);
	CString strText2 = pSortList->GetItemText(lParam2,pSortList->m_nSortedCol);

	int nRet = 0;
	BOOL bAreDigits(FALSE);
	float fTemp1(0),fTemp2(0);

	//判断是否纯数字
	if(strText1 == strText1.SpanIncluding(_T("0123456789.")) &&          //只包含数字和小数点
		strText1.Find(_T('.')) == strText1.ReverseFind(_T('.')) &&		 //只包含一个小数点
		strText1.Find(_T('.')) != 0 &&                                   //小数点不在首位
		strText1.Find(_T('.')) != (strText1.GetLength() - 1) &&          //小数点不在末尾
		strText2 == strText2.SpanIncluding(_T("0123456789.")) &&
		strText2.Find(_T('.')) == strText2.ReverseFind(_T('.')) &&
		strText2.Find(_T('.')) != 0 &&
		strText2.Find(_T('.')) != (strText2.GetLength() - 1)
		)
	{
		bAreDigits = TRUE;
		fTemp1 = _tstof(strText1);
		fTemp2 = _tstof(strText2);
	}

	//如果已经是升序，改为降序排列
	if(pSortList->m_bAsc)
	{
		if(bAreDigits)
		{
			nRet = fTemp1 - fTemp2;
		}
		else
		{
			nRet = strText1.CompareNoCase(strText2);
		}
	}
	else  //如果不是升序，则用升序排列
	{
		if(bAreDigits)
		{
			nRet = fTemp2 - fTemp1;
		}
		else
		{
			nRet = (-1) * strText1.CompareNoCase(strText2);
		}		
	}

	return nRet;
}

IMPLEMENT_DYNAMIC(CDlgBundleTime, CNDDialog)
CDlgBundleTime::CDlgBundleTime(CWnd* pParent /*=NULL*/)
: CNDDialog(CDlgBundleTime::IDD, pParent)
, m_nSelBundleTimeId(0), m_nSelBundleTimePrice(0)
, m_bAfterBundleTime(FALSE)
, m_nLastMemberID(0), m_nCurrBundleTimeIndex(0)
{
	m_nBudleTimeMoney = 0;
}

CDlgBundleTime::~CDlgBundleTime()
{
}

void CDlgBundleTime::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BUNDLETIME, m_BundleTimeList);
	DDX_Control(pDX, IDC_CHECK_PRINT, m_btnPrint);
	DDX_Control(pDX, IDC_STATIC_ALL, m_btnAllBalance);
	DDX_Control(pDX, IDC_BUTTON_CREDIT, m_btnCredit);
	DDX_Control(pDX, IDC_BUTTON_BALANCE, m_btnBalance);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strOpenNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_BTAREA, m_strBundleTimeArea);
	DDX_Text(pDX, IDC_STATIC_BTENDTIME, m_strBundleTimeEndTime);
	DDX_Text(pDX, IDC_STATIC_BTID, m_strBundleTimeID);
	DDX_Text(pDX, IDC_STATIC_BTMONEY, m_strBundleTimeMoney);
	DDX_Text(pDX, IDC_STATIC_BTMONEY2, m_strBundleTimeMoney2);
	DDX_Text(pDX, IDC_STATIC_BTSTARTTIME, m_strBundleTimeStartTime);
	DDX_Text(pDX, IDC_STATIC_BTYPE, m_strBundleTimeType);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
}


BEGIN_MESSAGE_MAP(CDlgBundleTime, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BUNDLETIME, OnNMClickListBundletime)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_BUNDLETIME, OnColumnListClicked) //2015-0825 liyajun 响应点击排序消息
	//ON_MESSAGE(WM_BUNDLETIMESORT, OnColumnListClicked)      
	ON_BN_CLICKED(IDC_BUTTON_CONTINUEBT, OnBnClickedButtonContinuebt)
	ON_BN_CLICKED(IDC_BUTTON_CANCELBT, OnBnClickedButtonCancelbt)
	ON_BN_CLICKED(IDC_BUTTON_FORWARDBT, OnBnClickedButtonForwardbt)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEBT, OnBnClickedButtonChangebt)
	ON_BN_CLICKED(IDC_BUTTON_REPRINT, OnBnClickedButtonReprint)
	ON_BN_CLICKED(IDC_CHECK_PRINT, OnBnClickedCheckPrint)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELFBUNDTIME, OnBnClickedSelfbundtime)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_ALL, OnStnClickedStaticAll)
	ON_BN_CLICKED(IDC_BUTTON_CREDIT, OnBnClickedButtonCredit)
	ON_BN_CLICKED(IDC_BUTTON_BALANCE, OnBnClickedButtonBalance)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
END_MESSAGE_MAP()


// CDlgBundleTime 消息处理程序

//   (包时-提前包时-转包时-取消包时-自定义包时)

BOOL CDlgBundleTime::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	m_btnAllBalance.SetFont(&m_Font);
	m_btnAllBalance.SetLinkColor(RGB(0, 0, 255));  // 钱用蓝色表示
	m_btnAllBalance.SetVisitedColor(RGB(0, 0, 255));

	m_btnBalance.SizeToContent();
	m_btnCredit.SizeToContent();

	InitListView();
	SetEditFont();

	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	// 2015/4/22 tmelody
	// { 续包时
	GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(FALSE);
	// }
	
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	
	GetDlgItem(IDC_EDIT_CREDITMONEY)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_CREDITMONEY)->SetFont(&m_BigFont);

	// m_nBundleTimeUseBalance == 0 是禁止账户包时
	// m_nBundleTimeUseBalance == 1 是允许账户包时
	// m_nBundleTimeUseBalance == 2 是只允许临时用户包时
	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_BTNEEDMONEY); 

		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(strTmp); //应收现金(元)：或者是应扣

		m_btnCredit.EnableWindow(FALSE);
		m_btnCredit.ShowWindow(SW_HIDE);   //隐藏充值按钮

		m_btnAllBalance.ShowWindow(SW_HIDE); //隐藏 账户合计后面的金额显示

		GetDlgItem(IDC_STATIC0)->ShowWindow(SW_HIDE); //隐藏 账户合计：

		GetDlgItem(IDC_STATIC_CREDITMONEY)->ShowWindow(SW_HIDE);//隐藏 充值金额文字，在应扣账户元：后面
	}
	else
	{
		m_btnBalance.ShowWindow(SW_HIDE); //隐藏查看余额按钮

		GetDlgItem(IDC_EDIT_CREDITMONEY)->ShowWindow(SW_HIDE);//隐藏充值金额编辑框
	}

	GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(FALSE);//重复打印按钮隐藏
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE);//自定义包时
	GetDlgItem(IDC_SELFBUNDTIME)->ShowWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断

	//{ 2011/05/24-gxx: 自动查询在刷卡及扫描证件的情况下采用
	if (m_bAutoQuery)
	{
		// 2011/07/08-8201-gxx: 判断如果为本地用户不允许包时
		if (GetUserState() == CIBAGlobal::emUserStateLocal)
		{
			// 账号对应的用户为本地用户
			theApp.IBAMsgBox(IDS_DISABLEBT);
			OnCancel();
		}
		//
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

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgBundleTime::CheckPopedom(LPCTSTR lpszModuleName)
{
	BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 

	if (bRet)
	{
		bRet = theApp.GetCurCashier()->HasPopedomOf(lpszModuleName);
	}

	return bRet;
}

void CDlgBundleTime::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance);

	// 2012/02/20-8237-gxx: 将EnableWindow(TRUE)改动
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断
	
	if (m_bAfterBundleTime || Balance.GetPayType() > 0
		|| 0 != CNetBarConfig::GetInstance()->GetCannotSelfBundleTime())
	{
		// 已经包时 or 付款类型为:累计封顶、足额预扣

		GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE); // 禁用自定义包时
	}

	if (m_nMemberId != m_nLastMemberID)
	{
		// 重新读取包时列表

		GetBundleTimeList();
		m_nLastMemberID = m_nMemberId;
	}	

	GetBundleTimeAction();

	//{ 2011/05/20-gxx: 
	if (!m_strBundleTimeEndTime.IsEmpty())
	{
		COleDateTime dtEndTime;

		dtEndTime.ParseDateTime(m_strBundleTimeEndTime);
		COleDateTimeSpan dtSpan = dtEndTime - COleDateTime::GetCurrentTime();

		//{ 2011/06/09-gxx: 
		int nBTLimit = CNetBarConfig::GetInstance()->GetBTLimitTime();
		if ( nBTLimit==0 )
		{
			nBTLimit = 50;
		}
		//}

		if (dtSpan.GetTotalMinutes() < nBTLimit) //可以自定义续包时
		{
			// 2012/02/20-8237-gxx: 将EnableWindow(TRUE)改动
			GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断
		}
	}
	
	//}
	
	m_btnPrint.SetCheck( CIBAConfig::GetInstance()->GetPrintTicketBT(m_bIsMember));

	


	UpdateData(FALSE);
}

void CDlgBundleTime::GetBundleTimeList()
{
	m_BundleTimeList.DeleteAllItems();

	BundleTimeList.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBundleTimeList(BundleTimeList);

	if (BundleTimeList.GetStatusCode() == 0)
	{
		for (INT i = 0; i < (INT)BundleTimeList.GetRecordCount(); i++)
		{
			CString strTmp;

			strTmp.Format(_T("%d"), BundleTimeList.GetBundleId(i));
			m_BundleTimeList.InsertItem(i, strTmp);

			strTmp = BundleTimeList.GetStartTime(i);
			m_BundleTimeList.SetItemText(i, 1, strTmp);

			strTmp = BundleTimeList.GetEndTime(i);
			m_BundleTimeList.SetItemText(i, 2, strTmp);

			strTmp.Format(_T("%d"), BundleTimeList.GetTimePass(i));
			m_BundleTimeList.SetItemText(i, 3, strTmp);

			strTmp.Format(_T("%.2f"), BundleTimeList.GetPrice(i) / 100.0);
			m_BundleTimeList.SetItemText(i, 4, strTmp);

			strTmp = BundleTimePayType(BundleTimeList.GetBundleType(i));
			m_BundleTimeList.SetItemText(i, 5, strTmp);

			strTmp = BundleTimeList.GetDescription(i);
			m_BundleTimeList.SetItemText(i, 6, strTmp);

			m_BundleTimeList.SetItemData(i, i);
		}
	}
}

void CDlgBundleTime::GetBundleTimeAction()
{
	CBundleTimeActionMethod BundleTimeAction;
	BundleTimeAction.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBundleTimeAction(BundleTimeAction);
	
	if (BundleTimeAction.GetStatusCode() == 0 && 
		BundleTimeAction.GetNetBarId() > 0  // 0表示无包时
		)
	{
		m_strBundleTimeID.Format(_T("%d"), BundleTimeAction.GetTimeId());
		
		for (INT i = 0; i < (INT)BundleTimeList.GetRecordCount(); i++)
		{
			if (BundleTimeAction.GetTimeId() == BundleTimeList.GetBundleId(i))
			{
				m_nCurrBundleTimeIndex = i;
				break;
			}
		}
		
		m_strBundleTimeStartTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetStartTime());
		m_strBundleTimeEndTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetEndTime());
		m_strBundleTimeType = BundleTimePayType(BundleTimeAction.GetPayType());
		m_strBundleTimeMoney.Format(_T("%.2f"), BundleTimeAction.GetAmount1() / 100.0);
		m_strBundleTimeMoney2.Format(_T("%.2f"), BundleTimeAction.GetAmount2() / 100.0);
		m_nBudleTimeMoney = BundleTimeAction.GetAmount1();
		
		m_strBundleTimeArea = CNetBarConfig::GetInstance()->GetPCClassName(BundleTimeAction.GetPcClass());
		
		if (m_strBundleTimeArea.IsEmpty())
		{
			m_strBundleTimeArea = LOAD_STRING(IDS_NOLIMITAREA);
		}

		GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(TRUE); // 允许取消包时
	}
}	

CString CDlgBundleTime::BundleTimePayType(UINT nBundleType)
{	
	CIBAString strTmp;

	strTmp.LoadString(IDS_BUNDLETIMETYPE);

	if (1 == nBundleType)
	{
		strTmp = strTmp.Left(strTmp.Find('|'));
	}
	else
	{
		strTmp = strTmp.Mid(strTmp.Find('|') + 1);
	}

	return strTmp;
}


void CDlgBundleTime::InitListView()
{
	//L"编号|开始时间|结束时间|时长|金额|包时类型|其他信息"
	
	CIBAString strHeader;
	strHeader.LoadString(IDS_BUNDLETIMELISTHEADER);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT nWidth[] = {40, 70, 70, 40, 50, 70, 220};

	const UINT nFormat[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_LEFT};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_BundleTimeList.InsertColumn(i, strArray.GetAt(i), nFormat[i], nWidth[i]);
	}

	m_BundleTimeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//2015-0825 liyajun 使列表可排序，以及设置状态
	//m_BundleTimeList.ModifyStyle(NULL,LVS_REPORT,0);
	//m_BundleTimeList.EnableMultipleSort(TRUE);
	//for(int i=0; i < strArray.GetCount(); ++i)
	//{
	//	m_BundleTimeList.SetSortColumn(i,TRUE,TRUE);
	//}
	
}

void CDlgBundleTime::CheckBundleTimeCondition(INT nBtIndex, BOOL& bNowCanBT, BOOL& bNowCanForwardBT, BOOL& bNowChangeBT, BOOL& bNowCanContinueBT)
{
	CString strNow(COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));//当前时间
	CString strStart(BundleTimeList.GetStartTime(nBtIndex));//对应ID包时策略的开始时间
	CString strEnd(BundleTimeList.GetEndTime(nBtIndex));//对应ID包时策略的结束时间

	if (strStart.IsEmpty())//没有开始时间的 - 可以包时
	{
		bNowCanBT = TRUE;
	}
	else
	{
		if (strNow.Compare(strStart) >= 0) //now >= start
		{
			if (strNow.Compare(strEnd) < 0) //now < end，可以包时
			{
				bNowCanBT = TRUE;
			}
			else if (strNow.Compare(_T("23:59:59")) <= 0 && strStart.Compare(strEnd) > 0) //now<24:00:00 且 now>end，可以包时？？？那不是都可以？
			{	
				bNowCanBT = TRUE;
			}
		}
		else//now < start
		{
			if (strNow.Compare(strEnd) < 0 && strStart.Compare(strEnd) > 0)//跨天才可以在包时开始前进行包时
			{
				bNowCanBT = TRUE;
			}
		}
	}

	//没有包时，现在不允许包时的，则可以提前包时
	bNowCanForwardBT = (!bNowCanBT && m_strBundleTimeID.IsEmpty());

	if (bNowCanBT) //可以包时
	{
		if (!m_strBundleTimeID.IsEmpty()) //已经包时了
		{
			COleDateTime dtEndTime;
			dtEndTime.ParseDateTime(m_strBundleTimeEndTime);
			COleDateTimeSpan dtSpan = dtEndTime - COleDateTime::GetCurrentTime();//离包时结束时间

			//{ 2011/06/09-gxx: 
			int nBTLimit = CNetBarConfig::GetInstance()->GetBTLimitTime();//包时的限制时间,即"离包时结束XXX分钟之内不允许包时"
			if ( nBTLimit==0 )
			{
				nBTLimit = 50;
			}
			//}
			
			if (dtSpan.GetTotalMinutes() <= nBTLimit) //离本次包时结束时间<=设定的值，不可以包时
			{
				bNowCanBT = FALSE; 
			}
			else//离本次包时结束时间>设定的值，才可以转包时
			{
				INT nCurrBtId = _ttoi(m_strBundleTimeID);

				//判断可不可以转包时
				if (m_nSelBundleTimeId != nCurrBtId  //不等于当前的包时ID，即相同的包时
					&& !strStart.IsEmpty()           //包时策略开始时间不为空
					&& strStart == BundleTimeList.GetStartTime(m_nCurrBundleTimeIndex)  //校验包时开始时间
					&& strEnd == BundleTimeList.GetEndTime(m_nCurrBundleTimeIndex)      //校验包时结束时间
					&& m_nSelBundleTimePrice >= BundleTimeList.GetPrice(m_nCurrBundleTimeIndex)  //校验价格 为何要>?
					) 
				{
					bNowChangeBT = TRUE;

					bNowCanBT = FALSE; //可以转包时时,包时按钮就置灰

					m_nSelBundleTimePrice -= BundleTimeList.GetPrice(m_nCurrBundleTimeIndex); //转包时补差价
				}
			}


			// 2015-4-22 tmelody
			// { 先前已经包了这种无时间限制的时，现在继续包
			if (strStart.IsEmpty() && strEnd.IsEmpty())
			{
				INT nCurrBtId = _ttoi(m_strBundleTimeID);
				if (BundleTimeList.GetBundleIdTime(nCurrBtId, strStart, strEnd))
				{
					if (strStart.IsEmpty() && strEnd.IsEmpty())
					{
						bNowCanBT = FALSE;
						bNowCanContinueBT = TRUE;
					}
				}
			}
			// }
		}
	}
}

void CDlgBundleTime::OnNMClickListBundletime(NMHDR *pNMHDR, LRESULT *pResult)
{
	//当前选中项目
	INT nIndex = m_BundleTimeList.GetSelectionMark();

	if (nIndex >= 0)
	{
		INT nBtIndex(0);//包时策略在BunTimeList中的Index

		//获取当前选中Item的第一项，i.e.BundleTime的Id.再从BundleTimeMethod中获取对应的Index。
		CString strIndex = m_BundleTimeList.GetItemText(nIndex,0);
		TCHAR* lpIndex = strIndex.GetBuffer();
		INT nItemData(0);//即BundleTimeID
		nItemData = _ttoi(lpIndex);
		strIndex.ReleaseBuffer();

		//找到BundleTimeList中BundletimeID与nItemData匹配的对象
		for(int i=0;i<BundleTimeList.GetRecordCount();++i) //获取BundleTimeList中对应的nBtIndex
		{
			if(BundleTimeList.GetBundleId(i) == nItemData)//获取Array中第i个BundleTimeInfo的BundleId
			{
				nBtIndex = i;
				break;
			}
		}

		//根据BundleTimeList的Index，查找对应策略的信息
		m_nSelBundleTimeId = BundleTimeList.GetBundleId(nBtIndex);//获取ID,其实 = nItemData
		m_nSelBundleTimePrice = BundleTimeList.GetPrice(nBtIndex);//获取Price

		BOOL bNowCanBT = FALSE; 
		BOOL bNowCanForwardBT = FALSE;
		BOOL bNowChangeBT = FALSE;
		BOOL bNowCanContinueBT = FALSE;
		
		CheckBundleTimeCondition(nBtIndex, bNowCanBT, bNowCanForwardBT, bNowChangeBT, bNowCanContinueBT);

		GetDlgItem(IDOK)->EnableWindow(bNowCanBT);
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(bNowCanForwardBT);
		GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(bNowChangeBT);
		// 2015/4/22 tmelody
		GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(bNowCanContinueBT);

		CString strTmp;
		strTmp.Format(_T("%.2f"), m_nSelBundleTimePrice / 100.0);
		GetDlgItem(IDC_STATIC_CREDITMONEY)->SetWindowText(strTmp);
		GetDlgItem(IDC_EDIT_CREDITMONEY)->SetWindowText(strTmp);
	}

	*pResult = 0;
}

void CDlgBundleTime::OnBnClickedButtonCancelbt()
{
	CString strTmp;
	strTmp.Format(IDS_CANCELBUNDLETIME, m_strName);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	CWaitCursor Wait;

	CCancelBundleTimeMethod CancelBundleTime;

	CancelBundleTime.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);

	CString strRet;

	if (CancelBundleTime.GetStatusCode() == 0)
	{
		OnBnClickedButtonQuery();

		strRet = LOAD_STRING(IDS_BTCANCELOK);

		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			strRet = LOAD_STRING(IDS_BTCANCELOK2);
		}

		m_bAfterBundleTime = FALSE;

		m_nBudleTimeMoney = 0;

		/*CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember))
		{
			
			CLocalServer::GetInstance()->ActiveMemberList.CheckActiveMember(m_nCardId, m_nMemberId, ActiveMember);
		}*/
	}
	else
	{
		strRet = CancelBundleTime.GetStatusMessage();
	}

	SetToolTipPos(IDC_BUTTON_CANCELBT);
	ShowToolTip(strRet, CancelBundleTime.GetStatusCode() != 0);
}

void CDlgBundleTime::ClearData()
{
	m_strName.Empty();
	m_strUserClass.Empty();
	m_strPersonalID.Empty();
	m_strBundleTimeID.Empty();
	m_strBundleTimeStartTime.Empty();
	m_strBundleTimeEndTime.Empty();
	m_strBundleTimeType.Empty();
	m_strBundleTimeMoney.Empty();
	m_strBundleTimeMoney2.Empty();
	m_strBundleTimeArea.Empty();
	m_nSelBundleTimeId = 0;

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(FALSE);

	//查询，重新清标志
	m_bAfterBundleTime = FALSE;
	GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_CREDITMONEY)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_CREDITMONEY)->SetWindowText(_T(""));
}

void CDlgBundleTime::OnBnClickedButtonForwardbt()
{
	//2015-0827 liyajun 增加提前包时功能
	int nForwardBundleMoney = 0;
	CDlgChoseForwardBundle dlg;

	dlg.SetForwarBundleInfo(_tstof(m_strAllBalance), m_nUserClassID, m_nSelBundleTimeId, m_bIsMember);

	if(IDOK != dlg.DoModal())
	{
		return ;
	}
	else
	{
		nForwardBundleMoney = dlg.GetForwardBundleMoney();

		// 如果WK设置了"禁止账户包时"，那么要充值的钱应该是，应收现金 - 包时所需金额
		// 因为禁止账户包时时，这里充值完应收现金，后面还会再充包时所需金额
		// 为什么不直接将后面的充值去掉，因为这样可以不改变现有逻辑
		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			nForwardBundleMoney -= HELPFUNC::GetBundleTimeMoney(m_nUserClassID, m_nSelBundleTimeId);
		}
			// 询问是否要包时
	//----------------------------------------------------------------------------------------------//

		CString strTmp;

		strTmp.Format(IDS_ASKBUNDLETIME);

		if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
		{
			return;

	//2015-0827 liyajun 增加提前包时功能
		}
		else
		{
			if(nForwardBundleMoney > 0)
			{
				m_strMoneyCredit.Format(_T("%d"), nForwardBundleMoney / 100);
				if(!DoCredit())
				{
					if(IDYES != AfxMessageBox(_T("充值失败!是否继续？"), MB_YESNO))
					{
						return;
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------------------------------//

	if (!m_strBundleTimeID.IsEmpty())
	{
		CCancelBundleTimeMethod CancelBundleTime;
		CancelBundleTime.SetMemberId(m_nMemberId);
		theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);
	}

	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);

	if (!DoBundleTime(1)) 
	{
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(TRUE);
	}
	else//提前包时成功了，则包时和提前包时按钮置灰
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	}


	//GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(TRUE);
}

void CDlgBundleTime::OnBnClickedButtonChangebt()
{
	CString strTmp;

	strTmp.Format(IDS_ASKBUNDLETIME);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);

	if (!DoBundleTime(2)) //转包时失败
	{
		GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(TRUE);
	}
	else//转包时成功，则包时和转包时按钮置灰
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(TRUE);
}

// 点击包时
void CDlgBundleTime::OnBnClickedOk()
{
	CString strTmp;

	if (m_strBundleTimeID.IsEmpty())
	{
		strTmp.Format(IDS_ASKBUNDLETIME);
	}
	else
	{
		strTmp.Format(IDS_ASKBUNDLETIMEAGEIN);
	}

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	if (!m_strBundleTimeID.IsEmpty())
	{
		CCancelBundleTimeMethod CancelBundleTime;
		CancelBundleTime.SetMemberId(m_nMemberId);
		theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	if (!DoBundleTime(0)) //包时失败
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else//包时成功后，包时和提前包时按钮置灰
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	}
	
}

// 2015-4-22 tmelody
// { 点击续包时
void CDlgBundleTime::OnBnClickedButtonContinuebt()
{
	CString strMsg;
	strMsg.Format(IDS_ASKBUNDLETIMEAGEIN);

	if (IDOK != MsgBox(strMsg, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(FALSE);

	if (!DoBundleTime(4))
	{
		// 失败
		GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(TRUE);
	}
	else//续包时成功，则不能再包时，提前包时以及再续包时
	{
		GetDlgItem(IDC_BUTTON_CONTINUEBT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}
// }

// 0:普通包时   
// 1:预包时
// 2:转包时
// 3:自定义包时
// 4:续包时
BOOL CDlgBundleTime::DoBundleTime(UINT nType)
{
	CWaitCursor Wait;

	CBundleTimeMethod BundleTime;

	BundleTime.SetMemberId(m_nMemberId);
	BundleTime.SetReserve(nType);
	BundleTime.SetTimeId(m_nSelBundleTimeId);

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		BundleTime.SetCreditAmount(m_nSelBundleTimePrice);
	}

	theApp.GetCurCashier()->DoBundleTime(BundleTime);

	if (BundleTime.GetStatusCode() >= 0 && BundleTime.GetNeedRelogin())
	{
		//在线包时,需要通知客户端重新登录

		CActiveMember ActiveMember;

		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember)) 
		{
			CActiveMemberList::TReLoginRequest MyReLoginRequest = {0};
			MyReLoginRequest.lastRefNo = ActiveMember.GetRefNo();
			MyReLoginRequest.memberId = m_nMemberId;
			MyReLoginRequest.firstTryTime = GetTickCount();

			CLocalServer::GetInstance()->ActiveMemberList.AddReLoginRequest(MyReLoginRequest);
			CLocalServer::GetInstance()->LocalBundleTime(m_nMemberId);
		}
	}

	CString strRet;

	if (BundleTime.GetStatusCode() == 0)
	{
		// 2011/07/29-8201-gxx: 更新现金包时时，预付款和明细的刷新
		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(m_nMemberId, m_nSelBundleTimePrice, 0, 0);
		}

		if (nType == 2)
		{
			CIBATTS::GetInstance()->SpeakBundleTimePatch(BundleTime.GetAmount());
		}
		else
		{
			CIBATTS::GetInstance()->SpeakBundleTimeResult(BundleTime.GetAmount());
		}
		

		ReQuery();

		strRet = LOAD_STRING(IDS_BTSUCCESSED);

		if (m_btnPrint.GetCheck() == BST_CHECKED)
		{
			PrintTicket();
		}

		m_bAfterBundleTime = TRUE;//包时成功标志置位
		OnBnClickedCheckPrint();//重复打印按钮显示

		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(7);
		OperationInfo.SetUserName(m_strName);
		OperationInfo.SetNetId(m_strNetId);
		OperationInfo.SetOperationAmount(BundleTime.GetAmount());
		OperationInfo.SetDescription(
			CIBAHelpper::CenterTimeToOleDateTime(BundleTime.GetStartTime()).Format(_T("%H:%M"))
			+ _T(" - ") +  
			CIBAHelpper::CenterTimeToOleDateTime(BundleTime.GetEndTime()).Format(_T("%H:%M")));

		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		/*UINT nAmount = BundleTime.GetAmount();
		CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember)) 
		{
			nAmount = ActiveMember.GetAmount() + nAmount;
			ActiveMember.SetAmount( nAmount );
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember,TRUE);
		}*/
	}
	else
	{
		m_bAfterBundleTime = FALSE;
		strRet = BundleTime.GetStatusMessage();
	}

	if (nType == 0)
	{
		SetToolTipPos(IDOK);
	}
	else if (nType == 1)
	{
		SetToolTipPos(IDC_BUTTON_FORWARDBT);
	}
	else if (nType == 2)
	{
		SetToolTipPos(IDC_BUTTON_CHANGEBT);
	}
	else if (nType == 4)
	{
		SetToolTipPos(IDC_BUTTON_CONTINUEBT);
	}

	ShowToolTip(strRet, !m_bAfterBundleTime);
	
	if (BundleTime.GetStatusCode() > 0 && BundleTime.GetNeedRelogin())
	{
		CIBAHelpper::WaitFor(3000); //等待3秒
	}

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	return m_bAfterBundleTime;
}

void CDlgBundleTime::PrintTicket()
{	
	CString strTicket;

	CIBAPrint::GetBundleTimeTicket(strTicket);

	strTicket.Replace(_T("[name]"), m_strName);
	strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
	strTicket.Replace(_T("[memberclass]"), m_strUserClass);
	strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
	strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
	strTicket.Replace(_T("[pcclass]"), m_strBundleTimeArea);
	strTicket.Replace(_T("[money]"), m_strBundleTimeMoney);
	strTicket.Replace(_T("[starttime]"), m_strBundleTimeStartTime.Mid(11, 5));
	strTicket.Replace(_T("[endtime]"), m_strBundleTimeEndTime.Mid(11, 5));
	strTicket.Replace(_T("[totalmoney]"), m_strAllBalance);
	
	CIBAPrint::Print(strTicket);
}


void CDlgBundleTime::OnBnClickedButtonReprint()
{
	PrintTicket();
}

void CDlgBundleTime::OnBnClickedCheckPrint()
{
	UpdateData(TRUE);

	if (m_btnPrint.GetCheck() == BST_CHECKED && m_bAfterBundleTime)
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_NORMAL);
	} 
	else
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);
	}
}

void CDlgBundleTime::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TimeOfOnlineBundTime)
	{	
		m_bAutoQuery = FALSE;

		KillTimer(TimeOfOnlineBundTime);
	}
	else if (nIDEvent == TimeIdOfAutoQuery)
	{
		KillTimer(TimeIdOfAutoQuery);//只查询一次
		OnBnClickedButtonQuery();
	}

	CNDDialog::OnTimer(nIDEvent);
}

void CDlgBundleTime::OnBnClickedSelfbundtime()
{
	CDlgBundTimeSelfDef SelfDefineBundTimeDlg;
	
	SelfDefineBundTimeDlg.SetMemberId(m_nMemberId);
	SelfDefineBundTimeDlg.SetIsMember(m_bIsMember);

	if (SelfDefineBundTimeDlg.DoModal() == IDOK)
	{
		ReQuery();
	}
}

HBRUSH CDlgBundleTime::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_CREDITMONEY)
		{
			pDC->SetTextColor(RGB(255, 0, 0));		
		}
	}
	else
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_CREDITMONEY)
		{
			pDC->SetTextColor(RGB(0, 0, 255));		
		}
	}

	return hbr;
}

void CDlgBundleTime::OnStnClickedStaticAll()
{
	if (m_nMemberId > 0)
	{
		CString strTmp;
		
		strTmp.Format(IDS_DETAILBALANCE, 
			m_nMoneyBalanceW / 100.0, m_nMoneyBalanceY / 100.0, 
			m_nMoneyBalanceZ / 100.0, m_nMoneyOnlineAmount / 100.0,
			(m_nMoneyBalanceW + m_nMoneyBalanceY  + m_nMoneyBalanceZ + m_nMoneyOnlineAmount) / 100.0);


		SetToolTipPos(IDC_STATIC_ALL);

		ShowToolTip(strTmp);
	}
}

void CDlgBundleTime::OnBnClickedButtonCredit()
{
	if (0 == m_nMemberId) //没有用户
	{
		return;
	}


	CNDCreditDlg dlg;
	
	dlg.SetLinkEnabled(FALSE);
	dlg.SetNetId(m_strNetId);

	if (IDOK == dlg.DoModal())
	{
		ReQuery();

		m_BundleTimeList.SetFocus();
	}
}

BOOL CDlgBundleTime::PreTranslateMessage(MSG* pMsg)
{
	//if (WM_KEYDOWN == pMsg->message)
	//{
	//	if (VK_F3 == pMsg->wParam && m_btnCredit.IsWindowVisible())
	//	{
	//		OnBnClickedButtonCredit();
	//		return TRUE;
	//	}	
	//}

	//if (LVN_COLUMNCLICK == pMsg->message)
	//{
	//	OnColumnListClicked((NMHDR*)pMsg->lParam, NULL);
	//}
	return CNDDialog::PreTranslateMessage(pMsg);
}

void CDlgBundleTime::OnBnClickedButtonBalance()
{
	if (m_nMemberId > 0)
	{
		CString strTmp;

		strTmp.Format(IDS_DETAILBALANCE, 
			m_nMoneyBalanceW / 100.0, m_nMoneyBalanceY / 100.0, 
			m_nMoneyBalanceZ / 100.0, m_nMoneyOnlineAmount / 100.0,
			(m_nMoneyBalanceW + m_nMoneyBalanceY  + m_nMoneyBalanceZ + m_nMoneyOnlineAmount) / 100.0);


		SetToolTipPos(IDC_BUTTON_BALANCE);

		ShowToolTip(strTmp);
	}
}

//2015-0723 liyajun 足额包时控制-会员（应收账户(元)：）非会员（应扣账户(元)：）
void CDlgBundleTime::OnBnClickedButtonQuery()
{
	// 2016-3-17 - qsc
	CNDDialog::OnBnClickedButtonQuery();
	BOOL bBundleTimeWithCash = CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember);

	// 禁止账户包时
	if(bBundleTimeWithCash)
	{
		CString sShow(LOAD_STRING(IDS_BTNEEDMONEY));
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(sShow);
	}
	else
	{
		CString sShow(LOAD_STRING(IDS_BUNDLETIME_DEDUCTION));
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(sShow);
	}
}

//2015-0825 liyajun 响应列表框排序
void CDlgBundleTime::OnColumnListClicked(NMHDR* pNMhdr, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMhdr;
	//如果当前选中行为之前选中的行，说明现在要反向排序，将顺序颠倒。否则就按升序排列。
	if(pNMListView->iSubItem == m_BundleTimeList.m_nSortedCol)
	{
		m_BundleTimeList.m_bAsc = !m_BundleTimeList.m_bAsc;
	}
	else
	{
		m_BundleTimeList.m_bAsc = TRUE;
		m_BundleTimeList.m_nSortedCol = pNMListView->iSubItem;
	}

	if(!m_BundleTimeList.GetItemCount()) return;  //列表中没有元素，直接返回

	//排序前可能Item没有Data,要设置一下ItemData
	for(int i=0 ; i<m_BundleTimeList.GetItemCount();++i)
	{
		m_BundleTimeList.SetItemData(i,i);
	}

	m_BundleTimeList.SortItems((PFNLVCOMPARE)CSortListCtrl::ListCompare,(DWORD_PTR)&m_BundleTimeList); 

	//排序完之后再重新设置ItemData
	for(int i=0 ; i<m_BundleTimeList.GetItemCount();++i)
	{
		m_BundleTimeList.SetItemData(i,i);
	}

}

//2015-0827 liyajun 增加提前包时功能
BOOL CDlgBundleTime::DoCredit()
{
	CWaitCursor WaitCursor;

	if (!CheckInput()) return FALSE;//检验金钱输入

	UINT nMoney = _tstof(m_strMoneyCredit) * 100;

	// 不询问 if (!MakeSureCredit(nMoney)) return FALSE;//询问是否要充值

	//if (m_bNDHasLocalQuery && m_LocalActiveMember.GetIsLocalUser())
	//{
	//	return DoLocalModeCredit(nMoney);
	//}

	///////////////////////账号充值/////////////////////
	CCreditMethod Credit;

	Credit.SetMemberId(m_nMemberId);
	Credit.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
	Credit.SetActivityId(0);
	Credit.SetNoPresent(TRUE);	// 默认为不赠送
	//Credit.SetNoPresent(m_bNoPresent);
	Credit.SetAmount(nMoney);

	theApp.GetCurCashier()->DoCredit(Credit);
	///////////////////////充值成功后设置各种状态///////////////////////
	if (Credit.GetStatusCode() == 0)
	{	
		//保存先前的充值金额
		//m_nCreditMoney = _tstof(m_strCreditMoney) * 100.0;

		//赠送金额
		/*if (m_bNoPresent)
		{
			m_nPresentMoney = 0;
		} 
		else
		{
			m_nPresentMoney = Credit.GetCreditPresent();
		} */   

		if (m_bOnLine) //发送充值消息
		{			
			CString strTmp;
			strTmp.Format(IDS_ONLINECREDIT, nMoney / 100.0, Credit.GetCreditPresent() / 100.0);
			CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(m_nMemberId, strTmp);
		}

		{//写到充值信息日志记录里
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(1);
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(m_strName);
			OperationInfo.SetNetId(m_strNetId);
			OperationInfo.SetMemberId(m_nMemberId);
			OperationInfo.SetOperationAmount(nMoney);
			//OperationInfo.SetDeposit(_ttoi(m_strDeposit) * 100);
			// OperationInfo.SetPassWord("");
			OperationInfo.SetPresentAmount(Credit.GetCreditPresent());
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));;
		}

		{//写入数据库

			CCreditInfoDB CreditInfo;

			CreditInfo.SetCardSerial(m_strNetId);
			CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(Credit.GetTrantime()));
			CreditInfo.SetRefNo(Credit.GetRefNo());
			CreditInfo.SetCreditPresent(Credit.GetCreditPresent());
			CreditInfo.SetCreditAmount(nMoney);
			CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
			CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			CreditInfo.SetMemberId(m_nMemberId);
			CreditInfo.SetSerialNo(Credit.GetLocalSerialNo());
			CreditInfo.SetCassId(m_nUserClassID);

			CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);
		}

		CString strResult;
		strResult.Format(IDS_CREDITRESULT, nMoney / 100.0, Credit.GetCreditPresent() / 100.0);
		SetToolTipPos(IDC_EDIT_CREDIT);
		ShowToolTip(strResult);

		////充值成功后查询数据
		//if (m_bAlreadyBundTime || m_nBundleTimeId == 0 || m_bOnLine)//已经包时或(没有包时但没选择包时信息）
		//{
		//	//发声
		//	CIBATTS::GetInstance()->SpeakCreditResult(nMoney, Credit.GetCreditPresent(), Credit.GetAvailableBalance());

		//	CheckCard(m_strNetId);

		//}

		return TRUE;
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(Credit.GetStatusMessage(), TRUE);

		return FALSE;
	}
}

BOOL CDlgBundleTime::CheckInput()
{
	//验证金钱输入
	UpdateData();

	INT nMoney = _tstof(m_strMoneyCredit) * 100;

	if (nMoney <= 0)
	{
		//SetToolTipPos(IDC_EDIT_MONEYTOFORWORDBUNDLE);
		//ShowToolTip(IDS_INPUTERROR);
		//GetDlgItem(IDC_EDIT_MONEYTOFORWORDBUNDLE)->SetFocus();
		return FALSE;
	}

	//// 2012/03/05-8238-gxx: 
	//if (CIBAConfig::GetInstance()->GetMaxCreditMoney() != 0)
	//{
	//	INT nMax = CIBAConfig::GetInstance()->GetMaxCreditMoney()*100;
	//	if (nMoney > nMax)
	//	{
	//		ShowToolTip(IDS_OVERFLOW_MAXMONEY, IDC_EDIT_MONEYTOBUNDLE, TRUE);
	//		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_MONEYTOBUNDLE);
	//		if (pEdit)
	//		{
	//			pEdit->SetFocus();
	//			pEdit->SetSel(0, m_strMoneyCredit.GetLength());
	//		}

	//		return FALSE;
	//	}
	//}
	//// --end

	return TRUE;
}

BOOL CDlgBundleTime::DoLocalModeCredit(UINT nCreditMoney)
{
	CString strErrMsg;
	strErrMsg = CCashierHelper::DoCredit_InLocal(m_ActiveMember, nCreditMoney);
	BOOL bOK = strErrMsg.IsEmpty()?TRUE:FALSE;

	if (bOK)
	{
		//重复打印按钮显隐
		/*if (m_bPrint)
		{
			GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_NORMAL);
		}*/  
		
		CString strResult;
		strResult.Format(IDS_CREDITRESULT, nCreditMoney / 100.0, 0 / 100.0);
		SetToolTipPos(IDC_EDIT_CREDIT);
		ShowToolTip(strResult);

		////打印小票
		//if (m_bPrint)
		//{
		//	CString strTmp;
		//	strTmp.Format(_T("%.2f"), nCreditMoney / 100.0);
		//	PrintTicket(strTmp);
		//}

		//清空充值的编辑框
		m_strMoneyCredit.Empty();
		/*m_strCreditMoney.Empty();
		m_edtCreditMoney.SetWindowText(_T(""));
		m_edtCreditMoney.SetFocus();*/
		return TRUE;
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(LOAD_STRING(IDS_LOCAL_CREDIT_FAULT), TRUE);  // 本地充值失败

		return FALSE;
	}

}
