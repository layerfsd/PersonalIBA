#pragma once

#include "IBADialog.h"
//
//class:CDlgBossPwd 没有权限时，弹出此对话框输入权限密码
//
class CDlgBossPwd : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgBossPwd)
	DECLARE_MESSAGE_MAP()

public:
	CDlgBossPwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBossPwd();

// 对话框数据
	enum { IDD = IDD_DIALOG_BOSSPWD };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	virtual void OnOK();

	CString m_strPwd;

};
//
//class:CDlgBossPwd 没有权限时，弹出此对话框输入权限密码
//
class CDlgBossPwdEx : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgBossPwdEx)
	DECLARE_MESSAGE_MAP()

public:
	CDlgBossPwdEx(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgBossPwdEx();

	// 对话框数据
	enum { IDD = IDD_DIALOG_BOSSPWD_EX };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	virtual void OnOK();

	CString m_strPwd;

public:
	CString m_strCaption;
	CString m_strRemark;
};