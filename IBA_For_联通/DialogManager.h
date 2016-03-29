#pragma once

class CDialogManager
{
public:
	CDialogManager(void);
	~CDialogManager(void);

private:

	struct TDialogData
	{
		HWND hWnd;
		UINT nMemberId;
	};

	CArray<TDialogData, TDialogData> m_DialogDataArray;

public:

	void AddDialogData(const HWND hWnd, const UINT nMemberId);

	BOOL IsDialogExists(const UINT nMemberId);

	void ReposDialogs();
};
