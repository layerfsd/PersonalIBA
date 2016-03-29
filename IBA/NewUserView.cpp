// NewUserView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "dal\IBADAL.h"
#include "..\L2Markup\Markup.h"
#include "MainFrm.h"
#include "MsgBoxDlg.h"
#include ".\newuserview.h"
// CNewUserView

IMPLEMENT_DYNCREATE(CNewUserView, CIBAFormView)

BEGIN_MESSAGE_MAP(CNewUserView, CIBAFormView)
	
END_MESSAGE_MAP()

CNewUserView::CNewUserView()
	: CIBAFormView(IDD)
{
	
}

CNewUserView::~CNewUserView()
{
}

void CNewUserView::DoDataExchange(CDataExchange* pDX)
{
	CIBAFormView::DoDataExchange(pDX);
}


// CNewUserView 诊断

#ifdef _DEBUG
void CNewUserView::AssertValid() const
{
	CIBAFormView::AssertValid();
}

void CNewUserView::Dump(CDumpContext& dc) const
{
	CIBAFormView::Dump(dc);
}
#endif //_DEBUG


// CNewUserView 消息处理程序

