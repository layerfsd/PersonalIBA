#pragma once

#include "IBASetPage.h"
#include "MonitorSetPage.h"
#include "IBABusinessSet.h"
#include "JXCPage.h"
#include "TTSSetPage.h"
#include "SeatSetPage.h"
#include "creditsetpage.h"
#include "onoffdutysetpage.h"
#include "monitorset2dlg.h"
#include "IBAStringLoader.h"
#include "PrintSetPage.h"
#include "ClearDataSetPage.h"

class CSetOfIBAPropertySheet : public CIBASheet
{
	DECLARE_DYNAMIC(CSetOfIBAPropertySheet)
	DECLARE_MESSAGE_MAP()

public:
	CSetOfIBAPropertySheet(CWnd* pParentWnd = NULL);
	~CSetOfIBAPropertySheet(void);

protected:
	
	virtual BOOL OnInitDialog();

private:

	CSetPageOfCommon m_IBASetPage;
	CSetPageOfMonitor m_MonitorSetPage;
	CSetPageOfRegister m_BusinessSetPage;
	CSetPageOfJXC m_JXCSetPage;
	CSetPageOfTTS m_TTSSetPage;
	CSetPageOfSeat m_SeatSetPage;
	CSetPageOfMonitor2 m_MonitorSetPage2;
	//充值退款设置
	CSetPageOfCredit m_CreditSetPage;
	//上下班设置
	CSetPageOfDuty m_OnOffDutySetPage;
	CPrintSetPage m_PrintSetPage;
	// 2014-10-21 - qsc
	CClearDataSetPage m_ClearDataSetPage;
};
