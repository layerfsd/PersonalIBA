#pragma once

#include "IBADialog.h"
#include ".\NDDialog.h"
#include "BCGPLabalButton.h"
#include "common\GxxListCtrl.h"

//class: CNetbarRoomDlg 包房管理对话框

class CNetbarRoomDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNetbarRoomDlg)
	DECLARE_MESSAGE_MAP()

public:
	CNetbarRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetbarRoomDlg();

// 对话框数据
	enum { IDD = IDD_ND_ROOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual void OnOldCard(UINT nCardId, UINT nMemberId);
	virtual void OnNewCard(UINT nCardId);
	virtual void ClearData();
	virtual void OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance);

	// 2011/07/22-8201-gxx: 
	virtual BOOL DoLocalModeQuery();
	virtual BOOL DoLocalModeOperate();

	void FillRoomList();

private :
	INT m_nAllMoney;
	CString m_strPwd;

	CString m_strIdCardSN;

	CString m_strCardID;

	CString m_strMemberID;

	CString m_strNetBar;

	CBCGPLabelButton m_btnCredit;
	NS_COMMON::CGxxListCtrl m_listCtrl;

	CNetbarRoomMethod m_roomMethod;
	CRoomInfoList m_arrRoomInfos;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnQuery();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnCredit();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnGlcNotify(NMHDR *pNMHDR, LRESULT *pResult);
	void ShowListCheckOutBtn(INT nRow , BOOL bShow);
	void ShowListOpenBtn(INT nRow, BOOL bShow, BOOL bOpened, BOOL bEanble);
	BOOL DoRoomCheckOut(CRoomInfo& Info);
	void AutoQueryRoom();
	BOOL ClearActiveMember(UINT nMemberID);
};
