#include "stdafx.h"
#include "servletmethod.h"
#include "CommonMethod.h"
#include "WinHttpRequest.h"
#include "..\IBA.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"
#include "..\BrokenNetwork.h"
#include "..\MsgBoxDlg.h"
#include "..\MainFrm.h"

using namespace NS_SERVLET;

UINT CServletMethod::m_nFirstCenterIndex = 1; //默认为主中心开始
UINT CServletMethod::m_nFirstQueryCenterIndex = 1; //默认为主查询中心开始

CServletMethod::CServletMethod(LPCTSTR lpszMethodName)
: m_strMethodName(lpszMethodName), m_nCardType(ECT_ID)
, m_nLocalSerialNo(0), m_nNetbarId(0)
, m_bUseQueryHost(FALSE), m_nRevTimeout(20)//20秒
{
	m_nNetbarId = CNetBarConfig::GetInstance()->GetNetBarId();
	m_strPSK = CNetBarConfig::GetInstance()->GetNetBarKey();
}

CServletMethod::~CServletMethod(void)
{
}

CString CServletMethod::GetMethodName()
{
	return m_strMethodName;
}

CString CServletMethod::GetRequestAddr(UINT nIndex)
{
	static const TCHAR HttpFormatString[] = _T("http://%s:%d/%s");

	CString strUrl;

	switch( nIndex )
	{
	case E_LOCAL_CENTER:
		{
			//本地中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetLocalServerIP(),
				8080, _T("localnetbar"));
		}
		break;
	case E_MAIN_CENTER:
		{
			//主中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetMainCenterIP(),
				CNetBarConfig::GetInstance()->GetMainCenterPort(),
				CNetBarConfig::GetInstance()->GetMainCenterPath());
		}
		break;
	case E_BACKUP_CENTER:
		{
			//备中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetBackupCenterIP(),
				CNetBarConfig::GetInstance()->GetBackupCenterPort(),
				CNetBarConfig::GetInstance()->GetBackupCenterPath());
		}
		break;
	case E_OTHER_CENTER:
		{
			//异地中心
			strUrl.Format(_T("http://%s/netbar"), CNetBarConfig::GetInstance()->GetStandbyHost());
		}
		break;
	case E_QUERY_CENTER:
		{
			//主查询机
			strUrl.Format(_T("http://%s/netbar"), CNetBarConfig::GetInstance()->GetQueryHost());
		}
		break;
	case E_QUERY_CENTER2:
		{
			//备查询机
			strUrl.Format(_T("http://%s/netbar"), CNetBarConfig::GetInstance()->GetQueryHost2());
		}
		break;
	default:
		{
			IBA_ASSERT2(false,"未知中心位置，请检查代码");
			WriteLog(_T("未知中心位置，请检查代码"));
		}
		break;
	}

//#ifdef DEBUG
//	strUrl = _T("http://192.168.10.233:8080/netbar");   //2015-0826 liyajun Temp
//	//strUrl = _T("http://127.0.0.1:8080/netbar");   //2015-0826 liyajun Temp
//	//strUrl = _T("http://121.11.92.182/netbar");   //2015-0826 liyajun Temp
//#endif

	strUrl.Append(_T("/servlet/CommonRequest?command="));
	strUrl.Append(GetMethodName());
	strUrl.Append(_T("&version=2&param="));

	//WriteLog(_T("URL:") + strUrl + _T("xxx(Params)"));
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		WriteLog(_T("URL:") + strUrl);
	}	
	
	return strUrl;
}

void CServletMethod::AddParams()
{
	m_Params.AddParam(_T("NetbarId"), EPT_NUMBER, 5);
	m_Params.SetValue(_T("NetbarId"), m_nNetbarId);
}

