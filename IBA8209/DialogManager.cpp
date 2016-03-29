#include "stdafx.h"
#include ".\dialogmanager.h"

CDialogManager::CDialogManager(void)
{
}

CDialogManager::~CDialogManager(void)
{
}

void CDialogManager::AddDialogData(const HWND hWnd, const UINT nMemberId)
{
	TDialogData da = {0};

	da.hWnd = hWnd;
	da.nMemberId = nMemberId;

	m_DialogDataArray.Add(da);
}

BOOL CDialogManager::IsDialogExists(const UINT nMemberId)
{
	for (INT i = 0; i < m_DialogDataArray.GetSize(); i++)
	{
		TDialogData da = m_DialogDataArray.GetAt(i);

		if (IsWindow(da.hWnd))
		{
			if (nMemberId == da.nMemberId)
			{
				return TRUE;
			}
		}
		else
		{
			m_DialogDataArray.RemoveAt(i);
			i--;
		}
	}

	return FALSE;
}

void CDialogManager::ReposDialogs()
{
	for (INT i = m_DialogDataArray.GetSize(); i > 0; i--)
	{
		TDialogData da = m_DialogDataArray.GetAt(i - 1);

		if (IsWindow(da.hWnd))
		{
			SetWindowPos(da.hWnd, AfxGetMainWnd()->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
	}
}