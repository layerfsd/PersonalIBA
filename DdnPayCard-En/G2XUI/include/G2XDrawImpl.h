#pragma once
#include "G2XUser.h"
#include "GraphicsEx.h"

G2XUI_BEGIN

namespace G2XUIFrame{
class G2X_CLASSES CG2XCaptionBar;
class G2X_CLASSES CG2XDialog;

}

class G2X_CLASSES CG2XCommandBarButton;
class G2X_CLASSES CG2XWnd;

class G2X_CLASSES CG2XDrawImpl
{
public:
	static const UINT COLOR_DISABLE_BORDER		= RGB(150,150,150);
	static const UINT COLOR_DISABLE_BK			= RGB(200,200,200);
	static const UINT COLOR_DISABLE_TEXT		= RGB(126,140,150);
	static const UINT COLOR_TEXT				= RGB(0  ,0  ,0  );
	static const UINT COLOR_BORDER				= RGB(185,205,226);
	static const UINT COLOR_HOVER_BORDER		= RGB(62, 156,212);

public:
	CG2XDrawImpl();
	virtual ~CG2XDrawImpl(void);

	static Font* CreateFont(
		WCHAR* lpzName, 
		INT nSize, 
		INT nStyle=FontStyleRegular);

	static Font* CreateFont(CFont* pFont);

	static CSize MeasureString(
		Graphics* pGraphics, 
		LPCTSTR lpcText, 
		Font* lpFont);

	static void MeasureString(
		Graphics* pGraphics,
		LPCTSTR lpcText,
		const INT nTextLength,
		IN Font* lpFont,
		OUT LPRECT lpMaxRect,
		OUT LPRECT lpMinRect);

	static void MeasureString(
		CG2XWnd* pG2XWnd,
		LPCTSTR lpcText,
		const INT nTextLength,
		OUT LPRECT lpMaxRect,
		OUT LPRECT lpMinRect);

	static void MeasureStringEx(
		Graphics* pGraphics,
		LPCTSTR lpcText,
		IN INT * nRanges,
		IN INT nRangeCount,
		IN Font* lpFont,
		OUT SIZE * szRanges);

	//---------------------------------------------------------------------
	//- 创建一幅图像对应的倒影图像
	//---------------------------------------------------------------------
	static Bitmap* CreateInvertedImage(
		Bitmap* pBmp);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//- 基本绘制方法
//- 不能重载
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:

	//---------------------------------------------------------------------
	//- 绘制矩形
	//---------------------------------------------------------------------
	void DrawRectangle(
		Graphics* pGraphics,
		LPCRECT lpRect,
		const Color& clr,
		INT nPenWidth=1);

	//---------------------------------------------------------------------
	//- 绘制矩形
	//---------------------------------------------------------------------
	void DrawRectangle(
		Graphics* pGraphics,
		INT x,
		INT y,
		INT cx,
		INT cy,
		const Color& clr,
		INT nPenWidth=1);
	
	//---------------------------------------------------------------------
	//- 绘制矩形边框
	//- clr1和clr2分别指定边框最外层的颜色和最里层的颜色, 中间层的颜色=clr1和clr2的中间值
	//- bOuterBorder指定边框是否向外扩展,否则向内扩展
	//---------------------------------------------------------------------
	void DrawRectangleBorder(
		Graphics* pGraphics,
		LPCRECT lpRect,
		const Color& clr1,
		const Color& clr2,
		INT nBorderWidth,
		BOOL bOuterBorder=TRUE
		); 

	//---------------------------------------------------------------------
	//- 绘制矩形边框
	//- nColorCount和nBorderWidth最好一致
	//- 如果nColorCount = 1,边框的颜色只有一种颜色
	//- 如果pColors=NULL,那么边框的颜色为黑色
	//- bOuterBorder指定边框是否向外扩展,否则向内扩展
	//---------------------------------------------------------------------
	void DrawRectangleBorder(
		Graphics* pGraphics,
		LPCRECT lpRect,
		INT nBorderWidth,
		const Color* pColors,
		INT nColorCount,
		BOOL bOuterBorder=FALSE);

