// PayCardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MD5.h"
#include "WinHttpRequest.h"
#include "PayVerifyDlg.h"
#include "PcpThread.h"
#include ".\PayCardDlg.h"

//******************************************************************************

BEGIN_DISPATCH_MAP(CPayCardDlg, CDialog)
	DISP_FUNCTION(CPayCardDlg, "PrintText", PrintText, VT_BOOL, VTS_I4 VTS_BSTR)
END_DISPATCH_MAP()

// CPayCardDlg 对话框

IMPLEMENT_DYNAMIC(CPayCardDlg, CDialog)
CPayCardDlg::CPayCardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPayCardDlg::IDD, pParent)
	, m_lpWebBrowser(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);

	EnableAutomation();
}

CPayCardDlg::~CPayCardDlg()
{
}

void CPayCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPayCardDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPayCardDlg 消息处理程序

void CPayCardDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CPayCardDlg::OnOK()
{
	//CDialog::OnOK();
}

void CPayCardDlg::OnClose()
{
	//if (IDOK != MessageBox(_T("确定关闭点卡平台吗？"), NULL, MB_ICONQUESTION | MB_OKCANCEL))
	//{
	//	return;
	//}

	CPcpThread::DestoryPcp();
}

void CPayCardDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	if (m_lpWebBrowser != NULL)
	{
		m_lpWebBrowser->DestroyWindow();
		delete m_lpWebBrowser;
		m_lpWebBrowser = NULL;
	}

	delete this;
}

BOOL CPayCardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	SetWindowPos(NULL, 0, 0, 950 + GetSystemMetrics(SM_CXDLGFRAME) * 2, 
		640 +GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME) * 2, SWP_NOMOVE);

	CreateWebBrower();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPayCardDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_lpWebBrowser != NULL)
		m_lpWebBrowser->MoveWindow(0, 0, cx, cy);	
}

BOOL CPayCardDlg::CreateWebBrower()
{
	m_lpWebBrowser = new CWebBrowser;	
	m_lpWebBrowser->SetFlatStyle();

	RECT rectClient;
	GetClientRect(&rectClient);

	BOOL bRet = m_lpWebBrowser->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rectClient, this, 0, NULL);

//#ifndef _DEBUG
//	m_lpWebBrowser->EnableContextMenu(FALSE);
//#endif
	
	m_lpWebBrowser->SetEventHandler(this);

	GetURL();

	m_lpWebBrowser->Navigate(m_strCurrentURL);

	return bRet;

}

inline BYTE toHex(const BYTE& x)
{
	return x > 9 ? x + 55 : x + 48;
}

int UrlEncode(LPBYTE in, LPBYTE out)
{
	int count = 0;
	if (out)
	{
		while (*in)
		{
			if (isalnum(*in))
			{
				*out++ = *in;
				count++;
			}
			else if (isspace(*in))
			{
				*out++ = '+';
				count++;
			}
			else
			{
				*out++ = '%';
				*out++ = toHex(*in >> 4);
				*out++ = toHex(*in % 16);
				count += 3;
			}
			in++;
			if (count >= 1021)
				break;
		}
		*out = '\0';
	}
	return 1;
}



void CPayCardDlg::WebBrowser_BeforeNavigate2(CWebBrowser& Sender, LPCTSTR lpszURL, DWORD nFlags, 
											 LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, 
											 LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	CString strURL(lpszURL);

	CString strTmp = strURL.Left(3);

	strTmp.MakeLower();

	if (strTmp.Find(_T("ddn")) == -1)
	{
		return;
	}

	TRACE(strURL);

	*pbCancel = TRUE;

	CString strDdn(_T("http"));
	strDdn.Append(strURL.Mid(3));

	char szResult[1024] = {0};
	UrlEncode((BYTE*)(LPCSTR)strDdn, (BYTE*)szResult);
	strDdn = szResult;

	CString postData;

	postData.Format(_T("domainId=%s&netBarId=%s&cashier=%s&dutyId=%s&ddn=%s&version=%d&mydodo=%d&t=%d"),
		m_strDomainId, m_strNetbarId, m_strCashier,m_strDutyId, strDdn, 51, 0, time(NULL));

	CString strMD5Src = postData + _T("&payKey=05633018A178AD52");

	postData = postData + _T("&Hash=") + CMD5::MD5String(strMD5Src);

	TRACE("%s",postData);

	CPayVerifyDlg dlg;
	
	dlg.m_strURL.Format(_T("http://%s/dodonew/front/index.jsp"), m_strDdnServer);
	
	dlg.m_strPostData = postData;

	dlg.DoModal();

}


