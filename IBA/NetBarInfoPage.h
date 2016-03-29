#pragma once

#include "IBADialog.h"
#include "NetBarConfig2.h"
#include "afxwin.h"

//
//class: CDlgNetbarConfig 门店建设设置对话框
//
class CDlgNetbarConfig : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgNetbarConfig)
	DECLARE_MESSAGE_MAP()

public:
	CDlgNetbarConfig();
	virtual ~CDlgNetbarConfig();

// 对话框数据
	enum { IDD = IDD_NETBARINFO };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSync();//中心时间同步
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);//系统热键
	afx_msg void OnBnClickedButtonAllcheckout();

private:

	CString m_strCenterIP;//中心IP
	UINT m_nCenterPort;//中心端口
	CString m_strPath;//未用
	UINT m_nNetBarID;//网吧id

	CString m_strNetBarKey;//网吧识别码
	CString m_strNetBarName;//网吧姓名
	CString m_strPrincipal;//负责人
	CString m_strAddr;//网吧地址
	CString m_strTel;//网吧联系电话
	CString m_strCashRegisterId;//收银台id
	CString m_strBossPwd;//老板密码
	CString m_IServerIP;//服务器IP
	CString m_strMAC;//网卡地址

	CNetBarConfig2 m_NetBarConfig2;//不常用的网吧信息

	INT m_nLocalModeStartKind; // 2011/08/10-8201-gxx: 应急模式的启用方式

private:

	void InitData();
	void SaveData();

//public:
//	afx_msg void OnBnClickedRadio1();
//	afx_msg void OnBnClickedRadio2();
//	afx_msg void OnBnClickedRadio3();
//	void DoChangeLocalStartKind(INT nNewKind); // 2011/08/10-8201-gxx: 
};