void CServletMethod::AddRespFields()
{
	m_RespFields.AddRespField(_T("MAC"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("serialNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Status"), EPT_NUMBER, 2);
	m_RespFields.SetValue(_T("Status"), _T("-1"));
}

CString CServletMethod::EncodeParams(CString strParams)
{
	//strParams = CURLEncode(strParams);

	return strParams;
}

UINT CServletMethod::MakeLocalSerialNo()
{
	return CIBAConfig::GetInstance()->GetLocalSerialNo();
}

CString CServletMethod::MakeParamsString()
{
	CString strParams;
	
	m_nLocalSerialNo = MakeLocalSerialNo();

	CIBAHelpper::AppendString(strParams, m_nLocalSerialNo, 6);

	strParams.Append(MergeParams());

	CString strMac(MakeMacString(strParams));
	
	if (strMac.IsEmpty())
	{
		CIBAHelpper::FillString(strMac, 16, '0');
	}

	strParams.Insert(0, strMac);
	strParams = EncodeParams(strParams);

	// 2015-3-27 tmelody
	// 上报不打日志
	if (0 == GetMethodName().CompareNoCase(_T("NonSysService")))
	{
#if _DEBUG
		WriteLog(_T("Params:") + strParams);
#endif
	}
	else
	{
		if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
		{
			WriteLog(_T("Params:") + strParams);
		}		
	}

	return strParams;
}

BOOL CServletMethod::OnBegin()
{
	return TRUE;
}

//设置m_RespFields.Status和打印日志
void CServletMethod::OnEnd(BOOL bRet)
{
	CString strTmp = m_RespFields.GetValueAsString(_T("Status"));

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("CServletMethod::Status - %s"), strTmp);
	}	

	if (!bRet) //失败了
	{
		m_RespFields.ResetAllValue();
		m_RespFields.SetValue(_T("Status"), _T("-1")); //重置为-1
	}
}

BOOL CServletMethod::Execute()
{
	if (!OnBegin())//开始
	{
		return FALSE;
	}

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("CServletMethod::Execute - %s"), GetMethodName());
	}	
	//重置
	m_RespFields.ResetAllValue();
	m_RespFields.SetValue(_T("Status"), _T("-1")); //重置为-1

	BOOL bRet = FALSE;
	
	CString strParams(MakeParamsString());//组合成参数字串
	
	CString strResp;  
	
	//获取中心数据，尝试3次
	bRet = GetDecodeVerify(strParams,strResp);
	
	//如果当前状态为Restore,则改为Normal
	if(bRet)
	{
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
		{
			CBrokenNetwork::GetInstance()->EnableNetwork();
		}
	}
	else //如果之前状态为Normal,则改为Restore,同时开启测试网络断开线程
	{
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
		{
			CBrokenNetwork::GetInstance()->RestoreNetwork();
			if(FALSE == CBrokenNetwork::GetInstance()->bTestBrokenStarted)
			{
				CBrokenNetwork::GetInstance()->bTestBrokenStarted = TRUE;
				CBrokenNetwork::GetInstance()->m_threadTestBroken->Start();
			}
		}
	}

	OnEnd(bRet);

	return bRet;
}

BOOL CServletMethod::GetDecodeVerify(CString strParams,CString& strResp)
{
	//获取中心数据，日志获取所花费时间
	DWORD dwTick = GetTickCount();
	BOOL bRet = GetCenterData(strParams, strResp);
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("connect center using time: %u"), GetTickCount() - dwTick);
	}	

	//获取成功且解析校验成功，才算成功
	if (bRet) //从中心获得了数据
	{	//解析			
		bRet = DecodeResp(strResp); //先解析还是先校验呢？

		ASSERT(bRet);				//因为无法肯定中心无论失败成功是否都做加密，所以这里先解析后校验！

		if (bRet && GetStatusCode() == 0) //只校验操作成功的数据，对失败的数据没必要做校验
		{
			bRet = VerifyResp(strResp);//验证
			ASSERT(bRet);
		}
	}
	return bRet;
}

