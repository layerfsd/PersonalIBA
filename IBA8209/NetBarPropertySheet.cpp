// NetBarPropertySheet.cpp : 实现文件
//
#include "stdafx.h"
#include "IBA.h"
#include ".\netbarpropertysheet.h"


// CNetBarPropertySheet

IMPLEMENT_DYNAMIC(CNetBarPropertySheet, CBCGPPropertySheet)


BEGIN_MESSAGE_MAP(CNetBarPropertySheet, CBCGPPropertySheet)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
END_MESSAGE_MAP()

CNetBarPropertySheet::CNetBarPropertySheet(CWnd* pParentWnd /*= NULL*/)
: CBCGPPropertySheet(IDS_NETBARCONFIG, pParentWnd)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SetLook(CBCGPPropertySheet::PropSheetLook_List);
	//SetLook(CBCGPPropertySheet::PropSheetLook_Tabs);

	//AddPage(&m_NetBarInfo);
	//AddPage(&m_NetBarConfig);
	
	EnableVisualManagerStyle();
}

// CNetBarPropertySheet 消息处理程序

BOOL CNetBarPropertySheet::OnInitDialog()
{
	BOOL bResult = CBCGPPropertySheet::OnInitDialog();

	RegisterHotKey(m_hWnd, 0xC100, MOD_CONTROL, VK_F10);

	return bResult;
}

CNetBarPropertySheet::~CNetBarPropertySheet()
{
	
}


LRESULT CNetBarPropertySheet::OnHotKey(WPARAM wParam, LPARAM lParam)
{	
	if (m_wndList.GetCount() < 3)
	{
		//AddPage(&m_NetBarOtherInfo);
	}
	
	m_wndList.SetCurSel(2);
	OnSelectList();

	return 0;
}