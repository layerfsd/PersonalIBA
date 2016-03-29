#include "stdafx.h"
#include "HelpFunc.h"
#include "..\IBA\DAL\IBADAL.h"
#include "IBA.h"

// 找出开始小时到结束小时之中最大的扣率
int HELPFUNC::GetMaxCostRateImp(int nBeginHour, int nEndHour, int nClassId, int nWeek, int nPCClass )
{
	INT iCostRate = -1;
	INT iCostRateTmp = -1;
	for(; nBeginHour <= nEndHour; nBeginHour++)
	{
		CString strHour;
		strHour.Format(_T("t%d"), nBeginHour);

		CString strSQL;
		if(-1 == nPCClass)
		{
			strSQL.Format(_T("SELECT %s FROM netbarbasepolicy WHERE classId = %d AND  week = %d ORDER BY %s DESC"), 
				strHour, nClassId, nWeek, strHour );
		}
		else
		{
			strSQL.Format(_T("SELECT %s FROM netbarbasepolicy WHERE classId = %d AND  week = %d AND pcClass = %d ORDER BY %s DESC"), 
				strHour, nClassId, nWeek, nPCClass, strHour );
		}

		//try
		//{
		CADODBConnInfo* pDb = NULL;

		if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) 
			return -1;

		CADORecordset Rs(pDb->GetConn());

		if (Rs.Open(strSQL) )
		{
			if (0 != Rs.GetRecordCount())
			{				
				Rs.GetFieldValue(strHour, iCostRateTmp);
			}
		}		
		Rs.Close();
		pDb->Release();

		if(iCostRate < iCostRateTmp)
		{
			iCostRate = iCostRateTmp;
		}
		//}
		//catch(...)
		//{
		//	IBA_LOG0(_T("查询扣率异常"));
		//	return iCostRate;
		//}
	}
	return iCostRate;
}


int HELPFUNC::GetMaxCostRate(int nBeginHour, int nEndHour, int nClassId, int nPCClass)
{
	COleDateTime dtCurrent = COleDateTime::GetCurrentTime();
	// 包时开始时间小于结束时间，也就是跨天包时
	if(nBeginHour > nEndHour)
	{
		// 跨天最大扣率计算
		int nCostRateTmp1 = HELPFUNC::GetMaxCostRateImp(nEndHour, 23, nClassId, dtCurrent.GetDayOfWeek(), nPCClass);
		int nCostRateTmp2 = HELPFUNC::GetMaxCostRateImp(0, nBeginHour, nClassId,
			(dtCurrent.GetDayOfWeek() + 1 > 7 ) ? 1 : dtCurrent.GetDayOfWeek() + 1, nPCClass);
		int nCostRate = (nCostRateTmp1 > nCostRateTmp2) ? nCostRateTmp1 : nCostRateTmp2;
		return nCostRate;
	}
	else
	{
		return HELPFUNC::GetMaxCostRateImp(nBeginHour, nEndHour, nClassId, dtCurrent.GetDayOfWeek(), nPCClass);
	}
}

