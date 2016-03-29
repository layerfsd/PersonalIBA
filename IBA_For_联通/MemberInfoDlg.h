#pragma once
/**************************************************************************
提供查找和选择用户的中间对话框
**************************************************************************/

#include "IBADialog.h"

//
//class:CDlgMemberInfo 查询会员的对话框，在快速通道里输入账号可能对应多个用户，CDlgMemberInfo确保获取唯一的一个会员
//    可以认为CDlgMemberInfo是过渡窗口
//
class GPropertyClass CDlgMemberInfo : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgMemberInfo)
	DECLARE_MESSAGE_MAP()

public:
	CDlgMemberInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMemberInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_MEMBERINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnNMDblclkListMemberinfo(NMHDR *pNMHDR, LRESULT *pResult);

public:
	GProperty( UserName,   CString ); // 用户名称
	GProperty( PersonalID, CString ); // 证件号码
	GProperty( Sex,        CString ); // 性别
	GProperty( NetId,      CString ); // 用户账号
	GProperty( UserClass,  CString ); // 用户级别
	GProperty( IDType,     CString ); // 证件类型

	GPropertyReadOnly( MemberId, UINT ); // 用户ID
	
public:
	
	// 查询方式
	// 0: 为按姓名，证件号码从中心查询
	// 1: 从本地member表查询
	// 2: 中心查询方式，有效用户
	void SetQueryStyle(UINT NewValue) { m_nQueryStyle = NewValue; }

private:
	
	UINT m_nQueryStyle; // 查询方式

	// 控件变量
	CBCGPListCtrl m_MemberInfoList;

private:
	//从中心查询,查询方式0为按姓名，证件号码从中心查询，1为按账号从本地member表查询
	void FindInfoFromServer(UINT nConsumeType = 0);

	// 设置列表子项数据
	void SetMemberInfoToList(UINT nQueryStyle);

	// 初始化列表
	void InitList(void);

	// 取得选中的用户信息
	int GetMemberInfoFromList(void);

	// 从本地查找数据
	void FindInfoFromLocal(BOOL bValidUser);
};
