#include "StdAfx.h"
#include "ExportOutputListThread.h"
#include "Excel.h"

CExportOutputListThread::CExportOutputListThread()
:m_strExportExcel(""), m_strFullPath(""), m_nRowCount(0)
{
}

CExportOutputListThread::~CExportOutputListThread()
{
}

int CExportOutputListThread::Run()
{
	ExportExcel();
	return 0;
}

void CExportOutputListThread::ExportExcel()
{
	if(m_strExportExcel.IsEmpty() )
	{
		AfxMessageBox(_T("导出内容为空"));
		return;
	}

	if(m_strFullPath.IsEmpty())
	{
		AfxMessageBox(_T("导出路径为空"));
		return;
	}
	
	try
	{	
		// 没有Flush会导致一些操作滞留，为了安全起见，在每个操作后都应该Flush一次
		// 不同的单元格格式设置设置的位置不同，带来的效果也不同，需要根据实际效果调整

		// 额外增加的内容
		UINT nAddRowCount = 0;

		NS_GXX::CExcelExport exEp;
		// 创建对象
		exEp.CreateOneBlankBook();

		exEp.AppendData("希之光收银台交班记录\t\n");
		exEp.Flush();
		nAddRowCount++;

		//exEp.AppendData("结账时间\t用户级别\t终端号\t上机时间\t上网账号\t姓名\t证件号码\t金额\t备注\n");
		CStringA strTitleA;
		/*strTitleA += ("ID\tStartDate\tStandbyAmount\tDeposit\tCreditAmount");
		strTitleA += ("\tPresentAmount\tReturnAmount\tCardSoldAmount\tCashSoldAmount");
		strTitleA += ("\tCardCreditedAmount\tTotalAmount\tCostExpense\tOperator");
		strTitleA += ("\tNextOperator\tDescription\tEndDate\tPayout\tremainAmount");
		strTitleA += ("\tCountAmount\tCashRegisterID");*/
		strTitleA += ("班次开始时间\t班次结束时间\t充值总额\t退款总额");
		strTitleA += ("\t上一班预留\t充值卡金额\t现金购物\t点卡充值总额");
		strTitleA += ("\t上缴金额\t工本费\t操作员\t下一个班次名称");
		strTitleA += ("\t其他支出\t预留金额\t班次描述");
		
		exEp.AppendData((LPSTR)strTitleA.GetString());
		exEp.Flush();
		nAddRowCount++;

		// 总的行数
		UINT nAllRowCount = m_nRowCount + nAddRowCount;
		// 内容开始第一行
		UINT nBegRow = nAddRowCount + 1;

		// 设置单元格格式为文本， 这个要在内容加入前设置
		exEp.Format_PutNumberFormat(exEp.GetActiveSheet(), m_nColCount, nBegRow, m_nColCount, nAllRowCount,"@");
		exEp.Flush();
		exEp.Format_PutNumberFormat(exEp.GetActiveSheet(), m_nColCount, nBegRow, m_nColCount, nAllRowCount,"@");
		exEp.Flush();

		USES_CONVERSION;		// 使用W2A（）需要调用这个宏
		exEp.AppendData(CT2A((LPCTSTR)m_strExportExcel));
		exEp.Flush();

		// 第一行标题，黑体、粗体、字号20、 合并居中
		exEp.Format_RangeFont(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 1, m_nColCount, 1,
			RGB(0, 0, 0), "黑体", 20, TRUE);
		exEp.Flush();
		exEp.Format_RangeAlignment(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 1, m_nColCount, 1, 2);
		exEp.Flush();

		exEp.Format_RangeMerge(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 1, m_nColCount, 1);
		exEp.Flush();

		exEp.Format_RangeAlignment(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 2, m_nColCount, nAllRowCount, 2);
		exEp.Flush();

		// 设置第二行标题的字体以及背景颜色
		exEp.Format_RangeFont(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 2, m_nColCount, 2,
			RGB(0, 0, 0), "宋体", 14, TRUE);
		exEp.Flush();

		// 设置背景颜色
		exEp.Format_RangeBkColor(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 2, m_nColCount, 2,
			RGB(204, 204, 255));
		exEp.Flush();

		// 设置边框， 从第二行标题开始
		exEp.Format_PutBorderAround(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 2, m_nColCount, nAllRowCount);
		exEp.Flush();

		// 设置单元格左对齐
		exEp.Format_RangeAlignment(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 2, m_nColCount, nAllRowCount, 1);
		exEp.Flush();

		// 对内容的操作
		// 设置单元格宽为20磅
		//exEp.Format_ColsWidth(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, m_nColCount, 20);

		// 设置所有单元格宽度自适应，这个要在其他的设置完后再设置，才能有最佳效果
		exEp.Format_PutAutoFit(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, 1, m_nColCount, nAllRowCount, TRUE);
		exEp.Flush();
		
		// 添加当前日期 2013-3-12 星期二 15:37:23, 合并居左
		UINT DayOfWeek[] = {
			LOCALE_SDAYNAME7,   // Sunday
				LOCALE_SDAYNAME1,   
				LOCALE_SDAYNAME2,
				LOCALE_SDAYNAME3,
				LOCALE_SDAYNAME4, 
				LOCALE_SDAYNAME5, 
				LOCALE_SDAYNAME6   // Saturday
		};

		TCHAR strWeekday[256];
		CTime time(CTime::GetCurrentTime());   // Initialize CTime with current time
		::GetLocaleInfo(LOCALE_USER_DEFAULT,   // Get string for day of the week from system
			DayOfWeek[time.GetDayOfWeek()-1],   // Get day of week from CTime
			strWeekday, sizeof(strWeekday));

		CString strData;
		strData.Format(_T("制表日期： %s %s %s\n"), time.Format("%Y-%m-%d"), strWeekday, time.Format("%H:%M:%S"));
		
		exEp.AppendData(CT2A((LPCTSTR)strData));
		exEp.Flush();

		exEp.Format_RangeAlignment(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, nAllRowCount + 1, m_nColCount, nAllRowCount + 1, 1);
		exEp.Flush();

		exEp.Format_RangeMerge(exEp.GetActiveSheet(), NS_GXX::CExcel::_A_, nAllRowCount + 1, m_nColCount, nAllRowCount + 1);
		exEp.Flush();



		if(exEp.Save(m_strFullPath))
		{
			exEp.Close();
			ShellExecute(NULL, _T("open"), m_strFullPath, NULL, NULL, SW_HIDE);
		//	CString strMsg;
		//	strMsg.Format(_T("导出到\"%s\"成功"), m_strFullPath);
		//	AfxMessageBox(strMsg);
		}
		
	}
	catch(...)
	{
		AfxMessageBox(_T("导出失败"));
	}
}

void CExportOutputListThread::SetParam( LPCTSTR szExportExcel, LPCTSTR szFullPath, UINT nRowCount, UINT nColCount)
{
	m_strExportExcel = szExportExcel;
	m_strFullPath = szFullPath;
	m_nRowCount = nRowCount;
	m_nColCount = nColCount;
}

