// AllCheckOut.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "ComputerInfo.h"
#include "Socket\LocalServer.h"
#include "ComputerListView.h"
#include "Servlet\UserClassInfo.h"
#include "Dal\IBADAL.h"
#include "Cashier.h"
#include "RealName\IBARealName.h"
#include "MainFrm.h"
#include "..\L2Markup\Markup.h"
#include ".\AllCheckOut.h"
#include "ReturnDlg.h"
#include "CashierHelper.h"
#include "ServletHellper.h"
////////////////////////////////////////////////////////////////////////////////
// CBCGPGridButtonItem Class

CBCGPGridButtonItem::CBCGPGridButtonItem (LPCTSTR lpszText, UINT id) :
CBCGPGridItem (lpszText)
{
	m_id = id;

}
//*****************************************************************************************
void CBCGPGridButtonItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID(pDC);

	rect.DeflateRect(1, 1);
	pDC->FillRect(rect, &globalData.brBarFace);
	pDC->Draw3dRect(rect, globalData.clrBarLight, globalData.clrBarDkShadow);
	rect.DeflateRect(1, 1);
	pDC->Draw3dRect(rect, globalData.clrBarHilite, globalData.clrBarShadow);

	COLORREF clrText = pDC->SetTextColor(globalData.clrBarText);

	CString strText = (LPCTSTR)(_bstr_t)m_varValue;

	pDC->DrawText(strText, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	pDC->SetTextColor(clrText);
}
//*****************************************************************************************
BOOL CBCGPGridButtonItem::OnClickValue(UINT uiMsg, CPoint point)
{
	if (uiMsg != WM_LBUTTONUP)
	{
		return CBCGPGridItem::OnClickValue(uiMsg, point);
	}

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	ASSERT_VALID (pGridCtrl);

	pGridCtrl->GetOwner()->PostMessage(WM_COMMAND, MAKEWPARAM(m_id, BN_CLICKED));

	return TRUE;
}


IMPLEMENT_DYNAMIC(CDlgAllCheckOut, CIBADialog)
CDlgAllCheckOut::CDlgAllCheckOut(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgAllCheckOut::IDD, pParent)
{
	m_bHasAllout = FALSE;
}

CDlgAllCheckOut::~CDlgAllCheckOut()
{
}

void CDlgAllCheckOut::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FIND, m_strFind);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CDlgAllCheckOut, CIBADialog)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, OnItemChanged)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_WM_CLOSE()
	ON_BN_CLICKED(GirdButtonId, OnBnClickedGirdButton)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnBnClickedButtonPrint)
	ON_NOTIFY(GLCN_CHILD_CONTROL, IDC_LIST1, OnGLCNotify)
END_MESSAGE_MAP()

// CDlgAllCheckOut 消息处理程序

void CDlgAllCheckOut::OnBnClickedOk() //执行全场结账
{
	if (IDOK != MsgBox(_T("确定要执行全场停止计费吗？"), MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
	{
		return;
	}
	
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		MessageBox(_T("正常模式严禁全场结账!只有本地模式才允许."),NULL,MB_OK);
		return;
	}

	m_bHasAllout = TRUE;

	m_ReturnInfoArray.RemoveAll();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	UINT nMemberCount = 0, 
		 nTmpUserCount = 0,
		 nLocalUserCount = 0;

	arrMembers.RemoveAll();

	if( GetActiveMembers(arrMembers) >= 0)
	{	
		for(int i = 0 ; i < arrMembers.GetCount() ; i++ )
		{
			ReturnInfo rinfo;
			CActiveMember& ActiveMember = arrMembers[i];		

			rinfo.bLocal = FALSE;
			CCashierHelper::MakeCheckOutInfo(ActiveMember, rinfo.CheckOutInfo);

			if ( rinfo.CheckOutInfo.GetClassState() == 0) //临时用户
			{
				nTmpUserCount++;
				if(FALSE == DeregisterUser(ActiveMember.GetMemberID()))
				{
					continue;
				}
			}
			else
			{
				nMemberCount++;
				if(FALSE == PayoutMember(ActiveMember.GetMemberID()))
				{
					continue;
				}
				//rinfo.CheckOutInfo.SetReturnState(1);		
			}
			CString strErrMsg;
			/*if(FALSE == CDeregisterHelpper::DeregisterUser(ActiveMember.GetMemberID(),strErrMsg))
			{
				IBA_LOG(_T("全场结账.自动退款失败.MemberId=%d,错误:%s"),ActiveMember.GetMemberID(),strErrMsg);
			}*/

			//if ( rinfo.CheckOutInfo.GetClassState() == 0) //临时用户
			//{
			//	CIBADAL::GetInstance()->UpdateCheckOutInfo(ActiveMember.GetNetId(), 0);
			//	rinfo.CheckOutInfo.SetReturnState(0);
			//}
			
			m_ReturnInfoArray.Add(rinfo);
		}	
	}
	
	//锁定
	CLocalServer::GetInstance()->LocalCheckOutAllUser();

	//刷新
	CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);

	FillList();

	SelFirstOne();

	CString strResult;
	
	strResult.Format(_T("全场停止计费，其中成功结账会员用户 %d 位, 临时用户 %d 位, 本地用户 %d 位！"), 
		nMemberCount, nTmpUserCount, nLocalUserCount);
	
	GetDlgItem(IDC_STATIC_RESULT)->SetWindowText(strResult);

	// 2011/07/21-8201-gxx: 增加本地用户的判断
	GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(nTmpUserCount > 0 || nLocalUserCount > 0);

	
}

