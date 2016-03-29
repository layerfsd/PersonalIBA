#pragma once

#include "NDDialog.h"
#include "ActiveMember.h"
#include "BCGPLabalButton.h"
#include "BCGPLinkStatic.h"
#include "afxwin.h"

// CNDReturnDlg �Ի���

class CNDReturnDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNDReturnDlg)
	DECLARE_MESSAGE_MAP()

public:
	CNDReturnDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNDReturnDlg();

// �Ի�������
	enum { IDD = IDD_ND_DREG };

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCredit();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonConsumelist();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonReturn2();
	afx_msg void OnBnClickedButtonBundletime();
	afx_msg void OnBnClickedButtonConsumelist2();
	afx_msg void OnBnClickedButtonCancelactive();

	void OnGetBalanceOK(CBalanceMethod & Balance);
	void InitControls();
	BOOL PayoutMember(CString &ErrMsg);
	BOOL DeregisterUser(CString &ErrMsg,UINT nReturnMode = 0);
	void ShowBundleTimeInfo(CBalanceMethod & Balance);

	// 2011/04/29-gxx: 
	//virtual void OnCenterDisconnect();
	
	// 2011/04/29-gxx: ���������ĶϿ�����ʱ�Ľ��˴���
	//BOOL DoLocalModeReturn();  

	// 2011/07/12-8201-gxx: ���ؽ��˲�ѯ
	virtual BOOL DoLocalModeQuery();

	// 2011/07/12-8201-gxx: ���ؽ��˴���
	//virtual BOOL DoLocalModeOperate();

	// 2011/10/26-8230-gxx: 
	BOOL ReturnRoomUser();

	void PrintTicket(CActiveMember& ActiveMember);

private:

	BOOL m_bFromCard;			// ��Ϣ�Ƿ�����ˢ��

	CActiveMember m_ActiveMember;

	BOOL m_bAutoQuery;

	BOOL m_bUserHasCard; //!<�û��Դ���

	CString m_strPwd;//����Ŀؼ�����

	//INT m_nAllMoney; //!<�ϼ����

	CString m_strLastLogoutTime;
	CString m_strDeposit;
	CString m_strUserInfo;
	CString m_strBundleTimeInfo;

	CBCGPLabelButton m_btnConsumeList;
	CBCGPLabelButton m_btnCredit;
	CBCGPLabelButton m_btnReturn2;
	CBCGPLabelButton m_btnBundleTime;
	CBCGPLabelButton m_btnZSBConsumeList;
	CBCGPLabelButton m_btnCancelActivation;

	BOOL m_bPrintTicket;
	INT m_nConsumeMoney;

public:
	
	void SetNetId(CString newVal);
	
		// 2014-9-16 - qsc
		void SetFromCard(BOOL bFromCard = TRUE) { m_bFromCard = bFromCard; }
};