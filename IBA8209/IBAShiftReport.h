
#pragma once

class CIBAShiftReport : private COleDispatchDriver
{
public:

	CIBAShiftReport(BOOL bAutoCreate = TRUE) // 调用 COleDispatchDriver 默认构造函数
	{
		if (bAutoCreate)
		{
			Create();
		}
	}

	BOOL Create()
	{
		return CreateDispatch(_T("DdnReport.Unity"));
	}

	long GetVersion()
	{
		long result = 0;
		InvokeHelper(0x68030000, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}

	void ShowReport(LONG nType, LPCTSTR strXML)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR ;

		InvokeHelper(0x60030001, DISPATCH_METHOD, VT_EMPTY, NULL, parms, nType, strXML);
	}

};