CBCGPGridRow* CDlgAllCheckOut::CreateNewRow()
{
	CBCGPGridRow* pRow = m_GridCtrl.CreateRow(m_GridCtrl.GetColumnCount());

	for (int i = 0; i < m_GridCtrl.GetColumnCount();i++)
	{
		pRow->GetItem(i)->AllowEdit(FALSE);
	}

	return pRow;
}


void CDlgAllCheckOut::AddRow(INT_PTR nIndex, ReturnInfo &rInfo)
{
	CBCGPGridRow* pRow = CreateNewRow();

	pRow->SetData(nIndex);

	if (rInfo.bLocal)
	{
		pRow->GetItem(0)->SetValue(rInfo.LocalInfo.GetSerialNum().GetString());
		pRow->GetItem(1)->SetValue(rInfo.LocalInfo.GetName().GetString());
		pRow->GetItem(2)->SetValue(CNetBarConfig::GetInstance()->GetUserClassName(rInfo.LocalInfo.GetClassId()).GetString());
		pRow->GetItem(3)->SetValue(rInfo.LocalInfo.GetTermId().GetString());
	
		CString strTmp;

		UINT nTmp = CIBAHelpper::RoundMoney(rInfo.LocalInfo.GetReturnAmount(), CNetBarConfig::GetInstance()->GetRoundType());
		strTmp.Format(_T("%.2f"), nTmp / 100.0);
		pRow->GetItem(4)->SetValue(strTmp.GetString());

		if ( rInfo.bLocalReturned )
		{
			pRow->GetItem(5)->SetValue(_T("已退款"));
		}
		else
		{
			CBCGPGridItem* pItem = new CBCGPGridButtonItem(_T("退款"), GirdButtonId);
			pRow->ReplaceItem(6, pItem);
		}
		
		m_listCtrl.InsertItem(nIndex,_T(""));
		m_listCtrl.SetItemText(nIndex,0,rInfo.LocalInfo.GetSerialNum().GetString());
		m_listCtrl.SetItemText(nIndex,1,rInfo.LocalInfo.GetName().GetString());
		m_listCtrl.SetItemText(nIndex,2,CNetBarConfig::GetInstance()->GetUserClassName(rInfo.LocalInfo.GetClassId()).GetString());
		m_listCtrl.SetItemText(nIndex,3,rInfo.LocalInfo.GetTermId().GetString());
		m_listCtrl.SetItemText(nIndex,4,strTmp.GetString());
		if ( rInfo.bLocalReturned )
		{
			m_listCtrl.SetItemText(nIndex,5,_T("已退款"));
		}
		else
		{
			m_listCtrl.SetItem(nIndex,6,_T("退款"),NS_COMMON::CGxxListCtrl::GLC_Button);
		}
	}
	else
	{
		CCheckOutinfo& CheckOutinfo = rInfo.CheckOutInfo;

		pRow->GetItem(0)->SetValue(CheckOutinfo.GetSerialNum().GetString());
		pRow->GetItem(1)->SetValue(CheckOutinfo.GetUserName().GetString());
		pRow->GetItem(2)->SetValue(CheckOutinfo.GetClassName().GetString());
		pRow->GetItem(3)->SetValue(CheckOutinfo.GetTerminalId().GetString());

		CString strTmp;

		UINT nTmp = CIBAHelpper::RoundMoney(CheckOutinfo.GetBalanceMoney(), CNetBarConfig::GetInstance()->GetRoundType());
		strTmp.Format(_T("%.2f"), nTmp / 100.0);
		pRow->GetItem(4)->SetValue(strTmp.GetString());

		if (CheckOutinfo.GetReturnState() == 1)
		{
			pRow->GetItem(5)->SetValue(_T("已退款"));
		}
		else
		{
			CBCGPGridItem* pItem = new CBCGPGridButtonItem(_T("退款"), GirdButtonId);
			pRow->ReplaceItem(6, pItem);
		}

		m_listCtrl.InsertItem(nIndex,_T(""));
		m_listCtrl.SetItemText(nIndex,0,CheckOutinfo.GetSerialNum().GetString());
		m_listCtrl.SetItemText(nIndex,1,CheckOutinfo.GetUserName().GetString());
		m_listCtrl.SetItemText(nIndex,2,CheckOutinfo.GetClassName().GetString());
		m_listCtrl.SetItemText(nIndex,3,CheckOutinfo.GetTerminalId().GetString());

		m_listCtrl.SetItemText(nIndex,4,strTmp.GetString());
		if ( CheckOutinfo.GetReturnState() == 1 )
		{
			m_listCtrl.SetItemText(nIndex,5,_T("已退款"));
		}
		else
		{
			m_listCtrl.SetItem(nIndex,6,_T("退款"),NS_COMMON::CGxxListCtrl::GLC_Button);
		}
	}

	

	//CBCGPGridCheckItem* pItem = new CBCGPGridCheckItem(CheckOutinfo.GetReturnState() > 0 ? true : false);
	//pRow->ReplaceItem(7, pItem);

	m_GridCtrl.AddRow(pRow, FALSE);
}

