#pragma once

//========================================
// CG2XFoundationWnd
//   G2XLib库的基础界面类, CWnd中建立的G2XLib UI界面的顶层G2XUI窗口应该使用
//   CG2XFoundationWnd类对象。
// CG2XFoundationWnd由两部分组成， 标题栏和客户区
//   标题栏: CG2XCaptionBar对象
//   客户区: CG2XPanel对象
//========================================

G2XUI_BEGIN
G2X_FRAME

class G2X_CLASSES CG2XFoundationWnd : public CG2XWnd
{
	DECLARE_DYNCREATE(CG2XFoundationWnd)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XFoundationWnd();
	virtual ~CG2XFoundationWnd();

protected:
	CG2XCaptionBar* m_pCaptionBar; // 标题栏
	CG2XPanel* m_pClientArea;      // 客户区域面板
};
G2X_END
G2X_END