#include "stdafx.h"
#include ".\chinaunicomconfig.h"
#include "IBA.h"

CChinaUnicomConfig::CChinaUnicomConfig(void)
{
	m_bLoaded = FALSE;
}

CChinaUnicomConfig::~CChinaUnicomConfig(void)
{
}

CChinaUnicomConfig* CChinaUnicomConfig::GetInstance()
{
	static CChinaUnicomConfig config;
	if (!config.m_bLoaded)
	{
		config.Load();
	}
	return &config;
}

void CChinaUnicomConfig::Load()
{
	CString strIniPath = theApp.GetWorkPath();
	strIniPath.Append(_T("\\IBAConfig\\ChinaUnicom.ini"));

	INT nCount = ::GetPrivateProfileInt(_T("Config"), _T("ItemCount"), 0, strIniPath);
	
	for (int i=0; i < nCount; i++)
	{
		CString strKey ;
		strKey.Format(_T("item%d"), i + 1);
		
		ClassIDItem item;
		item.nClassID = ::GetPrivateProfileInt(strKey, _T("classID"), 0, strIniPath);

		::GetPrivateProfileString(strKey, _T("name"), _T("Î´ÖªÀàÐÍÃû³Æ£¬ÇëÅäÖÃChinaUnicom.ini"), item.strName.GetBuffer(256), 256, strIniPath);
		item.strName.ReleaseBuffer();
		
		::GetPrivateProfileString(strKey, _T("detail"), _T("Î´ÖªÀàÐÍÃèÊö£¬ÇëÅäÖÃChinaUnicom.ini"), item.strDetail.GetBuffer(256), 256, strIniPath);
		item.strDetail.ReleaseBuffer();

		::GetPrivateProfileString(strKey, _T("time"), _T(""), item.strTime.GetBuffer(256), 256, strIniPath);
		item.strTime.ReleaseBuffer();

		m_mapClassItem.SetAt(item.nClassID, item);
	}
	m_bLoaded = TRUE;
}

BOOL CChinaUnicomConfig::GetClassIDItem(UINT nClassID, ClassIDItem& item)
{
	if (m_mapClassItem.PLookup(nClassID) == NULL)
	{
		return FALSE;
	}
	item = m_mapClassItem[nClassID];
	return TRUE;
}