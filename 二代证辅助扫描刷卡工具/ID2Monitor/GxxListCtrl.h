#pragma once
/*********************************************************************************************
CGxxHeaderCtrl:
	派生于CHeadCtrl：提供文本的颜色设置功能，表头的高度设置功能，背景颜色设置功能

CGxxListCtrl:
	派生于CListCtrl,提供以下几种功能：
		1、提供单元格支持常用控件或自定义控件的功能
		2、提供表头的设置功能
		3、提供单元格文本的颜色/背景色设置的功能
		4、支持两种方式的单元格控件功能：列共享式和列非共享式
		5、提供列表基本控制功能
	注：
	1、列共享式：整列共用一个控件，子窗口类型或子窗口指针在插入列时被指定
	2、非列共享式：子窗口类型或子窗口指针在插入项时指定,调用SetItem
		给指定单元格设置文本/控件，并且子窗口会一直显示,不会消失

	3、如果要添加属于自己的控件类型，需要重载CreateChildWnd（）,但新的
		控件类型不能和已有的空间类型相同。

Author:guoxuxing
Date:2010/3/1
*********************************************************************************************/

#include <vector>
#include <afxcmn.h>
#include <afxwin.h>
using namespace std;


///
///
/// CGxxListCtrl
/// CGxxListCtrl::CListCtrl 
///
/// 注：注该列表不适合大量数据的显示，因为这样会占用很大的存储空间
///
	typedef struct NMHDR_GLC_CONTROL
	{
		NMHDR hdr;
		int nRowID;
		int nColID;
		int nKind;  // CGxxListCtrl::emChildWndKind
		LPNMHDR pCtrlHdr;
	}*LPNMHDR_GLC_CONTROL;

