//==============================================
//= CG2XTreeCtrl 树形控件
//=  树形控件只有一个根节点，根节点可以隐藏起来，根节点没有父节点
//=  有子节点的节点可以展开和折叠子节点列表, 前提是节点的bEnable属性指定true
////============================================
#pragma once

G2XUI_BEGIN

struct CG2XTreeNode;
typedef std::vector<CG2XTreeNode*> CG2XTreeNodeList;

// 树节点
struct CG2XTreeNode
{
	CG2XTreeNode* pParentNode;				// 父节点, NULL表示此节点为根节点
	CG2XTreeNodeList children;	// 子节点列表
	INT iIndex;                         // 节点在父节点的孩子节点链中的位置(加快迭代速度)
	G2XNodeState state;					// 节点的状态
	INT iImage;							// 图像列表中的索引
	CString strText;					// 节点的文字内容
	DWORD dataPtr;                      // 节点的数据
	CG2XWnd* pNodeWnd;					// 节点窗口，一般为NULL
	BOOL bEnable;						// 节点是否处于允许状态
	BOOL bSelected;
	CRect rc;                           // 节点所在区域
	INT nNodeHeight;                    // 节点所在位置的高度
};



// 树路径, 空白路径对应的节点为根节点
// 例如: 路径(0->1)表示根节点下的第1个节点的第2个子节点,依次类推
struct G2X_CLASSES CG2XTreePath : public std::vector<INT>
{

};

class G2X_CLASSES CG2XTreeCtrl : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XTreeCtrl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XTreeCtrl();
	virtual ~CG2XTreeCtrl();

public:
	// 创建树形控件
	BOOL Create(CG2XWnd* pParent, BOOL bShowRootNode, LPCRECT lpRect, UINT nID);

	// 调整树形控件的显示
	void AdjustTree();

public:
	// 获取根节点
	CG2XTreeNode* GetRoot();

	// 创建一个空白节点
	CG2XTreeNode* CreateNode();

	// 得到路径对应的节点
	CG2XTreeNode* GetTreeNode(const CG2XTreePath& path);

	// 得到节点对应的路径
	CG2XTreePath GetPath(CG2XTreeNode* pNode);

	// 删除路径对应的节点
	void DeleteTreeNode(const CG2XTreePath& path, BOOL bUpdate=TRUE);

	//----------------------------------------
	// 添加节点
	//- path: 添加的节点所在路径位置
	//- bAsChild:TRUE, 新节点作为path对应节点的子节点，且新节点被添加到末尾
	//- bAsChild:FALSE,在path指定的位置处插入节点
	//----------------------------------------
	void AddNode(
		const CG2XTreePath& path, 
		CG2XTreeNode* pNewNode, 
		BOOL bAsChild,
		BOOL bUpdate=TRUE);

	//----------------------------------------
	// 插入节点
	//- path: 在path指定的位置处插入节点
	//----------------------------------------
	void InsertNode(
		const CG2XTreePath& path,
		CG2XTreeNode* pNewNode,
		BOOL bUpdate=TRUE);

	//----------------------------------------
	// 展开/折叠节点
	//- bExpanding:TRUE, 展开节点
	//- bExpanding:FALSE, 折起节点
	//----------------------------------------
	void ExpandingNode(const CG2XTreePath& path, BOOL bExpanding, BOOL bUpdate=TRUE);


	//----------------------------------------
	// 返回某个节点的下一个节点,
	// 注: 忽略被折叠的节点(如果pPreNode有孩子并且它是折叠的，那么它的孩子不会被遍历到)
	//- return NULL: 表明迭代完毕
	//----------------------------------------
	CG2XTreeNode* NextNode(CG2XTreeNode* pPreNode);

	//----------------------------------------
	// 返回某个节点的下一个节点
	// 注: 不忽略任何节点(就像所有节点被展开时调用NextNode一样)
	//     用于遍历树的所有节点
	//- return NULL: 表明迭代完毕
	//----------------------------------------
	CG2XTreeNode* NextNodeEx(CG2XTreeNode* pPreNode);

	//----------------------------------------
	// 获取节点在树中所处的层次
	// 注: 根节点的层次=0, 其他节点的层 > 0
	//----------------------------------------
	INT GetLayer(CG2XTreeNode* pNode);
	
	//----------------------------------------
	// 获取所有被选择的节点，返回节点个数
	//- pResults: 
	//   [NULL]: 不获取节点
	//   [!NULL]:保存被选择的节点
	//- return: 被选择的节点个数
	//----------------------------------------
	INT GetSelectedNode(std::vector<CG2XTreeNode*> *pResults=NULL);
	
	//----------------------------------------
	//- 对path指定的路径下的直系子节点重新排序
	//----------------------------------------
	void Resort(G2XSortStyle sortStyle,const CG2XTreePath& path);

	//----------------------------------------
	//- 搜索节点
	//- lptszNodeText:要搜索的节点文本
	//- bSearchAll:
	//    [TRUE]: 搜索指定路径下的所有节点
	//    [FALSE]: 搜索指定路径下的第一级的所有孩子节点
	//- pIn: 在指定节点下搜索
	//    [NULL]: 在根节点下搜索
	//----------------------------------------
	CG2XTreeNode* SearchNode(LPCTSTR lptszNodeText, BOOL bSearchAll, CG2XTreeNode* pIn);

	//----------------------------------------
	//- 选择节点
	//- bCenterShown:
	//-   [TRUE]: 在有滚动条的情况下，将节点居中显示在界面中
	//----------------------------------------
	void SelectNode(CG2XTreeNode* pNode, BOOL bSelected=TRUE, BOOL bCenterShown = FALSE);

public:
	//----------------------------------------
	// 测试点所在的节点
	//- return:
	//   [NULL]: 该点不位于任何节点
	//----------------------------------------
	virtual CG2XTreeNode* TestPtInNode(LPPOINT);

	//----------------------------------------
	// 测试点所在图标的节点,图标是指折叠图标
	//- return:
	//   [NULL]: 该点不位于任何节点的图标位置
	//----------------------------------------
	virtual CG2XTreeNode* TestPtInIcon(LPPOINT);

	
protected:
	virtual BOOL CompareItem(G2XSortStyle sortStyle,const CG2XTreeNode* item1, const CG2XTreeNode* item2);
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);
	virtual void DrawNode(Graphics* pGraphics,CG2XTreeNode* pNode, G2XControlState ctrlState);

	g_msg void OnCreate();
	g_msg void OnDestroy();
	g_msg void OnSize();
	g_msg void OnVScrollNotify(NMHDR* pHdr, LRESULT* lpResult);
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();
	g_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

private:
	void DeleteNode(CG2XTreeNode*& pNode, BOOL bUpdate=TRUE);
	
public:
	CG2XImageList m_imgList;
protected:
	CG2XTreeNode *m_pRoot;
	CG2XTreeNode *m_pFirstVisualNode;
	CG2XTreeNode *m_pLastVisualNode;
	BOOL m_bShowRootNode;
	
	CG2XScrollBar *m_pVScrollBar;
	INT m_nIndentation;      // 层与层之间的缩进量
	BOOL m_bNeedIndentation; // 层和层之间是否需要缩进
	SIZE m_szIcon;           // 折叠/展开图标的大小
	BOOL m_bEnableFoldingByClickAnywhere; // 允许点击节点的任何地方进行折叠和展开，如果为FALSE，那么只能点击图标进行折叠和展开
	CG2XTreeNode *m_pHoverNode;
	BOOL m_bMulSelected;     // 允许多选
};

G2X_END