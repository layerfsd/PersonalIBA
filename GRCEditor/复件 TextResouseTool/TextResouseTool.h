// TextResouseTool.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号

#include "StrProviderLoader.h"

// CTextResouseToolApp:
// 有关此类的实现，请参阅 TextResouseTool.cpp
//

class CGlobalParam
{
protected:
	CGlobalParam();
	void LoadParam();
public:
	static CGlobalParam* GetInstance()
	{
		static CGlobalParam param;
		return &param;
	}

	BOOL GetIsNextHit()const;    // 是否下一次提醒
	void SetIsNextHit(BOOL bVal);

	BOOL GetReplace()const;
	void SetReplace(BOOL bVal);
	BOOL GetMenuBefore()const;
	void SetMenuBefore(BOOL bVal);
	BOOL GetCheckChange();
	void SetCheckChange(BOOL bVal);
	CString GetFormatChars();
	void SetFormatChars(CString strVal);
	BOOL GetForbidEditID();
	void SetForbidEditID(BOOL bVal);

private:
	BOOL m_bIsNextHit;  // 下一次是否做提醒
	BOOL m_bReplace;    // ID重复时，是否替换内容
	BOOL m_bMenuBefore; // 加载RC时，是否先解析菜单
	BOOL m_bCheckChange; // 当编辑内容时，决定是否检查格式发生改变
	CString m_strFormatChars; // 检查格式的字符序列
	BOOL m_bForbidEditID; // 是否禁用ID可编辑
};
class CTextResouseToolApp : public CWinApp
{
public:
	CTextResouseToolApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	HACCEL m_hAccelTable;
	virtual int ExitInstance();
};

extern CTextResouseToolApp theApp;
