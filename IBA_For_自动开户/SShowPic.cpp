// SShowPic.cpp : 实现文件
//

#include "stdafx.h"

#include ".\sshowpic.h"


// CSShowPic
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR gdiplusToken;

using namespace Gdiplus;

#pragma comment(lib,"gdiplus.lib")

IMPLEMENT_DYNAMIC(CSShowPic, CStatic)
CSShowPic::CSShowPic()
{
	ZeroMemory(m_szFullPath, sizeof(m_szFullPath));
	
	BOOL bOK = Gdiplus::GdiplusStartup(&gdiplusToken, 
		&gdiplusStartupInput, NULL) == 0;

	m_pImage = NULL;
	m_clr = RGB(255,255,255);
}

CSShowPic::~CSShowPic()
{
	if (m_pImage)
	{
		delete m_pImage;
	}
	m_pImage = NULL;
	Gdiplus::GdiplusShutdown(gdiplusToken);
}


BEGIN_MESSAGE_MAP(CSShowPic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CSShowPic 消息处理程序


void CSShowPic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	if(m_pImage)
	{
		CRect rect;
		GetClientRect(rect);

		Graphics gr(dc.GetSafeHdc());
		gr.DrawImage(m_pImage, Rect(rect.top, rect.left, rect.Width(),rect.Height()), 0, 0, m_pImage->GetWidth(), m_pImage->GetHeight(), UnitPixel);
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		dc.FillSolidRect(rect, m_clr);
		/*CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
		CPen* pOld = dc.SelectObject(&pen);
		dc.MoveTo(CPoint(0,0));
		dc.LineTo(CPoint(rect.right, rect.bottom));
		dc.MoveTo(CPoint(rect.right, 0));
		dc.LineTo(CPoint(0, rect.bottom));

		dc.SelectObject(pOld);*/
		
	}
}

void CSShowPic::ShowPic(LPCTSTR szFullPath)
{
	ZeroMemory(m_szFullPath, sizeof(m_szFullPath));

	if (szFullPath == NULL)
	{
		_tcsncpy(m_szFullPath, _T(""), MAX_PATH);
	}

	else
	{
		_tcsncpy(m_szFullPath, szFullPath, MAX_PATH);
	}

	if('\0' != m_szFullPath[0])
	{
		Image * pImge = Image::FromFile(CT2W(m_szFullPath));
		if (pImge == NULL || pImge->GetLastStatus() != Ok)
		{
			if (m_pImage)
			{
				delete m_pImage;
			}
			m_pImage = NULL;
		}
		else
		{
			m_pImage = new Bitmap(pImge->GetWidth(), pImge->GetHeight());
			Graphics gr(m_pImage);
			gr.DrawImage(pImge, 0, 0, 0, 0, pImge->GetWidth(), pImge->GetHeight(), UnitPixel);
		}
		if (pImge)
		{
			delete pImge;
		}
	}
	else
	{
		if (m_pImage)
		{
			delete m_pImage;
		}
		m_pImage = NULL;
	}
	// 是窗口无效，进而调用重绘函数绘图
	Invalidate();
}
