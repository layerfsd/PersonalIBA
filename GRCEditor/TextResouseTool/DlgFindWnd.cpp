// DlgFindWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "TextResouseTool.h"
#include "DlgFindWnd.h"
#include ".\dlgfindwnd.h"

#include "TextResouseToolDlg.h"
// CDlgFindWnd 对话框

IMPLEMENT_DYNAMIC(CDlgFindWnd, CDialog)
CDlgFindWnd::CDlgFindWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindWnd::IDD, pParent)
	, m_bCase(FALSE)
	, m_bAll(FALSE)
	, m_strKeyword(_T(""))
{
	m_nLastResult = -1;
}

CDlgFindWnd::~CDlgFindWnd()
{
}

void CDlgFindWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_CAP, m_bCase);
	DDX_Check(pDX, IDC_CHECK_WHOLE, m_bAll);
	DDX_CBString(pDX, IDC_COMBO1, m_strKeyword);
}


BEGIN_MESSAGE_MAP(CDlgFindWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgFindWnd 消息处理程序

void CDlgFindWnd::OnBnClickedOk()
{
	UpdateData();

	CTextResouseToolDlg* pParentDlg = (CTextResouseToolDlg*)GetOwner();
	ASSERT(pParentDlg->IsKindOf(RUNTIME_CLASS(CTextResouseToolDlg)));

	if (m_strLastKeyword != m_strKeyword)
	{
		m_nLastResult = -1;
	}

	BOOL bFound = pParentDlg->Search(m_strKeyword, m_bCase, m_bAll, m_nLastResult, m_nLastResult);

	CComboBox *pComb = (CComboBox *)GetDlgItem(IDC_COMBO1);
	if ( pComb->FindString(0, m_strKeyword) < 0 )
	{
		pComb->AddString(m_strKeyword);
	}


	m_strLastKeyword = m_strKeyword;

	if (!bFound)
	{
		MessageBox(_T("没有找到任何结果"));
	}
}

void CDlgFindWnd::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgFindWnd::SearchNext()
{
	if (!IsWindow(GetSafeHwnd()))
	{
		GXX_ASSERT(0,"窗口无效");
		return;
	}
	OnBnClickedOk();
}