#pragma once

#include "IBADialog.h"
#include "afxcmn.h"
#include <wtypes.h>
// CSelComputerDlg 对话框

class CSelComputerDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CSelComputerDlg)
	DECLARE_MESSAGE_MAP()

public:
	CSelComputerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelComputerDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SELTERMID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult);

private:
	// 初始化控件
	BOOL InitCtrl(void);
	// 填充数据
	BOOL FillList(void);
	//选择计算机
	void SelComputer();
	//判断占用状态
	BOOL CheckTakeUp(CString strTermId);
	
private:
	// 终端号
	CString m_strTermId;
	//用户号
	UINT m_nMemberId;
	//返回的终端
	CString m_strSelTermId;
	// 终端列表信息
	CBCGPListCtrl m_ListInfo;

public:
	UINT GetMemberId() const { return m_nMemberId; }
	void SetMemberId(UINT val) { m_nMemberId = val; }

	CString GetSelTermId() const { return m_strSelTermId; }
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
