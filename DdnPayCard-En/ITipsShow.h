#pragma once

#include "Common.h"

class ITipsShow
{
public:
	virtual void SetShowParam(Tips_ShowParam tips_ShowParam) = 0;

	virtual int ShowTips(LPCTSTR szURL) = 0;

	virtual int ShowTips(LPCTSTR szTitle, LPCTSTR szContent) = 0;

	virtual void SetExternal(LPDISPATCH lpExternal) = 0;

	virtual void HideTips() = 0;
};

ITipsShow * CreateTipsShowObject();

