#pragma once

#include "Cashier.h"

// CLocalCashier 命令目标

class CLocalCashier : public CCashier
{
public:
	CLocalCashier();
	virtual ~CLocalCashier();

public :

	virtual BOOL CheckIn(CCashRegisterLoginMethod & CashRegisterLogin, CString strOperator);//登入检测
	
	// 执行向中心发送断网的通知
	virtual BOOL DoLocalMode();

	virtual BOOL CheckLastDuty(CString& strLastOperator);//检查最后操作者是否正常

private:

	// 初始化时设置本地数据库
	BOOL SetLocaNetbarlInfo(void);

	virtual void WriteDutyInfo();//保存班次信息
};


