// ComsumeListView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "Cashier.h"
#include "dal\IBADAL.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include "MemberInfoDlg.h"
#include ".\ComsumeListView.h"

// CComsumeListView

IMPLEMENT_DYNCREATE(CComsumeListView, CIBAFormView)

CComsumeListView::CComsumeListView()
: CIBAFormView(CComsumeListView::IDD)
, m_nMemberId(0)
{
}

CComsumeListView::~CComsumeListView()
{
}

void CComsumeListView::DoDataExchange(CDataExchange* pDX)
{
	CIBAFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CONSUMELIST, m_ConsumeList);
	DDX_Control(pDX, IDC_COMBO_CONSUMETYPE, m_cboConsumeType);
	DDX_Control(pDX, IDC_BUTTON_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_BUTTON_ENDTIME, m_EndTime);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strNetId);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strResult);
}

BEGIN_MESSAGE_MAP(CComsumeListView, CIBAFormView)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CONSUMELIST, OnBnClickedButtonConsumelist)
	//ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONSUMELIST, OnNMClickListConsumelist)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CONSUMELIST, OnNMDblclkListConsumelist)
END_MESSAGE_MAP()

// CComsumeListView 消息处理程序

void CComsumeListView::OnInitialUpdate()
{
	CIBAFormView::OnInitialUpdate();

	InitListView();
	SetEditFont();

	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_Font);

#ifndef __USING_INTER_RESOURCE
	// 全部|上网|充值|充值卡|退款|嘟嘟牛消费|嘟嘟牛充值
	CStringArray strArr;
	CIBAString strComb;
	strComb.LoadString(IDS_COM_LIST_TYPES);
	CIBAHelpper::SplitLine(strComb,strArr);
	ASSERT(strArr.GetCount()==7);
	m_cboConsumeType.ResetContent();
	for(int i=0; i < strArr.GetCount(); i++)
	{
		m_cboConsumeType.AddString(strArr.GetAt(i));
	}
#endif

	m_cboConsumeType.SetCurSel(0);
	m_cboConsumeType.SetFont(&m_Font);

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

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	m_nMemberId = 0;

	SetTimer(1000, 1000 * 60, NULL);
}

HBRUSH CComsumeListView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CComsumeListView::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(0, 0, cx, cy, FALSE);

	if (m_ConsumeList.GetSafeHwnd() != NULL)
	{
		m_ConsumeList.MoveWindow(10, 145, cx - 15, cy - 150, TRUE);
		InitData();
	}
}

void CComsumeListView::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_CONSUMELSITHEADER);
	//strHeader = L"上网账号|消费地点|消费类型|时间|充值|消费|退款|相关信息|备注";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {150, 100, 90, 260, 70, 70, 70, 100, 100};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_ConsumeList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_ConsumeList.SetFont(&m_Font);
	m_ConsumeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}


UINT CComsumeListView::GetConsumeType()
{
	/**
	*消费类型：0: 全部 1:上网 2:充值 3:卡购物 4：嘟嘟牛消费 5:卡购物(旧) 6:退款 7：充值卡，8：嘟嘟牛充值
	*/

	/**
	*全部;上网;充值;充值卡;退款;嘟嘟牛消费;嘟嘟牛充值
	*/

	switch(m_cboConsumeType.GetCurSel()) 
	{
	case 0: return 0;

	case 1: return 1;

	case 2: return 2;

	case 3: return 7;

	case 4: return 6;

	case 5: return 4;

	case 6: return 8;

	}

	return 0;
}

