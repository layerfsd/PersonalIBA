#include "stdafx.h"
#include ".\ibapropertysheet.h"
#include "IBA.h"
#include "Cashier.h"

IMPLEMENT_DYNAMIC(CSetOfIBAPropertySheet, CIBASheet)


BEGIN_MESSAGE_MAP(CSetOfIBAPropertySheet, CIBASheet)

END_MESSAGE_MAP()

CSetOfIBAPropertySheet::CSetOfIBAPropertySheet(CWnd* pParentWnd)
: CIBASheet(IDS_IBASET, pParentWnd)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SetLook(CIBASheet::PropSheetLook_List);

	AddPage(&m_IBASetPage);
	AddPage(&m_MonitorSetPage);
	AddPage(&m_MonitorSetPage2);
	AddPage(&m_OnOffDutySetPage);
	AddPage(&m_BusinessSetPage);
	AddPage(&m_CreditSetPage);
	AddPage(&m_JXCSetPage);
	AddPage(&m_TTSSetPage);
	AddPage(&m_SeatSetPage);
	AddPage(&m_PrintSetPage);

	
	EnableVisualManagerStyle(); 
}

CSetOfIBAPropertySheet::~CSetOfIBAPropertySheet(void)
{
}

BOOL CSetOfIBAPropertySheet::OnInitDialog()
{
	BOOL bResult = CIBASheet::OnInitDialog();
	

	return bResult;
}