BOOL CServletMethod::GetHttpData(LPCTSTR lpszURL, CString& strResp)
{
	strResp.Empty();

	static const LONG CodePage_GB2312 = 936; // 简体中文 (GB2312)

	static const LONG WinHttpRequestOption_URLCodePage = 2;

	CWinHttpRequest HttpRequest(FALSE); 
	
	try//暂时保留此处try
	{
		HttpRequest.Create();
		HttpRequest.SetOption(WinHttpRequestOption_URLCodePage, COleVariant(CodePage_GB2312));
		HttpRequest.SetTimeouts(5000, 5000, 10000, 1000 * m_nRevTimeout);
		HttpRequest.Open(lpszURL);
		HttpRequest.Send();
	}
	catch (...)
	{
		IBA_LOG(_T("CServletMethod::GetHttpData Error!"));

		return FALSE;
	}

	if (HttpRequest.GetReadyState() == 4)//连接成功
	{
		strResp = HttpRequest.GetResponseText(); //坚决不能TRIM

		if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
		{
			WriteLog(_T("Resp:") + strResp);
		}		

		if (strResp.GetAt(0) == '<') //正在升级
		{
			strResp.Empty();

			IBA_LOG(_T("Center is being upgraded!"));

			return FALSE;
		}
	}

	if (HttpRequest.GetReadyState() == 4)
	{
		return TRUE;
	}
	else
	{
		//if (!CBrokenNetwork::GetInstance()->m_bHasFirstTrying)
		//{
		//	theApp.UpdateIBAStatus(TRUE);  //更新状态栏显示

		//	CBrokenNetwork::GetInstance()->m_bHasFirstTrying = TRUE;
		//}
	}
	

	return (HttpRequest.GetReadyState() == 4);
}	

CString CServletMethod::MergeParams()
{
	return m_Params.GetAllValue();
}

CString CServletMethod::MakeMacString(CString strParams)
{
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		WriteLog(_T("PSK:") + m_strPSK);
	}

	CString strMac;
	
	CEncrypt::CalcMac(strParams, strMac, m_strPSK, TRUE);

	return strMac;
}

BOOL CServletMethod::VerifyResp(CString strResp)
{
	CString strMac = strResp.Left(16);

	CString strTmp;

	CEncrypt::CalcMac(strResp.Mid(16), strTmp, m_strPSK, TRUE);

	if (0 == strMac.CompareNoCase(strTmp))
	{
		return TRUE;
	}
	else
	{
		CIBALog::GetInstance()->WriteFormat(_T("VerifyResp Failed. PSK:%s MAC:%s"), m_strPSK, strTmp);
		CIBALog::GetInstance()->WriteFormat(_T("Resp:%s"), strResp);
	}

	return FALSE;
}

BOOL CServletMethod::DecodeResp(CString strResp)
{
	return m_RespFields.DecodeResp(strResp);
}	

INT CServletMethod::GetStatusCode()
{
	CString strTmp = m_RespFields.GetValueAsString(_T("Status"));

	int nTmp = _ttoi(strTmp);

	return nTmp;
}

void CServletMethod::WriteLog(LPCTSTR lpszInfo)
{
	CIBALog::GetInstance()->Write(lpszInfo, CIBALog::ELT_SERVLET);
}

void CServletMethod::SetPSK(CString newVal)
{
	m_strPSK = newVal;
}

CString CServletMethod::GetPSK() const
{
	return m_strPSK;
}

ECARDTYPE CServletMethod::GetCardType() const
{
	return m_nCardType;
}

void CServletMethod::SetCardType(ECARDTYPE newVal)
{
	m_nCardType = newVal;
}

UINT CServletMethod::GetSerialNo()
{
	return m_RespFields.GetValueAsUINT(_T("serialNo"));
}

UINT CServletMethod::GetLocalSerialNo() const
{
	return m_nLocalSerialNo;
}

CString CServletMethod::GetStatusMessage()
{
	CStatusMessageMethod sm;
	sm.SetStatusCode(GetStatusCode());
	sm.Execute();
	return sm.GetMessage();
}

