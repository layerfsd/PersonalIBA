#pragma once

#include "GxxListCtrl.h"
#include "FileAnalyzer.h"
using namespace NS_COMMON;

// CStringListCtrl

#define WM_ITEMCHANGE (WM_USER+101)

class CStringListCtrl : public CGxxListCtrl
{
	DECLARE_DYNAMIC(CStringListCtrl)

public:
	CStringListCtrl();
	virtual ~CStringListCtrl();

	enum{
		COL_ID = 0,       // ID值
		COL_MASK = 1,     // 标识符
		COL_CONTENT = 2,  // 字符串内容
		COL_ORG = 3,      // 源字符换参照
		COL_COUNT  // 列数
	};

	void InitList();
	void SaveAs(CString strNewFilePath);

	BOOL m_bInit;
	vector<StringItem> m_items;

	BOOL m_bHaveChanged;

	void SetChanged(BOOL bChanged = TRUE);
	BOOL GetChanged(){
		return m_bHaveChanged;
	}

protected:
	DECLARE_MESSAGE_MAP()

	UINT GetNewID();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CString GetChildWndText(CWnd*pWnd,int nItem,int nSubItem);

	BOOL CheckModify(CString newString, int nRow);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL AddItem(StringItem& it);
	BOOL AddItems(vector<StringItem>& items);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	
	void DeleteAllStrings();
	void DeleteString(int nRow);
	void InsertString(int nRow);
	void AddString();
	BOOL Search(CString strKeyword, BOOL bCase, BOOL bWholeMatch,int nLastResult, int& nNewResult);
	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnClickItem(int nItem,int nSubItem);
};


