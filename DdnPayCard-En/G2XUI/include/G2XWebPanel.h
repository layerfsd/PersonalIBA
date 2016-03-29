#pragma once
#include ".\\G2XWebBrowser.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XWebPanel :
	public CG2XPanel
{
	DECLARE_DYNCREATE(CG2XWebPanel)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XWebPanel(void);
	~CG2XWebPanel(void);

	BOOL Create(CG2XWnd* pParent, LPCRECT lpRect, UINT nID, LPCTSTR lpURL);

	CG2XWebBrowser* GetWebBrowser();

	BOOL OpenURL(LPCTSTR lpURL);

	BOOL Open(LPCTSTR lpszHTMLCode);

	void SetNoScrollBar(BOOL bNoScrollBar);

protected:
	g_msg void OnCreate();

protected:
	CG2XWebBrowser* m_lpBrowser;

	CString m_strDefaultURL;
};

G2X_END
