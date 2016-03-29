#include "StdAfx.h"
#include ".\capturebase.h"
#include <atlbase.h>

CCaptureBase::CCaptureBase(void)
{
	m_nNetidID = 0;
	m_nIdNumberID = 0;
	m_nNameID = 0;

	m_bEx = FALSE;
}

CCaptureBase::~CCaptureBase(void)
{
}

BOOL CCaptureBase::Create(BOOL bDialog, LPCTSTR lpszCaption, LPCTSTR lpszClassName)
{
	if (bDialog)
	{
		return m_wndFinder.Init(lpszCaption);
	}
	else
	{
		return m_wndFinder.Init(lpszClassName, lpszCaption);
	}
}


BOOL CCaptureBase::CreateFrom(const CaptureItem& item)
{
	m_nNetidID = item.nCardId;
	m_nIdNumberID = item.nNumberId;
	m_nNameID = item.nNameId;
	
	m_bEx = FALSE;
	return Create(item.bDialog, item.strWindowName, item.strClassName);
}

BOOL CCaptureBase::CreateFromEx(const CaptureItemEx& item)
{
	m_nNetidID = item.nCardId;
	m_nIdNumberID = item.nNumberId;
	m_nNameID = item.nNameId;

	m_bEx = TRUE;
	return Create(item.bDialog, item.strWindowName, item.strClassName);
}
void CCaptureBase::AddNetidID(UINT nID)
{
	m_nNetidID = nID;
}

void CCaptureBase::AddIdNumberID(UINT nID)
{
	m_nIdNumberID = nID;
}	

void CCaptureBase::AddNameID(UINT nID)
{
	m_nNameID = nID;
}

BOOL CCaptureBase::CaptureAndSend(HWND hIBAWnd)
{
	if (!hIBAWnd)
	{
		MessageBox(NULL, "嘟嘟牛收银台没有启动!", "提示", MB_OK);
		GXX_LOG_WARNING("嘟嘟牛收银台没有启动!");
		return FALSE;
	}

	CString strNetId,strIdNumber,strName;
	m_wndFinder.GetChildCtrlText(m_nNetidID,strNetId);
	m_wndFinder.GetChildCtrlText(m_nIdNumberID,strIdNumber);
	m_wndFinder.GetChildCtrlText(m_nNameID,strName);

	if (strNetId.IsEmpty())
	{
		if (strIdNumber.IsEmpty())
		{
			GXX_LOG_WARNING("证件号码、上网账号均为空，不发送信息!");
			return FALSE;
		}
		strNetId = strIdNumber; // 让上网账号和证件号码一样
	}
	else
	{
		if (strIdNumber.IsEmpty())
		{
			GXX_LOG_WARNING("证件号码为空，不发送信息!");
			return FALSE;
		}
	}

	if (strName.IsEmpty())
	{
		strName = "网吧用户";
	}

	return SendCardInfo(hIBAWnd, strNetId, strIdNumber, strName);
}

