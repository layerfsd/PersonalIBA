#pragma once

struct CaptureItem
{
	CString strName;  // 系统的名称
	BOOL bDialog;     // 是否为对话框
	CString strWindowName; // 要捕获的窗口标题
	CString strClassName;  // 要捕获的窗口类名称
	UINT nCardId;          // 账号的控件ID
	UINT nNumberId;        // 证件号码的控件ID
	UINT nNameId;          // 姓名的控件ID
};

struct CaptureItemEx
{
	CString strName;  // 系统的名称
	BOOL bDialog;     // 是否为对话框
	CString strWindowName; // 要捕获的窗口标题
	CString strClassName;  // 要捕获的窗口类名称
	UINT nCardId;          // 账号的控件在界面中的顺序
	UINT nNumberId;        // 证件号码的控件在界面中的顺序
	UINT nNameId;          // 姓名的控件在界面中的顺序
};


class CCaptureManager
{
public:
	CCaptureManager(void);
	~CCaptureManager(void);

	void ReadFromConfig();

	CaptureItem& GetItem(int iIndex);

	int GetCount();

	vector<CaptureItem> m_items;
};