// 获取到包时开始时间所需要的钱，传递开始时间和结束时间，返回金钱(单位分)
// 主要是为了兼容老代码
BOOL HELPFUNC::GetMoneyToBundleTimeNotPCClass(CString &strStartTime, CString &strEndTime, int nUserClassID, int &nMoney)
{
	// 如果值传递了, 小时:分钟的话就补上秒数
	if(5 == strStartTime.GetLength())
	{
		strStartTime += _T(":00");
	}

	if(5 == strEndTime.GetLength())
	{
		strEndTime += _T(":00");
	}

	COleDateTime dtBegin, dtEnd, dtCurrent;
	CString strCurrent;
	dtCurrent = COleDateTime::GetCurrentTime();
	strCurrent.Format(_T("%d:%d:%d"), dtCurrent.GetHour(), dtCurrent.GetMinute(), dtCurrent.GetSecond()); 
	dtCurrent.ParseDateTime(strCurrent);	// 去除日期

	dtBegin.ParseDateTime(strStartTime);
	dtEnd.ParseDateTime(strEndTime);

	// 0:跨天提前包时， 1:跨天提前包时
	int iToNextDay = -1;

	COleDateTimeSpan dtSpan;
	if(dtCurrent < dtBegin)
	{
		// 普通提前包时
		iToNextDay = 0;

		dtSpan.SetDateTimeSpan(0, 
			dtBegin.GetHour() - dtCurrent.GetHour(),
			dtBegin.GetMinute() - dtCurrent.GetMinute(),
			dtBegin.GetSecond() - dtCurrent.GetSecond());
	}
	else if(dtCurrent > dtEnd)
	{
		// 当前时间大于包时结束时间， 则是包到第二天
		// 跨天提前包时
		iToNextDay = 1;

		dtSpan.SetDateTimeSpan(0, 
			23 - dtCurrent.GetHour() + dtBegin.GetHour(),
			59 - dtCurrent.GetMinute(),
			59 - dtCurrent.GetSecond());
	}
	else
	{
		// 当前小时在当前包时时间段,则不用交补充金额
		nMoney = 0;
		return TRUE;
	}

	int iCurrHour = dtCurrent.GetHour();
	int iBeginHour = dtBegin.GetHour();
	int iSpanHour = dtSpan.GetHours();

	if((iCurrHour >= 0 && iCurrHour <= 23) &&
		(iBeginHour >= 0 && iBeginHour  <= 23))
	{
		// 获取开始当前时间到包时时间段之间的最大扣率
		int iCostRate = HELPFUNC::GetMaxCostRate(iCurrHour, iBeginHour, nUserClassID);

		if(0 <= iCostRate)
		{	
			// 补充金额 = 扣率的最大值 *　时长
			double dTime = dtSpan.GetHours() + (double)dtSpan.GetMinutes() / 60 + (double)dtSpan.GetSeconds() / 3600;
			double dCost = iCostRate * dTime / 100;

			int iCost = (int)dCost;
			if(dCost > iCost)
			{
				iCost++;		// 取整，例如为5.2元时，则取到6元
			}

			nMoney = iCost * 100;

			return TRUE;
		}
		else
		{
			IBA_LOG0(_T("查询扣率出错"));
		}
	}

	nMoney = 0;
	return FALSE;
}

