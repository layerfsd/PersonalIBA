// JXCMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\IBA.h"
#include ".\JXCMainDlg.h"
#include "..\Cashier.h"
#include "..\IBAHelpper.h"

CJXCMainDlg* CJXCMainDlg::lpJXCMainDlg = NULL;

void CJXCMainDlg::ShowJxcMainDlg()
{
	if (NULL == lpJXCMainDlg)
	{
		lpJXCMainDlg = new CJXCMainDlg;
		lpJXCMainDlg->Create(CJXCMainDlg::IDD, GetDesktopWindow());
		lpJXCMainDlg->CenterWindow(GetDesktopWindow());
	}

	lpJXCMainDlg->BringWindowToTop();//置为最前端
	lpJXCMainDlg->ShowWindow(SW_NORMAL);//显示
	lpJXCMainDlg->RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	
}

void CJXCMainDlg::DestroyJxcMainDlg()
{
	if (lpJXCMainDlg != NULL)
	{
		lpJXCMainDlg->DestroyWindow();
		delete lpJXCMainDlg;
		lpJXCMainDlg = NULL;
	}
}

BOOL CJXCMainDlg::IsJxcMainDlgCreated()
{
	if (lpJXCMainDlg != NULL)
	{
		return IsWindow(lpJXCMainDlg->GetSafeHwnd());
	}

	return FALSE;
}

void CJXCMainDlg::AutoChangeSkin()
{
	if (IsJxcMainDlgCreated())
	{
		lpJXCMainDlg->RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}
}

void CJXCMainDlg::RetailOnline(CString strTermId, CString strNetId)
{
	ShowJxcMainDlg();
	lpJXCMainDlg->m_wndTab.SetActiveTab(1);
	lpJXCMainDlg->m_RetailOnlineDlg.InitUserData(strTermId, strNetId);
}

//******************************************************************************

// CJXCMainDlg 对话框

IMPLEMENT_DYNAMIC(CJXCMainDlg, CIBADialog)
CJXCMainDlg::CJXCMainDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CJXCMainDlg::IDD, pParent)
{
}

CJXCMainDlg::~CJXCMainDlg()
{
}

void CJXCMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CJXCMainDlg, CIBADialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CJXCMainDlg 消息处理程序

BOOL CJXCMainDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	CreateTab();

	//{ 2011/08/31-8201-gxx: 
	
	CIBAString strJXCTabs;
	//销  售|在线购物|退  货|赠  送|积分兑换|入  库|查  询
	strJXCTabs.LoadString(IDS_JXC_TABS);
	CStringArray strArr;
	CIBAHelpper::SplitLine(strJXCTabs,strArr);

	UINT nIDs[7] ={
		CJXCSellDlg::IDD,
		CJXCRetailOnlineDlg::IDD,
		CJXCReturnDlg::IDD,
		CJXCPresentDlg::IDD,
		CJXCExchangeDlg::IDD,
		CJXCStoreDlg::IDD,
		CJXCQueryDlg::IDD
	};
	CDialog *pDlgs[7] ={
		&m_SellDlg,
		&m_RetailOnlineDlg,
		&m_ReturnDlg,
		&m_PresentDlg,
		&m_ExchangeDlg,
		&m_StoreDlg,
		&m_QueryDlg
	};
	if (strArr.GetCount() != 7)
	{
		strArr.RemoveAll();
		strJXCTabs = _T("销  售|在线购物|退  货|赠  送|积分兑换|入  库|查  询");
		CIBAHelpper::SplitLine(strJXCTabs,strArr);
	}
	
	for(int i=0; i < strArr.GetCount(); i++)
	{
		pDlgs[i]->Create(nIDs[i], &m_wndTab);
		m_wndTab.AddTab(pDlgs[i], strArr.GetAt(i));
	}

	//}

	//m_SellDlg.Create(CJXCSellDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_SellDlg, _T(""));       

	//m_RetailOnlineDlg.Create(CJXCRetailOnlineDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_RetailOnlineDlg, _T("在线购物"));

	//m_ReturnDlg.Create(CJXCReturnDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_ReturnDlg, _T("退  货"));

	//m_PresentDlg.Create(CJXCPresentDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_PresentDlg, _T("赠  送"));

	//m_ExchangeDlg.Create(CJXCExchangeDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_ExchangeDlg, _T("积分兑换"));

	//m_StoreDlg.Create(CJXCStoreDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_StoreDlg, _T("入  库"));
	//
	//m_QueryDlg.Create(CJXCQueryDlg::IDD, &m_wndTab);
	//m_wndTab.AddTab(&m_QueryDlg, _T("查  询"));
	
	m_SellDlg.GetDlgItem(IDC_EDIT_COMMODITYID)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJXCMainDlg::OnClose()
{	
	CIBADialog::OnClose();
	
	ShowWindow(SW_HIDE);

	DestroyJxcMainDlg();
}

void CJXCMainDlg::CreateTab()
{
	CRect rectTab;
	GetClientRect(&rectTab);
	rectTab.left = 5;
	rectTab.top = 5;
	rectTab.bottom = rectTab.Height();
	rectTab.right = rectTab.Width();

	m_wndTab.EnableTabSwap(FALSE);

	m_wndTab.Create(CBCGPTabWnd::STYLE_3D_ONENOTE, rectTab, this, 1, CBCGPTabWnd::LOCATION_TOP);
}

void CJXCMainDlg::OnDestroy()
{
	m_SellDlg.DestroyWindow();
	m_ReturnDlg.DestroyWindow();
	m_ExchangeDlg.DestroyWindow();
	m_QueryDlg.DestroyWindow();
	m_StoreDlg.DestroyWindow();
	m_RetailOnlineDlg.DestroyWindow();

	m_wndTab.DestroyWindow();

	CIBADialog::OnDestroy();
}

void CJXCMainDlg::OnNcDestroy()
{
	CIBADialog::OnNcDestroy();

	//AfxGetMainWnd()->PostMessage(WM_CHILDCLOSE, CJXCMainDlg::IDD);
}




