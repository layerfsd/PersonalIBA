// DodonewView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DodonewView.h"
#include ".\dodonewview.h"
#include "Cashier.h"
#include "Encrypt.h"


// CDodonewView

IMPLEMENT_DYNCREATE(CDodonewView, CHtmlView)

CDodonewView::CDodonewView()
{
}

CDodonewView::~CDodonewView()
{
}

void CDodonewView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDodonewView, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CDodonewView 诊断

#ifdef _DEBUG
void CDodonewView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CDodonewView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CDodonewView 消息处理程序

void CDodonewView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	//this->Navigate(_T("http://www.duduniu.cn"));

	UINT nNetbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strCasherID = theApp.GetCurCashier()->GetName();
	CString strPwd = theApp.GetCurCashier()->GetPassword();
	CString strLinkPage = _T("http://www.duduniu.cn/iba.jsp");

	CString strUrl = _T("http://");
	strUrl += CNetBarConfig::GetInstance()->GetQueryHost();

	CString strCodePwd;
	CEncrypt::CalcMD5(strPwd,strCodePwd);
	strCodePwd.MakeLower();
	
	strUrl.AppendFormat( _T("/netbar/common/thirdpartdodonew.jsp?netBarId=%d&userId=%s&password=%s&linkPage=%s"),
		nNetbarID, strCasherID, strCodePwd, strLinkPage);
	strUrl += _T("&flag=duduniu");

	this->Navigate2(strUrl);
	
	CIBALog::GetInstance()->WriteFormat(_T("打开增收宝地址:%s"),strUrl);
}

void CDodonewView::Reflash()
{
	UINT nNetbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strCasherID = theApp.GetCurCashier()->GetName();
	CString strPwd = theApp.GetCurCashier()->GetPassword();
	CString strLinkPage = _T("http://www.duduniu.cn/iba.jsp");

	CString strUrl = _T("http://");
	strUrl += CNetBarConfig::GetInstance()->GetQueryHost();

	CString strCodePwd;
	CEncrypt::CalcMD5(strPwd,strCodePwd);
	strCodePwd.MakeLower();

	strUrl.AppendFormat( _T("/netbar/common/thirdpartdodonew.jsp?netBarId=%d&userId=%s&password=%s&linkPage=%s"),
		nNetbarID, strCasherID, strCodePwd, strLinkPage);
	strUrl += _T("&flag=duduniu");

	this->Navigate2(strUrl);
}

void CDodonewView::OnDestroy()
{
	//CHtmlView::OnDestroy();

	CView::OnDestroy();
}

void CDodonewView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CHtmlView::OnClose();
	theApp.GetIBAView()->PostMessage(WM_CHILDCLOSE, GetDlgCtrlID(), (LPARAM)GetSafeHwnd());
}

void CDodonewView::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CHtmlView::PostNcDestroy();
}

void CDodonewView::OnNcDestroy()
{
	CHtmlView::OnNcDestroy();

	// TODO: 在此处添加消息处理程序代码
}
