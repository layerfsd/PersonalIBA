#pragma once
#include "afxwin.h"
#include "common\ThreadTemplate.h"
#include "Excel\CriticalSection.h"

struct IDInfo
{
	TCHAR idNumber[20];
	TCHAR name[10];
	BYTE sex; // 0: 女 1:男
};

struct TaskNode
{
	IDInfo idInfo; // 证件信息
	int nDeltaSeconds; // 秒数增量
	int nAmount; // 金额, 分
	//int dayIndex;
};

struct ConsumeNode
{
	TCHAR idNumber[20];
	UINT nMemberId;
	TCHAR checkInTime[20];
	TCHAR checkOutTime[20];
	TCHAR name[20];
	COleDateTime OleCheckOutTime;
	COleDateTime OleCheckInTime;
	int	nAmount;
	int nTimeConsume;
	BOOL bInvalid;
};

struct DeRegisterNode
{
	TCHAR idNumber[20];
	UINT nMemberId;
	COleDateTime OldDeRegisterTime;
	TCHAR name[10];
	BOOL bInvalid;
};


class CAutoRegisterConfig
{

public:
	std::vector<UINT> m_array24Amount;
	
	std::vector<UINT> m_arrayRandomAmount;

	COleDateTime m_startTime;
	COleDateTime m_endTime;

	CString m_strFilePath;

	bool SetTimeRange(
		CDateTimeCtrl& start, CDateTimeCtrl& end, 
		CString& strStartMin, CString& strEndMin,
		int nStartHour, int nEndHour);

	void SetRandomAmount(CString& strRandomAmount);
	void Set24Amount(CString& str24Amount);

	void SaveTo();
	void ReadFrom();

	CString GetRandomAmount();
	CString Get24Amount();
};

// CDlgAutoRegister dialog

class CDlgAutoRegister : public CDialog
{
	DECLARE_DYNAMIC(CDlgAutoRegister)

public:
	CDlgAutoRegister(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAutoRegister();

// Dialog Data
	enum { IDD = IDD_AUTO_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSelect();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg LRESULT OnInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskOver(WPARAM wParam, LPARAM lParam);

	BOOL OnInitDialog();

	bool SaveConfig();
	void SetRandomArray(int *pArray, int nFac, int nDayIndex);

	void Run();

	void Run2();

	void Run3();

	void RegisterID(TaskNode& task);
	void SendConsumeRecord(ConsumeNode& node);
	void DeRegister( DeRegisterNode& node );

	CString m_strTotalAmount;
	CString m_str24Amount;
	CString m_strRandomAmount;
	CDateTimeCtrl m_dateCtrlStart;
	CDateTimeCtrl m_dateCtrlEnd;
	CListBox m_listBox;
	int m_nStartHour;
	int m_nEndHour;
	CString m_strStartMin;
	CString m_strEndMin;
	CString m_strFilePath;

	CButton m_btnStart;
	CButton m_btnStop;

	CAutoRegisterConfig m_config;

	std::vector<IDInfo> m_arrayIDs;
	std::vector<TaskNode> m_arrayTasks;
	std::vector<ConsumeNode> m_arrConsumeNode;
	std::vector<DeRegisterNode> m_arrDeRegisterNode;

	NS_COMMON::CThreadTemplate<CDlgAutoRegister> m_theThread1;
	NS_COMMON::CThreadTemplate<CDlgAutoRegister> m_theThread2;
	NS_COMMON::CThreadTemplate<CDlgAutoRegister> m_theThread3;
	HANDLE m_hEventSetExit1;
	HANDLE m_hEventSetExit2;
	HANDLE m_hEventSetExit3;
	HANDLE m_hEventTheadNoRun1;
	HANDLE m_hEventTheadNoRun2;
	HANDLE m_hEventTheadNoRun3;
	void SleepThread(int nSeconds, int nThreadTag);
	void WaitThreadExit();

	CCriticalSection m_cs;
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnHide();
protected:
	virtual void OnCancel();
};