static int g_index = 0;
BOOL   CALLBACK   EnumChildProc1(HWND hwnd,LPARAM lParam)       //回调函数
{
	if(hwnd) //如果子窗口存在
	{
		CString strOut;
		::SendMessage(hwnd,   WM_GETTEXT,   WPARAM(100),   LPARAM(strOut.GetBuffer(100)));
		strOut.ReleaseBuffer();
		CString strTmp;
		strTmp.Format("ID顺序:%d, 文本内容:%s", g_index, strOut);
		GXX_LOG_HIT(strTmp);

		g_index++;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



BOOL CCaptureBase::CaptureAndSendEx(HWND hIBAWnd)
{
	if (!hIBAWnd)
	{
		MessageBox(NULL, "嘟嘟牛收银台没有启动!", "提示", MB_OK);
		GXX_LOG_WARNING("嘟嘟牛收银台没有启动!");
		return FALSE;
	}

	g_index = 0;
	::EnumChildWindows(m_wndFinder.m_hBaseWnd,  (WNDENUMPROC)EnumChildProc1, 0);

	////输出测试日志
	//HWND hWnd = ::GetWindow(m_wndFinder.m_hBaseWnd, GW_CHILD);
	//UINT iIndex = 0;
	//while(hWnd)
	//{
	//	CString strOut;
	//	::SendMessage(hWnd,   WM_GETTEXT,   WPARAM(100),   LPARAM(strOut.GetBuffer(100)));
	//	strOut.ReleaseBuffer();
	//	CString strTmp;
	//	strTmp.Format("ID顺序:%d, 文本内容:%s", iIndex,strOut);
	//	GXX_LOG_HIT(strTmp);
	//	iIndex ++;

	//	hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	//}

	//

	CString strNetId,strIdNumber,strName;
	m_wndFinder.GetChildCtrlTextEx(m_nNetidID,strNetId);
	m_wndFinder.GetChildCtrlTextEx(m_nIdNumberID,strIdNumber);
	m_wndFinder.GetChildCtrlTextEx(m_nNameID,strName);


	if (strNetId.IsEmpty())
	{
		if (strIdNumber.IsEmpty())
		{
			GXX_LOG_WARNING("证件号码、上网账号均为空，不发送信息!");
			return FALSE;
		}
		strNetId = strIdNumber; // 让上网账号和证件号码一样
	}
	else
	{
		if (strIdNumber.IsEmpty())
		{
			GXX_LOG_WARNING("证件号码为空，不发送信息!");
			return FALSE;
		}
	}

	if (strName.IsEmpty())
	{
		strName = "网吧用户";
	}

	return SendCardInfo(hIBAWnd, strNetId, strIdNumber, strName);
}

BOOL SendCardInfo(HWND hIBAWnd, LPCTSTR lpszNetid, LPCTSTR lpszIDNumber, LPCTSTR lpszName)
{
	// 有刷卡信息,发送信息给收银台

	GXX_LOG_HIT("send card information: name:%s, idnumber:%s, netid:%s", lpszName, lpszIDNumber, lpszNetid);


	int		nIdType = 12;         // 二代证
	CString strPassword = "123";  // 密码
//
//#ifdef DEBUG
//	strNetId = "420114198607072210";
//	strIdNumber = "420114198607072210";
//	strName = "小郭";
//#endif

	CRegKey reg;

	if(0 == reg.Create(HKEY_CURRENT_USER, "SoftWare\\Sunlike\\IBA\\RealName\\GLW\\OpenUser"))/*Base\\NewUser"))*/
	{
		reg.SetStringValue("IDNo", lpszNetid);
		reg.SetStringValue("CertificateID", lpszIDNumber);
		reg.SetDWORDValue("CertificateType", nIdType);
		reg.SetStringValue("Name", lpszName);
		reg.SetStringValue("Password", strPassword);

		/*time_t tm = time(NULL);
		CString strTm;
		strTm.Format("%ld", tm);
		reg.SetStringValue(_T("tValue1"), strTm);
		CString strOrgCode;
		strOrgCode += lpszNetid;
		strOrgCode.AppendFormat("%d", tm);
		INT nLen = strOrgCode.GetLength();
		BYTE *pBuff = new BYTE[nLen+8];
		memcpy(pBuff,strOrgCode.GetString(), nLen);
		BYTE byCheck[8] = {0x03,0xF1,0x00,0x0A,0x0B,0xED,0x02,0xC1};
		memcpy(pBuff+nLen, byCheck, 8);
		char pCode[33] = {0};
		G2XUtility::CG2XMD5::CalculateMD5(pBuff, nLen+8, pCode);
		reg.SetStringValue("tValue2", pCode);
		delete []pBuff;*/

		reg.Close();
	}
	else
	{
		GXX_LOG_HIT("写入失败!");
	}

	if( hIBAWnd )
	{
		::SendMessage(hIBAWnd, WM_REALNAME, 999, 0);
		//::SendMessage(hIBAWnd, WM_REALNAME, 0, 100);
	}
	else
	{
		MessageBox(NULL, "嘟嘟牛收银台没有启动!", "提示", MB_OK);
		GXX_LOG_HIT("嘟嘟牛收银台没有启动!");
		return FALSE;
	}

	return TRUE;
}