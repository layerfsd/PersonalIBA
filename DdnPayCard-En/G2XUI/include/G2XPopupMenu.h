#pragma  once

G2XUI_BEGIN

class G2X_CLASSES CG2XPopupMenu;
struct G2XPopupMenuItem
{
	UINT nID;			// 菜单项ID, 0表示分隔线
	CString strText;	// 菜单项文本
	INT iImage;			// 图像索引
	CG2XPopupMenu *pChildMenu; // 子菜单
	BOOL bEnable;
};

#define ID_G2XUI_CHILD_MENU (UINT(-1))

class G2X_CLASSES CG2XPopupMenuListBox : public CG2XListBox
{
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XPopupMenuListBox();
	virtual void DrawItem(Graphics* pGraphics, ListItem* pItem);

	g_msg void OnMouseHover(UINT nFlag, CPoint point);
	g_msg void OnDestroy();

	INT m_nInsdention; // 文字缩进像素数
	INT m_nInsdentionRight; // 文字右边缩进像素数

	//- 获取listbox完全容纳所有项的最合适的大小
	CSize GetBestSize();
};

class G2X_CLASSES CG2XPopupMenu : public G2XUIFrame::CG2XFoundationWindow
{
	DECLARE_MESSAGE_MAP()
public:
	CG2XPopupMenu();

	//---------------------------------------------------------------------
	//- 创建
	//- bAutoWidth: 菜单的宽度是否自动适应
	//-    [FALSE]: nWidth生效
	//- nWidth: bAutoWidth为FALSE时,菜单的固定宽度
	//- pParentWnd: 菜单的父窗口, 接收菜单的命令
	//- pOwnerWnd: 菜单的所有者的G2XUI窗口, 菜单的命令事件会发送给它
	//- pParentMenu: 父菜单
	//---------------------------------------------------------------------
	BOOL Create(BOOL bAutoWidth, INT nWidth,CWnd* pParentWnd, CG2XWnd* pOwnerWnd, CG2XPopupMenu* pParentMenu=NULL);

	//- 菜单的初始化动作
	virtual void OnInitialUpdate();

public:

	//- 插入菜单项, 无效的nIndex效果将会和AppendMenuItem效果一致
	void InsertMenuItem(INT nIndex, const G2XPopupMenuItem& item);

	//---------------------------------------------------------------------
	//- 插入子菜单
	//- nIndex: 插入的位置, 无效的位置表示插入到末尾
	//- lpText: 子菜单的标题
	//- pChildPopup: 子菜单的指针, 子菜单对象必须使用CreateChildPopupMenu来创建
	//- iImage: 子菜单项的图像索引, 菜单需要已调用SetImageList
	//---------------------------------------------------------------------
	void InsertPopupMenu(INT nIndex, LPCTSTR lpText, CG2XPopupMenu* pChildPopup, INT iImage=-1);

	//- 插入分隔线
	void InsertSeparator(INT nIndex);

	//- 添加菜单项
	void AppendMenuItem(const G2XPopupMenuItem& item);
	
	//---------------------------------------------------------------------
	//- 添加菜单项
	//- nID:
	//   [ID_SEPARATOR]: 表示分割线
	//   [ID_G2XUI_CHILD_MENU]: 子菜单
	//   [其他]: 菜单项ID
	//- iImage: 图像索引, 菜单需要已调用SetImageList
	//---------------------------------------------------------------------
	void AppendMenuItem(LPCTSTR lpText, UINT nID, INT iImage=-1); 

	//---------------------------------------------------------------------
	//- 添加子菜单
	//- lpText: 子菜单的标题
	//- pChildPopup: 子菜单的指针, 子菜单对象必须使用CreateChildPopupMenu来创建
	//- iImage: 子菜单项的图像索引, 菜单需要已调用SetImageList
	//---------------------------------------------------------------------
	void AppendPopupMenu(LPCTSTR lpText, CG2XPopupMenu* pChildPopup, INT iImage=-1);

	//- 添加一个分隔线
	void AppendSeparator();

	//- 显示菜单,传入屏幕的坐标位置
	void ShowPopupMenu(INT nPosX, INT nPosY);

	//- 设置图像列表
	void SetImageList(const CG2XImageList& imgList);
	
	//- 创建一个弹出式的子菜单
	CG2XPopupMenu* CreateChildPopupMenu();

	//- 获取顶层的菜单
	CG2XPopupMenu* GetTopMenu();

	//- 隐藏菜单
	//- pSelf: 隐藏pSelf菜单下的所有子菜单
	//- bSelfHide: 是否也隐藏自身
	//- pExcept: 除了pExcept指向的菜单不隐藏，其他的子菜单全部隐藏
	void HideChildMenu(CG2XPopupMenu* pSelf, BOOL bSelfHide, CG2XPopupMenu* pExcept=NULL);

	//- 通过菜单项ID获取菜单项在菜单里的位置, 不遍历它的子菜单, 无效的菜单项返回-1
	INT CommandToIndex(UINT nID);

	G2XPopupMenuItem* GetMenuItem(INT nIndex);

	//- 获取菜单项
	INT GetMenuItemCount();

	//- 擦除菜单项
	BOOL EraseMenuItem(INT iIndex);
	
	//- 擦除菜单项
	BOOL EraseMenuItemByID(UINT nID);

	//- 禁用或启用菜单项
	BOOL EnableMenuItem(INT iIndex, BOOL bEnable);
	
protected:
	void DoLoseFocus();
	CG2XPopupMenu* FindMenu(CG2XPopupMenu* pTop, HWND hWnd);

protected:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	g_msg void OnListBoxClick();

protected:
	CG2XPopupMenuListBox* m_pListBox;

public:
	BOOL m_bAutoWidth;
	INT m_nMenuWidth;
	CG2XWnd* m_pG2XOwner;
	CG2XPopupMenu* m_pParentMenu;
	BOOL m_bMenuShown;
};

G2X_END