BOOL CDlgAllCheckOut::InitList(void)
{
	CRect rectGrid;
	GetDlgItem(IDC_STATIC_CHECKOUT)->GetClientRect(&rectGrid);
	GetDlgItem(IDC_STATIC_CHECKOUT)->MapWindowPoints(this, &rectGrid);
	m_GridCtrl.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	
	m_GridCtrl.RemoveAll();
	
	while(m_GridCtrl.GetColumnCount() > 0)
	{
		m_GridCtrl.DeleteColumn(0);
	}

	CString strHeader;
	//strHeader.LoadString(IDS_DUTYJXC);
	strHeader = L"上网账号|姓名|用户级别|电脑名称|可退金额(元)|退款状态";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {125, 80, 90, 70, 90, 80, 80};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_GridCtrl.InsertColumn(i, strArray.GetAt(i), Width[i]);
		m_GridCtrl.SetColumnAlign(i, HDF_LEFT);
	}

	m_GridCtrl.InsertColumn(i, _T(""), 80);

	m_GridCtrl.SetWholeRowSel();
	m_GridCtrl.SetSingleSel(FALSE);
	m_GridCtrl.EnableHeader(TRUE, BCGP_GRID_HEADER_SORT);

	// Set grid colors
	CBCGPGridColors colors;
	colors.m_LeftOffsetColors.m_clrBackground = globalData.clrWindow;
	colors.m_OddColors.m_clrBackground = RGB(238, 246, 255);

	m_GridCtrl.SetColorTheme(colors);//颜色调配器

	//m_GridCtrl.SetFont(&m_Font);

	for(INT i=0; i < strArray.GetCount(); i++)
	{
		m_listCtrl.InsertColumn(i, strArray.GetAt(i),LVCFMT_LEFT,Width[i]);
	}
	m_listCtrl.InsertColumn(strArray.GetCount(), _T(""),LVCFMT_LEFT,80,TRUE,FALSE,TRUE,NULL,NS_COMMON::CGxxListCtrl::GLC_Button);

	return TRUE;
}

BOOL CDlgAllCheckOut::FillList(void)
{
	m_GridCtrl.RemoveAll();

	m_listCtrl.DeleteAllItems();

	//if (m_bHasAllout == FALSE)
	//{
	//	CCheckOutInfoArray arrCheckOut;
	//	CIBADAL::GetInstance()->GetCheckOutInfo(arrCheckOut);//查询数据库
	//	for (int i = 0; i < arrCheckOut.GetCount(); i++ )
	//	{
	//		ReturnInfo rInfo;
	//		rInfo.bLocal = FALSE;
	//		rInfo.CheckOutInfo = arrCheckOut.GetAt(i);
	//		m_ReturnInfoArray.Add(rInfo);
	//	}
	//}

	for (INT i = 0; i < m_ReturnInfoArray.GetCount(); i++)
	{
		ReturnInfo & rInfo = m_ReturnInfoArray.GetAt(i);

		if (!rInfo.bLocal && rInfo.CheckOutInfo.GetClassState() != 0)
		{
			continue; // 只添加中心临时用户
		}
		
		AddRow(i, rInfo);
	}

	m_listCtrl.ShowAllSubWindows();

	SelFirstOne();

	//GetDlgItem(IDOK)->EnableWindow(m_ReturnInfoArray.GetCount() <= 0);

	return TRUE;
}

void CDlgAllCheckOut::OnOK()
{
	CBCGPDialog::OnOK();
}

