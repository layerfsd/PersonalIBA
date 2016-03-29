#pragma once

#include "ServletMethod.h"

namespace NS_SERVLET
{
class CMachReportMethod : public CServletMethod
{
	//friend class CComputerList;
public:
	CMachReportMethod();
	virtual ~CMachReportMethod();

public:
	void SetData(CString strData);

	BOOL DoReport();

	INT GetAddNum();
	INT GetMergeNum();
	INT GetErrorNum();
	INT GetDelNum();
	CString GetOperateTime();

protected:
	void AddParams();
	void AddRespFields();
};

}	// namespace NS_SERVLET