// ClientCOnfigUnlockPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\ClientCOnfigUnlockPage.h"


// CClientCOnfigUnlockPage 对话框

IMPLEMENT_DYNAMIC(CClientCOnfigUnlockPage, CIBAPage)
CClientCOnfigUnlockPage::CClientCOnfigUnlockPage()
: CIBAPage(CClientCOnfigUnlockPage::IDD)
, m_nMaxKeepTime(0) ,m_nMaxKeepTimeOfUKey(0)
{
}

CClientCOnfigUnlockPage::~CClientCOnfigUnlockPage()
{
}

void CClientCOnfigUnlockPage::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
	DDX_Text(pDX, IDC_EDIT_MAXKEEPTIME, m_nMaxKeepTime);
	DDX_Text(pDX, IDC_EDIT_MAXKEEPTIMEOFUKEY, m_nMaxKeepTimeOfUKey);
}


BEGIN_MESSAGE_MAP(CClientCOnfigUnlockPage, CIBAPage)
END_MESSAGE_MAP()

BOOL CClientCOnfigUnlockPage::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	m_NetBarConfig2.LoadFromDB();

	m_strPwd = m_NetBarConfig2.GetUnLockPassword();

	///////////////////////////////维护时间//////////////////////////
	m_nMaxKeepTime = m_NetBarConfig2.GetMaxKeepTime();
	m_nMaxKeepTimeOfUKey = m_NetBarConfig2.GetMaxKeepTimeOfUKey();

	/**
	*
	软件类型 
	0：IC卡
	1: ID卡
	2: 牛盾V1
	3：软件
	4: 牛盾V2
	*/

	if (CNetBarConfig::GetInstance()->GetSoftwareType() < 3)
	{
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_MAXKEEPTIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_MAXKEEPTIMEOFUKEY)->ShowWindow(SW_HIDE);
	}
	else if (CNetBarConfig::GetInstance()->GetSoftwareType() == 3)
	{
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_MAXKEEPTIMEOFUKEY)->ShowWindow(SW_HIDE);
	}

	if (CNetBarConfig::GetInstance()->GetDisallowUnlock() >= 1)
	{
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_MAXKEEPTIME)->ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);

	return TRUE;
}

void CClientCOnfigUnlockPage::OnOK()
{
	UpdateData();

	m_strPwd.Trim();

	if (m_strPwd != m_NetBarConfig2.GetUnLockPassword())
	{
		m_NetBarConfig2.SetUnLockPassword(m_strPwd);
	}

	//////////////////////////////////////////////////////////
	if (m_nMaxKeepTime != m_NetBarConfig2.GetMaxKeepTime())
	{
		m_NetBarConfig2.SetMaxKeepTime(m_nMaxKeepTime);
	}

	if (m_nMaxKeepTimeOfUKey != m_NetBarConfig2.GetMaxKeepTimeOfUKey())
	{
		m_NetBarConfig2.SetMaxKeepTimeOfUKey(m_nMaxKeepTimeOfUKey);
	}

	CIBAPage::OnOK();

}
// CClientCOnfigUnlockPage 消息处理程序