void CDlgAllCheckOut::OnCancel()
{
	CBCGPDialog::OnCancel();
}

BOOL CDlgAllCheckOut::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd * pWnd = GetFocus();
		
		if (pWnd != NULL)
		{
			if (pWnd->GetDlgCtrlID() == IDC_EDIT_FIND)
			{
				OnBnClickedButtonQuery();
				return TRUE;
			}
		}
	}

	//拦截焦点

	if (WM_CHAR == pMsg->message && pMsg->wParam != VK_RETURN)
	{
		CWnd* pWnd = GetFocus();
		
		if (pWnd != NULL && pWnd->GetDlgCtrlID() != IDC_EDIT_FIND)
		{
			GetDlgItem(IDC_EDIT_FIND)->SetFocus();
			GetDlgItem(IDC_EDIT_FIND)->PostMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}

BOOL CDlgAllCheckOut::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	

	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_NOSORTHEADER);
	m_listCtrl.SetItemsHeight(20);


	InitList();
	FillList();

	GetDlgItem(IDC_EDIT_FIND)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_Font);

	m_GridCtrl.ShowWindow(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
}

HBRUSH CDlgAllCheckOut::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_INFO)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	return hbr;
}

void CDlgAllCheckOut::OnBnClickedButtonQuery()
{
	UpdateData();
	
	//条件不足
	if (m_strFind.IsEmpty())
	{
		ShowToolTip(IDS_INPUTERROR, IDC_EDIT_FIND);
		return;
	}

	if (m_strFind.GetAt(0) >= 65 || m_strFind.GetLength() < 4) //字母开头或者长度小于4
	{
		for (INT i = 0; i < m_GridCtrl.GetRowCount(); i++)
		{
			CBCGPGridRow* pRow = m_GridCtrl.GetRow(i);
			
			CString RowValue = pRow->GetItem(0)->GetValue();

			if (RowValue.GetLength() > m_strFind.GetLength())
			{
				RowValue = RowValue.Right(m_strFind.GetLength());
			}

			if (m_strFind.CompareNoCase(RowValue) == 0)
			{
				break;
			}
		}

		if (i < m_GridCtrl.GetRowCount())
		{
			m_GridCtrl.SetFocus();
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetRow(i));
			m_GridCtrl.SetCurSel(m_GridCtrl.GetRow(i));

			m_listCtrl.SetSelectionMark(i);
			m_listCtrl.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
			m_listCtrl.EnsureVisible(i,TRUE);
		}

		m_strFind.Empty();
		UpdateData(FALSE);

		return;
	}

	for (INT ii = 0; ii < m_GridCtrl.GetRowCount(); ii++)
	{
		CBCGPGridRow * pRow = m_GridCtrl.GetRow(ii);
		CString RowValue = pRow->GetItem(0)->GetValue();
		
		if (RowValue.GetLength() > m_strFind.GetLength())
		{
			RowValue = RowValue.Right(m_strFind.GetLength());
		}

		if (m_strFind.CompareNoCase(RowValue) == 0)
		{
			break;
		}
	}

	if (ii < m_GridCtrl.GetRowCount())
	{
		m_GridCtrl.SetFocus();
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRow(ii));
		m_GridCtrl.SetCurSel(m_GridCtrl.GetRow(ii));

		m_listCtrl.SetSelectionMark(ii);
		m_listCtrl.SetItemState(ii, LVIS_SELECTED | LVIS_FOCUSED,  LVIS_SELECTED | LVIS_FOCUSED);
		m_listCtrl.EnsureVisible(ii,TRUE);
	}

	m_strFind.Empty();

	UpdateData(FALSE);
}

void CDlgAllCheckOut::SelFirstOne()
{
	//////////////选中第一项,这里的设置顺序很重要//////////
	m_GridCtrl.AdjustLayout();
	m_GridCtrl.SetFocus();

	CBCGPGridRow* pRow = m_GridCtrl.GetRow(0);

	if (pRow != NULL)
	{
		m_GridCtrl.EnsureVisible(pRow);
		m_GridCtrl.SetCurSel(pRow);
	}
}

