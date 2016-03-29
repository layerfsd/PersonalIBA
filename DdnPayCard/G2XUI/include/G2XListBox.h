#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XListBox : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XListBox)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XListBox(void);
	~CG2XListBox(void);

	struct ListItem
	{
		INT iImage;
		CString strText;
		BOOL bEnable;   // 项是否为允许状态
		BOOL bSelected; // 项是否被选中
		BOOL bHover;    // 是否鼠标在上方滑过
		CRect rcItem;   //
		CG2XWnd* pG2XWnd; // 项的G2XUI窗口指针
		INT nItemHeight; // 项的高度
		DWORD dwData;    // 项数据
	};

	BOOL Create(CG2XWnd* pParent, LPCRECT lpRect, UINT nID, G2XSortStyle sortStyle=G2XSortStyleNormal);

	// 设置是否允许多选
	void SetEnableMulSelect(BOOL bEnable);

	// 设置是否允许鼠标移动项的位置
	void SetEnableMoveItem(BOOL bEnable);

	// 插入字符串
	void InsertString(INT index, LPCTSTR, INT iImage=-1, BOOL bUpdate=TRUE);

	void SetItemWnd(INT iIndex, CG2XWnd* pWnd);

	void AddString(LPCTSTR, INT iImage=-1, BOOL bUpdate=FALSE);
	
	CString GetString(INT nAt);

	void SetItemData(INT iIndex, DWORD dwData);
	DWORD GetItemData(INT iIndex);

	// 擦除字符串
	void EraseString(LPCTSTR);
	void EraseString(INT index);

	// 选择字符串
	void SelectString(LPCTSTR);

	// 选择某项
	void SetCurSel(INT index);

	void SetImage(INT index, INT iImage);

	
	
	// 获取单个的选择项，如果listbox允许多选，那么返回第一个被选择项的位置
	INT GetSingleSel();
	
	// 获取多个被选择的行
	// return: 被选择的行的个数
	INT GetMulSel(IN INT* pResArray, IN const INT nSize);

	INT GetCount();

	// 设置排序风格
	void SetSortStyle(G2XSortStyle style);

	G2XSortStyle GetSortStyle();

	void Resort();
	void AdjustListBox();

	void RemoveAll(BOOL bRepaint=TRUE);

	// 设置ListBox的边框，如果指定为0，ListBox没有边框
	void SetBorder(INT nBorderBreadth=1);

	void SetItemHeight(INT index, INT nHeight);
	
	ListItem* GetItem(INT index);

protected:
	virtual BOOL CompareItem(const ListItem* item1, const ListItem* item2);

	ListItem* CreateEmptyItem();

protected:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);
	virtual void DrawItem(Graphics* pGraphics, ListItem* pItem);
	INT TestPointItem(const CPoint& point);

	g_msg void OnCreate();
	g_msg void OnSize();
	g_msg void OnDestroy();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();
	g_msg void OnVScrollNotify(NMHDR* pHdr, LRESULT* lpResult);
	g_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	g_msg void OnDragging(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnDragged(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnLButtonUp(UINT nFlags, CPoint point);

	virtual void OnItemClicking(INT nItem, UINT nFlags);
	virtual void OnItemClicked(INT nItem, UINT nFlags);

protected:
	std::vector<ListItem*> m_items;
	G2XSortStyle m_sortStyle;
	BOOL m_bEnableMulSelect;
	BOOL m_bEnableMoveItem;
	CG2XScrollBar* m_pVScrollBar;
	ListItem* m_pHoverItem;
	INT m_indexFirstVisual;  // 看的见的第一项
	INT m_indexLastVisual;   // 看的见的最后一项
	G2XUtility::CG2XStatusArray<CString> m_selectStatus;
	INT m_iClickingItem;
	INT m_iOldSel;
};

G2X_END
