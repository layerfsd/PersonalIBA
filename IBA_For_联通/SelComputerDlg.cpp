// SelComputerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "Dal\IBADAL.h"
#include "ComputerInfo.h"
#include "Socket\LocalServer.h"
#include "ComputerListView.h"
#include ".\selcomputerdlg.h"

// CSelComputerDlg 对话框

IMPLEMENT_DYNAMIC(CSelComputerDlg, CIBADialog)
CSelComputerDlg::CSelComputerDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CSelComputerDlg::IDD, pParent)
, m_nMemberId(0)
{
}

CSelComputerDlg::~CSelComputerDlg()
{
}

void CSelComputerDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_ListInfo);
	DDX_Text(pDX, IDC_EDIT_TERMID, m_strTermId);
}


BEGIN_MESSAGE_MAP(CSelComputerDlg, CIBADialog)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO, OnNMDblclkListInfo)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CSelComputerDlg 消息处理程序

void CSelComputerDlg::OnOK()
{
	CBCGPDialog::OnOK();
}

void CSelComputerDlg::OnCancel()
{
	CBCGPDialog::OnCancel();
}

// 初始化控件
BOOL CSelComputerDlg::InitCtrl(void)
{
	CString strHeader;
	strHeader = L"电脑名称|机器区域";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {100, 100};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_ListInfo.InsertColumn(i, strArray.GetAt(i), LVCFMT_CENTER, Width[i]);
	}

	m_ListInfo.SetFont(&m_Font);
	m_ListInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

	GetDlgItem(IDC_EDIT_TERMID)->SetFont(&m_Font);

	return FALSE;
}

// 填充数据
BOOL CSelComputerDlg::FillList(void)
{
	m_ListInfo.DeleteAllItems();

	for(INT_PTR i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	{
		CComputerInfo & ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(i);
		//仅抽取离线用户
		if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_OFFLINE)
		{
			m_ListInfo.InsertItem(0, ComputerInfo.GetTerminalID());
			m_ListInfo.SetItemText(0, 1, ComputerInfo.GetPCClass());
		}

		//该机器没有用户资料
		// 		if (ComputerInfo.GetMemberID() == 0)
		// 		{
		// 			m_ListInfo.InsertItem(0, ComputerInfo.GetTerminalID());
		// 			m_ListInfo.SetItemText(0, 1, ComputerInfo.GetPCClass());
		// 		}
	}

	if (m_ListInfo.GetItemCount() <= 0)
	{
		MsgBox(_T("没有空闲机器，请到原来机器继续上机！"));
		OnCancel();
	}

	return TRUE;
}

BOOL CSelComputerDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitCtrl();

	FillList();

	return FALSE;  // return TRUE unless you set the focus to a control
}

BOOL CSelComputerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd * pWnd = GetFocus();
		
		if (pWnd->GetDlgCtrlID() == IDC_BUTTON_QUERY)
		{
			OnBnClickedButtonQuery();
			return TRUE;
		}
	}
	return CIBADialog::PreTranslateMessage(pMsg);
}

void CSelComputerDlg::OnBnClickedButtonQuery()
{
	UpdateData();
	//条件不足
	
	if (m_strTermId.IsEmpty())
	{
		SetToolTipPos(IDC_BUTTON_QUERY);
		CString strText;
		strText.LoadString(IDS_INPUTEMPTY);
		ShowToolTip(strText);
		return;
	}
	//终端优先
	if (!m_strTermId.IsEmpty())
	{
		for (INT i = 0; i < m_ListInfo.GetItemCount(); i++)
		{
			if (m_strTermId.CompareNoCase(m_ListInfo.GetItemText(i, 0)) == 0)
			{
				break;
			}
		}

		if (i < m_ListInfo.GetItemCount())
		{
			m_ListInfo.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			//m_listctrl.SetItemState(nIndex,0,LVIS_SELECTED|LVIS_FOCUSED);
			m_ListInfo.SetFocus();

			m_ListInfo.EnsureVisible(i,   FALSE); 
			return;
		}
	}
}

void CSelComputerDlg::OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	SelComputer();

	*pResult = 0;
}

void CSelComputerDlg::OnBnClickedOk()
{
	SelComputer();
}

void CSelComputerDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CSelComputerDlg::SelComputer()
{
	UpdateData();

	INT nIndex = m_ListInfo.GetSelectionMark();
	if (nIndex >= 0 && nIndex < m_ListInfo.GetItemCount())
	{
		CString strTermId = m_ListInfo.GetItemText(nIndex, 0);
		if (!strTermId.IsEmpty())
		{
			BOOL bTakeUp = CheckTakeUp(strTermId);
			if (!bTakeUp)
			{
				m_strSelTermId = strTermId;
				
				BOOL bSuccess = CIBADAL::GetInstance()->UpdateLastUserInfo(strTermId, CIBAHelpper::GetCurrentTimeAsString(TRUE), m_nMemberId);
				
				if (bSuccess)
				{
					CString strText;
					strText.Format(IDS_CHANGECOMPUTERSUCCESS, strTermId);
					//SetToolTipPos(IDOK);
					//ShowToolTip(strText);
					MsgBox(strText);

					OnOK();
				}
				else
				{
					CString strText;
					strText.Format(IDS_OPREATEFAILED);
					SetToolTipPos(IDOK);
					ShowToolTip(strText);
				}
			}
			else
			{
				//有人占用
				CString strText;
				strText.LoadString(IDS_CHANGECOMPUTERERROR);
				SetToolTipPos(IDOK);
				ShowToolTip(strText);
			}
		}
	}
}

BOOL CSelComputerDlg::CheckTakeUp(CString strTermId)
{
	//查询该终端是否被占用
	BOOL bTakeUp = FALSE;

	CLastUserInfo LastUserInfo;
	BOOL bRet = CIBADAL::GetInstance()->GetLastUserInfo(LastUserInfo, 0, strTermId);
	if (bRet)
	{
		///////////////////挂机//////////////////
		if (LastUserInfo.GetSuspend())
		{
			bTakeUp = TRUE;
		}
		else
		{
			////////////////////非挂机/////////////////
			COleDateTime updateTime;
			updateTime.ParseDateTime(LastUserInfo.GetUpdateTime());
			COleDateTimeSpan interval = COleDateTime::GetCurrentTime() - updateTime;
			if (interval.GetTotalMinutes() < 15)//被占用，并且用户还有效
			{
				m_strSelTermId.Empty();
				//有人占用
				CString strText;
				strText.LoadString(IDS_CHANGECOMPUTERERROR);
				SetToolTipPos(IDOK);
				ShowToolTip(strText);

				CString strTip;
				strTip.Format(_T("CSelComputerDlg::OnNMDblclkListInfo: find it: %s, time interval is %.2f"), 
					strTermId, interval.GetTotalMinutes());
				CIBALog::GetInstance()->Write(strTip);

				bTakeUp = TRUE;
			} 
			else//删除无效项,否则会更新失败
			{
				//清空登入登出状态表
				//CIBADAL::GetInstance()->DeleteLastUserInfo(strTermId);

				bTakeUp = FALSE;
			}
		}
	}
	else
	{//不存在该记录
		bTakeUp = FALSE;
	}

	return bTakeUp;
}