HRESULT CDlgAllCheckOut::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
	BCGPGRID_ITEM_INFO* pItemInfo = (BCGPGRID_ITEM_INFO*)lParam;

	/*COleVariant vTmp = pItemInfo->pItem->GetValue();
	
	CBCGPGridRow * pRow = m_GridCtrl.GetCurSel();

	UINT nReturnState = vTmp.boolVal == 0 ? 0 : 1;

	if (pRow != NULL)
	{
		CString strNetId = pRow->GetItem(0)->GetValue();

		for (INT i = 0; i < m_ReturnInfoArray.GetCount(); i++)
		{
			ReturnInfo & rInfo = m_ReturnInfoArray.GetAt(i);

			if (rInfo.bLocal)
			{
				if (rInfo.LocalInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
					CActiveMember ActiveMember;
					rInfo.bLocalReturned = CCashierHelper::ReturnLocalUser(ActiveMember,
						&rInfo.LocalInfo);
				}
			}
			else
			{
				if (rInfo.CheckOutInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
					rInfo.CheckOutInfo.SetReturnState(1);
					CActiveMember ActiveMember;
					if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(rInfo.CheckOutInfo.GetMemberId(), ActiveMember) )
					{
						continue;
					}

					rInfo.bLocalReturned = CCashierHelper::ReturnLocalCenterUser(
						ActiveMember, &rInfo.CheckOutInfo);
				}
			}
			
		}
	}*/

	return 0;
}


BOOL CDlgAllCheckOut::SaveToDB(CCheckOutInfoArray& CheckOutArray)
{
	if (CheckOutArray.GetCount() <= 0)
	{
		return TRUE;
	}

	//2011-4-7-gxx
	CString strSQL;

	strSQL = _T("select * from checkoutinfo where 1=0");

	IBA_TRACE(strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) 
	{
		return FALSE;
	}

	CADORecordset Rs(pDb->GetConn());

	if (!Rs.Open(strSQL)) 
	{
		pDb->Release();
		return FALSE;
	}
	
	for (int i = 0 ; i < CheckOutArray.GetCount() ; i++ )
	{
		CCheckOutinfo& CheckOutinfo = CheckOutArray.GetAt(i);

		Rs.AddNew();

		Rs.SetFieldValue(_T("terminalId"),CheckOutinfo.GetTerminalId());
		Rs.SetFieldValue(_T("checkOutTime"),CheckOutinfo.GetCheckOutTime());
		Rs.SetFieldValue(_T("serialNum"), CheckOutinfo.GetSerialNum());
		Rs.SetFieldValue(_T("className"), CheckOutinfo.GetClassName());
		Rs.SetFieldValue(_T("memberId"), CheckOutinfo.GetMemberId());
		Rs.SetFieldValue(_T("classState"), CheckOutinfo.GetClassState());
		Rs.SetFieldValue(_T("guarantyMoney"), CheckOutinfo.GetGuarantyMoney());
		Rs.SetFieldValue(_T("balanceMoney"), CheckOutinfo.GetBalanceMoney());
		Rs.SetFieldValue(_T("userName"), CheckOutinfo.GetUserName());
		Rs.SetFieldValue(_T("personalId"), CheckOutinfo.GetPersonalId());
		Rs.SetFieldValue(_T("returnState"), CheckOutinfo.GetReturnState());

		Rs.Update();
	}
	
	Rs.Close();

	pDb->Release();

	return TRUE;
	//
}