BOOL HELPFUNC::GetMoneyToBundleTime( CString &strStartTime, CString &strEndTime, int nUserClassID, int nPCClass, int &nMoney )
{
	// 如果值传递了, 小时:分钟的话就补上秒数
	if(5 == strStartTime.GetLength())
	{
		strStartTime += _T(":00");
	}

	if(5 == strEndTime.GetLength())
	{
		strEndTime += _T(":00");
	}

	COleDateTime dtBegin, dtEnd, dtCurrent;
	CString strCurrent;
	dtCurrent = COleDateTime::GetCurrentTime();
	strCurrent.Format(_T("%d:%d:%d"), dtCurrent.GetHour(), dtCurrent.GetMinute(), dtCurrent.GetSecond()); 
	dtCurrent.ParseDateTime(strCurrent);	// 去除日期

	dtBegin.ParseDateTime(strStartTime);
	dtEnd.ParseDateTime(strEndTime);

	// 0:跨天提前包时， 1:跨天提前包时
	int iToNextDay = -1;

	COleDateTimeSpan dtSpan;
	if(dtCurrent < dtBegin)
	{
		// 普通提前包时
		iToNextDay = 0;

		dtSpan.SetDateTimeSpan(0, 
			dtBegin.GetHour() - dtCurrent.GetHour(),
			dtBegin.GetMinute() - dtCurrent.GetMinute(),
			dtBegin.GetSecond() - dtCurrent.GetSecond());
	}
	else if(dtCurrent > dtEnd)
	{
		// 当前时间大于包时结束时间， 则是包到第二天
		// 跨天提前包时
		iToNextDay = 1;

		dtSpan.SetDateTimeSpan(0, 
			23 - dtCurrent.GetHour() + dtBegin.GetHour(),
			59 - dtCurrent.GetMinute(),
			59 - dtCurrent.GetSecond());
	}
	else
	{
		// 当前小时在当前包时时间段,则不用交补充金额
		nMoney = 0;
		return TRUE;
	}

	int iCurrHour = dtCurrent.GetHour();
	int iBeginHour = dtBegin.GetHour();
	int iSpanHour = dtSpan.GetHours();

	if((iCurrHour >= 0 && iCurrHour <= 23) &&
		(iBeginHour >= 0 && iBeginHour  <= 23))
	{
		// 获取开始当前时间到包时时间段之间的最大扣率
		int iCostRate = HELPFUNC::GetMaxCostRate(iCurrHour, iBeginHour, nUserClassID, nPCClass);

		if(0 <= iCostRate)
		{	
			// 补充金额 = 扣率的最大值 *　时长
			double dTime = dtSpan.GetHours() + (double)dtSpan.GetMinutes() / 60 + (double)dtSpan.GetSeconds() / 3600;
			double dCost = iCostRate * dTime / 100;

			int iCost = (int)dCost;
			if(dCost > iCost)
			{
				iCost++;		// 取整，例如为5.2元时，则取到6元
			}

			nMoney = iCost * 100;

			return TRUE;
		}
		else
		{
			IBA_LOG0(_T("查询扣率出错"));
		}
	}

	nMoney = 0;
	return FALSE;
}

// 获取到包时结束不够的钱
int HELPFUNC::GetMoneyToForwardBundle(double dAllBalance, int nUserClassID , int nSelBundleTimeId , int nPCClass, BOOL bIsForwardBundle)
{
	CFullBundleTimeInfo BundleTimeInfo;
	CString strBundleTimeStartTime(""), strBundleTimeEndTime("");

	int nMoneyToForwardBundle = 0;
	if(CNetBarConfig::GetInstance()->m_NetbarBundleTime.GetBundleTimeInfo(nUserClassID, nSelBundleTimeId, BundleTimeInfo))
	{
		// 包时金额
		UINT nBundleMoney = BundleTimeInfo.GetBundleTimeMoney();
		// 转换成分
		dAllBalance *= 100;

		if(!bIsForwardBundle)		// 当前时间在包时时间段内
		{
			nMoneyToForwardBundle = dAllBalance - nBundleMoney;
			if(nMoneyToForwardBundle > 0)
			{
				nMoneyToForwardBundle = 0;	// 钱足够包时
			}
			else
			{
				nMoneyToForwardBundle = nBundleMoney - dAllBalance;
			}


		}
		else
		{
			//开始时间
			strBundleTimeStartTime = BundleTimeInfo.GetStartTime(TRUE).Left(5);		
			//结束时间
			strBundleTimeEndTime = BundleTimeInfo.GetEndTime(TRUE).Left(5);

			int nMoney;

			// 获取到包时开始时间段所需金额

			if(-1 == nPCClass)
			{
				if(!HELPFUNC::GetMoneyToBundleTimeNotPCClass(strBundleTimeStartTime, strBundleTimeEndTime, nUserClassID, nMoney))
				{
					IBA_LOG0(_T("提前包时出错"));
				}
			}
			else
			{
				if(!HELPFUNC::GetMoneyToBundleTime(strBundleTimeStartTime, strBundleTimeEndTime, nUserClassID, nPCClass, nMoney))
				{
					IBA_LOG0(_T("提前包时出错"));
				}
			}
			

			// 补充金额 = 账户剩余总的金额 - 包时到当前时段所需要的钱 - 包时金额

			nMoneyToForwardBundle =  dAllBalance -  nMoney - nBundleMoney;

			// 如果用户剩余总金额，足够包时则不用补充
			if(nMoneyToForwardBundle > 0)
			{
				nMoneyToForwardBundle = 0;
			}
			else
			{
				nMoneyToForwardBundle = nMoney + nBundleMoney - dAllBalance;
			}
		}

		// 假设，假设充值4.3元就充值5元
		if(0 != nMoneyToForwardBundle % 100)
		{
			nMoneyToForwardBundle += 100;
		}
	}
	return nMoneyToForwardBundle;
}

