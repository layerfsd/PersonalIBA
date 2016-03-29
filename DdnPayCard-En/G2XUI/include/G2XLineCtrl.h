#pragma once

namespace G2XUI{
	

class G2X_CLASSES CG2XLineCtrl : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XLineCtrl)
public:
	CG2XLineCtrl();
	virtual ~CG2XLineCtrl();

	DECLARE_GDI_MESSAGE_MAP()

public:
	//---------------------------------------------------------------------
	//- (x,y): 直线的起始坐标,以它为起点，绘制水平向右或者垂直向下的直线
	//- nBreadth: 直线的粗度
	//- nLong: 直线的长度
	//- bHorizontal: TRUE:水平方向, FALSE:垂直方向
	//---------------------------------------------------------------------
	BOOL Create(CG2XWnd* pParent, INT x, INT y, INT nBreadth, INT nLong, bool bHorizontal, UINT nID);

	void SetGradientStyle(G2XGradientStyle style, const Color& clr1, const Color& clr2, 
		Color* pClr3=NULL, Color* pClr4=NULL, BOOL bReDraw=TRUE);

public:
	virtual void DrawControl(Graphics* pGraphics, G2XControlState ctrlState);

	G2XGradientStyle m_styleGradient;

	CColor m_colors[4];
};




}