BOOL CPayCardDlg::AnalyzeParamString(LPCTSTR lpszParams)
{
	TRACE(lpszParams);

	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hr = pDoc.CoCreateInstance(__uuidof(DOMDocument30));

	if (FAILED(hr))
	{  
		return FALSE;
	}

	_bstr_t strXML(lpszParams);

	VARIANT_BOOL bRet = VARIANT_FALSE;

	hr = pDoc->loadXML(strXML, &bRet);
	
	if (!bRet)
	{
		AfxMessageBox(_T("XML参数有误！"));

		return FALSE;
	}

	//******************************************************************************

	_bstr_t strNode(_T("//DdnPayCard"));

	IXMLDOMNode* pNode = NULL;
	pDoc->selectSingleNode(strNode, &pNode);

	if (!pNode)
	{
		AfxMessageBox(_T("找不到节点DdnPayCard！"));

		return FALSE;
	}

	//******************************************************************************

	_bstr_t strVal;
	pNode->get_text(strVal.GetAddress());
	m_strCurrentURL = (LPCTSTR)strVal;

	//m_strCurrentURL = EncodeURL(m_strCurrentURL);

//#ifdef _DEBUG
//	AfxMessageBox(m_strCurrentURL);
//#endif

	TRACE("m_strCurrentURL:%s\n", m_strCurrentURL);

	//******************************************************************************

	IXMLDOMNamedNodeMap* pAttrMap = NULL;
	pNode->get_attributes(&pAttrMap);

	if (!pAttrMap)
	{
		return FALSE;
	}

	//******************************************************************************

	IXMLDOMNode* pAttrItem = NULL;
	_variant_t variantValue;

	_bstr_t strAttrDomainId(_T("DomainId"));
	pAttrMap->getNamedItem(strAttrDomainId, &pAttrItem);

	if (pAttrItem != NULL)
	{
		pAttrItem->get_nodeTypedValue(&variantValue);
		_bstr_t strVal(variantValue.bstrVal);
		m_strDomainId = (LPCTSTR)strVal;
	}

	//******************************************************************************

	_bstr_t strAttrNetbarId (_T("NetbarId"));
	pAttrMap->getNamedItem(strAttrNetbarId, &pAttrItem);

	if (pAttrItem != NULL)
	{
		pAttrItem->get_nodeTypedValue(&variantValue);
		_bstr_t strVal(variantValue.bstrVal);
		m_strNetbarId = (LPCTSTR)strVal;
	}

	//******************************************************************************

	_bstr_t strAttrCashier(_T("Cashier"));
	pAttrMap->getNamedItem(strAttrCashier, &pAttrItem);

	if (pAttrItem != NULL)
	{
		pAttrItem->get_nodeTypedValue(&variantValue);
		_bstr_t strVal(variantValue.bstrVal);
		m_strCashier = (LPCTSTR)strVal;
	}

	//******************************************************************************

	_bstr_t strAttrDutyId(_T("DutyId"));
	pAttrMap->getNamedItem(strAttrDutyId, &pAttrItem);

	if (pAttrItem != NULL)
	{
		pAttrItem->get_nodeTypedValue(&variantValue);
		_bstr_t strVal(variantValue.bstrVal);
		m_strDutyId = (LPCTSTR)strVal;
	}

	//******************************************************************************

	_bstr_t strAttrDdnServer(_T("DdnServer"));
	pAttrMap->getNamedItem(strAttrDdnServer, &pAttrItem);

	if (pAttrItem != NULL)
	{
		pAttrItem->get_nodeTypedValue(&variantValue);
		_bstr_t strVal(variantValue.bstrVal);
		m_strDdnServer = (LPCTSTR)strVal;
	}

	//******************************************************************************

	return TRUE;
}

