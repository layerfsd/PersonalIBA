#include "stdafx.h"
#include "ibaprint.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib") ////请修改为你的.lib文件路径
using namespace Gdiplus;

static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

BOOL G2XStartup()
{
	BOOL bOK = Gdiplus::GdiplusStartup(&gdiplusToken, 
		&gdiplusStartupInput, NULL) == 0;

	return bOK;
}

void G2XShutdown()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}



CIBAPrint::CIBAPrint(void)
{
}

CIBAPrint::~CIBAPrint(void)
{
}

void CIBAPrint::ReplaceHeaderInfo(CString &strContent)
{
	strContent.Replace(_T("[netbarname]"), CNetBarConfig::GetInstance()->GetNetBarName());
	strContent.Replace(_T("[operator]"), theApp.GetCurCashier()->GetName());
	strContent.Replace(_T("[now]"), CIBAHelpper::GetCurrentTimeAsString(TRUE));
}

void CIBAPrint::Print(CString& strContent)
{
	if (strContent.IsEmpty())
	{
		CIBALog::GetInstance()->Write(_T("The Content to print is empty!"));
		return;
	}

	CPrintDialog PrintDlg(FALSE, PD_DISABLEPRINTTOFILE);// 打印机选择对话框
	
	if (!PrintDlg.GetDefaults())//没有选择打印机
	{
		CIBALog::GetInstance()->Write(_T("Can not find any printers!"));
		return;
	}
	
	CDC dcPrinter;
	dcPrinter.Attach(PrintDlg.GetPrinterDC());//捆绑打印DC，也可以读取注册表得到打印机后创建DC，
	//打印字体设置
	LOGFONT lf = {0};
	lf.lfHeight = -MulDiv(CIBAConfig::GetInstance()->GetPrintFontSize(), dcPrinter.GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	lstrcpy(lf.lfFaceName, _T("黑体"));

	CFont PrinterFont;
	PrinterFont.CreateFontIndirect(&lf);
	dcPrinter.SelectObject(PrinterFont.GetSafeHandle());

	CStringArray strLines;
	CIBAHelpper::SplitLine(strContent, strLines, '\n');//得到各分割字串，用回车分割
	
	INT nX = 0;
	INT nY = 0;
	//开始打印
	dcPrinter.StartDoc(_T("IBA"));

	for (INT i = 0; i < strLines.GetCount(); i++)
	{
		dcPrinter.TextOut(nX, nY, strLines.GetAt(i));
		nY += dcPrinter.GetTextExtent(strLines.GetAt(i)).cy;//本行字串的宽度
		nY += 4;
	}
	
	dcPrinter.EndDoc();//结束打印
	dcPrinter.DeleteDC();//释放对象
}

void CIBAPrint::GetRegisterTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\Register.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetCreditTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\Credit.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetBundleTimeTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\BundleTime.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetRegisterAndBundleTimeTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\RegisterAndBundleTime.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetCreditAndBundleTimeTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\CreditAndBundleTime.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetShiftCashTicket(CString& strContent)
{
	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBAConfig\\PrintTemplate\\ShiftCash.txt"));

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}

void CIBAPrint::GetRefundTicket(CString& strContent,BOOL bIsMember)
{	
	CString strFile(theApp.GetWorkPath());
	if (bIsMember)
	{
		strFile.Append(_T("\\IBAConfig\\PrintTemplate\\RefundVIP.txt"));
	}
	else
	{
		strFile.Append(_T("\\IBAConfig\\PrintTemplate\\Refund.txt"));
	}
	

	strContent.Empty();

	CIBAHelpper::ReadFile(strFile, strContent);

	ReplaceHeaderInfo(strContent);
}


//-------------------------------------------------------------------------

void CIBAPrintA4::SetOwner(CWnd* pWnd)
{
	m_pOwner = pWnd;
	m_pView = NULL;
	m_pTemplate = NULL;
	m_strName.Empty();
	m_strNumber.Empty();
	m_strID.Empty();
	m_strAddress.Empty();
	m_strContent[0].Empty();
	m_strContent[1].Empty();
	m_strContent[2].Empty();
	m_strContent[3].Empty();
	m_strContent[4].Empty();
}

BOOL CIBAPrintA4::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return CG2XPrintHelper::OnPreparePrinting(pInfo);
}

void CIBAPrintA4::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	/*m_strAddress = _T("广东省台山市台城镇塔山路8号504房");

	m_strContent[0] = _T("内容1");
	m_strContent[1] = _T("内容2");
	m_strContent[2] = _T("内容3");*/

	ASSERT(pDC) ;
	ASSERT(pInfo) ;
	// do some default output onto the page
	CString	text ;
	CSize	csText ;
	CFont	*pOldFont ;
	CFont	myFont ;

	int fontSize = -MulDiv(11, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	VERIFY(myFont.CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_ROMAN, _T("Arial"))) ;
	pOldFont = pDC->SelectObject(&myFont) ;
	//pDC->Rectangle(pInfo->m_rectDraw) ;
	//pDC->Ellipse(pInfo->m_rectDraw) ;
	//	text.Format("PAGE %1d of %1d", pInfo->m_nCurPage, pInfo->GetMaxPage()) ;
	//csText = pDC->GetTextExtent(text) ;
	//pDC->TextOut(pInfo->m_rectDraw.left + (pInfo->m_rectDraw.Width() - csText.cx) / 2, 
	//			 pInfo->m_rectDraw.top + (pInfo->m_rectDraw.Height() - csText.cy) / 2, text) ;

	CRect rcArea = pInfo->m_rectDraw;
	INT x = rcArea.left + (rcArea.Width() / 4);
	INT nGapH = rcArea.Height() / 29;
	INT y = rcArea.top + nGapH * 5;
	pDC->TextOut(x,y,m_strName);
	pDC->TextOut(x,y + nGapH,m_strNumber);
	pDC->TextOut(x,y + nGapH + nGapH,m_strAddress);

	float fX = (rcArea.Width() / 42.0f) * 29.0f;
	pDC->TextOut(rcArea.left + (INT)fX,y,m_strID);

	x = rcArea.left + rcArea.Width() / 9;
	y = rcArea.top + INT(35.0f*(rcArea.Height() / 73.0f));

	G2XStartup();
	Gdiplus::Font *pFont = new Gdiplus::Font(L"Arial",(REAL)-fontSize-5, FontStyleRegular, UnitPixel, NULL);
	Graphics gr(pDC->GetSafeHdc());
	gr.SetPageUnit(UnitPixel);
	StringFormat sFmt;
	sFmt.SetAlignment(StringAlignmentNear);
	sFmt.SetLineAlignment(StringAlignmentNear);
	
	for (int i=0; i < 5; i++)
	{
		if (!m_strContent[i].IsEmpty())
		{
			//pDC->TextOut(x, y , m_strContent[i]);

			RectF rc;
			rc.X = x;
			rc.Y = y;
			rc.Width = rcArea.Width() - x;
			rc.Height = nGapH;
			gr.DrawString(CT2W(m_strContent[i]), -1, pFont, rc, &sFmt,&SolidBrush(Color(80,80,80)));
		}
		y += nGapH;
	}
	delete pFont;
	G2XShutdown();

	pDC->SelectObject(pOldFont) ;
	VERIFY(myFont.DeleteObject()) ;
}