void CDlgAllCheckOut::OnClose()
{
	//SaveToFile(m_CheckOutArray);

	CIBADialog::OnClose();
}
void CDlgAllCheckOut::OnGLCNotify(LPNMHDR lpHdr, LRESULT *pResult)
{
	NS_COMMON::LPNMHDR_GLC_CONTROL lpGLC = (NS_COMMON::LPNMHDR_GLC_CONTROL)lpHdr;
	*pResult = 0;

	if (lpGLC->nRowID != -1 && lpGLC->pCtrlHdr->code == BN_CLICKED)
	{
		CString strNetId = m_listCtrl.GetItemText(lpGLC->nRowID, 0);

		for (INT i = 0; i < m_ReturnInfoArray.GetCount(); i++)
		{
			ReturnInfo & rInfo = m_ReturnInfoArray.GetAt(i);

			if (rInfo.bLocal)
			{
				/*if (rInfo.LocalInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
					CActiveMember ActiveMember;
					ActiveMember.SetIsLocalUser(TRUE);
					rInfo.bLocalReturned = CCashierHelper::ReturnLocalUser(ActiveMember,
						&rInfo.LocalInfo);

					if (rInfo.bLocalReturned)
					{
						m_listCtrl.SetItemText(lpGLC->nRowID, 5, _T("已退款"));
						m_listCtrl.SetItem(lpGLC->nRowID, 6, _T(""));
					}
				}*/
			}
			else
			{
				/*if (rInfo.CheckOutInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
					CReturnDlg dlg;

					dlg.SetCheckOutInfo(rInfo.CheckOutInfo);

					if (IDOK == dlg.DoModal())
					{
						rInfo.CheckOutInfo.SetReturnState(1);
						BOOL bResult = CIBADAL::GetInstance()->UpdateCheckOutInfo(strNetId, 1);
						if (bResult)
						{
							m_listCtrl.SetItemText(lpGLC->nRowID, 5, _T("已退款"));
							m_listCtrl.SetItem(lpGLC->nRowID, 6, _T(""));
						}
					}*/
				//}
			}
		}
	}
}
void CDlgAllCheckOut::OnBnClickedGirdButton()
{
	CBCGPGridRow * pRow = m_GridCtrl.GetCurSel();

	if (pRow != NULL)
	{
		CString strNetId = pRow->GetItem(0)->GetValue();

		for (INT i = 0; i < m_ReturnInfoArray.GetCount(); i++)
		{
			ReturnInfo & rInfo = m_ReturnInfoArray.GetAt(i);

			if (rInfo.bLocal)
			{
				if (rInfo.LocalInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
	/*				CActiveMember ActiveMember;
					ActiveMember.SetIsLocalUser(TRUE);
					rInfo.bLocalReturned = CCashierHelper::ReturnLocalUser(ActiveMember,
						&rInfo.LocalInfo);

					if (rInfo.bLocalReturned)
					{
						try
						{
							pRow->GetItem(5)->SetValue(_T("已退款"));
							CBCGPGridItem* pItem = new CBCGPGridItem(_T(""));
							pRow->ReplaceItem(6, pItem);
							m_GridCtrl.RedrawWindow();
							m_GridCtrl.UpdateWindow();
						}
						//catch(...){}
					}*/
				}
			}
			else
			{
				if (rInfo.CheckOutInfo.GetSerialNum().CompareNoCase(strNetId) == 0)
				{
					CActiveMember *pActiveMember = NULL;
					for(int ar = 0 ; ar < arrMembers.GetCount(); ar++)
					{
						if (arrMembers.GetAt(ar).GetNetId().CompareNoCase(strNetId) == 0)
						{
							pActiveMember = &arrMembers.GetAt(ar);
						}
					}

					if (!pActiveMember)
					{
						continue;
					}

					rInfo.CheckOutInfo.SetReturnState(1);
					//BOOL bResult = CCashierHelper::ReturnLocalCenterUser(
					//	*pActiveMember, &rInfo.CheckOutInfo);
					//BOOL bResult = CIBADAL::GetInstance()->UpdateCheckOutInfo(strNetId, 1);


					//if (bResult)
					//{
					//	try
					//	{
					pRow->GetItem(5)->SetValue(_T("已退款"));
					CBCGPGridItem* pItem = new CBCGPGridItem(_T(""));
					pRow->ReplaceItem(6, pItem);
					m_GridCtrl.RedrawWindow();
					m_GridCtrl.UpdateWindow();
					//	}
					//	catch(...){}
					//}
				}
			}
		}

	}
}

void CDlgAllCheckOut::OnBnClickedButtonPrint()
{
	m_GridCtrl.Print();
}


