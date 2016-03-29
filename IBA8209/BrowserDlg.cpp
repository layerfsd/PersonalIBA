// BrowserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\browserdlg.h"


// CBrowserDlg 对话框

IMPLEMENT_DYNAMIC(CBrowserDlg, CBCGPDialog)
CBrowserDlg::CBrowserDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CBrowserDlg::IDD, pParent)
{
	EnableVisualManagerStyle();
}

CBrowserDlg::~CBrowserDlg()
{
}

void CBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBrowserDlg, CBCGPDialog)
	ON_NOTIFY(CSimpleBrowser::DocumentComplete, IDC_STATIC_IE, OnDocumentComplete)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CBrowserDlg 消息处理程序

void CBrowserDlg::OnOK()
{
	//CDialog::OnOK();
}

void CBrowserDlg::OnCancel()
{
	//CDialog::OnCancel();
}

BOOL CBrowserDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	CRect rect;
	GetClientRect(rect);
	GetDlgItem(IDC_STATIC_IE)->MoveWindow(rect);
	GetDlgItem(IDC_STATIC_IE)->ShowWindow(SW_NORMAL);
	
	VERIFY(m_WebBrowser.CreateFromControl(this, IDC_STATIC_IE));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBrowserDlg::OnDocumentComplete(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	HideBrowserBorder();
}

void CBrowserDlg::HideBrowserBorder()
{
	IHTMLDocument2* phtmldoc2 = NULL;   
	IHTMLElement* phtmlElement = NULL;   
	IDispatch* pdisp = NULL;   
	pdisp = m_WebBrowser.GetDocument();  

	if (pdisp != NULL)
	{
		pdisp->QueryInterface(IID_IHTMLDocument2, (void**) &phtmldoc2);   
		pdisp->Release();
	}   

	if (phtmldoc2 != NULL)
	{
		phtmldoc2->get_body(&phtmlElement);   
		phtmldoc2->Release();
	}   

	if (phtmlElement != NULL)
	{
		IHTMLStyle* phtmlStyle = NULL;   
		phtmlElement->get_style(&phtmlStyle);   

		if (phtmlStyle != NULL)
		{
			phtmlStyle->put_overflow(L"hidden");   
			phtmlStyle->put_border(L"none");//   去除边框   

			phtmlStyle->Release();   
			phtmlElement->Release();
		}
	}
}

void CBrowserDlg::Navigate(LPCTSTR URL, LPCTSTR postData /*= NULL*/, LPCTSTR headData /*= NULL*/)
{
	m_WebBrowser.Navigate(URL, postData, headData);
}


void CBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialog::OnSize(nType, cx, cy);
	
	CWnd* pWnd = GetDlgItem(IDC_STATIC_IE);
	
	if (pWnd != NULL)
	{
		CRect rect;
		GetClientRect(rect);

		pWnd->MoveWindow(rect);
		m_WebBrowser.MoveWindow(rect);
	}
}


BOOL CBrowserDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CBCGPDialog::OnEraseBkgnd(pDC);
}


