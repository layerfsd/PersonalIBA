#pragma once

class CStringContainer
{
public:
	CStringContainer(void);
	~CStringContainer(void);

public:
	const char* GetStringA(UINT nID);
	const wchar_t* GetStringW(UINT nID);
};