// 返回-1表示无效
int CDlgAllCheckOut::GetActiveMembers(CArray<CActiveMember>& ActiveMembers)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL = _T("select activemember.*, member.classId as userClassId from activemember");

	strSQL.Append(_T(" left outer join member on activemember.memberId = member.memberId where activemember.memberId <> 0"));

	IBA_TRACE(strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL)) 
	{
		bRet = TRUE;
	}
	else
	{
		pDb->Release();
		return FALSE;
	}

	for (UINT i = 0; i < Rs.GetRecordCount(); i++)
	{
		CActiveMember ActiveMember;

		CString strTmp; INT nTmp = 0;

		if (Rs.GetFieldValue(_T("memberId"), nTmp))
		{
			ActiveMember.SetMemberID(nTmp);
		}

		if (Rs.GetFieldValue(_T("netId"), strTmp))
		{
			ActiveMember.SetNetId(strTmp);
		}

		if (Rs.GetFieldValue(_T("userClassId"), nTmp))
		{
			ActiveMember.SetClassId(nTmp);
		}

		if (Rs.GetFieldValue(_T("name"), strTmp))
		{
			ActiveMember.SetUserName(strTmp);
		}

		if (Rs.GetFieldValue(_T("sex"), nTmp))
		{
			ActiveMember.SetSex(nTmp);
		}

		if (Rs.GetFieldValue(_T("idNumber"), strTmp))
		{
			ActiveMember.SetPersonalID(strTmp);
		}

		if (Rs.GetFieldValue(_T("idType"), nTmp))
		{
			ActiveMember.SetIdType(nTmp);
		}

		if (Rs.GetFieldValue(_T("lastRemainAmount"), nTmp))
		{
			ActiveMember.SetAvailavleBalance(nTmp);
		}

		if (Rs.GetFieldValue(_T("amount"), nTmp))
		{
			ActiveMember.SetAmount(nTmp);
		}

		if (Rs.GetFieldValue(_T("refNo"), nTmp))
		{
			ActiveMember.SetIsOnLine( nTmp > 0 );

			ActiveMember.SetRefNo(nTmp);
		}

		if (Rs.GetFieldValue(_T("status"), nTmp))
		{
			ActiveMember.SetStatus(nTmp);
		}

		Rs.GetFieldValue(_T("activationTime"), ActiveMember.SET(ActivationTime));

		Rs.GetFieldValue(_T("updateTime"), ActiveMember.SET(UpdateDataTime));

		Rs.GetFieldValue(_T("checkinTime"), ActiveMember.SET(CheckInTime));

		if (Rs.GetFieldValue(_T("termId"), strTmp))
		{
			ActiveMember.SetTerminalID(strTmp);
		}

		if (Rs.GetFieldValue(_T("pcClass"), nTmp))
		{
			ActiveMember.SetPCClassID(nTmp);
		}

		if (Rs.GetFieldValue(_T("isActive"), nTmp))
		{
			ActiveMember.SetActive(nTmp);
		}

		if (Rs.GetFieldValue(_T("payType"), nTmp))
		{
			ActiveMember.SetPayType(nTmp);
		}

		if (Rs.GetFieldValue(_T("advancePayment"), nTmp))
		{
			ActiveMember.SetAdvancePayment(nTmp);
		}

		if (Rs.GetFieldValue(_T("creditProcess"), strTmp))
		{
			ActiveMember.SetCreditProcess(strTmp);
		}

		// 2011-3-29-gxx-添加收银台ID的读取
		if (Rs.GetFieldValue(_T("lastNetBarId"), nTmp))
		{
			ActiveMember.SetCasherID(nTmp);
		}

		// 2011/05/13-gxx: 添加备注
		if (Rs.GetFieldValue(_T("Remark"), strTmp))
		{
			ActiveMember.SetRemark(strTmp);
		}

		// 2011/05/19-gxx: 开户是否为包时
		if (Rs.GetFieldValue(_T("registerBT"), nTmp))
		{
			ActiveMember.SetIsRegisterBT(nTmp);
		}

		// 2011/07/21-8201-gxx: {
		// 累计封顶额
		if (Rs.GetFieldValue(_T("dynamicMaxAmount"), nTmp))
		{
			ActiveMember.SetDynamicMaxAmount(nTmp);
		}

		// 是否本地用户
		if (Rs.GetFieldValue(_T("isLocal"), nTmp))
		{
			ActiveMember.SetIsLocalUser(nTmp);
		}

		// 上机密码, MD5值
		if (Rs.GetFieldValue(_T("password"), strTmp))
		{
			ActiveMember.SetPassword(strTmp);
		}

		// 扣率
		if (Rs.GetFieldValue(_T("PolicyInfo"), strTmp))
		{
			ActiveMember.SetPolicyInfo(strTmp);
		}
		//}

		ActiveMembers.Add(ActiveMember);

		Rs.MoveNext();
	}

	IBA_TRACE("用户总数为:%d",Rs.GetRecordCount());

	Rs.Close();

	pDb->Release();

	//CLocalServer::GetInstance()->ActiveMemberList.Copy(ActiveMembers);

	return ActiveMembers.GetCount();
}

