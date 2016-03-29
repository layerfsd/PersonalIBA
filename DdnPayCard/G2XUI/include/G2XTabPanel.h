/**************************************************************************
CG2XTabPanel: tab面板集合
	CG2XTabPanel中包含多个子面板, 但同一时间只能显示一个面板在最上层
**************************************************************************/

#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XTabPanel : public CG2XPanel
{
	DECLARE_DYNCREATE(CG2XTabPanel)
public:
	CG2XTabPanel();
	virtual ~CG2XTabPanel();

protected:
	DECLARE_GDI_MESSAGE_MAP()

	g_msg void OnCreate();
	g_msg void OnSize();

public:

	//---------------------------------------------------------------------
	//- 插入面板
	//- iIndex: 插入的位置, -1表示插入到尾部
	//- pNewPanel: 要插入的面板
	//---------------------------------------------------------------------
	BOOL InsertPanel(INT iIndex, CG2XPanel* pNewPanel);

	//- 获取面板个数
	INT GetCount();

	//- 激活面板
	void ActivePanel(INT iIndex);

	//- 通过面板的控件ID来激活面板
	BOOL ActivePanelByID(UINT nPanelID);

	//- 获取面板
	CG2XPanel* GetPanel(INT iIndex);

	//- 通过面板的控件ID来获取面板
	CG2XPanel* GetPanelByID(UINT nPanelID);

	//- 设置子面板和CG2XTabPanel边缘的间隙
	void SetSpacing(UINT nPixel=2);

	//- 获取活动面板
	CG2XPanel* GetActivePanel();

	//- 移除面板, 面板占用的资源也会被删除
	BOOL RemovePanel(INT iIndex);

	//- 移除所有面板
	BOOL RemoveAll();

	//- 交换面板位置
	BOOL ExchangePanel(INT nPanel1, INT nPanel2);

protected:
	typedef std::vector<CG2XPanel*> PanelList;
	PanelList m_panelList;

	// 子面板和CG2XTabPanel边缘的空隙大小
	INT m_nSpacing;

	INT m_nActivePanel;
};

G2X_END