#pragma once
#include "g2xpanel.h"

//========================================
//= CG2XSplitPanel 可分割的面板
//=		面板可分割成:
//=			左右两部分 or 上下两部分
//=		分割面板中可以在创建分割面板
//========================================
G2XUI_BEGIN

class G2X_CLASSES CG2XSplitPanel :
	public CG2XPanel
{
	DECLARE_DYNCREATE(CG2XSplitPanel)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XSplitPanel(void);
	virtual ~CG2XSplitPanel(void);

	//----------------------------------------------------
	// style:     面板的分隔风格
	// sizeStyle: 子面板的大小风格
	//----------------------------------------------------
	BOOL Create(
		CG2XWnd* pParent, 
		LPCRECT lpRect, 
		UINT nID, 
		G2XSplitPanelStyle style=G2XSplitPanelStyleHorizontal,
		G2XSplitPanelSizeStyle sizeStyle=G2XSplitPanelSizeStylePercentage);

	// 为分隔的块绑定面板
	void AttachPanelLeft	(CG2XPanel* pPanel);
	void AttachPanelRight	(CG2XPanel* pPanel);
	void AttachPanelTop		(CG2XPanel* pPanel);
	void AttachPanelBottom	(CG2XPanel* pPanel);

	// 调整面板, 重载此方法，子类有机会在调整面板后处理事情
	//   例如： 在分隔线的中间添加按钮，调整按钮位置。
	virtual void AdjustPanel();	
	
	G2XSplitPanelStyle GetStyle();

	CG2XPanel* GetPanelLeft();
	CG2XPanel* GetPanelRight();
	CG2XPanel* GetPanelTop();
	CG2XPanel* GetPanelBottom();

	// 设置分隔线的宽度，如果宽度指定为0，那么分隔线不能显示也不能拖动
	void SetSplitLineBreadth(INT nBreadth);

	//----------------------------------------------------
	// 设置分割线的显示位置的比例，分隔面板大小发生改变时，
	// 第一个子面板和第二个子面板按照这个比例调整大小
	//----------------------------------------------------
	void SetPercentage(BYTE nPercentage);

	//----------------------------------------------------
	// 设置子面板当前保持的大小
	//   m_sizeStyle = G2XSplitPanelSizeStyleFirst/Second时，调用才有效
	//----------------------------------------------------
	void SetKeepSize(UINT nSize);

	//----------------------------------------------------
	// 固定分隔线
	//----------------------------------------------------
	void FixedSplitLine(BOOL bFixed);

	//----------------------------------------------------
	// nDis1: 设置分隔线离第一个面板边界的最小距离
	// nDis2: 设置分隔线离第二个面板边界的最小距离
	//----------------------------------------------------
	void SetMinimizeDistance(UINT nDis1, UINT nDis2);

protected:
	g_msg void OnCreate();
	g_msg void OnMouseMove(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();
	g_msg void OnDragging(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnDragged(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnSize();

	virtual void DrawSelf(Gdiplus::Graphics *pGraphics);

protected:
	RECT GetSplitLineRect(LPPOINT lpPt);

	void DrawTempSplitLine(CPoint& ptTmp);

protected:
	G2XSplitPanelStyle m_style;
	G2XSplitPanelSizeStyle m_sizeStyle;

	CG2XPanel* m_pPanelA;
	CG2XPanel* m_pPanelB;

	// 分隔线的宽度
	INT m_nSplitLineBreadth; 

	// 分隔线的所在位置
	CPoint m_ptSplitLine;

	// 分隔线所在位置的百分比, 0~100
	BYTE m_nPercentage;

	// 是否固定分隔线,固定的分隔线不允许拖动
	BOOL m_bFixedSplitLine;

	INT m_nKeepedSize;

	INT m_nMinimizeDistance1;

	INT m_nMinimizeDistance2;

	HCURSOR m_hSizeCursor;

	CDC* m_pTemDC;
};

G2X_END