	//---------------------------------------------------------------------
	//- 绘制圆角矩形边框
	//- nCornerX: 圆角的水平半径
	//- nCornerY: 圆角的垂直半径,nCornerY和nCornerX一般是相等的
	//---------------------------------------------------------------------
	void DrawRoundBorder(
		Graphics* pGraphics,
		LPCRECT lpRect,
		INT nBorderWidth,
		INT nCornerX,
		INT nCornerY,
		Color* pColors,
		INT nColorCount,
		BOOL bOuterBorder=FALSE
		);


	void DrawRoundBorder(Graphics* pGraphics,LPCRECT lpRect,Color* pColor,INT nCornerX,INT nCornerY,INT nBorderWidth);
	void DrawRoundBorder(Graphics* pGraphics,INT x,INT y,INT cx,INT cy,Color* pColor,INT nCornerX,INT nCornerY,INT nBorderWidth);
	void FillRoundRect(Graphics* pGraphics,LPCRECT lpRect,Color* pColor,INT nCornerX,INT nCornerY);

	void DrawFocusRect(Graphics* pGraphics,LPRECT lpRect);

	

	//---------------------------------------------------------------------
	//- 在指定区域中绘制图像,图像将平铺在区域中
	//---------------------------------------------------------------------
	void DrawImg(
		Graphics* pGraphics,
		LPRECT lpDes, 
		Image* pImg,
		LPRECT lpSrc,
		BYTE nAlpha=255,
		BOOL bToGray = FALSE           // 是否做灰度变换
		);

	//---------------------------------------------------------------------
	//- 将图像中的指定颜色转换为不透明度为0的图像
	//---------------------------------------------------------------------
	void TranformToTransparent(
		Bitmap* pBmp, 
		const Color& clrTransparent);

	//---------------------------------------------------------------------
	//- 填充矩形区域
	//---------------------------------------------------------------------
	void FillRectangle(
		Graphics* pGraphics,
		INT x,
		INT y,
		INT cx,
		INT cy,
		const Color& clr);

	//---------------------------------------------------------------------
	//- 填充矩形区域
	//---------------------------------------------------------------------
	void FillRectangle(
		Graphics* pGraphics,
		LPCRECT lpRect,
		const Color& clr);

	//---------------------------------------------------------------------
	//- 填充矩形区域
	//- gradientStyle: 渐变风格, 参考G2XGradientStyle的定义
	//---------------------------------------------------------------------
	void FillRectangle(
		Graphics* pGraphics,
		LPCRECT lpRect,
		G2XGradientStyle gradientStyle,
		const Color& clr1,
		const Color& clr2,
		const Color* pClr3 = NULL,
		const Color* pClr4 = NULL);

	//---------------------------------------------------------------------
	//- 绘制箭头
	//- 旋转时，箭头绕lpCenterPoint点旋转。此函数默认绘制向右的箭头
	//- 例如: 当要绘制向下的箭头,需要指定旋转角度为90,向左:180度,向上:270度
	void DrawArrow(
		Graphics* pGraphics,
		LPCRECT lpRect, 
		LPPOINT lpCenterPoint, 
		INT nRotateAngle=0,
		const Color& clr=Color(0,0,0,0), 
		BOOL bFill=TRUE);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//- 特殊绘制
//- 可以重载
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:
	
	//- 绘制最下化框
	virtual void DrawMinimizeBox(
		Graphics* pGraphics, 
		LPCRECT lpRect,
		G2XControlState state);

	//---------------------------------------------------------------------
	//- 绘制最大化框或者恢复框
	//- bMaximized=TRUE: 最大化框, =FALSE: 恢复框
	//---------------------------------------------------------------------
	virtual void DrawMaximizeBox(
		Graphics* pGraphics, 
		LPCRECT lpRect,
		G2XControlState state,
		BOOL bMaximized=TRUE);

	//- 绘制标题栏的关闭按钮
	virtual void DrawCaptionBarClose(
		Graphics* pGraphics, 
		LPCRECT lpRect,
		G2XControlState state);

	//- 绘制标题栏的背景, 重载此方法可以为特定的标题栏绘制背景
	virtual void DrawCaptionBarBk(
		Graphics* pGraphics,
		G2XUIFrame::CG2XCaptionBar* pCaptionBar,
		LPRECT lpRect);

