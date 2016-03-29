#pragma once
#include "AnalyseTextAttribute.h"

G2XUI_BEGIN

/*-------------------------------------------------------------------------
CG2XStaticEx : public CG2XStatic

- 扩展的静态文本控件, 支持样式文本。只需在调用SetWindowText时，指定文本格式。
  请参考SetWindowText的说明。必须直接调用CG2XStaticEx::SetWindowText才会有作用
-------------------------------------------------------------------------*/
class G2X_CLASSES CG2XStaticEx : public CG2XStatic
{
	DECLARE_DYNCREATE(CG2XStaticEx)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XStaticEx(void);
	~CG2XStaticEx(void);

	//---------------------------------------------------------------------
	//- bMulLines: 是否支持多行，如果不支持文本中的回车符被忽略
	//- hAlign: 文本的水平对齐方式
	//- vAlign: 文本的垂直对齐方式
	//---------------------------------------------------------------------
	BOOL Create(
		CG2XWnd* pParent, 
		LPCTSTR lpWndText, 
		LPCRECT lpRect, 
		UINT nID, 
		BOOL bMulLines=FALSE, 
		G2XTextHAlign hAlign=G2XTextHAlignLeft, 
		G2XTextVAlign vAlign=G2XTextVAlignMiddle);

	//---------------------------------------------------------------------
	//- strText: 文本格式
	//    支持标签有: 
	//       <font>...</font>
	//          属性:
	//             name: "字体名称", 例如: name="宋体"
	//             size: 字体大小, 例如: size="16"
	//             style: 风格,例如: style="bold,underline,italic"
	//          注: 如果不指定属性,文本使用控件的字体和大小
	//
	//       <text>..</text>
	//          属性:
	//             color: 文字颜色, 例如: color="RGB(0,0,255"
	//             bk-color: "文字背景颜色, 例如: bk-color="RGB(255,255,255)"
	//          注: 如果不指定属性,文本颜色默认为黑色, 背景颜色默认为透明
	//
	// 注: 如果文本中含有回车符,可以转换成 &#13;, 换行符需要转换成 &#10;
	// <font> 和 <text> 支持嵌套
	//---------------------------------------------------------------------
	void SetWindowText(LPCTSTR strText, BOOL bRepaint=TRUE);

	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

public:
	CAnalyseTextAttribute m_textAttrs;
	BOOL m_bMulLines;
};

G2X_END
