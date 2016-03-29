#pragma once

#include "G2XWnd.h"
#include "G2XImageList.h"

G2XUI_BEGIN

struct G2XCommandBarButtonInfo
{
	UINT nID;
	CString strTitle;
	INT nImgIndex;
	BOOL bEnable;
	BOOL bSelected;
	COLORREF clrText;
	BOOL bHover;
};

class G2X_CLASSES CG2XCommandBar;

class G2X_CLASSES CG2XCommandBarButton 
{
public:
	CG2XCommandBarButton(G2XCommandBarButtonInfo& btnInfo, CG2XCommandBar* pBar);
	virtual void DrawButton(Graphics* pGraphics, CRect& rt);

	CG2XCommandBar* m_pBar;
	G2XCommandBarButtonInfo m_btnInfo;
};

//-------------------------------------------------------------------------
// CG2XCommandBar
//		就像QQ电脑管家、360一样的工具栏（图片+文字的按钮）
//      CG2XCommandBar背景默认透明的
// 基本使用: 1) 调用Create
//          2) 调用SetImageList,如果没用图片可以使用不调用
//          3) 调用InsertButtons,插入一组按钮
//-------------------------------------------------------------------------
class G2X_CLASSES CG2XCommandBar : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XCommandBar)
	DECLARE_GDI_MESSAGE_MAP()
	friend class CG2XCommandBarButton;
public:
	CG2XCommandBar();
	virtual ~CG2XCommandBar();

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);

	g_msg void OnCreate();
public:
	//---------------------------------------------------------------------
	//- 创建
	//- bHasTitle: 按钮是否有文本标题
	//- lpBtnSize:
	//      [NULL]: CG2XCommandBar将会自动调整按钮的大小
	//     [!NULL]: 每个按钮都是这个固定大小
	//---------------------------------------------------------------------
	BOOL Create(
		CG2XWnd* pParent, 
		CRect rcInParent,        
		BOOL bHasTitle,          
		UINT nID, 
		LPSIZE lpBtnSize=NULL    // if lpBtnSize=NULL buttons auto size, else fixed size
		);

	// 设置图像列表
	BOOL SetImageList(const CG2XImageList& imgList);

	// 插入命令按钮
	void InsertButtons(G2XCommandBarButtonInfo* pBtns, int nCount);

	// 调整布局
	void Relayout();

	// 设置某个按钮为被选中状态
	void SetSelBtn(INT iIndex);

	// 设置某个按钮为被选中状态
	void SetSelBtnByID(UINT nID);

	// 使按钮可用或不可用
	BOOL EnableButton(UINT nID, BOOL bEnable);

	// 命令映射到按钮的索引
	INT CommandToIndex(UINT nID);

	// 移除按钮
	BOOL RemoveButton(UINT nID);
	
	// 插入按钮,如果nIndex指定的是一个无效位置，按钮将会插入到末尾
	void InsertButton(INT nIndex, G2XCommandBarButtonInfo& info);

	// 按钮数量
	INT GetCount();

protected:
	CG2XImageList* m_pImgList;

	std::vector<CG2XCommandBarButton*> m_arrBtns;
	std::vector<RECT> m_arrBtnRect;

	SIZE m_szImg;
	INT m_iSelBtn;
	INT m_iSelTemp;
	BOOL m_bHasTitle;

	SIZE *m_pFixedSize;
};

G2X_END