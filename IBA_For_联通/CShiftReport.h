
#pragma once

class CShiftReport : private COleDispatchDriver
{
public:
	
	CShiftReport(BOOL bAutoCreate = TRUE) // 调用 COleDispatchDriver 默认构造函数
	{
		if (bAutoCreate)
		{
			CreateDispatch(_T("IBAReport.CShiftReport"));
		}
	}

	void SetNextRemain(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetFetchAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetOperator(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x6803000c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetCashSoldAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetRemainAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6803000a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetDescription(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x68030009, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetReturnAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030008, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetCostExpense(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030007, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetCardCreditedAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030006, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetDeposit(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030005, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetTotalAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030004, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetCreditAmount(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030003, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetOnDutyTime(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x68030002, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetPayout(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x68030001, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetNetBarName(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x68030000, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}

	void Show()
	{
		InvokeHelper(0x6003000f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

};
