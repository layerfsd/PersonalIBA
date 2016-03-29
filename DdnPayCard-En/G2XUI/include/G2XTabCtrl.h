#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XTabCtrl;
class G2X_CLASSES CG2XTabCtrlItem
{
public:
	CG2XTabCtrlItem(CG2XTabCtrl* pTabCtrl);
	virtual ~CG2XTabCtrlItem();
	void CopyFrom(const CG2XTabCtrlItem& item);
	CString strText;
	CColor clrText[G2XControlStateCount];
	CColor clrBk[G2XControlStateCount];
	INT iImages[G2XControlStateCount];
	CRect  rcItem;
	DWORD dwData;
	BOOL bSelected;
	Font* lpFont;
	CG2XTabCtrl* m_pTabCtrl;

	virtual void DrawItem(Graphics* pGraphics, LPRECT lpRect, G2XControlState ctrlState);
};

class G2X_CLASSES CG2XTabCtrl : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XTabCtrl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XTabCtrl();
	virtual ~CG2XTabCtrl();

public:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);
	
	g_msg void OnCreate();
	g_msg void OnSize();
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnLButtonUp(UINT nFlags, CPoint point);
	

public:
	INT GetCount();
	void AddTab(CG2XPanel* pPanel, LPCTSTR lpTitle, INT iImage=-1);
	void AddTab(CG2XPanel* pPanel, const CG2XTabCtrlItem& item);
	void InsertTab(INT iWhere, CG2XPanel* pPanel, LPCTSTR lpTitle, INT iImage=-1);
	void InsertTab(INT iWhere, CG2XPanel* pPanel, const CG2XTabCtrlItem& item);
	CG2XPanel* GetPanel(INT iTabIndex);
	CG2XTabPanel* GetTabPanel();
	CG2XTabCtrlItem* GetTabItem(INT iTabIndex);
	INT GetCurSel();
	void SetCurSel(INT iTabIndex);
	void DeleteTab(INT iTabIndex);
	void DeleteAll();
	
	void AdjustLayout();
	void ReplaceTab(INT iWhere, CG2XTabCtrlItem* pNewItem);
	void SetItemsHeight(UINT nNewHeight);
	RECT GetPanelArea(); // 获取面板区域

protected:
	INT TestPointAtItem(CPoint point);
protected:
	std::vector<CG2XTabCtrlItem*> m_items;
	CG2XTabPanel *m_pTabPanel;
	UINT m_nItemsHeight; // tab项的高度
	INT m_iHoverItem;
	INT m_iSinkageItem; 
	INT m_nLeftestItemMargin;
	BOOL m_bHasPanelBorder; 
};

G2X_END
