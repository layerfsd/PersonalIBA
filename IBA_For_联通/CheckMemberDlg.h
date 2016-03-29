#pragma once

#include "Servlet\BalanceMethod.h"
#include "IBADialog.h"
#include "NetBarUser.h"

//
//class:CCheckMemberDlg 检查会员的对话框
//
class CCheckMemberDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CCheckMemberDlg)
	DECLARE_MESSAGE_MAP()

public:
	CCheckMemberDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckMemberDlg();

// 对话框数据
	enum { IDD = IDD_RN_CHECKMEMBER };

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	virtual void OnCancel();

	void DoModify();

public:
	
	void SetRealName(CNetBarUser& NetBarUser);
	void SetServlet(NS_SERVLET::CBalanceMethod& BalanceMethod);
	void SetCardID(CString strCardID);

private:
	//中心里登录名称，由用户号生成
	CString m_strMemberName;
	//中心里开户网吧ID
	UINT m_nNetbarID;
	//中心的密码
	CString m_strPwd;

	// 实名卡上网账号
	CString m_strRNCardID;
	// 中心保存的上网账号
	CString m_strSLCardID;
	// 实名卡姓名
	CString m_strRNName;
	// 中心姓名
	CString m_strSLName;
	// 实名卡证件类型
	CString m_strRNCertificateType;
	UINT m_nRNCertificateType;
	// 中心证件类型
	CString m_strSLCertificateType;
	UINT m_nSLCertificateType;
	// 实名卡证件号码
	CString m_strRNCertificateID;
	// 中心证件号码
	CString m_strSLCertificateID;
 
	// 提示文本
	CStatic m_STTip;
};
