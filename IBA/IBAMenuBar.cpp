
#include "StdAfx.h"
#include "IBA.h"
#include "IBAMenuBar.h"

CIBAMenuBar::CIBAMenuBar()
{
	CRegKey RegKey;
	
	if (ERROR_SUCCESS == RegKey.Open(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\BCGUI")))
	{
		RegKey.DeleteSubKey(_T("BCGPBaseControlBar-593980"));
		RegKey.DeleteSubKey(_T("BCGPControlBar-593980"));
		RegKey.DeleteSubKey(_T("BCGToolBar-593980"));

		RegKey.DeleteSubKey(_T("BCGPBaseControlBar-593990"));
		RegKey.DeleteSubKey(_T("BCGPControlBar-593990"));
		RegKey.DeleteSubKey(_T("BCGToolBar-593990"));

		const DWORD CurIBAFlag = 17; 

		DWORD dwIBAFlag = 0;
		
		RegKey.QueryDWORDValue(_T("IBAFlag"), dwIBAFlag);

		if (dwIBAFlag != CurIBAFlag)
		{
			//删除快捷键
			RegKey.DeleteSubKey(_T("BCGKeyboard-0")); 

			//删除工具栏设置
			RegKey.DeleteSubKey(_T("BCGPBaseControlBar-59393"));
			RegKey.DeleteSubKey(_T("BCGPControlBar-59393"));
			RegKey.DeleteSubKey(_T("BCGToolBar-59393"));

			//设置当前版本的标志
			RegKey.SetDWORDValue(_T("IBAFlag"), CurIBAFlag);
		}

		RegKey.Close();
	}
}

CIBAMenuBar::~CIBAMenuBar()
{

}

BOOL CIBAMenuBar::SaveState(LPCTSTR lpszProfileName /*= NULL*/, int nIndex /*= -1*/, UINT uiID /*= (UINT) -1*/)
{
	//return CBCGPMenuBar::SaveState(lpszProfileName, nIndex, uiID);

	return TRUE;
}

BOOL CIBAMenuBar::CreateMenuBar(CWnd* pParentWnd)
{
	if (!Create(pParentWnd, dwDefaultToolbarStyle, BCG_IDW_MENUBAR))
	{
		TRACE0("Failed to create menubar\n");
		return FALSE;      // fail to create
	}

	SetBarStyle(GetBarStyle() | CBRS_SIZE_DYNAMIC);
	SetRecentlyUsedMenus(FALSE);
	SetShowAllCommands(TRUE);

	return TRUE;
}

BOOL CIBAMenuBar::LoadState(LPCTSTR lpszProfileName /*= NULL*/, int nIndex /*= -1*/, UINT uiID /*= (UINT) -1*/)
{
	//__super::LoadState(lpszProfileName, nIndex, uiID);
	
	return FALSE;
}

void CIBAMenuBar::OnDefaultMenuLoaded(HMENU hMenu)
{
	CMenu menu;

	menu.Attach(hMenu);

	if (!CNetBarConfig::GetInstance()->GetCreditDirect()) //账户直冲
	{
		menu.GetSubMenu(1)->RemoveMenu(IDM_CREDITDIRECT, MF_BYCOMMAND);
	}

#ifndef __USING_INTER_RESOURCE
	CMenu *pMenu = menu.GetSubMenu(3);
	if (pMenu)
	{
		HMENU hTmp = pMenu->GetSubMenu(4)->GetSafeHmenu();
		pMenu->ModifyMenu((UINT_PTR)hTmp,MF_POPUP,(UINT_PTR)hTmp,LOAD_STRING(IDS_WINDOW_LAYOUT));
		hTmp = pMenu->GetSubMenu(5)->GetSafeHmenu();
		pMenu->ModifyMenu((UINT_PTR)hTmp,MF_POPUP,(UINT_PTR)hTmp,LOAD_STRING(IDS_CHANGE_SKIN));
	}
	pMenu = menu.GetSubMenu(5);
	if (pMenu)
	{
		HMENU hTmp = pMenu->GetSubMenu(4)->GetSafeHmenu();
		pMenu->ModifyMenu((UINT_PTR)hTmp,MF_POPUP,(UINT_PTR)hTmp,LOAD_STRING(IDS_SITE_OPERATION));
	}
#endif

	if(!CIBAConfig::GetInstance()->GetIsShowShiftCash())
	{
		menu.GetSubMenu(2)->RemoveMenu(IDM_SHIFTCASH, MF_BYCOMMAND);	// 如果使用了实名则删除开户子菜单
	}

	menu.Detach();

}