//会员结账
BOOL CDlgAllCheckOut::PayoutMember(UINT nMemberId)
{
	CActiveMember ActiveMember;
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);

	//会员在线
	if (ActiveMember.GetIsOnLine())
	{
		//人工登出会员
		CCheckout2Method Checkout2;
		Checkout2.SetSafeLogout(0); 
		Checkout2.SetMemberId(nMemberId);
		BOOL bRes = theApp.GetCurCashier()->DoCheckout2(Checkout2);	

		//只要连接成功
		if(TRUE == bRes || -1!= Checkout2.GetStatusCode())
		{
			//更改终端信息并更新终端视图(清除用户信息,状态改为锁屏)
			INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
			if (nIndex >= 0) 
			{
				if(0 == Checkout2.GetStatusCode() || TRUE == bRes )
				{
					CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId,2);//设置为2,会员下机
				}
				else
				{
					//如用户已登出或其他错误情况,向UDO说明是换机.
					CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId,0);//设置为0,换机/余额不足被踢出
				}				
				Sleep(1000);
				CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
				computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
				computer.ClearUserInfo();
				CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);
				CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
			}
		
		}
		else//请求失败,返回-1.返回错误信息
		{
			return FALSE;
		}
	}
	else//会员不在线.取消激活
	{
		//取消激活,从激活列表中删除,更新视图
		CActivationMethod Activation;
		Activation.SetMemberId(nMemberId);
		Activation.SetMode(CActivationMethod::ACTIVATION_NO);

		//本地模式
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			//查询LocalMember是否有记录
			CLocalMember lMember;
			BOOL bHasRecord = CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,lMember);
			if(bHasRecord)//有记录,更新退款信息
			{
				COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
				if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(nMemberId,CheckoutTime,TRUE))
				{
					IBA_LOG(_T("Error.更新本地结账记录失败."));
				}
			}
			else//无记录,创建记录,插入记录,更新记录
			{
				GxxDictionaryPtr lmPrt = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(nMemberId);  //创建记录
				if(lmPrt)
				{
					if(CIBADAL::GetInstance()->InsertNewLocalMember(lmPrt))//插入记录
					{
						COleDateTime CheckoutTime(0,0,0,0,0,0);//取消激活时,登出时间设为无效
						if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(nMemberId,CheckoutTime,TRUE))//更新记录
						{
							IBA_LOG(_T("Error.更新本地结账记录失败."));
						}	
					}
					else
					{
						IBA_LOG(_T("Error.为结账插入本地记录失败."));
					}
				}
				else
				{
					IBA_LOG(_T("Error.为结账创建本地记录失败."));
				}
			}
		}
		else//正常模式
		{
			//取消激活
			theApp.GetCurCashier()->DoActivation(Activation);
		}

		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(nMemberId);
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
	}

	//输出操作日志
	COperationInfo OperationInfo;
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)//本地模式 - 本地结账
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturnVip);
	}
	else
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeVipReturn);
	}		
	OperationInfo.SetNetId(ActiveMember.GetNetId());
	OperationInfo.SetUserName(ActiveMember.GetUserName());
	OperationInfo.SetDescription(ActiveMember.GetTerminalID());
	OperationInfo.SetClassId(ActiveMember.GetClassId());
	OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
	OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
	OperationInfo.SetTermId(ActiveMember.GetTerminalID());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

	return TRUE;
}

//临时用户退款
BOOL CDlgAllCheckOut::DeregisterUser(UINT nMemberId,UINT nReturnMode/* = 0*/)
{
	CActiveMember ActiveMember;
	if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember))
	{
		return FALSE;
	}
	//用户在线,通知UDO下机
	if(ActiveMember.GetIsOnLine())
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
		if(nIndex >= 0)
		{
			if (nMemberId == ActiveMember.GetRoomMemberId()) //包房用户,nCheckoutType = 2
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId, 2);
			}
			else												//非包房用户,nCheckoutType = 0
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId, 0);
			}
			Sleep(500);
		}
	}
	//临时用户退款
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(nMemberId);
	Deregister.SetPersonalId(ActiveMember.GetPersonalID());
	Deregister.SetMemberName(ActiveMember.GetUserName());
	//Deregister.SetCardId();
	Deregister.SetPassword(ActiveMember.GetPassword());
	//Deregister.SetEndUsingTime(m_strLastLogoutTime);

	BOOL bRes = theApp.GetCurCashier()->DoDeregister(Deregister,nReturnMode); 

	//退款失败,返回错误信息
	if(FALSE == bRes)
	{
		return FALSE;
	}
	else//成功-输出操作日志
	{
		UINT nAll = 0;
		//本地模式,本地退款
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			UINT nDeposit=0;
			CIBADAL::GetInstance()->GetDepositFromMember(ActiveMember.GetNetId(),nDeposit);
			nAll = ActiveMember.GetAvailavleBalance() + nDeposit;
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturn);//本地退款
			OperationInfo.SetUserName(ActiveMember.GetUserName());
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetOperationAmount(ActiveMember.GetAvailavleBalance());
			OperationInfo.SetDeposit(nDeposit);
			OperationInfo.SetDescription(ActiveMember.GetTerminalID());
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}
		else//正常模式,退款
		{
			nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeReturn);//退款
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(ActiveMember.GetUserName());
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetMemberId(nMemberId);
			OperationInfo.SetOperationAmount(Deregister.GetBalance());
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
			OperationInfo.SetOperationTime(ActiveMember.GetActivationTime());
			OperationInfo.SetDescription(ActiveMember.GetTerminalID());
			OperationInfo.SetClassId(ActiveMember.GetClassId());
			OperationInfo.SetTermId(ActiveMember.GetTerminalID());
			OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
			OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
			OperationInfo.SetConsumeMoney(ActiveMember.GetAdvancePayment()-nAll);

			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}
 
		//更新终端信息(清除用户信息,刷新视图) 
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
		if (nIndex >= 0)                                                                      
		{
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			computer.ClearUserInfo();
			CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);
			CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
		}
	}
	return TRUE;
}