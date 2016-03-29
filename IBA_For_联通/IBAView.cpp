
#include "stdafx.h"
#include "IBA.h"
#include "ComputerListView.h"
#include "ActiveMemberView.h"
#include "NDTermConsumeView.h"
#include "OperationView.h"
#include "ShiftCashView.h"
#include "ComsumeListView.h"
#include "SeatLayoutView.h"
#include "Cashier.h"
#include "shortmsgview.h"
#include "cardopenconsumedlg.h"
#include ".\IBAView.h"

#include "DodonewView.h"
#include "CurrentNetBarUser.h"
#include "NDCreditDlg.h"
#include "NDReturnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CIBAView

IMPLEMENT_DYNCREATE(CIBAView, CBCGPTabView)

BEGIN_MESSAGE_MAP(CIBAView, CBCGPTabView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_CHILDCLOSE, OnChildClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIBAView construction/destruction

CIBAView::CIBAView()
: m_hTermConsumeView(NULL)
, m_hSeatView(NULL)
, m_hShortMsgView(NULL)
, m_hShiftCashView(NULL)
, m_hCardOpenConsumeView(NULL)
, m_hComsumeListView(NULL)
, m_hOperationView(NULL)
, m_hDodonewView(NULL) // 2011/05/06-gxx: 
{

}

CIBAView::~CIBAView()
{
}


CIBADoc* CIBAView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIBADoc)));
	return (CIBADoc*)m_pDocument;
}

void CIBAView::OnInitialUpdate()
{
	CBCGPTabView::OnInitialUpdate();
}

int CIBAView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CBCGPTabWnd::STYLE_3D_ONENOTE, rectDummy, this, 1))
	{
		TRACE0("Failed to create tab window\n");
		return -1;      // fail to create
	}

	theApp.SetIBAView(this);

	m_wndTabs.SetFlatFrame();
	m_wndTabs.SetTabBorderSize(0);
	m_wndTabs.AutoDestroyWindow(FALSE);
	m_wndTabs.EnableTabSwap(FALSE);
	
	CIBAString strTmp;
	strTmp.LoadString(IDS_COMPUTERLIST);
	INT nIdx = AddView(RUNTIME_CLASS(CComputerListView), strTmp);

	strTmp.LoadString(IDS_ACTIVEMEMBERVIEW);
	nIdx = AddView(RUNTIME_CLASS(CActiveMemberView), strTmp);

	if (CIBAConfig::GetInstance()->GetDisplayType())
	{
		strTmp.LoadString(IDS_NETBARSEAT);
		nIdx = AddView(RUNTIME_CLASS(CSeatLayoutView), strTmp);
		m_hSeatView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();
		GetTabControl().ShowTab(nIdx, TRUE);//显示座位视图
	} 

	//strTmp.LoadString(IDS_OPERATIONQUERY);
	//nIdx = AddView(RUNTIME_CLASS(COperationView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hOperationView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	//strTmp.LoadString(IDS_SHIFICASHQUERY);
	//nIdx = AddView(RUNTIME_CLASS(CShiftCashView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hShiftCashView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	//strTmp.LoadString(IDS_CONSUMELISTQUERY);
	//nIdx = AddView(RUNTIME_CLASS(CComsumeListView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hComsumeListView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	//strTmp.LoadString(IDS_TERMCONSUME);
	//nIdx = AddView(RUNTIME_CLASS(CNDTermConsumeView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hTermConsumeView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	//strTmp.LoadString(IDM_SHORTMSGQUERY);
	//nIdx = AddView(RUNTIME_CLASS(CShortMsgView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hShortMsgView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	//strTmp.LoadString(IDS_CARDOPENCONSUME);
	//nIdx = AddView(RUNTIME_CLASS(CCardopenConsumeView), strTmp);
	//GetTabControl().ShowTab(nIdx, FALSE);
	//m_hCardOpenConsumeView = GetTabControl().GetTabWnd(nIdx)->GetSafeHwnd();

	INT nTmp = theApp.GetInt(_T("CurrentView"), 1);
	GetTabControl().EnsureVisible(nTmp);
	SetActiveView(nTmp);

	return 0;
}

