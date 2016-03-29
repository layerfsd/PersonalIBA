// PrintSetPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "PrintSetPage.h"


// CPrintSetPage 对话框

IMPLEMENT_DYNAMIC(CPrintSetPage, CIBAPage)
CPrintSetPage::CPrintSetPage()
	: CIBAPage(CPrintSetPage::IDD)
{
	m_nTicketFontSize = 9;

	m_bPrintTempReturn = FALSE;
	m_bPrintTempBT = FALSE;
	m_bPrintTempCredit = FALSE;
	m_bPrintTempOpen = FALSE;
	m_bPrintMemberReturn = FALSE;
	m_bPrintMemberBT = FALSE;
	m_bPrintMemberCredit = FALSE;
	m_bPrintMemberOpen = FALSE;
}

CPrintSetPage::~CPrintSetPage()
{
}

void CPrintSetPage::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_COMBO_FONTSIZE, m_nTicketFontSize);

	DDX_Check(pDX, IDC_CHECK_MEM_OPEN, m_bPrintMemberOpen);
	DDX_Check(pDX, IDC_CHECK_MEM_CREDIT, m_bPrintMemberCredit);
	DDX_Check(pDX, IDC_CHECK_MEM_BT, m_bPrintMemberBT);
	DDX_Check(pDX, IDC_CHECK_MEM_RETURN, m_bPrintMemberReturn);

	DDX_Check(pDX, IDC_CHECK_TEMP_OPEN, m_bPrintTempOpen);
	DDX_Check(pDX, IDC_CHECK_TEMP_CREDIT, m_bPrintTempCredit);
	DDX_Check(pDX, IDC_CHECK_TEMP_BT, m_bPrintTempBT);
	DDX_Check(pDX, IDC_CHECK_TEMP_RETURN, m_bPrintTempReturn);
}

BEGIN_MESSAGE_MAP(CPrintSetPage, CIBAPage)
END_MESSAGE_MAP()

void CPrintSetPage::OnOK()
{
	UpdateData();

	CIBAConfig::GetInstance()->SetPrintFontSize(m_nTicketFontSize);

	CIBAConfig::GetInstance()->SetPrintTicketOpenMember(m_bPrintMemberOpen);
	CIBAConfig::GetInstance()->SetPrintTicketCreditMember(m_bPrintMemberCredit);
	CIBAConfig::GetInstance()->SetPrintTicketBTMember(m_bPrintMemberBT);
	CIBAConfig::GetInstance()->SetPrintTicketReturnMember(m_bPrintMemberReturn);
	CIBAConfig::GetInstance()->SetPrintTicketOpenTemp(m_bPrintTempOpen);
	CIBAConfig::GetInstance()->SetPrintTicketCreditTemp(m_bPrintTempCredit);
	CIBAConfig::GetInstance()->SetPrintTicketBTTemp(m_bPrintTempBT);
	CIBAConfig::GetInstance()->SetPrintTicketReturnTemp(m_bPrintTempReturn);

	CIBAPage::OnOK();
}

BOOL CPrintSetPage::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	m_nTicketFontSize = CIBAConfig::GetInstance()->GetPrintFontSize();

	m_bPrintMemberOpen   = CIBAConfig::GetInstance()->GetPrintTicketOpenMember();
	m_bPrintMemberCredit = CIBAConfig::GetInstance()->GetPrintTicketCreditMember();
	m_bPrintMemberBT     = CIBAConfig::GetInstance()->GetPrintTicketBTMember();
	m_bPrintMemberReturn = CIBAConfig::GetInstance()->GetPrintTicketReturnMember();
	m_bPrintTempOpen     = CIBAConfig::GetInstance()->GetPrintTicketOpenTemp();
	m_bPrintTempCredit   = CIBAConfig::GetInstance()->GetPrintTicketCreditTemp();
	m_bPrintTempBT       = CIBAConfig::GetInstance()->GetPrintTicketBTTemp();
	m_bPrintTempReturn   = CIBAConfig::GetInstance()->GetPrintTicketReturnTemp();

	UpdateData(FALSE);

	return TRUE;
}
// CPrintSetPage 消息处理程序
