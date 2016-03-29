#pragma once

class CIBASheet : public CBCGPPropertySheet
{
public:
	CIBASheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0)
		:CBCGPPropertySheet(nIDCaption,pParentWnd,iSelectPage)
	{
		m_nIDCaption = nIDCaption;
	}

#ifndef __USING_INTER_RESOURCE
	virtual BOOL OnInitDialog()
	{
		BOOL bRes = CBCGPPropertySheet::OnInitDialog();

		if (::IsWindow(m_wndList.m_hWnd))
		{
			int nCount = m_wndList.GetCount();
			while(nCount > 0)
			{
				--nCount;
				CIBAPage *pPage = (CIBAPage*)DYNAMIC_DOWNCAST (CBCGPPropertyPage, GetPage (nCount));
				if (pPage == NULL)
				{
					ASSERT(FALSE);
					return bRes;
				}
				UINT nID = pPage->m_nDlgID; 
				nID <<= 16;
				m_wndList.DeleteString(nCount);
				m_wndList.InsertString(nCount,LOAD_STRING(nID));
				m_wndList.SetItemData(nCount,(DWORD_PTR)pPage);
			}
		}

		SetWindowText(LOAD_STRING(m_nIDCaption));
		return bRes;

	}
#endif

	UINT m_nIDCaption;
};
