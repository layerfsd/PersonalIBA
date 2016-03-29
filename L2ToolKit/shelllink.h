
#pragma once

#include <shlobj.h>
#include <intshcut.h>
#include <ATLCOM.h>

/////////////////////////////// Classes ///////////////////////////////////////

//Class which contains all the parameters related to shortcut
class L2_EXT_CLASS CShellLinkInfo
{
public:
	//Constructors / Destructors
	CShellLinkInfo();
	CShellLinkInfo(const CShellLinkInfo& sli);
	~CShellLinkInfo();

	//Methods 
	CShellLinkInfo& operator=(const CShellLinkInfo& sli);

	//Member Variables
	CString m_sTarget;
	LPITEMIDLIST m_pidl;
	CString m_sArguments;
	CString m_sDescription;
	WORD m_wHotkey;
	CString m_sIconLocation;
	int m_nIconIndex;
	int m_nShowCmd;
	CString m_sWorkingDirectory;
	HRESULT m_hResFinalConstruct;

protected:
	//Methods
	HRESULT FinalConstruct();
	UINT CalculatePidlSize(LPITEMIDLIST pidl);
	LPITEMIDLIST GetNextItemID(LPITEMIDLIST pidl);

	//Member Variables
	CComPtr<IMalloc> m_pShellMalloc;
};


//Class which wraps standard shortcuts i.e. IShellLink
class L2_EXT_CLASS CShellLink
{
public:
	//Constructors / Destructors
	CShellLink();

	//Methods
	HRESULT Create(const CShellLinkInfo& sli);
	HRESULT Load(const CString& sFilename);
	HRESULT Save(const CString& sFilename);
	HRESULT Resolve(CWnd* pParentWnd, DWORD dwFlags);

	//Accessors
	CString GetPath() const;
	LPITEMIDLIST GetPathIDList() const;
	CString GetArguments() const;
	CString GetDescription() const;
	WORD GetHotKey() const;
	CString GetIconLocation() const;
	int GetIconLocationIndex() const;
	int GetShowCommand() const;
	CString GetWorkingDirectory() const;

	//Mutators
	void SetPath(const CString& sPath);
	void SetPathIDList(LPITEMIDLIST pidl);
	void SetArguments(const CString& sArguments);
	void SetDescription(const CString& sDescription);
	void SetHotKey(WORD wHotkey);
	void SetIconLocation(const CString& sIconLocation);
	void SetIconLocationIndex(int nIconIndex);
	void SetShowCommand(int nShowCmd);
	void SetWorkingDirectory(const CString& sWorkingDirectory);

protected:
	//Methods
	HRESULT FinalConstruct();

	//Member variables  
	CShellLinkInfo m_sli;
	CComPtr<IShellLink> m_pShellLink;
	CComPtr<IPersistFile> m_pPersistFile;
	CComPtr<IMalloc> m_pShellMalloc;
	HRESULT m_hResFinalConstruct;
};

//Class which wraps internet shortcuts i.e. IUniformResourceLocator
class L2_EXT_CLASS CUrlShellLink
{
public:
	//Constructors / Destructors
	CUrlShellLink();

	//Methods
	HRESULT Create(const CShellLinkInfo& sli);
	HRESULT Load(const CString& sFilename);
	HRESULT Save(const CString& sFilename);
	HRESULT Invoke(CWnd* pParentWnd, DWORD dwFlags, const CString& sVerb);

	//Accessors
	CString GetURL() const;
	CString GetDescription() const;
	WORD GetHotKey() const;
	CString GetIconLocation() const;
	int GetIconLocationIndex() const;
	int GetShowCommand() const;
	CString GetWorkingDirectory() const;

	//Mutators
	void SetURL(const CString& sURL);
	void SetDescription(const CString& sDescription);
	void SetHotKey(WORD wHotkey);
	void SetIconLocation(const CString& sIconLocation);
	void SetIconLocationIndex(int nIconIndex);
	void SetShowCommand(int nShowCmd);
	void SetWorkingDirectory(const CString& sWorkingDirectory);

protected:
	//Methods
	HRESULT FinalConstruct();

	//Member variables  
	CComPtr<IUniformResourceLocator> m_pURL;
	CShellLinkInfo m_sli;
	CComPtr<IShellLink> m_pShellLink;
	CComPtr<IPersistFile> m_pPersistFile;
	CComPtr<IMalloc> m_pShellMalloc;
	HRESULT m_hResFinalConstruct;
};

