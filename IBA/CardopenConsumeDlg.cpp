// CardopenConsumeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "Dal\DALUSER.h"
#include "Dal\IBADAL.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include ".\cardopenconsumedlg.h"
#include "Socket\LocalServer.h"

// CCardopenConsumeView 对话框

IMPLEMENT_DYNCREATE(CCardopenConsumeView, CIBAFormView)

CCardopenConsumeView::CCardopenConsumeView()
: CIBAFormView(CCardopenConsumeView::IDD)
{
}

CCardopenConsumeView::~CCardopenConsumeView()
{
}

void CCardopenConsumeView::DoDataExchange(CDataExchange* pDX)
{
	CIBAFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_BUTTON_ENDTIME, m_EndTime);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_STATICResult);
	DDX_Control(pDX, IDC_LIST_INFO, m_InfoList);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strNoConsumeResult);
}

BEGIN_MESSAGE_MAP(CCardopenConsumeView, CIBAFormView)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_WM_CTLCOLOR()
	//ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO, OnNMDblclkListInfo)
END_MESSAGE_MAP()


// CCardopenConsumeView 消息处理程序

void CCardopenConsumeView::OnBnClickedButtonQuery()
{
	m_InfoList.DeleteAllItems();
	m_strNoConsumeResult.Empty();

	FillList();

	UpdateData(FALSE);

	RedrawWindow();
}

HBRUSH CCardopenConsumeView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBAFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}

	return hbr;
}

// 初始化时间控件
void CCardopenConsumeView::InitTimeCtrl(void)
{
	//时间按钮控件的设置
	const UINT stateMask = 
		CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR | 
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_CHECKBOX |
		CBCGPDateTimeCtrl::DTM_TIME | 
		CBCGPDateTimeCtrl::DTM_TIME24HBYLOCALE;

	UINT stateFlags = 
		CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR | 
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_TIME;  

	m_StartTime.m_monthFormat = 2;
	m_StartTime.SetState(stateFlags, stateMask);
	m_StartTime.SetFont(&m_Font);
	m_StartTime.SizeToContent();

	m_EndTime.m_monthFormat = 2;
	m_EndTime.SetState(stateFlags, stateMask);
	m_EndTime.SetFont(&m_Font);
	m_EndTime.SizeToContent();
}

// 设置时间控件的数据
void CCardopenConsumeView::InitDate(void)
{
	COleDateTime tdate;
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		//一天前
		tdate = COleDateTime::GetCurrentTime();
		tdate -= COleDateTimeSpan(1, 0, 0, 0);
	}
	m_StartTime.SetDate(tdate);

	tdate = COleDateTime::GetCurrentTime();
	tdate += COleDateTimeSpan(1, 0, 0, 0);
	m_EndTime.SetDate(tdate);
}

// 初始化列表控件
void CCardopenConsumeView::InitList(void)
{
	UpdateData(FALSE);
	m_InfoList.DeleteAllItems();

	//取得列表头的子项数目
	INT nCount = m_InfoList.GetHeaderCtrl().GetItemCount();
	//清空数据
	for (INT i = 0; i < nCount; i++)
	{
		m_InfoList.DeleteColumn(0);
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_CARDOPENCONSUMEHEAD);

	//strHeader = L"上网账号|姓名|证件号码|开户时间|备注";

	if (m_InfoList.GetHeaderCtrl().GetItemCount() <= 0)
	{
		CStringArray strArray;
		CIBAHelpper::SplitLine(strHeader, strArray);

		const UINT Width[] = {180, 180, 180, 180,180};

		for (INT i = 0; i < strArray.GetCount(); i++)
		{
			m_InfoList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
		}
	}

	m_InfoList.SetFont(&m_Font);//子项字体

	m_InfoList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//子项样式
}

