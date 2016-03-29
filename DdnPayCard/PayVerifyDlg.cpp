// PayCardHtmlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include ".\PayVerifyDlg.h"
#include "MDataXSender.h"

// CPayCardHtmlDlg 对话框

IMPLEMENT_DYNCREATE(CPayVerifyDlg, CDHtmlDialog)

BEGIN_MESSAGE_MAP(CPayVerifyDlg, CDHtmlDialog)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CPayVerifyDlg)

END_DHTML_EVENT_MAP()

BEGIN_DISPATCH_MAP(CPayVerifyDlg, CDHtmlDialog)
	DISP_FUNCTION(CPayVerifyDlg, "AutoClose", OnClose, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CPayVerifyDlg, "PrintText", PrintText, VT_BOOL, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CPayVerifyDlg, "PcpNotify", PcpNotify, VT_BOOL, VTS_I4 VTS_BSTR)
END_DISPATCH_MAP()

CPayVerifyDlg::CPayVerifyDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CPayVerifyDlg::IDD, 0, pParent)
{
	EnableAutomation();
}

CPayVerifyDlg::~CPayVerifyDlg()
{
}

void CPayVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CPayVerifyDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	SetExternalDispatch(GetIDispatch(FALSE));

	CString headData = _T("User-Agent: DODONEW\r\nContent-Type: application/x-www-form-urlencoded\r\nAccept-Language: zh-cn\r\n");

	Navigate(m_strURL, 0, NULL, headData, m_strPostData.GetBuffer(), m_strPostData.GetLength());



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// CPayCardHtmlDlg 消息处理程序

void CPayVerifyDlg::PostNcDestroy()
{
	CDHtmlDialog::PostNcDestroy();
}

void CPayVerifyDlg::OnOK()
{
	//CDHtmlDialog::OnOK();
}

void CPayVerifyDlg::OnCancel()
{
	//CDHtmlDialog::OnCancel();
}

STDMETHODIMP CPayVerifyDlg::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, 
										   IDispatch *pdispReserved)
{

#if _DEBUG
	return S_FALSE;
#endif

	return S_OK; //不弹出菜单
}

void CPayVerifyDlg::OnClose()
{
	//CDHtmlDialog::OnClose();

	//CDHtmlDialog::OnCancel();

	((CDialog*)GetParent())->EndDialog(IDCANCEL);
}

BOOL CPayVerifyDlg::IsExternalDispatchSafe()
{
	return TRUE;
}

BOOL CPayVerifyDlg::PrintText(INT nFontSize, LPCTSTR lpszText)
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

void CPayVerifyDlg::SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch /*= '|'*/)
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

BOOL CPayVerifyDlg::PcpNotify(INT nCmd, LPCTSTR lpszText)
{
	if (1 == nCmd)
	{
		NS_DataX::CDataXMSender Sender;

		Sender.Create(1, _T("DdnPcp-0ED70A77-233A-43c0-9490-962283436DF5"));

		CString strTmp(lpszText);
		int nSpl = strTmp.Find(_T('|'),0);
		int nMemberId = 0;
		if (nSpl >= 0)
		{
			nMemberId = _ttoi(strTmp.Left(nSpl));
		}
		strTmp = strTmp.Mid(nSpl+1);

		Sender.PostData(1, nMemberId, 0, strTmp.GetBuffer(), strTmp.GetLength());

		strTmp.ReleaseBuffer();
	}

	return TRUE;
}
BOOL CPayVerifyDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	return CDHtmlDialog::OnEraseBkgnd(pDC);
}
