// IDRSUCore.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "IDRSUCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CIDRSUCoreApp

BEGIN_MESSAGE_MAP(CIDRSUCoreApp, CWinApp)
END_MESSAGE_MAP()


// CIDRSUCoreApp 构造

CIDRSUCoreApp::CIDRSUCoreApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CIDRSUCoreApp 对象

CIDRSUCoreApp theApp;


// CIDRSUCoreApp 初始化

BOOL CIDRSUCoreApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

struct WDUserInfo
{
	char szName[40];
	char szSex[10];
	char szNation[20];
	char szBirthDate[50];
	char szAddress[200];
	char szID[40];
	char szFromDepart[200];
	char szStartDate[50];
	char szEndDate[50];
	char szNewAddress[200];
};

INT init()
{
	return 1;
}

INT WINAPI GetInfoFirstType_1(WDUserInfo* lpUserInfo)
{
	lstrcpy(lpUserInfo->szName, "张三");
	lstrcpy(lpUserInfo->szSex, "男");
	lstrcpy(lpUserInfo->szID, "140401198001010039");
	lstrcpy(lpUserInfo->szNation, "汉族");

	return 1;
}

INT WINAPI GetInfoSecondType_1(WDUserInfo* lpUserInfo)
{
	lstrcpy(lpUserInfo->szName, "李四");
	lstrcpy(lpUserInfo->szSex, "女");
	lstrcpy(lpUserInfo->szID, "350101198001010023");
	lstrcpy(lpUserInfo->szNation, "汉族");

	return 1;
}

INT WINAPI GetInfoSecondTypeMachine_1(WDUserInfo* lpUserInfo)
{
	lstrcpy(lpUserInfo->szName, "王五");
	lstrcpy(lpUserInfo->szSex, "男");
	lstrcpy(lpUserInfo->szID, "371081198408010018");
	lstrcpy(lpUserInfo->szNation, "汉族");

	return 1;
}

INT WINAPI GetOtherType_1()
{
	return 1;
}
