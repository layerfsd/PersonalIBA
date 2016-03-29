// ClientConfigBundleTimePage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\clientconfigbundletimepage.h"


// CClientConfigBundleTimePage 对话框

IMPLEMENT_DYNAMIC(CClientConfigBundleTimePage, CIBAPage)
CClientConfigBundleTimePage::CClientConfigBundleTimePage()
: CIBAPage(CClientConfigBundleTimePage::IDD)
, m_bAllowBundTime(FALSE)
, m_nBTLimitTime(0)
{
	
}

CClientConfigBundleTimePage::~CClientConfigBundleTimePage()
{
}

void CClientConfigBundleTimePage::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_ALLOWBUNDTIME, m_bAllowBundTime);
	DDX_Text(pDX, IDC_EDIT_BT_LIMIT, m_nBTLimitTime);
}


BEGIN_MESSAGE_MAP(CClientConfigBundleTimePage, CIBAPage)
END_MESSAGE_MAP()


// CClientConfigBundleTimePage 消息处理程序

void CClientConfigBundleTimePage::OnOK()
{
	UpdateData();

	//允许包时
	if (m_bAllowBundTime != m_NetBarConfig2.GetAllowBundleTime())
	{
		m_NetBarConfig2.SetAllowBundleTime(m_bAllowBundTime);
	}

	if (((CComboBox*)GetDlgItem(IDC_COMBO_TEMP))->GetCurSel() != m_NetBarConfig2.GetBundleTimeEndTemp())
	{
		m_NetBarConfig2.SetBundleTimeEndTemp(((CComboBox*)GetDlgItem(IDC_COMBO_TEMP))->GetCurSel());
	}

	if (((CComboBox*)GetDlgItem(IDC_COMBO_MEMBER))->GetCurSel() != m_NetBarConfig2.GetBundleTimeEnd())
	{
		m_NetBarConfig2.SetBundleTimeEnd(((CComboBox*)GetDlgItem(IDC_COMBO_MEMBER))->GetCurSel());
	}
	
	if (m_nBTLimitTime==0)
	{
		CNetBarConfig::GetInstance()->SetBTLimitTime(50);
	}
	else
	{
		CNetBarConfig::GetInstance()->SetBTLimitTime(m_nBTLimitTime);
	}

	CIBAPage::OnOK();
}

BOOL CClientConfigBundleTimePage::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	m_NetBarConfig2.LoadFromDB();

	m_nBTLimitTime = CNetBarConfig::GetInstance()->GetBTLimitTime();

	//允许包时
	m_bAllowBundTime = m_NetBarConfig2.GetAllowBundleTime();
	
	((CComboBox*)GetDlgItem(IDC_COMBO_TEMP))->SetCurSel(m_NetBarConfig2.GetBundleTimeEndTemp());
	((CComboBox*)GetDlgItem(IDC_COMBO_MEMBER))->SetCurSel(m_NetBarConfig2.GetBundleTimeEnd());

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