BOOL CIBAView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CIBAView::OnActivateView(CView* pView)
{
	if (GetTabControl().GetActiveTab() < 2)
	{
		theApp.WriteInt(_T("CurrentView"), GetTabControl().GetActiveTab());

		m_wndTabs.EnableActiveTabCloseButton(FALSE);

		//关闭气泡提示
		::PostMessage(m_hComsumeListView, WM_KILLFOCUS, NULL, NULL);
		::PostMessage(m_hTermConsumeView, WM_KILLFOCUS, NULL, NULL);
		::PostMessage(m_hOperationView, WM_KILLFOCUS, NULL, NULL);
		::PostMessage(m_hShiftCashView, WM_KILLFOCUS, NULL, NULL);
		::PostMessage(m_hSeatView, WM_KILLFOCUS, NULL, NULL);
	}
	else
	{
		m_wndTabs.EnableActiveTabCloseButton();
	}
}

void CIBAView::ShowTermConsumeView()
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hTermConsumeView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_TERMCONSUME);
		nTabId = AddView(RUNTIME_CLASS(CNDTermConsumeView), strTmp);
		m_hTermConsumeView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

void CIBAView::QueryTermConsumeList(LPCTSTR lpszTermId)
{
	ShowTermConsumeView();
	((CNDTermConsumeView*)GetActiveView())->QueryTermId(lpszTermId);
}

void CIBAView::ShowComsumeListView()
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hComsumeListView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_CONSUMELISTQUERY);
		nTabId = AddView(RUNTIME_CLASS(CComsumeListView), strTmp);
		m_hComsumeListView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

void CIBAView::QueryUserComsumeList(LPCTSTR lpszNetId, UINT nMemberId /*= 0*/)
{
	ShowComsumeListView();
	((CComsumeListView*)GetActiveView())->QueryUserConsumeList(lpszNetId, nMemberId);
}

void CIBAView::ShowOperationView()
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hOperationView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_OPERATIONQUERY);
		nTabId = AddView(RUNTIME_CLASS(COperationView), strTmp);
		m_hOperationView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

void CIBAView::ShowShiftCashView()
{	
	//取得对应窗体的序号
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hShiftCashView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_SHIFICASHQUERY);
		nTabId = AddView(RUNTIME_CLASS(CShiftCashView), strTmp);
		m_hShiftCashView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

void CIBAView::ShowSeatLayoutView()
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hSeatView);

	if (nTabId < 0)//窗体存在
	{	
		CIBAString strTmp;
		strTmp.LoadString(IDS_NETBARSEAT);
		nTabId = AddView(RUNTIME_CLASS(CSeatLayoutView), strTmp);
		m_hShiftCashView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);

	CIBAConfig::GetInstance()->SetDisplayType(1);
}

BOOL CIBAView::FindComputer(UINT nKeyType, LPCTSTR lpszKeyValue)
{
	return ((CComputerListView*)(GetTabControl().GetTabWnd(0)))->FindComputer(nKeyType, lpszKeyValue);
}

// 显示客户端消息
void CIBAView::ShowShortMsgView(void)
{
	//取得对应窗体的序号
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hShortMsgView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_CLIENTINFO);
		nTabId = AddView(RUNTIME_CLASS(CShortMsgView), strTmp);
		m_hShiftCashView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

void CIBAView::QueryShortMsgList()
{
	ShowShortMsgView();
	((CShortMsgView*)GetActiveView())->QueryShortMsgList();
}
// 开户未消费查询显示
void CIBAView::ShowCardOpenConsumeView(void)
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hCardOpenConsumeView);

	if (nTabId < 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_CARDOPENCONSUME);
		nTabId = AddView(RUNTIME_CLASS(CCardopenConsumeView), strTmp);
		m_hCardOpenConsumeView= GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}

	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

LRESULT CIBAView::OnChildClose(WPARAM wp, LPARAM lp)
{
	INT nTabId = GetTabControl().GetTabFromHwnd((HWND)lp);

	if (nTabId >= 0)
	{
		GetTabControl().ShowTab(nTabId, FALSE);
		RemoveView(nTabId);
	}

	return 0;
}

void CIBAView::ShowDodonewView()
{
	INT nTabId = GetTabControl().GetTabFromHwnd(m_hDodonewView);

	if (nTabId < 0)
	{
		CIBAString strLabel;
		strLabel.LoadString(IDS_ZENGSHOUBAO);
		nTabId = AddView(RUNTIME_CLASS(CDodonewView), strLabel);
		m_wndTabs.AutoDestroyWindow(TRUE);
		m_hDodonewView = GetTabControl().GetTabWnd(nTabId)->GetSafeHwnd();
		((CView*)GetTabControl().GetTabWnd(nTabId))->OnInitialUpdate();
	}


	GetTabControl().ShowTab(nTabId, TRUE);
	GetTabControl().EnsureVisible(nTabId);
	SetActiveView(nTabId);
}

