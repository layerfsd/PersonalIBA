#pragma once

class CChinaUnicomConfig
{
public:
	CChinaUnicomConfig(void);
	~CChinaUnicomConfig(void);

	static CChinaUnicomConfig* GetInstance();

	void Load();

	
	struct ClassIDItem
	{
		UINT nClassID;
		CString strName;
		CString strDetail;
		CString strTime;
	};

public:
	BOOL GetClassIDItem(UINT nClassID, ClassIDItem& item);

protected:
	CMap<UINT,UINT, ClassIDItem, ClassIDItem> m_mapClassItem;
	BOOL m_bLoaded;
};