HRESULT CPayCardDlg::WebBrowser_GetExternal(CWebBrowser& Sender, LPDISPATCH *lppDispatch)
{
	*lppDispatch = GetIDispatch(TRUE);

	return S_OK;
}


BOOL CPayCardDlg::PrintText(INT nFontSize, LPCTSTR lpszText)
{
	CString strContent(lpszText);

	if (strContent.IsEmpty())
	{
		return FALSE;
	}

	CPrintDialog PrintDlg(FALSE, PD_DISABLEPRINTTOFILE);// 打印机选择对话框

	if (!PrintDlg.GetDefaults())//没有选择打印机
	{
		return FALSE;
	}

	CDC dcPrinter;
	dcPrinter.Attach(PrintDlg.GetPrinterDC());//捆绑打印DC，也可以读取注册表得到打印机后创建DC，
	
	//打印字体设置
	LOGFONT lf = {0};
	lf.lfHeight = -MulDiv(nFontSize, dcPrinter.GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	lstrcpy(lf.lfFaceName, _T("宋体"));

	CFont PrinterFont;
	PrinterFont.CreateFontIndirect(&lf);
	dcPrinter.SelectObject(PrinterFont.GetSafeHandle());

	CStringArray strLines;
	SplitLine(strContent, strLines, '\n');//得到各分割字串，用回车分割

	INT nX = 0;
	INT nY = 0;
	//开始打印
	dcPrinter.StartDoc(_T("DdnPcp"));

	for (INT i = 0; i < strLines.GetCount(); i++)
	{
		dcPrinter.TextOut(nX, nY, strLines.GetAt(i));
		nY += dcPrinter.GetTextExtent(strLines.GetAt(i)).cy;//本行字串的宽度
	}

	dcPrinter.EndDoc();//结束打印
	dcPrinter.DeleteDC();//释放对象

	return TRUE;
}

void CPayCardDlg::SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch /*= '|'*/)
{
	strArray.RemoveAll();

	if (strLine.IsEmpty())
	{
		return;
	}

	INT nPos = 0;

	INT nIndex = strLine.Find(ch);

	CString strTmp;

	while (nIndex >= 0)
	{
		strTmp.Empty();

		strTmp = strLine.Mid(nPos, nIndex - nPos);

		strArray.Add(strTmp);

		nPos = nIndex + 1;

		nIndex = strLine.Find(ch, nPos);
	}

	if (nPos == 0) //没有找到分割符合
	{
		strArray.Add(strLine);
	}
	else
	{
		if ((strLine.GetLength() - 1) >= nPos)
		{
			strTmp.Empty();
			strTmp = strLine.Mid(nPos, strLine.GetLength() - nPos);
			strArray.Add(strTmp);
		}
	}
}

void CPayCardDlg::GetURL()
{
	try
	{
		CWinHttpRequest HttpRequest;

		HttpRequest.CreateByReg();

		HttpRequest.SetOption(CWinHttpRequest::WinHttpRequestOption_EnableRedirects, COleVariant(0L));
		HttpRequest.SetOption(CWinHttpRequest::WinHttpRequestOption_URLCodePage, COleVariant(936L));

		HttpRequest.Open(_T("GET"), m_strCurrentURL);

		HttpRequest.Send();

		if (HttpRequest.GetStatus() == 302)
		{
			CString strRedirectURL;

			strRedirectURL = HttpRequest.GetResponseHeader(_T("Location"));

			m_strCurrentURL = strRedirectURL;
		}
	}
	catch (...)
	{
		
	}
}

//CString CPayCardDlg::EncodeURL(LPCTSTR lpszUrl)
//{
//	CWinHttpRequest WinHttpRequest;
//
//	WinHttpRequest.CreateByReg();
//
//	WinHttpRequest.SetOption(CWinHttpRequest::WinHttpRequestOption_URLCodePage, 
//		CWinHttpRequest::CodePage_GB2312);
//
//	WinHttpRequest.Open(_T("OPEN"), lpszUrl);
//
//	COleVariant vRet = WinHttpRequest.GetOption(CWinHttpRequest::WinHttpRequestOption_URL);
//
//	_bstr_t strVal(vRet.bstrVal);
//
//	CString strRet = (LPCTSTR)strVal;
//
//	return strRet;
//}