	//- 绘制CG2XDialog的边框
	virtual void DrawG2XDialogBorder(
		Graphics* pGraphics,
		G2XUIFrame::CG2XDialog* pDlg);

	//- 绘制从CG2XFoundationWindow继承的窗体的边框
	virtual void DrawG2XFoundationWindowBorder(
		Graphics* pGraphics,
		LPRECT lpRect,
		INT nBorderBreadth,
		BOOL bRoundBorder,
		INT nCornerX=8,
		INT nCornerY=8
		);

	//---------------------------------------------------------------------
	//- 绘制push button
	//---------------------------------------------------------------------
	virtual void DrawPushButton(
		Graphics* pGraphics,
		LPCRECT lpRect,
		LPCTSTR lptString,
		const Color& clrText,
		CFont* pFont,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign,
		G2XTextVAlign vAlign);

	//---------------------------------------------------------------------
	//- 绘制push button
	//---------------------------------------------------------------------
	virtual void DrawLinkButton(
		Graphics* pGraphics,
		LPCRECT lpRect,
		LPCTSTR lptString,
		const Color& clrText,
		CFont* pFont,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign,
		G2XTextVAlign vAlign);

	//---------------------------------------------------------------------
	//- 绘制alpha button
	//---------------------------------------------------------------------
	virtual void DrawAlphaButton(
		Graphics* pGraphics,
		LPRECT lpRect,
		LPCTSTR lptString,
		const Color& clrText,
		CFont* pFont,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign,
		G2XTextVAlign vAlign);

	//---------------------------------------------------------------------
	//- 绘制 复选框
	//---------------------------------------------------------------------

	virtual void DrawCheckBox(
		Graphics* pGraphics,
		LPCRECT lpRect,
		BOOL bCheck,
		LPCTSTR lptString,
		const Color& clrText,
		CFont* pFont,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign,
		G2XTextVAlign vAlign);

	// 绘制 单选框，只绘制框框，不绘制文本
	virtual void DrawRadioBox(
		Graphics* pGraphics,
		LPRECT lpRect,
		BOOL bCheck,
		G2XControlState ctrlState=G2XControlStateNormal);

	// 绘制复选框
	virtual void DrawCheckBox(
		Graphics* pGraphics,
		LPRECT lpRect,
		BOOL bCheck,
		G2XControlState ctrlState=G2XControlStateNormal);

	//---------------------------------------------------------------------
	//- 绘制 树形控件的折叠图标
	//---------------------------------------------------------------------

	virtual void DrawTreeIcon(
		HG2XWND, 
		Graphics*, 
		LPRECT, 
		G2XNodeState state, 
		G2XControlState ctrlState);

	virtual void DrawText(
		Graphics* pGraphics,
		LPCTSTR lpText,
		LPCRECT lpRect,
		CFont* pFont,
		const Color& clrText,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign=G2XTextHAlignLeft,
		G2XTextVAlign vAlign=G2XTextVAlignMiddle,
		BOOL bTrimmingText=FALSE);

	virtual void DrawText(
		Graphics* pGraphics,
		LPCTSTR lpText,
		LPCRECT lpRect,
		Font* lpFont,
		const Color& clrText,
		G2XControlState ctrlState,
		G2XTextHAlign hAlign=G2XTextHAlignLeft,
		G2XTextVAlign vAlign=G2XTextVAlignMiddle,
		BOOL bTrimmingText=FALSE);


	virtual void DrawCommandBarButtonMask(
		Graphics* pGraphics,
		CG2XCommandBarButton* pBtn,
		LPCRECT lpRect,
		G2XControlState state
		);
public:
	CColor colorDisableBorder; // 界面不可用时的边框颜色
	CColor colorDisableBk;     // 界面不可用时的背景颜色
	CColor colorDisableText;   // 文本不可用时的颜色
	CColor colorText;          // 通用文本的颜色
	CColor colorBorder;        // 通用边框的颜色
	CColor colorHoverBorder;   // 鼠标悬浮上空时的边框颜色
	Font* m_lpDefaultFont;     // 默认字体
	Gdiplus::SmoothingMode m_graphicsSmoothingMode; // 默认模式为消除锯齿
};


G2X_END