// ID2ReaderDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "SShowPic.h"
#include <afxmt.h>

#include "G2XThreadTemplate.h"
#include "DelFilePastDayThread.h"
using namespace G2XUtility;

#define WM_UPDATE_DATA (WM_USER+100)

// CID2ReaderDlg 对话框
class CID2ReaderDlg : public CDialog, public CG2XThreadDelegate
{
// 构造
public:
	CID2ReaderDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CID2ReaderDlg();

// 对话框数据
	enum { IDD = IDD_ID2READER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnUIUpdateData(WPARAM wParam, LPARAM lParam);
	CString m_strName;
	CString m_strSex;
	CString m_strNation;
	CString m_strBirthday;
	CString m_strAddress;
	CString m_strIdNumber;
	CString m_strOrg;
	CString m_strOutOfDate;

	CString m_strImgPath;

	// 删除日志文件线程
	CDelFilePastDayThread m_threadDelFile;
protected:
	virtual void didInitInstance(CG2XThread* pThread);
	virtual int didExitInstance(CG2XThread* pThread);
	virtual void didRun(CG2XThread* pThread);
	virtual bool didCanExitThread(CG2XThread* pThread);

public:
	// 清空所有信息
	void Clear();

	BOOL m_bReadOK;
	CString m_strDate;
	CSShowPic m_staticPic;
	afx_msg void OnTimer(UINT nIDEvent);

	DWORD m_dwReserveTime; // 保留时间

	CCriticalSection m_cs;
	CG2XThread m_thread;
	BOOL m_bDeviceOpened;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
};
