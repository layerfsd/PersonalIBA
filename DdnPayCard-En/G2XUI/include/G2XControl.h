#pragma once
#include "G2XWnd.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XControlColorData
{
public:
	CColor colorNormalBk;		// 控件正常状态时的背景颜色
	CColor colorNormalText;		// 控件正常状态时的文字颜色
	CColor colorNormalBorder;	// 控件正常状态时的边框颜色
	
	CColor colorHoverBk;		// 鼠标滑过控件时的背景颜色
	CColor colorHoverText;		// 鼠标滑过控件时的文字颜色
	CColor colorHoverBorder;	// 鼠标滑过控件时的边框颜色

	CColor colorSinkageBk;		// 鼠标按下时的背景颜色
	CColor colorSinkageText;	// 鼠标按下时的文字颜色
	CColor colorSinkageBorder;	// 鼠标按下时的边框颜色

	CColor colorDisableBk;		// 控件不可用时的背景颜色
	CColor colorDisableText;	// 控件不可用时的文字颜色
	CColor colorDisableBorder;	// 控件不可用时的边框颜色

	const CColor& GetBkColor(G2XControlState ctrlState) const;
	const CColor& GetTextColor(G2XControlState ctrlState) const;
	const CColor& GetBorderColor(G2XControlState ctrlState) const;
public:
	CG2XControlColorData::CG2XControlColorData()
	{
		colorNormalBk		= CColor(RGB(255,255,255));
		colorNormalText		= CColor(RGB(0,0,0));
		colorNormalBorder	= CColor(RGB(185,205,226));

		colorHoverBk		= CColor(RGB(255,255,255));
		colorHoverText		= CColor(RGB(0,0,0));
		colorHoverBorder	= CColor(RGB(58,146,199));

		colorSinkageBk		= CColor(RGB(255,255,255));
		colorSinkageText	= CColor(RGB(0,0,0));
		colorSinkageBorder	= CColor(RGB(58,146,199));

		colorDisableBk		= CColor(RGB(237,237,237));
		colorDisableText	= CColor(RGB(177,177,177));
		colorDisableBorder	= CColor(RGB(177,177,177));
	}
	CG2XControlColorData::CG2XControlColorData(const CG2XControlColorData& colorData)
	{
		CopyFrom(colorData);
	}
	void CopyFrom(const CG2XControlColorData& colorData);
};

class G2X_CLASSES CG2XControl : public CG2XWnd
{
	DECLARE_DYNCREATE(CG2XControl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XControl(void);
	virtual ~CG2XControl(void);

	static const VSCROLLBAR_ID = 101;
	static const HSCROLLBAR_ID = 102;
public:
	//---------------------------------------------------------------------
	//- 创建控件
	//- halign: 控件文本水平对齐方式
	//- valign: 控件文本垂直对齐方式
	//- borderStyle: 控件边框风格
	//---------------------------------------------------------------------
	BOOL CreateControl(
		CG2XWnd*  pParent,
		LPCTSTR  lpText,
		LPCRECT  lpRect,
		UINT  nCtrlID,
		G2XTextHAlign  halign=G2XTextHAlignLeft,
		G2XTextVAlign  valign=G2XTextVAlignMiddle,
		G2XBorderStyle borderStyle = G2XBorderNone);

	//
	void EnableMultiStyleText(BOOL bEnable);

	//- 得到控件当前的状态
	G2XControlState GetControlState();
	
	//- 在控件有焦点的情况下，是否允许绘制焦点矩形
	void SetDrawFocus(BOOL bDrawFocus);
	
	//- 此刻是否需要绘制焦点矩形, 派生类重画时,需要调要此函数来决定是否要画焦点矩形
	BOOL GetIsDrawFocus();

	//---------------------------------------------------------------------
	//- 设置图像列表
	//- imgList: 传入的图像列表
	//- bWndFitImage: 控件是否自动适应图像的大小
	//    [TRUE]: 调整控件的大小，使得它和图像的大小一致
	//   [FALSE]: 控件保持原样
	//---------------------------------------------------------------------
	BOOL SetImageList(const CG2XImageList& imgList, BOOL bWndFitImage = FALSE);

	//---------------------------------------------------------------------
	//- 设置图像列表
	//- nImgWidth: 子图像的宽度, 图像列表的子图像个数=图像的宽度/nImgWidth, 向上取整
	//- nIDRes: 资源ID
	//- lpszResType: 位图指定为RT_BITMAP,自定义的需要指定名称,例如:"PNG"
	//- bWndFitImage: 控件是否自动适应图像的大小
	//    [TRUE]: 调整控件的大小，使得它和图像的大小一致
	//   [FALSE]: 控件保持原样
	//- lpTransparentClr: 设置图像的透明色, 使得绘制图像时，不绘制透明色指定的颜色点
	//---------------------------------------------------------------------
	BOOL SetImageList(INT nImageWidth, UINT nResID, LPCTSTR lpResType=RT_BITMAP, BOOL bWndFitImage=FALSE, Color* lpTransparentClr=NULL);

	//---------------------------------------------------------------------
	//- 设置图像列表
	//- nImgWidth: 子图像的宽度, 图像列表的子图像个数=图像的宽度/nImgWidth, 向上取整
	//- lpszImagePath: 图像的路径
	//- bWndFitImage: 控件是否自动适应图像的大小
	//    [TRUE]: 调整控件的大小，使得它和图像的大小一致
	//   [FALSE]: 控件保持原样
	//- lpTransparentClr: 设置图像的透明色, 使得绘制图像时，不绘制透明色指定的颜色点
	//---------------------------------------------------------------------
	BOOL SetImageList(INT nImageWidth, LPCTSTR lpszImagePath, BOOL bWndFitImage=FALSE, Color* lpTransparentClr=NULL);

	CG2XImageList* GetImageList() { return m_pImageList; }

public:

	// 重载CG2XWnd::DrawSelf
	void DrawSelf(Gdiplus::Graphics *pGraphics);

	// 重载此函数，可以重绘控件边框
	virtual void DrawControlBorder(Graphics* pGraphics, G2XControlState ctrlState, G2XBorderStyle borderStyle);
	
	// 自绘控件必须重载此函数,才能完成绘图工作
	virtual void DrawControl(Graphics* pGraphics, G2XControlState ctrlState);

//private:
	// 重载CG2XWnd::DrawBorder
	void DrawBorder(Graphics *pGraphics);

protected:
	g_msg void OnCreate();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnLButtonUp(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();

protected:
	BOOL m_bMouseSinkage; // 鼠标处于下沉状态，即鼠标左键按下并没有释放
	BOOL m_bMouseHover;
	G2XTextHAlign m_HTextAlign;
	G2XTextVAlign m_VTextAlign;
	BOOL m_bDrawFocus;
	BOOL m_bNotify;
	
	CSize m_szRoundCorner; // 边框圆角的大小，默认SIZE(0,0)
public:
	CG2XImageList* m_pImageList; // 如果没有调用SetImageList,m_pImageList指针将为空
	CG2XControlColorData m_colorData;
};

G2X_END