// 查询同时为列表赋值
void CCardopenConsumeView::FillList(void)
{
	//{ 2011/12/28-8236-gxx: 

	CActiveMemberList& AList = CLocalServer::GetInstance()->ActiveMemberList;
	
	CSingleLock lock(&(AList.ActiveMemberListLock), TRUE);

	CActiveMember ActiveMember;
	AList.StartEnumActiveMember();
	while(AList.GetEachActiveMember(ActiveMember))
	{
		UINT nMemberId = 0;

		if (ActiveMember.IsMember())
		{
			continue;
		}

		if (CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()))
		{
			continue;
		}

		m_InfoList.InsertItem(0,ActiveMember.GetNetId());

		m_InfoList.SetItemText(0,1,ActiveMember.GetUserName(TRUE));

		m_InfoList.SetItemText(0,2,ActiveMember.GetPersonalID());

		CString strTmp;
		if (CIBAHelpper::IsValidTime(ActiveMember.GetActivationTime()))
		{
			strTmp = ActiveMember.GetActivationTime().Format(_T("%H:%M"));
		}
		m_InfoList.SetItemText(0,3,strTmp);

		// 备注
		CIBAString strRemark;
		if (!ActiveMember.GetIsLocalUser())
		{
			CIBAString strRegisterBT;
			strRegisterBT.LoadString(IDS_BT_NOT_PC); // 包时未上机
			strRemark = ActiveMember.GetRemark();

			if (ActiveMember.GetIsOnLine())
			{
				if (strRemark == strRegisterBT)
				{
					strRemark = _T("");
				}
			}
			else
			{
				if (ActiveMember.GetIsRegisterBT()&&(strRemark==strRegisterBT||strRemark.IsEmpty()))
				{
					strRemark = strRegisterBT;
				}
			}
		}
		else
		{
			strRemark.LoadString(IDS_LOCALUSER); // 本地用户
		}
		m_InfoList.SetItemText(0,4,strRemark);
	}

	m_strNoConsumeResult.Format(LOAD_STRING(IDS_NOCONSUME), m_InfoList.GetItemCount());

	//}

	//QueryRegister();

	//int nNoConsume = m_InfoList.GetItemCount();
 //	for(int i = 0; i < m_InfoList.GetItemCount(); i++)
 //	{
	//	CString strMemberId = m_InfoList.GetItemText(i, 4), strCardOpenTime;
	//	strCardOpenTime = m_InfoList.GetItemText(i, 5);
	//	strCardOpenTime.Trim();

	//	BOOL bIsConsume = QueryHistotyConsume(strMemberId, strCardOpenTime) || QueryActiveConsume(strMemberId);

	//	m_InfoList.SetItemText(i, 2, GetCardStateAsString(bIsConsume));
	//	nNoConsume -= (int)bIsConsume;
	//	// 删除已经消费的记录
	//	if (bIsConsume)
	//	{
	//		m_InfoList.DeleteItem(i);
	//		i--;
	//	}
	//} 
	//
	//m_strNoConsumeResult.Format(LOAD_STRING(IDS_NOCONSUME), nNoConsume);
}


