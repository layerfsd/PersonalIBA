// ClientConfigPropertySheet.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\clientconfigpropertysheet.h"


// CClientConfigPropertySheet

IMPLEMENT_DYNAMIC(CClientConfigPropertySheet, CIBASheet)
CClientConfigPropertySheet::CClientConfigPropertySheet(CWnd* pParentWnd)
: CIBASheet(IDS_CLIENTSET, pParentWnd)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	SetLook(CIBASheet::PropSheetLook_List);

	AddPage(&m_CommonConfig);
	AddPage(&m_BundleTimePage);
	AddPage(&m_UnlockPage);

	EnableVisualManagerStyle();
}


CClientConfigPropertySheet::~CClientConfigPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CClientConfigPropertySheet, CIBASheet)
END_MESSAGE_MAP()


// CClientConfigPropertySheet 消息处理程序

BOOL CClientConfigPropertySheet::OnInitDialog()
{
	BOOL bResult = CIBASheet::OnInitDialog();

	// TODO:  在此添加您的专用代码

	return bResult;
}
