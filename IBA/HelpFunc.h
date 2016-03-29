#pragma once

namespace HELPFUNC{
	int GetMaxCostRateImp(int nBeginHour, int nEndHour, int nClassId, int nWeek, int nPCClass = -1);
	// nPCClass加个默认参数主要是为了兼容老代码
	int GetMaxCostRate(int nBeginHour, int nEndHour, int nClassId, int nPCClass = -1);
	BOOL GetMoneyToBundleTimeNotPCClass(CString &strStartTime, CString &strEndTime, int nUserClassID, int &nMoney);
	BOOL GetMoneyToBundleTime( CString &strStartTime, CString &strEndTime, int nUserClassID, int nPCClass, int &nMoney );
	int GetMoneyToForwardBundle(double dAllBalance, int nUserClassID , int nSelBundleTimeId , int nPCClass = -1, BOOL bIsForwardBundle = TRUE);
	int GetCashForwardBundleNoBalance(double dAllBalance, int nUserClassID, int nSelBundleTimeId, int nPCClass = -1, BOOL bIsForwardBundle = TRUE);
	int GetBundleTimeMoney(int nUserClassID , int nSelBundleTimeId );
}
