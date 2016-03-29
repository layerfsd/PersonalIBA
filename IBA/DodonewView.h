#pragma once



// CDodonewView Html 视图

class CDodonewView : public CHtmlView
{
	DECLARE_DYNCREATE(CDodonewView)

protected:
	CDodonewView();           // 动态创建所使用的受保护的构造函数
	virtual ~CDodonewView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnNcDestroy();
	void Reflash();
};


