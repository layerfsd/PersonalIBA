#pragma once

class CFullBundleTimeInfo
{
public:
	CFullBundleTimeInfo();

	BOOL IsNowEnabled(); //现在是否可用

	UINT GetClassId() const { return m_nClassId; }
	void SetClassId(UINT newVal) { m_nClassId = newVal; }

	CString GetBundleTimeArea() const { return m_strBundleTimeArea; }
	void SetBundleTimeArea(CString strValue) { m_strBundleTimeArea = strValue; }

	CString GetStartTime(BOOL bForce = FALSE);
	void SetStartTime(CString strValue) { m_strStartTime = strValue; }

	CString GetEndTime(BOOL bForce = FALSE);
	void SetEndTime(CString strValue) { m_strEndTime = strValue; }

	UINT GetBundleTimeId() const { return m_nBundleTimeId; }
	void SetBundleTimeId(UINT nValue) { m_nBundleTimeId = nValue; }

	UINT GetBundleTimeType() const { return m_nBundleTimeType; }
	void SetBundleTimeType(UINT nValue) { m_nBundleTimeType = nValue; }
	CString GetBundleTimeTypeString();

	UINT GetTimePass() const { return m_nTimePass; }
	void SetTimePass(UINT nValue) { m_nTimePass = nValue; }

	UINT GetBundleTimeMoney() const { return m_nBundleTimeMoney; }
	void SetBundleTimeMoney(UINT nValue) { m_nBundleTimeMoney = nValue; }

	CString GetDescription() const { return m_strDescription; }
	void SetDescription(CString strValue) { m_strDescription = strValue; }

private:

	CString m_strDescription;	//包时信息描述
	CString m_strBundleTimeArea;//包时区域
	CString m_strStartTime;		//包时开始时间
	CString m_strEndTime;		//包时终止时间

	UINT	m_nClassId;			//用户级别
	UINT	m_nBundleTimeMoney;	//包时金额
	UINT    m_nTimePass;		//包时时长
	UINT	m_nBundleTimeId;	//包时ID，区分不同时段包时
	UINT	m_nBundleTimeType;	//包时类型，区分不同扣费方式
};

class CNetbarBundleTime : public CThread
{
public:
	CNetbarBundleTime(void);
	~CNetbarBundleTime(void);

	typedef CArray<CFullBundleTimeInfo, CFullBundleTimeInfo&> CBundleTimeInfoArray;

	CBundleTimeInfoArray m_BundleTimeInfoArray;//包时策略

	BOOL LoadBundTimeInfoFromServer();

	BOOL GetBundleTimeInfo(UINT nClassId, UINT nBundleTimeId, CFullBundleTimeInfo& BundleTimeInfo);

private:

	virtual int Run();

};
