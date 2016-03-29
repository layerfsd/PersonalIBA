#pragma once

#include "IBADialog.h"
#include "common\GxxListCtrl.h"

class CBCGPGridButtonItem : public CBCGPGridItem
{
public:
	CBCGPGridButtonItem(LPCTSTR lpszText, UINT id);

protected:
	virtual CWnd* CreateInPlaceEdit (CRect /*rectEdit*/, BOOL& /*bDefaultFormat*/)
	{
		return NULL;
	}

	virtual BOOL OnSetCursor () const
	{
		return FALSE; /* Use default */	
	}

	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);

	UINT	m_id;
};

//
//class:CDlgAllCheckOut 全场结账对话框
//

class CDlgAllCheckOut : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgAllCheckOut)
	DECLARE_MESSAGE_MAP()

public:
	CDlgAllCheckOut(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAllCheckOut();

	// 对话框数据
	enum { IDD = IDD_DIALOG_ALLCHECKOUT };

	struct ReturnInfo
	{
		CCheckOutinfo CheckOutInfo;
		CLocalConsumeInfo LocalInfo;
		BOOL bLocal;
		BOOL bLocalReturned;
	};
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonQuery();
	afx_msg HRESULT OnItemChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnBnClickedGirdButton();

protected:

	// 初始化列表
	BOOL InitList(void);

	// 为列表赋值
	BOOL FillList(void);

	//是否为会员
	BOOL IsMember(CString MemberName, UINT& nClassId);

	void AddRow(INT_PTR nIndex, ReturnInfo &rInfo);

	void SelFirstOne();

	//创建新行
	CBCGPGridRow* CreateNewRow();

	//会员结账
	BOOL PayoutMember(UINT nMemberId);

	//临时用户退款
	BOOL DeregisterUser(UINT nMemberId,UINT nReturnMode = 0);

private:
	// 捆绑控件，登出用户信息
	CBCGPGridCtrl  m_GridCtrl;

	// 2011/09/01-8201-gxx: 
	NS_COMMON::CGxxListCtrl m_listCtrl;

	CString m_strFind;

	static const UINT GirdButtonId = 100;

	CArray<ReturnInfo> m_ReturnInfoArray;

	CArray<CActiveMember> arrMembers;

	BOOL m_bHasAllout;
public:

	static BOOL SaveToDB(CCheckOutInfoArray& CheckOutArray);


protected:
	int GetActiveMembers(CArray<CActiveMember>& ActiveMembers);

	
	afx_msg void OnBnClickedButtonPrint();
	void OnGLCNotify(LPNMHDR lpHdr, LRESULT *pResult);
};
