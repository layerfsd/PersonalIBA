// DlgUserPhoto.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgUserPhoto.h"


// CDlgUserPhoto 对话框

IMPLEMENT_DYNAMIC(CDlgUserPhoto, CBCGPDialog)
CDlgUserPhoto::CDlgUserPhoto(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgUserPhoto::IDD, pParent)
{
	EnableVisualManagerStyle(TRUE);
}

CDlgUserPhoto::~CDlgUserPhoto()
{
}

void CDlgUserPhoto::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PHOTO, m_staticPhoto);
}


BEGIN_MESSAGE_MAP(CDlgUserPhoto, CBCGPDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CDlgUserPhoto::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_staticPhoto.m_hWnd))
	{
		CRect rcClient;
		GetClientRect(rcClient);

		CRect rcSize;
		m_staticPhoto.GetClientRect(rcSize);

		CRect rcNew;
		if (rcClient.Width() < rcSize.Width())
		{
			rcNew = rcSize;
		}
		else
		{
			rcNew.left = (rcClient.Width() - rcSize.Width())/2;
			rcNew.right = rcNew.left  + rcSize.Width();
			rcNew.top = 5;
			rcNew.bottom = rcNew.top + rcSize.Height();
		}
		m_staticPhoto.MoveWindow(rcNew);
	}
}

// CDlgUserPhoto 消息处理程序
