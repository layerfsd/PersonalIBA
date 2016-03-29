#pragma once
#include "G2XUser.h"

G2XUI_BEGIN

class G2X_CLASSES CGraphicsRoundRectPath;
class G2X_CLASSES CGraphicsEx
{
public:
	CGraphicsEx(HDC hdc);

	Gdiplus::Graphics* operator->();

public:
	Gdiplus::Graphics m_graphics;
};

// 圆角矩形路径
class G2X_CLASSES CGraphicsRoundRectPath: public Gdiplus::GraphicsPath
{
public:
	CGraphicsRoundRectPath();
	CGraphicsRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);

public:
	void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);
};

// 颜色对象, 提供设置"不透明度"的功能
class G2X_CLASSES CColor : public Gdiplus::Color
{
public:
	CColor();
	CColor(COLORREF clrREF, BYTE uAlpha=255);
	CColor(BYTE Alpha,BYTE r,BYTE g,BYTE b):
		Gdiplus::Color(Alpha,r,g,b)
	{
		SetValue( MakeARGB(Alpha,r,g,b) );
	}
	
	void SetAlpha(BYTE uAlpha);
};

class G2X_CLASSES CG2XBitmap
{
public:
	static Bitmap* FromBmpRes(UINT nBmpResID)
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(nBmpResID))
		{
			BITMAP bm;
			bmp.GetBitmap(&bm);
			Bitmap* pBmp = Bitmap::FromHBITMAP((HBITMAP)bmp.GetSafeHandle(),NULL);

			return pBmp;
		}
		else
		{
			return NULL;
		}
	}
	static Bitmap* FromIconRes(UINT nIconResID)
	{
		HICON hIcon = ::LoadIcon(NULL,MAKEINTRESOURCE(nIconResID));
		return Bitmap::FromHICON(hIcon);
	}
};

G2X_END