void CComsumeListView::QueryConsumeList(UINT nMemberId, CString strNetId)
{
	CIBAString strConsumeName; //= L"上网|充值|账户购物|嘟嘟牛消费|卡购物(旧)|退款|充值卡|嘟嘟牛充值";
	strConsumeName.LoadString(IDS_CONSUMETYPE);
	CStringArray saConsumeName;
	CIBAHelpper::SplitLine(strConsumeName, saConsumeName);

	CConsumeListMethod ConsumeListMethod;
	ConsumeListMethod.SetConsumeType(GetConsumeType());

	CString strTmp = m_StartTime.ToString();
	strTmp.Remove('-');
	strTmp.Remove(':');
	strTmp.Remove(' ');
	strTmp.Append(_T("00"));
	ConsumeListMethod.SetBeginTime(strTmp);

	strTmp = m_EndTime.ToString();
	strTmp.Remove('-');
	strTmp.Remove(':');
	strTmp.Remove(' ');
	strTmp.Append(_T("00"));
	ConsumeListMethod.SetEndTime(strTmp);

	ConsumeListMethod.SetMemberId(nMemberId);

	theApp.GetCurCashier()->DoConsumeList(ConsumeListMethod);

	if (ConsumeListMethod.GetStatusCode() == 0)
	{
		for (INT i = 0; i < (INT)ConsumeListMethod.GetRecordCount(); i++)
		{
			CString strTmp;

			strTmp = CNetBarConfig::GetInstance()->GetNetBarName(ConsumeListMethod.GetNetBarId(i));

			m_ConsumeList.InsertItem(i, strNetId);
			
			m_ConsumeList.SetItemText(i, 1, strTmp);

			m_ConsumeList.SetItemText(i, 2, saConsumeName.GetAt(ConsumeListMethod.GetConsumeType(i) -1));

			strTmp = ConsumeListMethod.GetBeginTime(i);
			strTmp.Insert(2, _T("-"));
			strTmp.Insert(5, _T(" "));
			strTmp.Insert(8, _T(":"));
			strTmp.Insert(11, _T(":"));
			
			m_ConsumeList.SetItemText(i, 3, strTmp);

			/**
			*0: 全部 1:上网 2:充值 3:卡购物 4：嘟嘟牛消费 5:卡购物(旧) 6:退款 7：充值卡，8：嘟嘟牛充值
			*/
			switch (ConsumeListMethod.GetConsumeType(i))
			{
			case 1:

				{
					CString strTmp2(ConsumeListMethod.GetEndTime(i));
					strTmp2.Insert(2, _T("-"));
					strTmp2.Insert(5, _T(" "));
					strTmp2.Insert(8, _T(":"));
					strTmp2.Insert(11, _T(":"));

					strTmp.Append(_T(" - "));
					strTmp.Append(strTmp2);

					m_ConsumeList.SetItemText(i, 3, strTmp);
				}

				//这里不要break

			case 3:
			case 4:
			case 5:

				m_nConsumeMoney -= ConsumeListMethod.GetAmount(i);
				strTmp.Format(_T("%.2f"), -ConsumeListMethod.GetAmount(i) / 100.0);
				m_ConsumeList.SetItemText(i, 5, strTmp);

				break;

			case 2:
			case 7:
			case 8:

				m_nCreditMoney += ConsumeListMethod.GetAmount(i);

				strTmp.Format(_T("%.2f"), ConsumeListMethod.GetAmount(i) / 100.0);
				m_ConsumeList.SetItemText(i, 4, strTmp);
				break;

			case 6:
				
				m_nReturnMoney += ConsumeListMethod.GetAmount(i);
				strTmp.Format(_T("%.2f"), ConsumeListMethod.GetAmount(i) / 100.0);
				m_ConsumeList.SetItemText(i, 6, strTmp);
				break;
			}

			strTmp = ConsumeListMethod.GetBusinessName(i);
			m_ConsumeList.SetItemText(i, 7, strTmp);

			strTmp = ConsumeListMethod.GetDescription(i);
			m_ConsumeList.SetItemText(i, 8, strTmp);

			m_ConsumeList.SetItemData(i, i);
		}

		m_strResult.Format(LOAD_STRING(IDS_CONSUMELISTQUERYRESULT),
			ConsumeListMethod.GetRecordCount(), m_nCreditMoney / 100.0, 
			m_nConsumeMoney / 100.0, m_nReturnMoney / 100.0);

		UpdateData(FALSE);

	}
	else
	{
		SetToolTipPos(IDC_BUTTON_CONSUMELIST);
		ShowToolTip(ConsumeListMethod.GetStatusMessage(), TRUE);
	}

	m_nMemberId = 0;//本次查询完毕以后，用户号清空，不再指定任务，即只查询一次
}

void CComsumeListView::OnBnClickedButtonConsumelist()
{
	m_nCreditMoney = 0;
	m_nReturnMoney = 0;
	m_nConsumeMoney = 0;

	CWaitCursor Wait;

	UpdateData();

	if (!m_strNetId.IsEmpty())
	{
		CheckCard(m_strNetId, m_nMemberId);
		return;
	}

	if (m_strName.IsEmpty() && m_strPersonalID.IsEmpty())
	{
		SetToolTipPos(IDC_BUTTON_CONSUMELIST);
		ShowToolTip(IDS_INPUTCONDITION);
		return;
	}

	CMemberInfoMethod MemberInfo;

	MemberInfo.SetPersonalId(m_strPersonalID);
	MemberInfo.SetUsername(m_strName);
	MemberInfo.SetAvailable(1);
	theApp.GetCurCashier()->DoMemberInfo(MemberInfo);

	if (MemberInfo.GetStatusCode() == 0)
	{
		m_ConsumeList.DeleteAllItems();
		
		if (MemberInfo.GetMemberCount() > 0)
		{
			if (MemberInfo.GetMemberCount() > 1)
			{
				CDlgMemberInfo MemberInfoDlg;
				MemberInfoDlg.SetQueryStyle(2);//中心查询方式，有效用户
				MemberInfoDlg.SetUserName(m_strName);
				MemberInfoDlg.SetPersonalID(m_strPersonalID);
				if (MemberInfoDlg.DoModal() == IDOK)
				{
					m_strNetId = MemberInfoDlg.GetNetId();
					m_strName = MemberInfoDlg.GetUserName();
					QueryConsumeList(MemberInfoDlg.GetMemberId(), MemberInfoDlg.GetNetId());
				}
			}
			else
			{
				m_strNetId = MemberInfo.GetSerailNum(0);
				m_strName = MemberInfo.GetUserName(0);
				QueryConsumeList(MemberInfo.GetMemberId(0), MemberInfo.GetSerailNum(0));
			}
		}
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_CONSUMELIST);
		ShowToolTip(MemberInfo.GetStatusMessage(), TRUE);
	}
}

