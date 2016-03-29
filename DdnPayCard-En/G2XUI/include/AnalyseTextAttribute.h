#pragma once
#include <vector>

G2XUI_BEGIN

#define G2X_INVALID_COLOR (COLORREF(-1))

struct G2X_CLASSES CG2XTextNode
{
	CG2XTextNode();

	CString strFontName; // 字体名称,如果为空表明为默认字体
	BYTE nFontSize;      // 字体大小
	CString strFontStyle;// 字体风格
	COLORREF clrText;    // 文字颜色
	
	COLORREF clrBk;      // 背景颜色,G2X_INVALID_COLOR表示透明


	INT nSelectFrom;	 // 文本的选择起点
	INT nSelectTo;		 // 文本的选择终点
	BYTE nReturnChar;	 // 来绝点行的第一个节点是否有回车符
	Font * pFont;		 // 字体
	INT nCX;			 // 节点占用的宽度
	INT nCY;			 // 节点占用的高度
	INT nCharCount;      // 字符的个数
	
protected:
	CString strText;     // 文本

public:
	void RefreshSize();
	void ClearSelect();

	CString& GetText() { return strText; }
	void SetText(LPCTSTR lpszText);

	BOOL IsEmptyNode() { return strText.IsEmpty(); }

	INT GetLengthByFrom();
	INT GetLengthByTo();

	CString GetSelectString();
};

class G2X_CLASSES CAnalyseTextAttribute;

//-------------------------------------------------------------------------
// CAnalyseTextAttribute
//   多样式文本的分析和绘制
//-------------------------------------------------------------------------
class G2X_CLASSES CAnalyseTextAttribute
{
public:
	CAnalyseTextAttribute(void);
	~CAnalyseTextAttribute(void);

	//- 获取字符个数对应的字符串长度
	static INT GetLegthByCharCount(LPCTSTR lpszText, INT nCharCount);

	//- 获取字符串中的字符个数, 无论汉字还是字母都会认为是单个字符
	static INT GetCharCount(LPCTSTR lpszText);

	static SIZE MessageString(LPCTSTR, Font* pFont);

	//---------------------------------------------------------------------
	//- lpszText: 文本格式
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
	//- pG2XWnd: 相关联的CG2XWnd
	//- bIgnoreReturn: 是否忽略回车符
	//
	// 注: 如果文本中含有回车符,可以转换成 &#13;, 换行符需要转换成 &#10;
	// <font> 和 <text> 支持嵌套
	//---------------------------------------------------------------------
	BOOL Analyse(LPCTSTR lpszText, CG2XWnd* pG2XWnd, BOOL bIgnoreReturn = FALSE);

	//- 自动换行分析
	void AnalyseAutoReturn();

	//- 解析后节点的数量,(最原始的节点数量)
	int GetCount();

	//---------------------------------------------------------------------
	// 绘制文本
	//- G2XTextHAlign: 水平对齐方式
	//- G2XTextVAlign: 垂直对齐方式
	//- G2XControlState: 控件状态
	//---------------------------------------------------------------------
	virtual void DrawText(Graphics* pGraphics, LPRECT lpRect, G2XTextHAlign, G2XTextVAlign, G2XControlState);

	//---------------------------------------------------------------------
	// 绘制文本( 自动换行 )
	//- G2XTextHAlign: 水平对齐方式
	//- G2XTextVAlign: 垂直对齐方式
	//- G2XControlState: 控件状态
	//---------------------------------------------------------------------
	virtual void DrawTextEx(Graphics* pGraphics, LPRECT lpRect, G2XTextHAlign, G2XTextVAlign, G2XControlState);

	//- 计算文本最小要满足的大小
	CSize CalcSize();

	//- 获取整串文本,忽略标签
	CString GetText();

	//- 获取整个字符串中的选择的文本
	CString GetSelectedString();
	
	//- 获取文本的选择范围
	//- pAbsFrom: 取得绝对的字符起始位置
	//- pAbsTo: 取得绝对的字符结束位置
	void GetSel(INT * pAbsFrom, INT * pAbsTo);

	//- 设置选择的文本
	//- nAbsFrom: 绝对的起始字符位置
	//- nAbsTo: 绝对的结束字符位置
	void SetSel(INT nAbsFrom, INT nAbsTo);

	//- 替换选择的文本
	void ReplaceSelected(LPCTSTR lpstr);

	//- 清除选择状态
	BOOL ClearSel();

	//- 移除被选择的文本
	BOOL RemoveSelected();

	//- 在指定位置处插入字符串
	BOOL InsertString(INT nAbsWhere, LPCTSTR lpszText);

	void SetSelToHome();
	void SetSelToEnd();

	//- 处理退格键的输入
	BOOL DoBackspace();

	//- 处理回车键的输入
	BOOL DoEnter();

	//- 处理DEL键的输入
	BOOL DoDelete();

private:
	void AnalyseText(IXMLDOMNode* pNode, IXMLDOMNode* pNodeText, IXMLDOMNode* pNodeFont);
	
	
protected:
	COLORREF GetColor(LPCTSTR lpszColor, LPCTSTR lpszAttrName);
	DOMNodeType GetNodeType(IXMLDOMNode* pNode);
	CString GetNodeName(IXMLDOMNode* pNode);
	CString GetAttributeValue(IXMLDOMNode* pNode, LPCTSTR lpszAttriName);

public:
	typedef std::vector<CG2XTextNode> NodeArray;	// 节点数组
	typedef std::vector<INT> IntArray;				// 整形数组
	typedef NodeArray NodeLine;						// 节点行
	typedef std::vector<NodeLine> LineArray;		// 行数组
	

	NodeArray m_arrayTextNodes; // 分析后，保存的所有文本节点

	LineArray m_arrayLines;    // 每一行的节点数组
	IntArray m_arrayHeight;     // 每一行的行高
	IntArray m_arrayWidth;      // 每一行的行宽

	//-自动换行需重新分析每一行的信息
	LineArray m_arraysNodeEx;
	IntArray m_arrayHeightEx;     // 每一行的行高
	IntArray m_arrayWidthEx;      // 每一行的行宽
	

	BOOL m_bIgnoreReturn;       // 是否忽略回车符, 默认不忽略

	CG2XWnd* m_pG2XWnd;

	BOOL m_bAutoReturn;
	INT m_nAutoReturnWidth;		// 自动换换行时的宽度参考

	BYTE m_nLineSpacing;		// 行间距

	COLORREF m_clrDefaultText;	// 默认的文本颜色

	INT m_nCursorAt;            // 光标所在位置

	COLORREF m_clrCursor;
	BOOL m_bShowCursor;
	UINT m_nTotalCharCount;		// 总共的字符个数

	INT m_nDefaultFontHeight;

public:
	//- 获取有选择字符串的节点
	void GetSelectedNodes(NodeArray& nodes);

	void InitializeBlankNode(CG2XTextNode& tn);

private:
	INT GenerateNodes(INT nOffX, INT nMaxWidth, CG2XTextNode* pNode,LineArray& lines,CG2XTextNode*& pLast);

	BOOL CreateFontForNode(CG2XTextNode& node);

	void GenerateLines();

	void GenerateSize();

	void GenerateWidthAndHeight();
};

G2X_END

