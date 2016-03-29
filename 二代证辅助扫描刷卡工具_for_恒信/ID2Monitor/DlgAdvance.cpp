// DlgAdvance.cpp : 实现文件
//

#include "stdafx.h"
#include "ID2Monitor.h"
#include "DlgAdvance.h"
#include ".\dlgadvance.h"
#include "capturemanager.h"


// CDlgAdvance 对话框

IMPLEMENT_DYNAMIC(CDlgAdvance, CDialog)
CDlgAdvance::CDlgAdvance(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAdvance::IDD, pParent)
{
}

CDlgAdvance::~CDlgAdvance()
{
}

void CDlgAdvance::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDlgAdvance, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
END_MESSAGE_MAP()


// CDlgAdvance 消息处理程序

BOOL CDlgAdvance::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listCtrl.InsertColumn(0,"显示名称",LVCFMT_LEFT, 120, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(1,"是否对话框",LVCFMT_LEFT, 80, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(2,"窗口标题",LVCFMT_LEFT, 100, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(3,"窗口类名称",LVCFMT_LEFT, 140, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(4,"卡号控件ID",LVCFMT_RIGHT, 90, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(5,"证件控件ID",LVCFMT_RIGHT, 90, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);
	m_listCtrl.InsertColumn(6,"姓名控件ID",LVCFMT_RIGHT, 90, TRUE, TRUE, TRUE, NULL,CGxxListCtrl::GLC_Edit);

	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_NOSORTHEADER);

	m_listCtrl.SetItemsHeight(20);

	CCaptureManager cm;
	cm.ReadFromConfig();

	for (int i=0; i < cm.GetCount(); i++)
	{
		CaptureItem& item = cm.GetItem(i);
		m_listCtrl.InsertItem(i, item.strName);

		CString strTmp;
		strTmp.Format("%d", item.bDialog);
		m_listCtrl.SetItem(i, 1, strTmp);
		m_listCtrl.SetItem(i, 2, item.strWindowName);
		m_listCtrl.SetItem(i, 3, item.strClassName);

		strTmp.Format("%x", item.nCardId);
		m_listCtrl.SetItem(i, 4, strTmp);

		strTmp.Format("%x", item.nNumberId);
		m_listCtrl.SetItem(i, 5, strTmp);

		strTmp.Format("%x", item.nNameId);
		m_listCtrl.SetItem(i, 6, strTmp);

	}

	return TRUE; 
}

BOOL CDlgAdvance::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CWnd * pWnd = GetFocus();
			if (pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
			{
				m_listCtrl.OnClickItem(-1,-1);
			}

			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgAdvance::OnBnClickedOk()
{
	int nCount = m_listCtrl.GetItemCount();

	CString strIni;
	GetModuleFileName(NULL,strIni.GetBuffer(256),256);
	strIni.ReleaseBuffer();

	strIni = strIni.Left(strIni.ReverseFind('\\'));
	strIni += "\\other.ini";

	CString strTmp;
	strTmp.Format("%d", nCount);

	::WritePrivateProfileString("items_count", "count", strTmp, strIni);

	for (int i=0; i < nCount; i++)
	{
		CString strAppName;
		strAppName.Format("item%d", i+1);

		::WritePrivateProfileString(strAppName, "name", m_listCtrl.GetItemText(i, 0), strIni);

		CString strIs = m_listCtrl.GetItemText(i, 1);
		if (strIs != "1")
		{
			strIs = "0";
		}
		::WritePrivateProfileString(strAppName, "isDialog", strIs, strIni);
		::WritePrivateProfileString(strAppName, "windowName", m_listCtrl.GetItemText(i, 2), strIni);
		::WritePrivateProfileString(strAppName, "className", m_listCtrl.GetItemText(i, 3), strIni);
		::WritePrivateProfileString(strAppName, "cardId", m_listCtrl.GetItemText(i, 4), strIni);
		::WritePrivateProfileString(strAppName, "numberId", m_listCtrl.GetItemText(i, 5), strIni);
		::WritePrivateProfileString(strAppName, "nameId", m_listCtrl.GetItemText(i, 6), strIni);
	}

	MessageBox("保存成功，请重新启动程序!");

	OnOK();
}

void CDlgAdvance::OnBnClickedBtnAdd()
{
	int i = m_listCtrl.GetItemCount();
	m_listCtrl.InsertItem(i, "在此输入名称");
	m_listCtrl.SetItem(i, 1, "输入0或1");
	m_listCtrl.SetItem(i, 2, "输入窗口标题");
	m_listCtrl.SetItem(i, 3, "输入类名称");
	m_listCtrl.SetItem(i, 4, "0");
	m_listCtrl.SetItem(i, 5, "0");
	m_listCtrl.SetItem(i, 6, "0");
}
