#pragma once
#include <afxhtml.h>

//事件回调接口，可以是接口也可以是类

//如果是接口就要在调用者类里面实现所有的事件
//有时我们只需要实现一两个事件，所以这里用类来实现事件回调接口

//把所有的事件都放在这个里面

class G2X_CLASSES CG2XWebBrowserEventHandler
{
protected:

	friend class CG2XWebBrowser;	 //让事件源可以调用事件回调函数
	
	CG2XWebBrowserEventHandler(){}; //保护构造函数，只能继承，不能实例化

	//具体的事件回调函数
	
	//第一个参数为事件源，只要是为了区分具体的事件源

	virtual void WebBrowser_BeforeNavigate2(CG2XWebBrowser& Sender, LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel){}

	virtual void WebBrowser_TitleChange(CG2XWebBrowser& Sender, LPCTSTR lpszText){}

	virtual void WebBrowser_DocumentComplete(CG2XWebBrowser& Sender, LPCTSTR lpszURL){}

	virtual void WebBrowser_NewWindow2(CG2XWebBrowser& Sender, CG2XWebBrowser* ppDisp, BOOL* Cancel){}

	virtual void WebBrowser_NavigateError(CG2XWebBrowser& Sender, LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel){}

	virtual HRESULT WebBrowser_GetExternal(CG2XWebBrowser& Sender, LPDISPATCH *lppDispatch){return S_FALSE;}
};


class G2X_CLASSES CG2XWebBrowser: public CHtmlView
{
	DECLARE_DYNCREATE(CG2XWebBrowser)
	DECLARE_MESSAGE_MAP()

public:

	CG2XWebBrowser();          
	virtual ~CG2XWebBrowser();

	//设置或者取消事件回调接口
	void SetEventHandler(CG2XWebBrowserEventHandler* lpEventHandler = NULL);

	BOOL IsContextMenuEnabled() const { return m_bContextMenuEnabled; }
	void EnableContextMenu(BOOL newVal = TRUE) { m_bContextMenuEnabled = newVal; }

	BOOL IsFlatStyle() const { return m_bIsFlatStyle; }
	void SetFlatStyle(BOOL newVal = TRUE) { m_bIsFlatStyle = newVal; }

	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID, CCreateContext* pContext);
protected:

	virtual void PostNcDestroy();

	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	
	BOOL PreCreateWindow(CREATESTRUCT & cs);

	

protected:

	//CHtmlView已经用虚函数实现事件的响应
	
	//具体是如何实现事件的响应可以查看CHtmlView的源码

	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);

	virtual void OnTitleChange(LPCTSTR lpszText);

	virtual void OnDocumentComplete(LPCTSTR lpszURL);

	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);

	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);

	virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
		LPUNKNOWN pcmdtReserved, CG2XWebBrowser pdispReserved);

protected:

	BOOL m_bContextMenuEnabled;

	BOOL m_bIsFlatStyle;

	//这里只有一个事件回调接口（其实可以用数组或链表来保存多个事件回调接口）
	//但大都情况下一个事件回调接口就已经足够了

	CG2XWebBrowserEventHandler* m_lpEventHandler;

};

