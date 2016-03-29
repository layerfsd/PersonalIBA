// LogListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "CreditDlg.h"
#include "NDCreditDlg.h"
#include "MainFrm.h"
#include "ShortMsgDlg.h"
#include "Socket\LocalServer.h"
#include "ServletHellper.h"
#include ".\loglistctrl.h"
// COutputListCtrl

IMPLEMENT_DYNAMIC(COutputListCtrl, CBCGPListCtrl)
COutputListCtrl::COutputListCtrl()
{
}

COutputListCtrl::~COutputListCtrl()
{
}	


BEGIN_MESSAGE_MAP(COutputListCtrl, CBCGPListCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	//ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	//ON_COMMAND(IDM_EXPORTEXCEL, OnExportExcel)
	ON_WM_CREATE()
	ON_WM_TIMER()
	
END_MESSAGE_MAP()

// COutputListCtrl 消息处理程序

void COutputListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	int TabIndex = ((CMainFrame *)AfxGetMainWnd())->m_wndOutput.m_wndTabs.GetActiveTab();
	switch (TabIndex)
	{
	case 0://操作
		{
			INT nIdx = GetSelectionMark();
			if (nIdx >= 0 && CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
			{//退款时弹出消费查询
				if (CServletHellper::GetMemberId(GetItemText(nIdx, 3)) > 0)
				{
					CNDCreditDlg dlg;
					dlg.SetNetId(GetItemText(nIdx, 3));
					dlg.DoModal();
				} 
				else
				{
					theApp.GetIBAView()->QueryUserComsumeList(GetItemText(nIdx, 3), _ttoi(GetItemText(nIdx, 5)));
				}
			}
			break;
		}
	case 1://激活
		{
			INT index = GetSelectionMark();
			if (index >= 0 && CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
			{
				if (CServletHellper::GetMemberId(GetItemText(index, 2)) > 0)
				{
					CNDCreditDlg dlg;
					dlg.SetNetId(GetItemText(index, 2));
					dlg.DoModal();
				} 
				else
				{
					theApp.GetIBAView()->QueryUserComsumeList(GetItemText(index, 2));
				}
			}
			break;
		}
	case 3://客户端消息
		{
			INT nIdx = GetSelectionMark();

			if (nIdx >= 0)
			{
				CString strTermId;
				strTermId = GetItemText(nIdx, 1);

				//UINT rrr = (UINT)CLocalServer::GetInstance()->ComputerList.LookFor(0, strTermId);
				CShortMsgDlg MsgDlg;

				MsgDlg.AddReceiver((UINT)CLocalServer::GetInstance()->ComputerList.LookFor(0, strTermId));
				MsgDlg.DoModal();
			}
			break;
		}
	case 2://消费记录
		{
			//INT nIdx = GetSelectionMark();
			//if (nIdx >= 0)
			//{//退款时弹出消费查询
			//	theApp.GetIBAView()->QueryUserComsumeList(GetItemText(nIdx, 4), 0);
			//}
			break;
		}
	}

	*pResult = 0;
}

//void COutputListCtrl::OnNMRclick( NMHDR *pNMHDR, LRESULT *pResult )
//{
//	//NM_LISTVIEW* pNMListView = NULL;
//	//int TabIndex = ((CMainFrame *)AfxGetMainWnd())->m_wndOutput.m_wndTabs.GetActiveTab();
//	//switch (TabIndex)
//	//{
//	//case 2:		// 只有当前页面是结账记录页面的时候才做弹出"导出Excel"操作
//	//	pNMListView = (NM_LISTVIEW*)pNMHDR;
//	//	if(pNMListView->iItem != -1)
//	//	{
//	//		DWORD dwPos = GetMessagePos();
//	//		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
//
//	//		CMenu menu;
//	//		VERIFY( menu.LoadMenu( IDR_MENU_CHECKOUT ) );
//	//		CMenu* popup = menu.GetSubMenu(0);
//	//		ASSERT( popup != NULL );
//	//		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
//	//	} 
//	//	break;
//	//default:
//	//	break;
//	//}
//	*pResult = 0;
//}
//
//void COutputListCtrl::OnExportExcel()
//{
//	UpdateData();
//
//	//// 打开存储文件对话框
//	//CFileDialog dlgSave(FALSE, _T("xls"), _T("结账记录表"));
//	//dlgSave.m_ofn.lpstrFilter = _T("*.xls");
//	//if(IDOK != dlgSave.DoModal())
//	//	return;
//
//	//// 获取全路径
//	//CString strFullPath;
//	//strFullPath = dlgSave.GetPathName();
//	
//	// 获取行数和列数
//	int nRowCount = GetItemCount();
//	CBCGPHeaderCtrl& Header = GetHeaderCtrl();
//	int nColCount = Header.GetItemCount();
//
//
//	CString strAppend("");
//	CString strTmp("");
//
//	// 标题行
//	strAppend += _T("结账时间\t用户级别\t终端号\t上机时间\t上网账号\t姓名\t证件号码\t金额\t备注\n");
//	for(int nRow = 0; nRow < nRowCount; nRow++)
//	{
//		for(int nCol = 0; nCol < nColCount; nCol++)
//		{
//			strTmp.Empty();
//			strTmp = GetItemText(nRow, nCol);
//			// "\t" 表示跳到下一个单元格
//			strAppend += strTmp + _T("\t");
//		}
//		// "\n" 表示跳到下一行
//		strAppend += _T("\n");	
//	}
//
//	
//	CString strFullPath;
//	GetTempPath(MAX_PATH, strFullPath.GetBuffer(MAX_PATH));
//	strFullPath.ReleaseBuffer();
//
//	strFullPath += _T("结账记录表.xls");
//
//	// nRowCount+1 数据行数加上一行标题行
//	m_ExpThread.SetParam(strAppend, strFullPath, nRowCount + 1);
//	m_ExpThread.Start();
//}

int COutputListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return 0;
}


void COutputListCtrl::OnTimer(UINT nIDEvent)
{
	static int iCount = 0;
	if (nIDEvent==200)
	{
		iCount++;
		if (iCount%4==0)
		{
			UpdateWindow();
			Invalidate();
		}
		else
		{
			CRect rc;
			GetClientRect(rc);

			rc.DeflateRect(2,2,2,2);
			CClientDC dc(this);
			CBrush br;
			br.CreateSolidBrush(RGB(255,0,0));
			dc.FrameRect(rc,&br);
			rc.DeflateRect(1,1,1,1);
			dc.FrameRect(rc,&br);
		}
		if (iCount/4 == 2)
		{
			KillTimer(200);
			iCount = 0;
		}
	}

	//CBCGPListCtrl::OnTimer(nIDEvent);
}

void COutputListCtrl::ActiveRemind()
{
	SetTimer(200,200,0);
}
