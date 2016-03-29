// ShortMsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "Socket\LocalServer.h"
#include ".\shortmsgdlg.h"



IMPLEMENT_DYNAMIC(CShortMsgDlg, CBCGPDialog)
CShortMsgDlg::CShortMsgDlg(CWnd* pParent /*=NULL*/)
: CStringLoaderDlg<CBCGPDialog>(CShortMsgDlg::IDD, pParent)
, m_nSendType(0)
{
	EnableVisualManagerStyle();

	CIBAHelpper::CreateFont(m_TextFont, 14);
}

CShortMsgDlg::~CShortMsgDlg()
{
}

void CShortMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SHORTMSG, m_ShortMsgContent);
	DDV_MaxChars(pDX, m_ShortMsgContent, 120);
	DDX_Control(pDX, IDC_COMBO_PCCLASS, m_cboPCClass);
}

BEGIN_MESSAGE_MAP(CShortMsgDlg, CBCGPDialog)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_SENDSHORMSG, OnBnClickedSendshormsg)
	ON_COMMAND_RANGE(IDM_MINQUICKMSG, IDM_MAXQUICKMSG, OnQuickMsg)
	ON_COMMAND(IDM_EDITQUICKMSG, OnEditQuickMsg)
END_MESSAGE_MAP()


// CShortMsgDlg 消息处理程序

BOOL CShortMsgDlg::OnInitDialog()
{
	CStringLoaderDlg<CBCGPDialog>::OnInitDialog();
	
	CIBAString strTmp;
	strTmp.LoadString(IDS_SELTERMS);
	m_cboPCClass.AddString(strTmp);
	m_cboPCClass.SetItemData(0, 200);

	strTmp.LoadString(IDS_ALLCASHIER);
	m_cboPCClass.AddString(strTmp);
	m_cboPCClass.SetItemData(1, 100);

	strTmp.LoadString(IDS_ALLTERMS);
	m_cboPCClass.AddString(strTmp);
	m_cboPCClass.SetItemData(2, 0);

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetCount(); i++)
	{
		INT nTmp = m_cboPCClass.AddString(CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetName());
		m_cboPCClass.SetItemData(nTmp, CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetID());
	}

	if (2 == m_nSendType) //回复收银台消息
	{
		m_cboPCClass.SetCurSel(1);
		m_cboPCClass.EnableWindow(FALSE);
	}
	else if (1 == m_nSendType)
	{
		m_cboPCClass.SetCurSel(0);
		m_cboPCClass.EnableWindow(FALSE);
	}
	else
	{
		m_cboPCClass.SetCurSel(0);
	}

	GetDlgItem(IDC_EDIT_SHORTMSG)->SetFont(&m_TextFont);
	GetDlgItem(IDC_EDIT_SHORTMSG)->SetFocus();

	
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CShortMsgDlg::OnBnClickedClose()
{
	OnOK();
}

void CShortMsgDlg::OnBnClickedSendshormsg()
{
	if (!UpdateData()) return;

	m_ShortMsgContent.Trim();

	if (m_ShortMsgContent.IsEmpty())
	{
		ShowPopupMenu();
	}
	else
	{
		const int nIndex = m_cboPCClass.GetCurSel();

		if (nIndex == 0) //选中的客户端
		{
			for (INT i = 0; i < m_nReceivers.GetCount(); i++)
			{
				CLocalServer::GetInstance()->LocalSendShortMessage(
					m_nReceivers.GetAt(i), m_ShortMsgContent);
			}	
		}
		else if (nIndex == 2) //全部终端
		{
			for (INT i = 0; i < CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetCount(); i++)
			{
				CLocalServer::GetInstance()->LocalSendGroupShortMessage(
					CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetID(), m_ShortMsgContent);
			}
		}
		else //区域
		{
			CLocalServer::GetInstance()->LocalSendGroupShortMessage(
				(int)m_cboPCClass.GetItemData(nIndex), m_ShortMsgContent);
		}

		m_nReceivers.RemoveAll();

		OnOK();
	}
}

BOOL CShortMsgDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && CIBAHelpper::IsCtrlPressed())
	{
		OnBnClickedSendshormsg();
		return TRUE;
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CShortMsgDlg::ShowPopupMenu()
{
	CString strTmp, strFileName;
	
	strFileName = theApp.GetWorkPath() + _T("\\IBAConfig\\QuickMsg.txt");

	CIBAHelpper::ReadFile(strFileName, strTmp);
	
	CIBAHelpper::SplitLine(strTmp, m_QucikMsgs, _T('\n'));

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_QUICKMSG_MENU));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	for (INT i = 0; i < m_QucikMsgs.GetCount(); i++)
	{
		pPopup->InsertMenu(i, MF_BYPOSITION, IDM_MINQUICKMSG + i, m_QucikMsgs.GetAt(i));
	}

	POINT pos = {0};
	GetCursorPos(&pos);

	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->Detach(), pos.x, pos.y, this, TRUE);
}

void CShortMsgDlg::OnQuickMsg(UINT id)
{
	m_ShortMsgContent = m_QucikMsgs.GetAt(id - IDM_MINQUICKMSG);

	UpdateData(FALSE);

	OnBnClickedSendshormsg();
}

void CShortMsgDlg::OnEditQuickMsg()
{
	ShellExecute(NULL, _T("open"), theApp.GetWorkPath() + _T("\\IBAConfig\\QuickMsg.txt"), NULL, NULL, SW_NORMAL);
}

void CShortMsgDlg::SetSendType(UINT nType)
{
	m_nSendType = nType;
}