CActiveMemberView* CIBAView::GetActiveMemberView()
{
	ASSERT( IsWindow(m_hWnd) );

	CActiveMemberView *pMemberView = (CActiveMemberView*)GetTabControl().
		GetTabWnd(ACTIVEMEMBER_VIEW);

	ASSERT( pMemberView );
	ASSERT( pMemberView->IsKindOf( RUNTIME_CLASS(CActiveMemberView) ) );

	return pMemberView;
}

CComputerListView* CIBAView::GetComputerListView()
{
	ASSERT( IsWindow(m_hWnd) );

	CComputerListView *pComputerView = (CComputerListView*)GetTabControl().
		GetTabWnd(COMPUTERLIST_VIEW);

	ASSERT( pComputerView );
	ASSERT( pComputerView->IsKindOf( RUNTIME_CLASS(CComputerListView) ) );

	return pComputerView;
}

BOOL CIBAView::IsCurrentComputerView()
{
	CView * pView = GetActiveView();
	ASSERT(pView);

	if (pView->IsKindOf(RUNTIME_CLASS(CComputerListView)))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CIBAView::GotoActiveMember(CString strNetId,BOOL bActiveView)
{
	if (bActiveView)
	{
		GetTabControl().SetActiveTab(ACTIVEMEMBER_VIEW);
	}
	
	CActiveMemberView *pMemberView = GetActiveMemberView();
	pMemberView->SetSelUser(pMemberView->FindUserAt(0, strNetId), TRUE);

	CActiveMember ActiveMember;

	//{ 2011/08/01-8201-gxx: 
	if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strNetId, ActiveMember, FALSE))
	{
		// 本地用户或者断网中心用户
		if (ActiveMember.GetIsLocalUser() || CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			CNDReturnDlg dlgReturn;
			dlgReturn.SetNetId(ActiveMember.GetNetId());
			dlgReturn.DoModal();
			return TRUE;
		}
	}
	else
	{
		//return FALSE;
	}
	//}
	
	if (CCurrentNetBarUser::GetInstance()->QueryBalance(strNetId))
	{
		BOOL bExists =  CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
			CCurrentNetBarUser::GetInstance()->Balance.GetMemberId(), ActiveMember);

		if (CCurrentNetBarUser::GetInstance()->Balance.IsNetbarMember() && !bExists)
		{
			CNDCreditDlg dlg;
			dlg.DoModal();
		}
		else
		{
			CNDReturnDlg dlg;
			dlg.DoModal();
		}

		return TRUE;
	}
	else
	{
		//
		//theApp.IBAMsgBox(IDS_HASNOTUSED);

		return FALSE;
	}
}

BOOL CIBAView::GotoActiveMemberEx(CString strTermID, CString* pBestTernID)
{
	// 查找终端对应的用户
	CActiveMember ActiveMember;
	BOOL bFound = CLocalServer::GetInstance()->ActiveMemberList.
		GetActiveMemberBest(strTermID,ActiveMember, FALSE);

	if (bFound)
	{
		if (pBestTernID)
		{
			*pBestTernID = ActiveMember.GetTerminalID();
		}
		return GotoActiveMember(ActiveMember.GetNetId(), FALSE);
	}
	else
	{
		return FALSE;
	}
}

void CIBAView::GotoComputer(CString strTermID, BOOL bActiveView)
{
	if (strTermID.IsEmpty())
	{
		return;
	}
	CComputerListView *pComputerView = GetComputerListView();

	if (pComputerView->FindComputer(0,strTermID) && bActiveView)
	{
		GetTabControl().SetActiveTab(COMPUTERLIST_VIEW);
	}
}

void CIBAView::GotoComputerByIP(CString strIP, BOOL bActiveView)
{
	CComputerListView *pComputerView = GetComputerListView();
	if (pComputerView->FindComputer(2,strIP) && bActiveView)
	{
		GetTabControl().SetActiveTab(COMPUTERLIST_VIEW);
	}
}

void CIBAView::GotoComputerByNetID(CString strNetId, BOOL bActiveView)
{
	CComputerListView *pComputerView = GetComputerListView();
	if (pComputerView->FindComputer(1,strNetId) && bActiveView)
	{
		GetTabControl().SetActiveTab(COMPUTERLIST_VIEW);
	}
}
