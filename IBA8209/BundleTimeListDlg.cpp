// BundleTimeListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include ".\bundletimelistdlg.h"

#define NetbarBundleTime CNetBarConfig::GetInstance()->m_NetbarBundleTime.m_BundleTimeInfoArray

// CDlgBundleTimeList 对话框

IMPLEMENT_DYNAMIC(CDlgBundleTimeList, CIBADialog)
CDlgBundleTimeList::CDlgBundleTimeList(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgBundleTimeList::IDD, pParent)
, m_nBundleTimeId(0), m_nBundleTimeClassId(0), m_nBundleTimeType(0)
, m_nSelectStyle(0), m_nUserClassId(0), m_nBundleTimeMoney(0)
{
	m_strRemainTime = _T("00:00:00");
}

CDlgBundleTimeList::~CDlgBundleTimeList()
{
}

void CDlgBundleTimeList::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BUNDLETIME, m_BundleTimeList);
}


BEGIN_MESSAGE_MAP(CDlgBundleTimeList, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BUNDLETIME, OnNMDblclkListBundletime)
END_MESSAGE_MAP()


// CDlgBundleTimeList 消息处理程序

BOOL CDlgBundleTimeList::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitListView();

	FillBundleTimeList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgBundleTimeList::FillBundleTimeList()
{
	m_BundleTimeList.DeleteAllItems();

	for (INT i = 0; i < NetbarBundleTime.GetCount(); i++)
	{
		if (NetbarBundleTime.GetAt(i).GetClassId() != m_nUserClassId)
		{
			if (m_BundleTimeList.GetItemCount() == 0)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		if (0 == m_nSelectStyle)
		{
			InsertItem(i, m_BundleTimeList.GetItemCount());//生成列表项
		}
		else
		{
			if (NetbarBundleTime.GetAt(i).IsNowEnabled())
			{
				InsertItem(i, m_BundleTimeList.GetItemCount());//生成列表项
			}
		}
	}
}

void CDlgBundleTimeList::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_BUNDLETIMELISTHEADER);

	//strHeader = L"编号|开始时间|结束时间|时长|金额|包时类型|其他信息";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {40, 70, 70, 40, 45, 70, 200};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_BundleTimeList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_BundleTimeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDlgBundleTimeList::OnOK()
{
	INT nIndex = m_BundleTimeList.GetSelectionMark();

	if (nIndex >= 0)
	{	
		//取得绑定数据
		INT nBtIndex = m_BundleTimeList.GetItemData(nIndex);
		
		m_nBundleTimeId = NetbarBundleTime.GetAt(nBtIndex).GetBundleTimeId();

		m_nBundleTimeClassId = NetbarBundleTime.GetAt(nBtIndex).IsNowEnabled() ? 0 : 1;

		//开始时间
		m_strStartTime = NetbarBundleTime.GetAt(nBtIndex).GetStartTime(TRUE).Left(5);
		//结束时间
		m_strEndTime = NetbarBundleTime.GetAt(nBtIndex).GetEndTime(TRUE).Left(5);
		//包时扣费类型
		m_nBundleTimeType = NetbarBundleTime.GetAt(nBtIndex).GetBundleTimeType();

		COleDateTimeSpan RemainTime;

		if (NetbarBundleTime.GetAt(nBtIndex).GetStartTime().IsEmpty())
		{
			RemainTime = COleDateTimeSpan(0, NetbarBundleTime.GetAt(nBtIndex).GetTimePass(), 0, 0);
		}
		else
		{
			COleDateTime StartTime, EndTime, CurTime = COleDateTime::GetCurrentTime();
			
			StartTime.ParseDateTime(NetbarBundleTime.GetAt(nBtIndex).GetStartTime(TRUE));
			StartTime.SetDateTime(CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
				StartTime.GetHour(), StartTime.GetMinute(), StartTime.GetSecond());

			EndTime.ParseDateTime(NetbarBundleTime.GetAt(nBtIndex).GetEndTime(TRUE));
			EndTime.SetDateTime(CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
				EndTime.GetHour(), EndTime.GetMinute(), EndTime.GetSecond());

			if (StartTime > EndTime) //跨天包时
			{
				if (CurTime < EndTime)
				{
					StartTime -= COleDateTimeSpan(1, 0, 0, 0); //减去一天
				}
				else
				{
					EndTime += COleDateTimeSpan(1, 0, 0, 0); //加上一天
				}
			}
			
			if (CurTime < StartTime) //包时还没开始
			{
				RemainTime = COleDateTimeSpan(0, NetbarBundleTime.GetAt(nBtIndex).GetTimePass(), 0, 0);
			}
			else if (CurTime > EndTime) //包时已经过期，只能开始算明天的包时
			{

			}
			else //包时正在进行
			{
				RemainTime = EndTime - CurTime;
			}

			//AfxMessageBox(StartTime.Format(_T("%Y-%m-%d %H:%M:%S")));
			//AfxMessageBox(EndTime.Format(_T("%Y-%m-%d %H:%M:%S")));
			//AfxMessageBox(m_strRemainTime);
		} 
		
		//m_strRemainTime = RemainTime.Format(_T("%H:%M:%S"));

		m_strRemainTime.Format(_T("%02d:%02d:%02d"), 
			(long)RemainTime.GetTotalHours(), RemainTime.GetMinutes(), RemainTime.GetSeconds());

		//包时金额
		m_nBundleTimeMoney = NetbarBundleTime.GetAt(nBtIndex).GetBundleTimeMoney();

		CBCGPDialog::OnOK();
	}
}

void CDlgBundleTimeList::OnBnClickedOk()
{
	OnOK();
}

void CDlgBundleTimeList::OnNMDblclkListBundletime(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnOK();

	*pResult = 0;
}

void CDlgBundleTimeList::InsertItem(INT nArrayOrder, INT nItem)
{
	CString strTmp;

	UINT nTmp = NetbarBundleTime.GetAt(nArrayOrder).GetBundleTimeId();
	strTmp.Format(_T("%d"), nTmp);
	m_BundleTimeList.InsertItem(nItem, strTmp);

	strTmp = NetbarBundleTime.GetAt(nArrayOrder).GetStartTime();
	m_BundleTimeList.SetItemText(nItem, 1, strTmp);

	strTmp = NetbarBundleTime.GetAt(nArrayOrder).GetEndTime();
	m_BundleTimeList.SetItemText(nItem, 2, strTmp);

	nTmp = NetbarBundleTime.GetAt(nArrayOrder).GetTimePass();
	strTmp.Format(_T("%d"), nTmp);
	m_BundleTimeList.SetItemText(nItem, 3, strTmp);

	nTmp = NetbarBundleTime.GetAt(nArrayOrder).GetBundleTimeMoney();
	strTmp.Format(_T("%.2f"), nTmp / 100.0);
	m_BundleTimeList.SetItemText(nItem, 4, strTmp);

	strTmp = NetbarBundleTime.GetAt(nArrayOrder).GetBundleTimeTypeString();
	m_BundleTimeList.SetItemText(nItem, 5, strTmp);

	strTmp = NetbarBundleTime.GetAt(nArrayOrder).GetDescription();
	m_BundleTimeList.SetItemText(nItem, 6, strTmp);

	m_BundleTimeList.SetItemData(nItem, nArrayOrder);
}

CString CDlgBundleTimeList::GetBundleTimeMoneyString() const
{
	CString strTmp;

	strTmp.Format(_T("%.2f"), m_nBundleTimeMoney / 100.0);

	return strTmp;
}