#pragma once
#include "WebBrowser.h"
#include "HoverButton.h"
#include "ImageEx.h"
#include "Common.h"

// CTipsWnd

class CTipsWnd : public CWnd, public CWebBrowserEventHandler
{
	DECLARE_DYNAMIC(CTipsWnd)
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()

public:
	CTipsWnd();
	virtual ~CTipsWnd();

protected:
	
	virtual void WebBrowser_TitleChange(CWebBrowser& Sender, LPCTSTR lpszText);
	virtual void WebBrowser_DocumentComplete(CWebBrowser& Sender, LPCTSTR lpszURL);
	virtual void WebBrowser_NewWindow2(CWebBrowser& Sender,LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual HRESULT WebBrowser_GetExternal(CWebBrowser& Sender, LPDISPATCH *lppDispatch);


public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedClosebtn();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void PostNcDestroy();
	virtual BOOL DestroyWindow();

private:
	Tips_ShowParam m_tips_showParam;

	BOOL m_blisComplete;
	BYTE m_Factor;

	static const UINT TimerIDOfStartDisplay = 1000; //开始显示
	static const UINT TimerIDOfDisplaying = 1001; //显示当中
	static const UINT TimerIDOfDisapper = 1002; //渐隐

public:
	CImageEx m_backImg;
	CRect m_ClientRect;

	CWebBrowser *m_pWebBrowser;
	CHoverButton m_CloseBtn;
	CStatic      m_TitleStatic;
	
	CFont m_TextFont;

	int m_nTimeCount;

	LPDISPATCH m_lpExternalDisp;

private:
	CString CombinationWebPage(CString strTitle, CString strContent);

	int WirteWebPage(CString strTitle, CString strContent);
	void OpenUrl(CString strUrl, LPCTSTR postData, LPCTSTR headData);

	BOOL Aminhide();

	void StartDisplayWindow();
	void DisplayingWindow(UINT nTimerEvent);
	BOOL DisapperWindow(UINT nTimerEvent);
	BOOL IsMouseInWindowRect(); //鼠标在窗口中为TURE
	void HideBrowserBorder();

	BOOL CreateWebBrower();
	virtual void InitResource(); //初始资源
	virtual void CalculateControlPostion(); //计算控件位置
	virtual BOOL DrawWindow(CDC* pDC); //绘制窗口
	
	virtual HBRUSH SetStaticTitle(CDC* pDC); //绘制窗口

	LPDISPATCH GetHostApp();

	void HideTips();

	void SetAppearTime(INT nAppearTime);

	void OnLookDetail();

public:
	void SetTipsParam(Tips_ShowParam tips_showParam);

	void ShowTips(CString strUrl);

	void ShowTips(CString strTitle, CString strContents);
};


