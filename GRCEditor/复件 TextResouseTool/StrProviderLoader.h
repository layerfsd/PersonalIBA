#pragma once
#include <vector>

#ifdef LOAD_STRING
#error LOAD_STRING ÖØ¸´¶¨Òå
#endif

#ifdef __USING_INTER_RESOURCE
#define LOAD_STRING(ID) CStrProviderLoader::LoadString(ID)
#else
#define LOAD_STRING(ID) CStrProviderLoader::GetInstance()->GetString(ID)
#endif

class CStrProviderLoader
{
	typedef BOOL(*FUN_LOADSTRINGCONTAINER)(const char*);
	typedef void(*FUN_GETSTRINGA)(UINT,char *,const unsigned int);
	typedef void(*FUN_GETSTRINGW)(UINT,wchar_t*,const unsigned int);

public:
	CStrProviderLoader(){}

	static CStrProviderLoader* GetInstance()
	{
		static CStrProviderLoader loader;
		return &loader;
	}
	~CStrProviderLoader(){
		if (m_hModule && m_hModule!=INVALID_HANDLE_VALUE)
		{
			::FreeLibrary(m_hModule);
		}
	}
	BOOL Loading(CString strDllName, CString strGrcFile)
	{
		m_hModule = ::LoadLibrary(strDllName);
		if (!m_hModule || m_hModule == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		m_fnLoad = (FUN_LOADSTRINGCONTAINER)GetProcAddress(m_hModule, "LoadStringContainer");
		m_fnGetA = (FUN_GETSTRINGA)GetProcAddress(m_hModule, "GetStringA");
		m_fnGetW = (FUN_GETSTRINGW)GetProcAddress(m_hModule, "GetStringW");
		if (m_fnLoad == NULL || m_fnGetA == NULL || m_fnGetW == NULL)
		{
			return FALSE;
		}
		return m_fnLoad(CT2A(strGrcFile));
	}

	CString GetString(UINT nID)
	{
#ifdef UNICODE
		wchar_t ch[1024] = {0};
		m_fnGetW(nID, ch, 1024);
		CString str(ch);	
#else
		char ch[1024] = {0};
		m_fnGetA(nID, ch, 1024);
		CString str(ch);
#endif
		return str;
	}

public:
	static CString LoadString(UINT ID)
	{
		CString strTmp;
		strTmp.LoadString(ID);
		return strTmp;
	}

private:
	HMODULE m_hModule;
	FUN_LOADSTRINGCONTAINER m_fnLoad;
	FUN_GETSTRINGA m_fnGetA;
	FUN_GETSTRINGW m_fnGetW;
};

template<class TDialog>
class CStringLoaderDlg : public TDialog
{
public:
	CStringLoaderDlg()
	{

	}
	CStringLoaderDlg(LPCTSTR lpszTemplateName, CWnd* pParent = NULL)
		:TDialog(lpszTemplateName, pParent)
	{

	}
	CStringLoaderDlg(UINT nIDTemplate, CWnd* pParent=NULL)
		:TDialog(nIDTemplate, pParent)
	{

	}
	virtual ~CStringLoaderDlg()
	{
		
	}
protected:
#ifndef __USING_INTER_RESOURCE
	virtual void InitializeStrings()
	{
		CWnd* pChildWnd = GetWindow(GW_CHILD);   

		while (pChildWnd != NULL)   
		{
			TCHAR szClassName[MAX_PATH] = {0};
			GetClassName(pChildWnd->GetSafeHwnd(), szClassName, MAX_PATH);

			SetStringID(pChildWnd->GetDlgCtrlID());
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}
	virtual BOOL OnInitDialog()
	{
		InitializeStrings();
		BOOL bRes = TDialog::OnInitDialog();
		for(size_t i = 0 ; i < m_vecIDs.size(); i++)
		{
			UINT nID = m_vecIDs[i];
			CWnd* pWnd = TDialog::GetDlgItem( nID );
			if (pWnd == NULL || !IsWindow(pWnd->GetSafeHwnd()) )
			{
				ASSERT(pWnd!=NULL);
				continue;
			}
			UINT nStrID = m_vecStrIDs[i];
			if (nStrID == 0)
			{
				nStrID = m_nIDHelp << 16;
				nStrID |= nID;
			}
			CString str = CStrProviderLoader::GetInstance()->GetString(nStrID);
#ifndef _DEBUG
			if (str == _T("Unknown string ID"))
			{
				continue;
			}
#endif // _DEBUG
			pWnd->SetWindowText( str );
		}

		UINT nDlgID = m_nIDHelp << 16;
		CString str = CStrProviderLoader::GetInstance()->GetString(nDlgID);
#ifndef _DEBUG
		if (str == _T("Unknown string ID"))
		{
			return bRes;
		}
#endif // _DEBUG
		SetWindowText(str);

		return bRes;
	}
#endif

public:

#ifndef __USING_INTER_RESOURCE
	void SetStringID(UINT nCtrlID, UINT nStrID=0)
	{
		m_vecIDs.push_back(nCtrlID);
		m_vecStrIDs.push_back(nStrID);
	}
#else
	void SetStringID(UINT nCtrlID, UINT nStrID=0)
	{
		// do nothing
	}
#endif



private:
	std::vector<UINT> m_vecIDs;  // controls ID
	std::vector<UINT> m_vecStrIDs; // strings ID
};