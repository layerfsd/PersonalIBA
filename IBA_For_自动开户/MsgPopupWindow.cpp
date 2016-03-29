// MsgPopupWindow.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "MsgPopupWindow.h"


// CMsgPopupWindow

IMPLEMENT_DYNAMIC(CMsgPopupWindow, CBCGPPopupWindow)
CMsgPopupWindow::CMsgPopupWindow()
{
	//窗口样式设计
	SetAnimationType(CBCGPPopupMenu::ANIMATION_TYPE::SLIDE);
	SetAnimationSpeed(100);
	SetTransparency(255);//透明
	SetSmallCaption(FALSE);//大标题
	SetAutoCloseTime(15000);//10s自动关闭
}

CMsgPopupWindow::~CMsgPopupWindow()
{
	
}


BEGIN_MESSAGE_MAP(CMsgPopupWindow, CBCGPPopupWindow)
	ON_COMMAND(ID_COPY_MSG,OnCopyText)
END_MESSAGE_MAP()

BOOL CMsgPopupWindow::OnClickLinkButton(UINT uiCmdID)
{
	AfxGetMainWnd()->PostMessage(WM_REPLYMESSAGE, uiCmdID, NULL);

	return TRUE;
}

void CMsgPopupWindow::OnCopyText()
{
	CIBAHelpper::PutTextToClipboard(m_strText);
}

BOOL CMsgPopupWindow::Create(CWnd* pWndOwner, CBCGPPopupWndParams& params, HMENU hMenu /*= NULL*/, CPoint ptPos /*= CPoint (-1, -1)*/)
{
	m_strText = params.m_strText;

	params.m_strURL = LOAD_STRING(IDS_RELPYMSG);

	VERIFY(m_Menu.LoadMenu(IDR_MSG_MENU));//载入

	CMenu* pPopup = m_Menu.GetSubMenu(0);

	// 瑞金特殊版本
	//if (pPopup)
	//{
	//	pPopup->RemoveMenu(ID_SCAN_OTHER, MF_BYCOMMAND);
	//}
	return CBCGPPopupWindow::Create(NULL, params, pPopup->Detach(), ptPos);
}

// CMsgPopupWindow 消息处理程序