#define GLCN_CHILD_CONTROL (1)

	class  CGxxListCtrl:public CListCtrl
	{
		DECLARE_DYNAMIC(CGxxListCtrl)

	public:
		CGxxListCtrl();
		virtual ~CGxxListCtrl();

		///所有无法识别的都认为是自定义的子窗口
		enum emChildWndKind
		{
			GLC_CUSTOM = 0,    //自定一个风格需要提供子窗口窗口指针
			
			/// 注:非自定义的子窗口都需要删除内存,CGxxListCtrl已做了此工作
			///
			
			GLC_Button = 1,    //按钮子窗口
			GLC_Edit   = 2,    //编辑框子窗口
			GLC_ComboBox = 3,  //组合框子窗口
			GLC_CheckBox = 4,  //checkBox子窗口
			GLC_RadioBox = 5,  //RadioBox子窗口
			GLC_Static = 6     //静态文本 
		};

		///单元格的描述信息
		struct SubItemStruct
		{
			CWnd* pChildWnd;     // 子窗口指针
			int   nChildWndKind; //子窗口的类型
			BOOL  bAutoAdjust;   //子窗口的大小随单元格的大小改变而改变
			SIZE  szWnd;         //子窗口的大小，bAutoAdjust为FALSE才有效
			int   nAlignment;    //子窗口在单元格中的对齐方式，bAutoAdjust为FALSE才有效，DT_LEFT,DT_CENTER,DT_RIGHT
			COLORREF clrTxt;     //单元格文本的颜色
			COLORREF clrBk;      //单元格背景色

			DWORD dataPtr;

			SubItemStruct()
			{
				pChildWnd = NULL;
				nChildWndKind = GLC_CUSTOM;
				bAutoAdjust = TRUE;
				szWnd.cx = 0;
				szWnd.cy = 0;
				nAlignment = DT_CENTER;
				clrTxt = RGB(0,0,0);
				clrBk = RGB(255,255,255);
				dataPtr = 0;
			}
		};

		///列的描述信息
		struct ColInfo
		{
			BOOL	bHaveChildWnd;  //决定该列是否支持子窗口,在插入列时指定

			/// 注:
			///   共享式：子窗口类型或子窗口指针在插入列时指定
			///   非共享式：子窗口类型或子窗口指针在插入项时指定
			///
			BOOL	bShared;        //决定该列的所有单元格共享一个子窗口
			BOOL	bAutoAdjust;    //决定子窗口是否随单元格的大小改变而改变

			/// 注：
			///   bHaveChildWnd 与 bShared都为True时，以下参数才会有用
			///
			int		nChildWndKind;
			CWnd*	pChildWnd;
			BOOL    bShown;
			int		iShowAtItem;
			SIZE    szWnd;

			DWORD   txtAlign;

			ColInfo()
			{
				bHaveChildWnd = FALSE;
				bShared = FALSE;
				nChildWndKind = 0;
				pChildWnd = NULL;
				bAutoAdjust = TRUE;
				bShown = FALSE;
				iShowAtItem = -1;
				szWnd.cx = 0;
				szWnd.cy = 0;
				txtAlign = DT_LEFT;
			}
		};
	protected:
		DECLARE_MESSAGE_MAP()

		afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
		void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

		virtual void PreSubclassWindow();

	public:
		///-------------------------------------------
		/// Description:插入列
		/// Returns [int]: 
		/// Parameters:            
		///    [ int nCol ]:					插入位置
		///    [ LPCTSTR lpszColumnTxt ]:	列标题
		///    [ int nFormat ]:				列对齐方式,LVCFMT_LEFT...
		///    [ int nWidth ]:				列宽
		///    [ BOOL bHaveChildWnd ]:		该列是否支持子窗口
		///    [ BOOL bShared ]:				是否共享式的支持子窗口
		///    [ BOOL bAutoAdjust ]:			决定该列的子窗口是否随单元格大小改变而改变
		///    [ int nChildWndKind ]:		该列的子窗口类型，当bShared为TRUE，才有效
		///    [ CWnd * pChildWnd ]:			该列的子窗口指针，当bShared和nChildWndKind=GLC_CUSTOM时，才有效
		/// Remark:
		///		当指定nChildWndKind为非GLC_CUSTOM时，CGxxListCtrl会调用CreateChildWnd为支持
		///    子窗口的单元格创建子窗口对象.
		/// Author:      
		/// CreateTime:2010/03/24    
		///-------------------------------------------
		virtual int  InsertColumn(
			int     nCol,
			LPCTSTR	lpszColumnTxt,
			int		nFormat,
			int		nWidth,
			BOOL	bHaveChildWnd,
			BOOL	bShared=FALSE,
			BOOL	bAutoAdjust=FALSE,
			LPSIZE  lpCtrlSize = NULL,
			int		nChildWndKind=GLC_CUSTOM,
			CWnd*	pChildWnd=NULL);


		///-------------------------------------------
		/// Description:插入列,不具有控件的支持           
		/// Returns [int]: 
		/// Parameters:            
		///    [ int nCol ]:   插入位置
		///    [ LPCTSTR lpszColumnTxt ]: 列标题  
		///    [ int nFormat ]:  列对齐方式 
		///    [ int nWidth ]:   列宽
		/// Author:       
		/// CreateTime:2010/03/24    
		///-------------------------------------------
		virtual int InsertColumn(
			int		nCol,
			LPCTSTR lpszColumnTxt,
			int     nFormat=LVCFMT_LEFT,
			int		nWidth = -1
			);

		/// 删除列
		virtual int DeleteColumn(int nCol);
		
		/// 插入一行
		virtual int InsertItem(
			int nItem, CString str,
			int nChildWndKind=GLC_CUSTOM,
			CWnd* pChildWnd=NULL,
			LPSIZE lpSize=NULL);
		
		/// 删除一行
		virtual BOOL DeleteItem(int nItem);

		/// 删除所有
		BOOL DeleteAllItems();
		
		/// 设置单元格项
		virtual int SetItem(
			int nItem,
			int nSubItem,
			CString str,
			int nChildWndKind=GLC_CUSTOM,
			CWnd* pChildWnd=NULL,
			LPSIZE lpSize=NULL);

		void GetItemIdByChildWndID(UINT nID,int* pItem,int*  pSubItem = NULL);
		
		/// 隐藏掉共享式的子控件
		void HideAllSharedWnd();

		/// 注册一个ID
		UINT RegeisterCtrlID();

	protected:
		
	protected:

		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		virtual void DrawSubItem(CDC* pDC,int nItem,int nSubItem,UINT itemState=ODS_DEFAULT);
		virtual void DrawSubItemText(CDC* pDC,int nItem,int nSubItem,LPCTSTR lpTxt,LPRECT lpRect,UINT itemState);
		
		virtual CWnd* CreateChildWnd(int nChildWndKind);

		virtual CString GetChildWndText(CWnd*pWnd,int nItem,int nSubItem);
		virtual void SetChildWndText(CWnd*pWnd,CString str, int nItem,int nSubItem);

	public:
		RECT GetSubItemRect(int iItem,int iSubItem);

		void SetHeaderFont(CFont* pFont);
		/// 设置行高
		void SetItemsHeight( int iHeight );
		
		/// 刷新所有子窗口显示
		void ShowAllSubWindows();
		
		/// 获取列数
		int GetColumnCount();
		
		/// 某列是否支持子窗口
		BOOL  GetColHasChildWnd(int iCol);
		/// 某列是否为列共享式
		BOOL  GetColIsShared(int iCol);
		/// 某列的子窗口类型
		int  GetColChildWndKind(int iCol);
		/// 某列的子窗口指针
		CWnd* GetColChildWnd(int iCol);
		/// 列共享式的子窗口的当前状态是否为显示状态
		BOOL  GetColChildWndShown(int iCol);
		/// 列共享式的子窗口的当前所在行
		int   GetColChildWndAtItem(int iCol);
		/// 设置列共享式的子窗口的显示状态
		void  SetColChildWndShown(int iCol,BOOL bShow);
		/// 设置列共享式的子窗口的当前所在行
		void  SetColChildWndAtItem(int iCol,int iAtItem );

		/// 得到单元格子窗口类型
		int GetSubItemWndKind(int iItem,int iSubItem);
		/// 得到单元格子窗口指针，无效返回NULL
		CWnd *GetSubItemWnd(int iItem,int iSubItem);
		/// 得到单元格子窗口ID，无效返回0
		UINT GetSubItemWndId(int iItem,int iSubItem);
		/// 得到单元格文本颜色
		COLORREF GetSubItemTxtColor(int iItem,int iSubItem);
		/// 得到单元格背景颜色
		COLORREF GetSubItemBkColor(int iItem,int iSubItem);
		/// 设置单元格文本颜色
		void	SetSubItemTxtColor(int iItem,int iSubItem,COLORREF clr);
		/// 设置单元背景颜色
		void	SetSubItemBkColor(int iItem,int iSubItem,COLORREF clr);

		/// 设置单元格数据
		void SetSubItemData(int iItem,int iSubItem,DWORD dataPtr);
		DWORD GetSubItemData(int iItem,int iSubItem);

		int GetHeaderHeight();

		SubItemStruct* GetSubItemStruct(int iItem, int iSubItem);

		BOOL GetColIsAutosize(int iCol);

	protected:

		CButton m_testBtn;
		CEdit m_testEdit;

		int m_nItemHeight;

		vector< ColInfo > m_vecColsInfo;  //存放各列的信息
		vector< vector<SubItemStruct> > m_vecSubItems; //存放各单元格的信息

		UINT m_nInnerMaxCtrlID;


	protected:
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		afx_msg BOOL OnEraseBkgnd(CDC*);

	public:
		virtual void OnClickItem(int nItem,int nSubItem);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