// 后台设置"禁止账户包时"， 也即账户的钱不能用来包时, 返回分
// 账户的钱不能用来包时后，到包时结束时间所需要的钱
int HELPFUNC::GetCashForwardBundleNoBalance(double dAllBalance, int nUserClassID,  int nSelBundleTimeId, int nPCClass, BOOL bIsForwardBundle)
{
	CFullBundleTimeInfo BundleTimeInfo;
	CString strBundleTimeStartTime(""), strBundleTimeEndTime("");
	int nMoneyToForwardBundle = 0;

	if(CNetBarConfig::GetInstance()->m_NetbarBundleTime.GetBundleTimeInfo(
		nUserClassID, nSelBundleTimeId, BundleTimeInfo))
	{
		UINT nBundleMoney = BundleTimeInfo.GetBundleTimeMoney();		// 包时金额

		if(!bIsForwardBundle)		// 当前时间在包时时间段内
		{
			// 应收现金 = 包时金额
			return nBundleMoney;
		}
		else
		{
			//开始时间
			strBundleTimeStartTime = BundleTimeInfo.GetStartTime(TRUE).Left(5);
			//结束时间
			strBundleTimeEndTime = BundleTimeInfo.GetEndTime(TRUE).Left(5);

			CString strBundleTimeMoney;
			int nMoney;

			// 获取到包时开始时间段所需金额
			if(-1 == nPCClass)
			{
				if(!HELPFUNC::GetMoneyToBundleTimeNotPCClass(strBundleTimeStartTime, strBundleTimeEndTime, nUserClassID, nMoney))
				{
					IBA_LOG0(_T("提前包时出错--获取应收现金出错"));
				}
			}
			else
			{
				if(!HELPFUNC::GetMoneyToBundleTime(strBundleTimeStartTime, strBundleTimeEndTime, nUserClassID, nPCClass, nMoney))
				{
					IBA_LOG0(_T("提前包时出错--获取应收现金出错"));
				}
			}

			// 转换成分
			dAllBalance *= 100;
			// 如果账户剩余总的金额 > 包时到当前时段所需要的钱 
			// 则应收现金 = 包时金额

			// 如果账户剩余总的金额 < 包时到当前时段所需要的钱 
			// 则应收现金 = 包时到当前时段所需要的钱 - 账户剩余总的金额 + 包时金额
			int nDiff = dAllBalance  - nMoney;
			if(nDiff > 0)
			{
				return nBundleMoney;
			}
			else
			{
				return static_cast<int>(nBundleMoney + (-nDiff));
			}

		}

	}
	else
	{
		return 0;
	}
}

// 获取包时所需金额
int HELPFUNC::GetBundleTimeMoney(int nUserClassID , int nSelBundleTimeId )
{
	CFullBundleTimeInfo BundleTimeInfo;
	CString strBundleTimeStartTime(""), strBundleTimeEndTime("");

	int nMoneyToForwardBundle = 0;
	if(CNetBarConfig::GetInstance()->m_NetbarBundleTime.GetBundleTimeInfo(
		nUserClassID, nSelBundleTimeId, BundleTimeInfo))
	{
		// 包时金额
		return BundleTimeInfo.GetBundleTimeMoney();

	}
	else
	{
		return 0;
	}
}
