#pragma once

#include "NDDialog.h"
#include "afxwin.h"
// CMergeMemberDlg 对话框

class CMergeMemberDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CMergeMemberDlg)
	DECLARE_MESSAGE_MAP()

public:
	CMergeMemberDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMergeMemberDlg();

// 对话框数据
	enum { IDD = IDD_ND_MERGEMEMBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	//******************主账户部分********************//
	// 上网账号
	CString m_strNetId;
	// 用户名字
	CString m_strUserName;
	// 用户号
	CString m_strMemberId;
	// 证件号
	CString m_strPersonalId;
	// 开户网吧
	CString m_strNetbarName;
	// 用户级别
	CString m_strClassName;
	// 无送账号
	CString m_strWSAmount;
	// 有送账户
	CString m_strYSAmount;
	// 赠送账户
	CString m_strZSAmount;
	// 合计账户
	CString m_strAllAmount;
	//****************被合并账户部分*****************//
	// 上网账号
	CString m_strNetId2;
	// 用户名字
	CString m_strUserName2;
	// 用户号
	CString m_strMemberId2;
	// 证件号
	CString m_strPersonalId2;
	// 开户网吧
	CString m_strNetbarName2;
	// 用户级别
	CString m_strClassName2;
	// 无送账号
	CString m_strWSAmount2;
	// 有送账户
	CString m_strYSAmount2;
	// 赠送账户
	CString m_strZSAmount2;
	// 合计账户
	CString m_strAllAmount2;

private:

	CFont m_FontAllMoney;

	BOOL m_bPrimary;

	CString m_strCardID;
	CString m_strMemberName;
	UINT m_nUserClassID;
	INT m_nAllMoney;

	CString m_strCardID2;
	CString m_strMemberName2;
	UINT m_nUserClassID2;
	INT m_nAllMoney2;

private:
	void InitControls();
	void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);
	void OnOldCard(UINT nCardId, UINT nMemberId);

	void OnNewCard(UINT nCardId);
	BOOL CheckInput();

	void ClearData();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnSetfocusEditIdcardsno();
	afx_msg void OnEnSetfocusEditIdcardsno2();
	afx_msg void OnBnClickedButtonQuery();
	void DoQuery(CString strNetId);
private:
	// 必须捆绑变量才能接受控件消息
	CEdit m_edtNetId;
	// 必须捆绑变量才能接受控件消息
	CEdit m_edtNetId2;

public:
	// 被合并账户密码
	CString m_strPassWord;
};
