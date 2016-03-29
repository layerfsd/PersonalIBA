#include "StdAfx.h"
#include ".\ibareporthelper.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "..\IBARes\resource.h"

CIBAReportHelper::CIBAReportHelper(void)
{
}

CIBAReportHelper::~CIBAReportHelper(void)
{
}

CIBAReportHelper* CIBAReportHelper::GetInstance()
{
	static CIBAReportHelper report;
	return &report;
}
// D:\123\456.htm => file:///D:\123\456.htm
CString CIBAReportHelper::pathToLocalPathURL(LPCTSTR lpPath)
{
	CString str(lpPath);
	str.Insert(0, _T("fill:///"));
	return str;
}

CString CIBAReportHelper::workPath()
{
	CString strPath;
	::GetModuleFileName(NULL, strPath.GetBuffer(512), 512);
	strPath.ReleaseBuffer(512);

	CString strWorkPath;
	strWorkPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath = strWorkPath;
	strWorkPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strWorkPath.Append(_T("\\IBAConfig"));

	return strWorkPath;
}

std::wstring CIBAReportHelper::Utf8_to_WideChar( std::string& strUtf)
{
	wchar_t *pWch = new wchar_t[strUtf.length()+1];
	int nLen = MultiByteToWideChar(CP_UTF8,0,strUtf.c_str(),
		(int)strUtf.size(),pWch,strUtf.length());
	if( nLen <= 0 )
	{
		delete pWch;
		return L"";
	}
	else {
		pWch[nLen] = L'\0';
	}
	std::wstring strRes(pWch);
	delete pWch;
	return strRes;
}
std::string CIBAReportHelper::WideChar_to_Utf8( std::wstring& strVar)
{
	char *pCh = new char[strVar.length()*3 + 1];
	memset(pCh, 0, strVar.length()*3+1);
	UINT nLen = WideCharToMultiByte(CP_UTF8,0,strVar.c_str(),(int)strVar.length(),pCh,strVar.length()*3,NULL,NULL);
	if( nLen <= 0 )
	{
		delete pCh;
		return "";
	}
	else
	{
		if( nLen > strVar.length()*3 )
		{
			pCh[strVar.length()*3] = '\0';
		}
		else
		{
			pCh[nLen] = '\0';
		}
	}
	std::string strRes(pCh);
	delete pCh;
	return strRes;
}

typedef void (*FuncPrint)(LPCWSTR filePath, LPCWSTR btnTitle);

void CIBAReportHelper::DoReport()
{
	CString strReportDllPath = theApp.GetWorkPath() + _T("\\IBABin\\DdnReport.dll");
	HMODULE hMod = ::LoadLibrary(strReportDllPath);
	if (hMod == NULL)
	{
		theApp.IBAMsgBox(_T("Load DdnReport.dll error!"));
		return;
	}
	FuncPrint pFun = (FuncPrint)::GetProcAddress(hMod, "PrintReport");
	if (!pFun)
	{
		theApp.IBAMsgBox(_T("Load DdnReport.dll error!"));
		FreeLibrary(hMod);
		return;
	}

	if (PathFileExists(_T("IBARes.dll")))
	{
		theApp.IBAMsgBox(_T("Load IBARes.dll error!"));
		return;
	}

	// 打开模板文件
	//CString tempLateFilePath = this->workPath() + _T("\\ReportTemplate.html");
	long lSize = 0;
	CIBAHelpper::GetSelfDefineResource(theApp.GetResHandle(), IDR_HTML_PRINTREPORT, MAKEINTRESOURCE(RT_HTML), NULL, &lSize);	// 获取文件大小

	char* pFileContent = new char[lSize+1];
	ZeroMemory(pFileContent, lSize+1);

	CIBAHelpper::GetSelfDefineResource(theApp.GetResHandle(), IDR_HTML_PRINTREPORT, MAKEINTRESOURCE(RT_HTML), pFileContent, &lSize);

	//CFile file;
	//if (!file.Open(tempLateFilePath, CFile::modeRead))
	//{
	//	theApp.IBAMsgBox(_T("Read ReportTemplate.html error!"));
	//	FreeLibrary(hMod);
	//	return;
	//}
	//
	//// 读取文件
	CString strTempFileContent;
	//long nFileLen = file.GetLength();
	//char* pFileContent = new char[nFileLen + 1];
	//file.Read(pFileContent, nFileLen);
	//pFileContent[nFileLen] = 0;

	// utf-8 转换
	std::wstring wcontent = this->Utf8_to_WideChar(std::string(pFileContent));
	strTempFileContent = wcontent.c_str();

	if(!CNetBarConfig::GetInstance()->GetOpenWx())
	{
		// 没有开通微信充值则删除
		strTempFileContent.Replace(_T("微信充值金额"), _T(""));
		strTempFileContent.Replace(_T("[value7]"), _T(""));
	}

	strTempFileContent.Replace(_T("[netbarname]"), this->m_strNetbarName);
	strTempFileContent.Replace(_T("[value1]"), this->m_strValue1);
	strTempFileContent.Replace(_T("[value2]"), this->m_strValue2);
	strTempFileContent.Replace(_T("[value3]"), this->m_strValue3);
	strTempFileContent.Replace(_T("[value4]"), this->m_strValue4);
	strTempFileContent.Replace(_T("[value5]"), this->m_strValue5);
	strTempFileContent.Replace(_T("[value6]"), this->m_strValue6);
	strTempFileContent.Replace(_T("[value7]"), this->m_strValue7);
	strTempFileContent.Replace(_T("[value8]"), this->m_strValue8);
	strTempFileContent.Replace(_T("[value9]"), this->m_strValue9);
	strTempFileContent.Replace(_T("[value10]"), this->m_strValue10);
	strTempFileContent.Replace(_T("[value11]"), this->m_strValue11);
	strTempFileContent.Replace(_T("[value12]"), this->m_strValue12);
	strTempFileContent.Replace(_T("[value13]"), this->m_strValue13);
	strTempFileContent.Replace(_T("[value14]"), this->m_strValue14);
	strTempFileContent.Replace(_T("[value15]"), this->m_strValue15);
	strTempFileContent.Replace(_T("[value16]"), this->m_strValue16);
	strTempFileContent.Replace(_T("[value17]"), this->m_strValue17);
	strTempFileContent.Replace(_T("[value18]"), this->m_strValue18);
	strTempFileContent.Replace(_T("[value19]"), this->m_strValue19);
	strTempFileContent.Replace(_T("[value20]"), this->m_strValue20);

	// 保存文件
	CString strFileNew;
	strFileNew = this->workPath() + _T("\\TempReport.html");

	CFile fileNew;
	if (!fileNew.Open(strFileNew, CFile::modeReadWrite|CFile::modeCreate))
	{
		AfxMessageBox(_T("Create TempReport.html error!"));
		FreeLibrary(hMod);
		return;
	}
	std::string newFileContent = this->WideChar_to_Utf8(std::wstring(strTempFileContent.GetString()));
	fileNew.Write(newFileContent.c_str(), newFileContent.size());
	fileNew.Flush();
	fileNew.Close();
	
	pFun(strFileNew.GetString(), _T("打印"));
	FreeLibrary(hMod);
}

