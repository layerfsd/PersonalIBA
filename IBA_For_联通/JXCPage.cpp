// JXCPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "dal\IBADAL.h"
#include ".\JXCPage.h"


// CJXCPage 对话框

IMPLEMENT_DYNAMIC(CSetPageOfJXC, CIBAPage)
CSetPageOfJXC::CSetPageOfJXC()
: CIBAPage(CSetPageOfJXC::IDD)
, m_bPayCash(FALSE)
{
}

CSetPageOfJXC::~CSetPageOfJXC()
{
}

void CSetPageOfJXC::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_STORE, m_cboStore);
	DDX_Check(pDX, IDC_CHECK_PAY_TYPE, m_bPayCash);
}


BEGIN_MESSAGE_MAP(CSetPageOfJXC, CIBAPage)
END_MESSAGE_MAP()


// CJXCPage 消息处理程序

void CSetPageOfJXC::OnOK()
{
	if (m_cboStore.GetCurSel() >= 0)
	{
		CIBAConfig::GetInstance()->SetStoreID(m_cboStore.GetItemData(m_cboStore.GetCurSel()));
	}

	UpdateData(TRUE);
	CIBAConfig::GetInstance()->SetJXCPayCash(m_bPayCash);

	CIBAPage::OnOK();
}

BOOL CSetPageOfJXC::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	LoadStoreInfo();

	m_bPayCash = CIBAConfig::GetInstance()->GetJXCPayCash();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CSetPageOfJXC::LoadStoreInfo()
{
	BOOL bRet = FALSE;

	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(_T("select * from netbarstore")))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CString strTmp; INT nTmp = 0;

			Rs.GetFieldValue(_T("storeName"), strTmp);
			m_cboStore.AddString(strTmp);

			Rs.GetFieldValue(_T("storeId"), nTmp);
			m_cboStore.SetItemData(i, nTmp);

			if (CIBAConfig::GetInstance()->GetStoreID() == nTmp)
			{
				m_cboStore.SetCurSel(i);
			}

			Rs.MoveNext();

			bRet = TRUE;
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}