void CComsumeListView::CheckCard(LPCTSTR strNetId, UINT nMemberId /*= 0*/)
{
	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strNetId);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	if (checkCard.GetStatusCode() == 0)
	{
		if (checkCard.GetMemberId() != 0) //已经开过的卡,充值退卡
		{
			m_ConsumeList.DeleteAllItems();
			QueryConsumeList(checkCard.GetMemberId(), strNetId);			
		}
		else //新卡注册
		{
			if (nMemberId == 0)
			{
				CDlgMemberInfo MemberInfoDlg;
				MemberInfoDlg.SetQueryStyle(4);//根据上网账号从本地查询
				MemberInfoDlg.SetNetId(strNetId);

				if (MemberInfoDlg.DoModal() == IDOK)
				{
					if (!MemberInfoDlg.GetNetId().IsEmpty())
					{
						m_ConsumeList.DeleteAllItems();
						QueryConsumeList(MemberInfoDlg.GetMemberId(), MemberInfoDlg.GetNetId());
						return;
					}
				}
			} 
			else
			{
				m_ConsumeList.DeleteAllItems();
				QueryConsumeList(nMemberId, strNetId);
				return;
			}

			SetToolTipPos(IDC_EDIT_IDCARDSNO);
			ShowToolTip(IDS_HASNOTUSED);
			GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
		}
	}
	else
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO);
		ShowToolTip(checkCard.GetStatusMessage());
	}
}

void CComsumeListView::OnClose()
{
	INT nIdx = theApp.GetIBAView()->FindTab(GetSafeHwnd());
	theApp.GetIBAView()->GetTabControl().ShowTab(nIdx, FALSE);

	m_ConsumeList.DeleteAllItems();
	m_strNetId.Empty();
	m_strResult.Empty();
	m_strPersonalID.Empty();
	m_strName.Empty();

	UpdateData(FALSE);
}

void CComsumeListView::InitData()
{
	COleDateTime dtTmp;

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		dtTmp.ParseDateTime(theApp.GetCurCashier()->GetOnDutyTime());
	}
	else
	{
		dtTmp = COleDateTime::GetCurrentTime();
		dtTmp -= COleDateTimeSpan(1, 0, 0, 0);
	}

	m_StartTime.SetDate(dtTmp);

	dtTmp = COleDateTime::GetCurrentTime();
	dtTmp.SetDateTime(dtTmp.GetYear(),dtTmp.GetMonth(),dtTmp.GetDay(),0,0,0);
	dtTmp += COleDateTimeSpan(1, 0, 0, 0);
	m_EndTime.SetDate(dtTmp);

}

void CComsumeListView::OnTimer(UINT nIDEvent)
{
	COleDateTime dtTmp;

	dtTmp = COleDateTime::GetCurrentTime();
	dtTmp += COleDateTimeSpan(0, 0, 10, 0);
	
	m_EndTime.SetDate(dtTmp);

	CIBAFormView::OnTimer(nIDEvent);
}

void CComsumeListView::QueryUserConsumeList(LPCTSTR lpszNetId, UINT nMemberId /*= 0*/)
{
	m_strNetId = lpszNetId;
	m_nMemberId = nMemberId;//指定用户Id，就指定查询的用户

	UpdateData(FALSE);

	OnBnClickedButtonConsumelist();
}

void CComsumeListView::OnNMClickListConsumelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT nIdx = m_ConsumeList.GetSelectionMark();

	if (nIdx >= 0)
	{
		m_strNetId = m_ConsumeList.GetItemText(nIdx, 0);
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CComsumeListView::OnNMDblclkListConsumelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		return;
	}

	INT nIdx = m_ConsumeList.GetSelectionMark();

	if (nIdx >= 0)
	{
		m_strNetId = m_ConsumeList.GetItemText(nIdx, 0);
		
		if (!m_strNetId.IsEmpty())
		{
			CNDCreditDlg dlg;
			dlg.SetNetId(m_strNetId);
			dlg.DoModal();
		}
	}

	*pResult = 0;
}
