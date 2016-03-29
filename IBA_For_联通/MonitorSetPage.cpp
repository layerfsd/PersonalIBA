// MonitorSetPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\monitorsetpage.h"


// CSetPageOfMonitor 对话框

IMPLEMENT_DYNAMIC(CSetPageOfMonitor, CIBAPage)
CSetPageOfMonitor::CSetPageOfMonitor()
: CIBAPage(CSetPageOfMonitor::IDD)
{
}

CSetPageOfMonitor::~CSetPageOfMonitor()
{
}

void CSetPageOfMonitor::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLONLINE, m_CPOnLine);
	DDX_Control(pDX, IDC_BUTTON_CLOFFLINE, m_CPOffLine);
	DDX_Control(pDX, IDC_BUTTON_CLLOCK, m_CPLockScreen);
	DDX_Control(pDX, IDC_BUTTON_CLLM, m_CPLittleMoney);
	DDX_Control(pDX, IDC_BUTTON_CLBLACK, m_CPBlack);
	DDX_Control(pDX, IDC_BUTTON_CLBD, m_CPBirthDay);
	DDX_Control(pDX, IDC_BUTTON_CLTHIEF, m_CPThief);
	DDX_Control(pDX, IDC_BUTTON_CLVIP, m_CPVIP);
	DDX_Control(pDX, IDC_BUTTON_CLSUSPEND, m_CPSuspend);
	DDX_Control(pDX, IDC_BUTTON_CLUNLOCK, m_CPUnLock);
	DDX_Control(pDX, IDC_BUTTON_BUNDTIMECOLOR, m_CPBundTimeColor);
}


BEGIN_MESSAGE_MAP(CSetPageOfMonitor, CIBAPage)
END_MESSAGE_MAP()


// CSetPageOfMonitor 消息处理程序

void CSetPageOfMonitor::OnOK()
{
	CIBAConfig::GetInstance()->SetOnLineColor(m_CPOnLine.GetColor());
	CIBAConfig::GetInstance()->SetOffLineColor(m_CPOffLine.GetColor());
	CIBAConfig::GetInstance()->SetSuspendColor(m_CPSuspend.GetColor());
	CIBAConfig::GetInstance()->SetThiefColor(m_CPThief.GetColor());
	CIBAConfig::GetInstance()->SetVIPColor(m_CPVIP.GetColor());
	CIBAConfig::GetInstance()->SetLockScreenColor(m_CPLockScreen.GetColor());
	CIBAConfig::GetInstance()->SetBirthDayColor(m_CPBirthDay.GetColor());
	CIBAConfig::GetInstance()->SetBlackColor(m_CPBlack.GetColor());
	CIBAConfig::GetInstance()->SetUnLockColor(m_CPUnLock.GetColor());
	CIBAConfig::GetInstance()->SetLittleMoneyColor(m_CPLittleMoney.GetColor());
	CIBAConfig::GetInstance()->SetBundTimeColor(m_CPBundTimeColor.GetColor());

	CIBAPage::OnOK();
}


BOOL CSetPageOfMonitor::OnInitDialog()
{
	CIBAPage::OnInitDialog();
	//设置状态颜色按钮配置
	CIBAString strTmp;
	strTmp.LoadString(IDS_MORE);//载入字串“更多...”

	const UINT ColumnsNumber = 10;//按钮列表列数

	m_CPOnLine.EnableOtherButton(strTmp);//使能按钮列表功能
	m_CPOnLine.SetColumnsNumber(ColumnsNumber);//设置列表的列数
	m_CPOnLine.SetColor(CIBAConfig::GetInstance()->GetOnLineColor());//取得配置文件的颜色

	m_CPOffLine.EnableOtherButton(strTmp);
	m_CPOffLine.SetColumnsNumber(ColumnsNumber);
	m_CPOffLine.SetColor(CIBAConfig::GetInstance()->GetOffLineColor());

	m_CPThief.EnableOtherButton(strTmp);
	m_CPThief.SetColumnsNumber(ColumnsNumber);
	m_CPThief.SetColor(CIBAConfig::GetInstance()->GetThiefColor());

	m_CPBlack.EnableOtherButton(strTmp);
	m_CPBlack.SetColumnsNumber(ColumnsNumber);
	m_CPBlack.SetColor(CIBAConfig::GetInstance()->GetBlackColor());

	m_CPBirthDay.EnableOtherButton(strTmp);
	m_CPBirthDay.SetColumnsNumber(ColumnsNumber);
	m_CPBirthDay.SetColor(CIBAConfig::GetInstance()->GetBirthDayColor());

	m_CPSuspend.EnableOtherButton(strTmp);
	m_CPSuspend.SetColumnsNumber(ColumnsNumber);
	m_CPSuspend.SetColor(CIBAConfig::GetInstance()->GetSuspendColor());

	m_CPLittleMoney.EnableOtherButton(strTmp);
	m_CPLittleMoney.SetColumnsNumber(ColumnsNumber);
	m_CPLittleMoney.SetColor(CIBAConfig::GetInstance()->GetLittleMoneyColor());

	m_CPUnLock.EnableOtherButton(strTmp);
	m_CPUnLock.SetColumnsNumber(ColumnsNumber);
	m_CPUnLock.SetColor(CIBAConfig::GetInstance()->GetUnLockColor());

	m_CPVIP.EnableOtherButton(strTmp);
	m_CPVIP.SetColumnsNumber(ColumnsNumber);
	m_CPVIP.SetColor(CIBAConfig::GetInstance()->GetVIPColor());

	m_CPLockScreen.EnableOtherButton(strTmp);
	m_CPLockScreen.SetColumnsNumber(ColumnsNumber);
	m_CPLockScreen.SetColor(CIBAConfig::GetInstance()->GetLockScreenColor());

	//////////////包时颜色按钮设置///////////////
	m_CPBundTimeColor.EnableOtherButton(strTmp);
	m_CPBundTimeColor.SetColumnsNumber(ColumnsNumber);
	m_CPBundTimeColor.SetColor(CIBAConfig::GetInstance()->GetBundTimeColor());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

