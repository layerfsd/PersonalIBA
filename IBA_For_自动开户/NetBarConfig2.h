#pragma once

class CNetBarConfig2
{
public:
	CNetBarConfig2(void);
	~CNetBarConfig2(void);

	BOOL LoadFromDB();

private:

	CString m_strWWMIP;

	UINT m_nShutDownMinutes;

	UINT m_nMaxKeepTime;//软件维护密码

	UINT m_nMaxKeepTimeOfUKey;//硬件维护密码

	UINT m_ShutDownFlag;

	UINT m_ClientLoginStyle;

	CString m_strUnLockPassword;//锁屏密码

	CString m_strSetupPassword;//安装密码

	UINT m_nCardOpenerVerID; 

	BOOL m_bAllowUnlockSuspend;

	BOOL m_bNetIdentityManage;

	UINT m_nMemberUseChangeComputer;//会员用户是否允许在线换机

	UINT m_nTmpUseChangeComputer;//普通用户是否允许在线换机

	BOOL m_bShowUseTime; //客户端显示使用时间

	BOOL m_bAllowAutoCheckIn; //允许自动登录

public:

	BOOL GetAllowUnlockSuspend() const { return m_bAllowUnlockSuspend; }
	void SetAllowUnlockSuspend(BOOL newVal);

	UINT GetShutDownFlag() const { return m_ShutDownFlag; }
	void SetShutDownFlag(UINT newVal);

	UINT GetShutDownMinutes() const { return m_nShutDownMinutes; }
	void SetShutDownMinutes(UINT newVal);

	CString GetWWMIP() const { return m_strWWMIP; }
	void SetWWMIP(CString newVal);
	
	UINT GetClientLoginStyle() const { return m_ClientLoginStyle; }
	void SetClientLoginStyle(UINT newVal);

	UINT GetCardOpenerVerID() const { return m_nCardOpenerVerID; }
	void SetCardOpenerVerID(UINT newVal);

	CString GetUnLockPassword() const { return m_strUnLockPassword; }
	void SetUnLockPassword(CString newVal);
	
	BOOL GetNetIdentityManage() const { return m_bNetIdentityManage; }
	void SetNetIdentityManage(BOOL newVal);

	UINT GetMaxKeepTimeOfUKey() const { return m_nMaxKeepTimeOfUKey; }
	void SetMaxKeepTimeOfUKey(UINT val);

	UINT GetMaxKeepTime() const { return m_nMaxKeepTime; }
	void SetMaxKeepTime(UINT val);

	CString GetSetupPassword() const { return m_strSetupPassword; }
	void SetSetupPassword(CString newVal);

	UINT GetMemberUserChangecomputer() const { return m_nMemberUseChangeComputer; }
	void SetMemberUserChangeComputer(UINT newVal); 

	UINT GetTmpUserChangeComputer() const { return m_nTmpUseChangeComputer; }
	void SetTmpUserChangeComputer(UINT newVal);

	BOOL GetShowUseTime() const { return m_bShowUseTime; }
	void SetShowUseTime(BOOL newVal);

	BOOL GetAllowAutoCheckIn() const { return m_bAllowAutoCheckIn; }
	void SetAllowAutoCheckIn(BOOL newVal);

private:
	//允许包时
	UINT m_nAllowBundleTime;
	//允许挂机
	UINT m_nAllowSuspend;

	UINT m_nBundleTimeEndTemp;

	UINT m_nBundleTimeEnd;

	UINT m_nNoEnoughTime;

public:
	UINT GetAllowBundleTime() const { return m_nAllowBundleTime; }
	void SetAllowBundleTime(UINT val);

	UINT GetAllowSuspend() const { return m_nAllowSuspend; }
	void SetAllowSuspend(UINT val);

	UINT GetBundleTimeEndTemp() const { return m_nBundleTimeEndTemp; }
	void SetBundleTimeEndTemp(UINT newVal);

	UINT GetBundleTimeEnd() const { return m_nBundleTimeEnd; }
	void SetBundleTimeEnd(UINT newVal);

	UINT GetNoEnoughTime() const { return m_nNoEnoughTime; }
	void SetNoEnoughTime(UINT newVal);
};
