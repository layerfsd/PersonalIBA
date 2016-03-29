#pragma once



// CShortMsgDlg 对话框

class CShortMsgDlg : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_DYNAMIC(CShortMsgDlg)

public:
	CShortMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShortMsgDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SHORTMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSendshormsg();
	afx_msg void OnQuickMsg(UINT id);
	afx_msg void OnEditQuickMsg();

	void ShowPopupMenu();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


private:
	CFont m_TextFont;//文字字体
	CString m_ShortMsgContent;//短信内容
	CBCGPComboBox m_cboPCClass;//
	CUIntArray m_nReceivers;//接收者
	CStringArray m_QucikMsgs;//短信队列
	
	UINT m_nSendType;//发送类型

public:

	void AddReceiver(UINT newVal) { m_nReceivers.Add(newVal); }
	void SetReceivers(CUIntArray& newVal) { m_nReceivers.Copy(newVal); }

	// nType = 1: 发送给所选终端,
	// nType = 2: 发送给其他收银台
	void SetSendType(UINT nType);
};