// 用户开户查询
void CCardopenConsumeView::QueryRegister(void)
{
	CArray<NS_DAL::CCardOpenInfo, NS_DAL::CCardOpenInfo &>  CardOpenDBArray;

	CString strSQL;

	strSQL.Append(_T("SELECT cardopen.*, member.classId, member.memberId, member.name, member.serialNum, member.idNumber  "));

	strSQL.Append(_T("FROM cardopen, member "));

	strSQL.Append(_T("WHERE  cardopen.memberId = member.memberId "));

	strSQL.Append(_T("AND  cardopen.classId = member.classId "));

	strSQL.AppendFormat(_T("AND  member.classId in %s "), GetCommonClassIdAsString());

	strSQL.AppendFormat(_T("AND cardopen.operationDateTime  BETWEEN '%s' AND '%s'")
		, m_StartTime.ToString(), m_EndTime.ToString());

	strSQL.Append(_T(" ORDER BY  cardopen.operationDateTime DESC"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("QueryRegister:%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		INT i = 0;

		while (!Rs.IsEof())
		{
			INT nTmp = 0;
			CString strTmp;
			CCardOpenInfo CardOpenInfo;

			Rs.GetFieldValue(_T("serialNum"), strTmp);
			CardOpenInfo.SetSerialNum(strTmp);

			Rs.GetFieldValue(_T("name"), strTmp);
			CardOpenInfo.SetUserName(strTmp);

			Rs.GetFieldValue(_T("idNumber"), strTmp);
			CardOpenInfo.SetIdNumber(strTmp);

			Rs.GetFieldValue(_T("memberId"), nTmp);
			CardOpenInfo.SetMemberId(nTmp);

			Rs.GetFieldValue(_T("operationDateTime"), strTmp);
			CardOpenInfo.SetOperationDateTime(strTmp);

			Rs.GetFieldValue(_T("operator"), strTmp);
			CardOpenInfo.SetOperator(strTmp);

			i++;
			CardOpenDBArray.Add(CardOpenInfo);
			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	for (INT ii = 0; ii < CardOpenDBArray.GetCount(); ii++)
	{
#ifdef _DEBUG
		if (CardOpenDBArray.GetAt(ii).GetMemberId() == 178547)
		{
			int kkk = 0;
		}
#endif

		CString strSQL;

		strSQL.Append(_T("SELECT returned.* "));

		strSQL.Append(_T("FROM returned "));

		strSQL.AppendFormat(_T("WHERE  returned.memberId = '%d' AND  returned.classId in %s  ")
			, CardOpenDBArray.GetAt(ii).GetMemberId(), GetCommonClassIdAsString());

		strSQL.AppendFormat(_T("AND returned.returnDate >=  '%s'")
			, CardOpenDBArray.GetAt(ii).GetOperationDateTime());

		strSQL.Append(_T(" ORDER BY returned.returnDate DESC"));

		CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("QueryReturned:%s"), strSQL);

		CADODBConnInfo* pDb = NULL;

		if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return;

		CADORecordset Rs(pDb->GetConn());

		if (Rs.Open(strSQL))
		{
			if (!Rs.IsEof())
			{
				CardOpenDBArray.RemoveAt(ii);
				ii--;
			}
			Rs.Close();
		}

		pDb->Release();
	}

	for (INT jj = 0; jj < CardOpenDBArray.GetCount(); jj++)
	{
		INT nTmp = 0;
		CString strTmp;

		m_InfoList.InsertItem(jj, CardOpenDBArray.GetAt(jj).GetSerialNum());

		m_InfoList.SetItemText(jj, 1, CardOpenDBArray.GetAt(jj).GetUserName());

		m_InfoList.SetItemText(jj, 3, CardOpenDBArray.GetAt(jj).GetIdNumber());

		strTmp.Format(_T("%d"), CardOpenDBArray.GetAt(jj).GetMemberId());
		m_InfoList.SetItemText(jj, 4, strTmp);

		m_InfoList.SetItemText(jj, 5, CardOpenDBArray.GetAt(jj).GetOperationDateTime());
	}
	CardOpenDBArray.RemoveAll();
}

// 查询当前上机记录
BOOL CCardopenConsumeView::QueryActiveConsume(CString strMemberId)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL.Append(_T("SELECT activemember.classId, activemember.updateTime, activemember.memberId  "));

	strSQL.Append(_T("FROM activemember "));

	strSQL.AppendFormat(_T("where activemember.memberId = %s and refNo > 0"), strMemberId);

	strSQL.Append(_T(" ORDER BY  activemember.updateTime DESC"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("Queryactivemember:%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) bRet = FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			bRet = TRUE;
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

// 取得账户状态的文字字串
CString CCardopenConsumeView::GetCardStateAsString(BOOL bNewCardState /*= FALSE*/)
{
	CIBAString strTxt, strState;
	strTxt.LoadString(IDS_CARDSTATE);

	if (bNewCardState)
	{
		strState = strTxt.Mid(strTxt.Find('|') + 1, strTxt.GetLength() - 1);
	}
	else
	{
		strState = strTxt.Mid(0, strTxt.Find('|'));
	}

	return strState;
}

// 查询记录
BOOL CCardopenConsumeView::QueryHistotyConsume(CString strMemberId, CString strCardOpenTime)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL.Append(_T("SELECT consume.checkinTime, consume.memberId  "));

	strSQL.Append(_T("FROM consume "));

	strSQL.AppendFormat(_T("where consume.memberId = %s "), strMemberId);
	//登入时间必须在开户时间之后，防止出现strMemberId重复的情况
	strSQL.AppendFormat(_T("AND consume.checkinTime  BETWEEN '%s' AND '%s'")
		, strCardOpenTime, m_EndTime.ToString());

	strSQL.Append(_T(" ORDER BY  consume.checkinTime DESC"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("QueryConsume:%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) bRet = FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			bRet = TRUE;
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}


// 从中心取得普通用户级别ID并组合成SQL字串
CString CCardopenConsumeView::GetCommonClassIdAsString(void)
{
	CString strClassId;
	strClassId.Append(_T("("));

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_UserClassArray.GetCount(); i++)
	{
		if (!CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetIsMember())
		{
			strClassId.AppendFormat(_T("%d, "), CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(i).GetClassID());
		}
	}

	strClassId = strClassId.Left(strClassId.ReverseFind(','));

	strClassId.Append(_T(")"));

	strClassId.Trim();

	return strClassId;
}

void CCardopenConsumeView::OnClose()
{
	INT nIdx = theApp.GetIBAView()->FindTab(GetSafeHwnd());
	theApp.GetIBAView()->GetTabControl().ShowTab(nIdx, FALSE);

	m_InfoList.DeleteAllItems();

	m_strNoConsumeResult.Empty();

	UpdateData(FALSE);
}

void CCardopenConsumeView::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(0, 0, cx, cy, FALSE);

	if (m_InfoList.GetSafeHwnd() != NULL)
	{
		m_InfoList.MoveWindow(10, 100, cx - 20, cy - 105, TRUE);

		InitDate();
	}
}

void CCardopenConsumeView::OnInitialUpdate()
{
	CIBAFormView::OnInitialUpdate();

	m_STATICResult.SetFont(&m_Font);
	m_STATICResult.SetWindowText(_T(""));

	InitTimeCtrl();

	InitDate();

	InitList();

	GetDlgItem(IDC_BUTTON_QUERY)->SetFocus();

	// 2011/12/28-8236-gxx: 自动查询一次
	OnBnClickedButtonQuery();
}


BOOL CCardopenConsumeView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd * pWnd = GetFocus();
		if (pWnd)
		{
			if (pWnd->GetDlgCtrlID() == IDC_BUTTON_STARTTIME 
				||pWnd->GetDlgCtrlID() == IDC_BUTTON_ENDTIME
				||pWnd->GetDlgCtrlID() == IDC_BUTTON_QUERY)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
			{
				PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_QUERY, BN_CLICKED),(LPARAM)GetDlgItem(IDC_BUTTON_QUERY)->m_hWnd);
			}
		}
	}

	return CIBAFormView::PreTranslateMessage(pMsg);
}

void CCardopenConsumeView::OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	//{ 2011/04/27-gxx: 没有选择就不弹出对话框
	if ( m_InfoList.GetSelectedCount() == 0 )
	{
		return;
	}
	//}
	
	
	
	CString strNetId = m_InfoList.GetItemText(m_InfoList.GetSelectionMark(), 0);

	CNDCreditDlg CreditDlg;
	CreditDlg.SetNetId(strNetId);
	CreditDlg.DoModal();

	*pResult = 0;
}