BOOL CServletMethod::GetCenterData(CString strParams, CString& strResp)
{
//#ifdef _DEBUG
//	m_bUseQueryHost = FALSE; //调试下不使用查询机
//#endif

	BOOL bRet = FALSE;

	CString strURL;

	if (m_bUseQueryHost)
	{
		for (UINT i = 0; i < 2; i++) //查询有2个中心
		{
			UINT nIdx = m_nFirstQueryCenterIndex + i;

			if (nIdx > 2) nIdx -= 2; //不能超过2

			strURL = GetRequestAddr(nIdx + 3) + strParams;

			if (bRet = GetHttpData(strURL, strResp))
			{
#ifdef _DEBUG
				IBA_LOG(_T("发送的数据：%s"), strURL);
				IBA_LOG(_T("返回的数据：%s"), strResp);
#endif
				m_nFirstQueryCenterIndex = nIdx; //记录成功的中心顺序
				break;
			}
		}
	}
	else
	{
		for (UINT i = 0; i < 3; i++) //主支持有3个中心
		{
			UINT nIdx = m_nFirstCenterIndex + i;

			if (nIdx > 3) nIdx -= 3; //不能超过3

			if (3 == nIdx && CNetBarConfig::GetInstance()->GetStandbyHost().IsEmpty())
			{
				continue; //没有配置异地服务器
			}

			CString strRequesAddr = GetUrlFromIndex(nIdx);

			strURL = GetRequestAddr(nIdx) + strParams;
			if (bRet = GetHttpData(strURL, strResp))
			{
#ifdef _DEBUG
				IBA_LOG(_T("发送的数据：%s"), strURL);
				IBA_LOG(_T("返回的数据：%s"), strResp);
#endif
				//若连接的是备中心，则在IBA.ini中交换主中心和备中心设置，使连通的始终是主中心

				// 2014-8-15 - qsc 添加最外层这个判断，主要是解决多线程同时调用这个函数的时候同步的问题，防止多次替换，产生错误
				if(0 != strRequesAddr.CompareNoCase(GetUrlFromIndex(E_MAIN_CENTER)) && m_nFirstCenterIndex != nIdx )	
				{
					// 2014-8-15 - qsc 中心切换到了备中心或异地中心则修改数据库主中心地址
					if(E_BACKUP_CENTER == nIdx)
					{
						CString strMain = GetUrlFromIndex(E_MAIN_CENTER);
						CString strBk = GetUrlFromIndex(nIdx);

						// 交换
						CNetBarConfig::GetInstance()->SetMainHost(strBk);
						CNetBarConfig::GetInstance()->SetBackUpHost(strMain);

						m_nFirstCenterIndex = E_MAIN_CENTER;			// 交换后能连通的始终是主中心

					}
					else if(E_OTHER_CENTER == nIdx)
					{
						CString strMain = GetUrlFromIndex(E_MAIN_CENTER);
						CString strStandby = GetUrlFromIndex(nIdx);

						// 交换
						CNetBarConfig::GetInstance()->SetMainHost(strStandby);
						CNetBarConfig::GetInstance()->SetStandbyHost(strMain);//异地服务器

						m_nFirstCenterIndex = E_MAIN_CENTER;			// 交换后能连通的始终是主中心
					}	
					else
					{
						m_nFirstCenterIndex = nIdx; //记录成功的中心顺序
					}
				}
				break;
			}
			else
			{
				TRACE("\n\n");
				OutputDebugString(strURL);
			}
		}
	}

	return bRet;
}

CString CServletMethod::GetUrlFromIndex(UINT nIndex)
{
	static const TCHAR HttpFormatString[] = _T("%s:%d");

	CString strUrl;

	switch( nIndex )
	{
	case E_LOCAL_CENTER:
		{
			//本地中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetLocalServerIP(),
				8080);
		}
		break;
	case E_MAIN_CENTER:
		{
			//主中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetMainCenterIP(),
				CNetBarConfig::GetInstance()->GetMainCenterPort());
		}
		break;
	case E_BACKUP_CENTER:
		{
			//备中心
			strUrl.Format(HttpFormatString, CNetBarConfig::GetInstance()->GetBackupCenterIP(),
				CNetBarConfig::GetInstance()->GetBackupCenterPort());
		}
		break;
	case E_OTHER_CENTER:
		{
			//异地中心
			strUrl.Format(_T("%s"), CNetBarConfig::GetInstance()->GetStandbyHost());
		}
		break;
	case E_QUERY_CENTER:
		{
			//主查询机
			strUrl.Format(_T("%s"), CNetBarConfig::GetInstance()->GetQueryHost());
		}
		break;
	case E_QUERY_CENTER2:
		{
			//备查询机
			strUrl.Format(_T("%s"), CNetBarConfig::GetInstance()->GetQueryHost2());
		}
		break;
	default:
		{
			IBA_ASSERT2(false,"未知中心位置，请检查代码");
			WriteLog(_T("未知中心位置，请检查代码"));
		}
		break;
	}

	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		WriteLog(_T("GetUrlFromIndex:") + strUrl);
	}	

	return strUrl;
}
