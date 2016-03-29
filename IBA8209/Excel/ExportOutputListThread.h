/*
	用于结账输出导出Excel表使用
	这个操作容易崩溃，放在线程里完成
	这样操作过程崩溃的时候是普通线程崩溃而不会导致整个线程崩溃
*/
#pragma once

class CExportOutputListThread :  public CThread
{
public:
	CExportOutputListThread();
	virtual ~CExportOutputListThread();
protected:
	virtual int Run();
	void ExportExcel();
	// 导出Excel的字符串
	CString m_strExportExcel;
	// Excel文件存储的全路径
	CString m_strFullPath;
	UINT m_nRowCount;
	UINT m_nColCount;
	
public:
	void SetParam(LPCTSTR szExportExcel, LPCTSTR szFullPath, UINT nRowCount, UINT nColCount);
};