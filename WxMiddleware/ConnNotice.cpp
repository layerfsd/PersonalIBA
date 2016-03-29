#include "StdAfx.h"
#include "ConnNotice.h"
#include "ClientSocket.h"
#include <string>

#include <atlutil.h>
#include "G2XCommonTool.h"
#include "encrypt.h"
#include <vector>
#include "GxxString.h"
#include "WxLog.h"
#include "WX_ado.h"
#include "WX_base.h"

using namespace G2XUtility;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const size_t LEN_MAXBUFFER = 65536 * 10;
static CString MD5Key("8d4ac26906c442a18ac7938a87090cb8");
CConnNotice* CConnNotice::m_Instance = NULL;

void OutputEx(LPCTSTR lpszStr)
{
	TRACE(lpszStr);
	TRACE(_T("\n"));
}

CString ToString(long nNumber)
{
	CString str;
	str.Format(_T("%d"), nNumber);
	return str;
}

enum Notice_Ret { TERMON = 0,
	OK = 1,
	CHECK_FAILED =1,
	NOTMATCHLENGTH = 3,
	DOMAINIDNULL = 4,
	NETBARIDNULL = 5,
	PSKNOTMATCH = 6,
	NOTSESSION = 7
};

CConnNotice* CConnNotice::GetInstance()
{
	if (NULL  == m_Instance)
	{
		m_Instance = new CConnNotice();
	}

	return m_Instance;
}

CConnNotice::CConnNotice()
{
	m_pWxDelegate = NULL;
	m_lpSocket = NULL;
}


void CConnNotice::SetWxMiddlewareDelegate( CWxMiddlewareDelete* pDelegate )
{
	m_pWxDelegate = pDelegate;
}

CWxMiddlewareDelete* CConnNotice::GetWxMiddlewareDelegate()
{	
	return m_pWxDelegate;
}

BOOL CConnNotice::ConnectNotice( void )
{
	WXM_LOG0(_T("开始连接服务器..."));
	if (m_pWxDelegate == NULL) {
		return FALSE;
	}
	TCHAR ip[60] = {0};
	UINT port = 0;
	if (!m_pWxDelegate->getMiddlewareCenterIPPort(ip, &port))
	{
		WXM_LOG(_T("连接%s:%d失败"), ip, port);
		return FALSE;
	}

	CString strTip;
	strTip.Format(_T("微信中间件IP:%s, port:%d"), ip, port);
	WXM_LOG(strTip);

	if (m_lpSocket == NULL)
	{
		m_lpSocket = new CG2XSocketClient(this);
	}
	
	m_lpSocket->Close();
	
	m_bAutoClose = true;
	
	//TRACE("\nport: %d", CGlobal::GetInst()->GetServerPort());
	//memcpy(ip, _T("192.168.10.201"), 60);
	m_lpSocket->ConnectServer(ip, port);

	return TRUE;
}

CConnNotice::~CConnNotice(void)
{
}

BOOL CConnNotice::IsSocketValid(void)
{
	if (m_lpSocket != NULL && m_lpSocket->IsConnected())
	{
		return TRUE;
	}

	return FALSE;
}

void CConnNotice::SendData(int nTag, const wchar_t *pStr, int nSize)
{
	//wchar_t *pwData = new wchar_t[SOCKT_MIN_SIZE + nSize + 1];
	wchar_t *pwData = new wchar_t[SOCKT_MIN_SIZE + nSize + 2];//20160119liyajun
	swprintf(pwData, L"%04X%04X%s\n", nTag, nSize, pStr);

	std::string strSendData = G2XUtility::CG2XCommonTool::WideChar_to_Utf8(std::wstring(pwData));
	m_lpSocket->SendData(strSendData.c_str(), (int)strSendData.size());
}

void CConnNotice::StopConn()
{
	WXM_LOG0(_T("微信线程停止运行"));
	try
	{
		if(m_lpSocket)
		{
			delete m_lpSocket;
			m_lpSocket = NULL;
		}
	}
	catch (...)
	{
		WXM_LOG0(_T("微信线程停止运行 异常"));
	}

	m_bAutoClose = false;
}

VOID CALLBACK CConnNotice::TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,  DWORD dwTime )
{
	if(idEvent == m_Instance->m_nTimerIDHeart)
	{
		m_Instance->SendHeart();
		KillTimer(NULL, m_Instance->m_nTimerIDHeart);
	}
	else if(idEvent == m_Instance->m_nTimerIDReConn)
	{
		m_Instance->ConnectNotice();
		KillTimer(NULL, m_Instance->m_nTimerIDReConn);
	}
	else
	{
		KillTimer(NULL, idEvent);
	}	
}

void CConnNotice::SendHeart()
{
	UINT nNetbarId,nDomainId;
	nNetbarId = nDomainId = 0;

	if (!m_pWxDelegate){
		return;
	}

	m_pWxDelegate->getNetbarIdAndDomainId(&nNetbarId, &nDomainId);
	wchar_t szHeart[20] = {0};
	// 心跳value为value为domainId#netBarId
	swprintf(szHeart, L"#%d#%d", nDomainId,	nNetbarId);

	m_Instance->SendData(TAG_HEART, szHeart, wcslen(szHeart));
}

void CConnNotice::SendLogin()
{
	UINT nNetbarId,nDomainId;
	nNetbarId = nDomainId = 0;

	if (!m_pWxDelegate){
		return;
	}

	m_pWxDelegate->getNetbarIdAndDomainId(&nNetbarId, &nDomainId);

	wchar_t szHead[20] = {0};
	// 登录value为domainId#netBarId#MD5(psk)#校验字符串
	swprintf(szHead, L"#%d#%d#", nDomainId,	nNetbarId);

	CString strT;
	strT.Format(_T("%d%d"), nDomainId, nNetbarId);

	CString strMD5;
	CEncrypt::CalcMD5(strT + MD5Key, strMD5);
	strMD5 = strMD5.Mid(8, 16);
	strMD5.MakeLower();

	std::wstring strMD5T = CT2W(strMD5);

	std::wstring str2 = szHead + strMD5T;

	SendData(TAG_LOGIN, str2.c_str(), str2.size());
}

// 00 成功，01 登录失败 03 客户端不是锁屏状态
void CConnNotice::SendClientLoginRet(LPCWSTR szTermID, int nRet)
{
	UINT nNetbarId,nDomainId;
	nNetbarId = nDomainId = 0;

	if (!m_pWxDelegate){
		return;
	}

	m_pWxDelegate->getNetbarIdAndDomainId(&nNetbarId, &nDomainId);

	wchar_t szHead[30] = {0};
	// 心跳value为value为domainId#netBarId
	swprintf(szHead, L"#%d#%d#%s#%02d", nDomainId, nNetbarId, szTermID, nRet);
	SendData(TAG_CLIENT_LOGIN, szHead, wcslen(szHead));
}

// 00 成功，01 失败
void CConnNotice::SendClientLogoutRet(LPCWSTR szTermID, int nRet)
{
	UINT nNetbarId,nDomainId;
	nNetbarId = nDomainId = 0;

	if (!m_pWxDelegate){
		return;
	}

	m_pWxDelegate->getNetbarIdAndDomainId(&nNetbarId, &nDomainId);

	wchar_t szHead[30] = {0};

	swprintf(szHead, L"#%d#%d#%s#%02d", nDomainId, nNetbarId, szTermID, nRet);
	SendData(TAG_CLIENT_LOGOUT, szHead, wcslen(szHead));
}

void CConnNotice::SendClientMessageRet(int nRet)
{
	UINT nNetbarId,nDomainId;
	nNetbarId = nDomainId = 0;

	if (!m_pWxDelegate){
		return;
	}

	m_pWxDelegate->getNetbarIdAndDomainId(&nNetbarId, &nDomainId);

	wchar_t szHead[30] = {0};

	swprintf(szHead, L"#%d#%d#%02d", nDomainId, nNetbarId, nRet);
	SendData(TAG_MESSAGE, szHead, wcslen(szHead));
}

int CConnNotice::ConvertRetToCenter(int nConvert)
{
	// 将UDO返回的值转换为中心需要的值

	// nRetCode=101非锁屏状态，=102登录失败，= 0登录成功
	switch(nConvert)
	{
	case 101:
		return 1;

	case 102:
		return 2;

	case 0:
		return 0;

	default:
		return 0;
	}
}

void CConnNotice::didServerConnected( CG2XSocketClient* pSender )
{
	WXM_LOG0(_T("Connect success."));
	SendLogin();
}

void CConnNotice::didReceiveData( CG2XSocketClient* pSender, void* buf, int bufLen )
{
	std::wstring str = G2XUtility::CG2XCommonTool::Utf8_to_WideChar(std::string((const char*)buf));

	CString strTip;
	strTip.Format(L"中心返回:%s", str.c_str());

	WXM_LOG0(strTip);

	// 小于最小数据值, 或大于最大数据值都是错的
	if(str.size() < SOCKT_MIN_SIZE)
	{
		return ;
	}
	long nValLen = wcstol(str.substr(4, 4).c_str(), NULL, 16);
	if(str.size() - SOCKT_MIN_SIZE < nValLen)
	{
		return ;
	}
	long nTag = wcstol(str.substr(0, 4).c_str(), NULL, 16);

	switch(nTag)
	{
	case TAG_LOGIN:
		{
			long lRet = _wtoi(str.substr(8, nValLen).c_str());
			if(IDOK == lRet)
			{
				WXM_LOG0(_T("登录成功!"));
				m_nTimerIDHeart = SetTimer(NULL, 0, 5 * 60 * 1000, TimerProc);
			}
			else
			{
				StopConn();
				ConnectNotice();// 重连
				WXM_LOG0(_T("登录失败!") + ToString(lRet));
			}	
		}	
		break;

	case TAG_HEART:
		{
			long lRet = _wtoi(str.substr(8, nValLen).c_str());
			if(IDOK == lRet)
			{
				WXM_LOG0(_T("心跳成功!"));
				m_nTimerIDHeart = SetTimer(NULL, 0, 5 * 60 * 1000, TimerProc);
			}
			else
			{
				StopConn();
				ConnectNotice();// 重连
				WXM_LOG0(_T("心跳失败!"));
			}	
		}
		break;

	case TAG_CLIENT_LOGIN:
	case TAG_CLIENT_LOGOUT:
		{
			// a001#admin#123456#ABCDEFGH
			// termId#账号#密码#校验字符串
			std::vector<CGxxStringW> strArr;
			CGxxStringW strVal = CW2T(str.substr(8, nValLen).c_str());
			strVal.Split(L"#", strArr);
			if(strArr.size() < 4)
			{
				WXM_LOG0(_T("数据参数个数不对"));
				break;
			}

			CGxxStringW strIn = strArr[0] + strArr[1] + strArr[2] + (const wchar_t*)MD5Key;//8d4ac26906c442a18ac7938a87090cb8

			CString strMD5; //CISHellper::MD5String(strIn);
			CEncrypt::CalcMD5((LPCTSTR)strIn, strMD5);
			strMD5 = strMD5.Mid(8, 16);
			strMD5.MakeLower();
			if( 0 != strMD5.CompareNoCase((const TCHAR*)strArr[3]))
			{
				// MD5校验不对
				break;
			}

			CGxxStringW strTermId = strArr[0];
			CGxxStringW strNetId = strArr[1];
			CGxxStringW strPassword = strArr[2];

			if (TAG_CLIENT_LOGIN == nTag)
				m_pWxDelegate->checkin(strNetId, strTermId, strPassword);
			else if (TAG_CLIENT_LOGOUT == nTag)
				m_pWxDelegate->checkout(strNetId, strTermId);
		}
		break;

	case TAG_MESSAGE:
		{
			// termId#账号#金额#赠送金额#支付时间#校验字符串
			std::vector<CGxxStringW> strArr;
			CGxxStringW strVal = CW2T(str.substr(8, nValLen).c_str());
			strVal.Split(L"#", strArr);
			if(strArr.size() < 6)
			{
				WXM_LOG0(_T("数据参数个数不对"));
				break;
			}

			CGxxStringW strIn = strArr[0] + strArr[1] + 
				strArr[2] + strArr[3] + strArr[4] + (const wchar_t*)MD5Key;

			CString strMD5;
			CEncrypt::CalcMD5((LPCTSTR)strIn, strMD5);
			strMD5 = strMD5.Mid(8, 16);
			strMD5.MakeLower();
			if( 0 != strMD5.CompareNoCase((const TCHAR*)strArr[5]))
			{
				// MD5校验不对
				WXM_LOG0(_T("消息校验失败"));
				break;
			}

			WXM_LOG0(_T("显示消息"));
			m_pWxDelegate->showmsg(strArr[0], strArr[1], strArr[2], strArr[3], strArr[4]);
		}
		break;

	case TAG_TAKESEAT:
		{
			//先取整个字串
			//按照#进行分割
			//第一个为开始时间，第二个为结束时间，第三个为终端名，第四个为check码
			//然后再打印出来，保存数据库

			std::wstring sVal = str.substr(8,nValLen);
			std::wstring::size_type nFirst = sVal.find(L'#',0);
			WXM_LOG(_T("nFirst = %d"),nFirst);
			std::wstring::size_type nSecond = sVal.find(L'#',nFirst+1);
			WXM_LOG(_T("nSecond = %d"),nSecond);
			std::wstring::size_type nThird = sVal.find(L'#',nSecond+1);
			WXM_LOG(_T("nThird = %d"),nThird);
			std::wstring::size_type nForth = sVal.find(L'#',nThird+1);
			WXM_LOG(_T("nForth = %d"),nForth);
			if(nFirst == std::wstring::npos || nSecond == std::wstring::npos || nThird == std::wstring::npos || nForth == std::wstring::npos)
			{
				WXM_LOG0(_T("占座信息校验失败"));
				break;
			}
			std::wstring sStartTime = sVal.substr(0,nFirst); 

			std::wstring sEndTime = sVal.substr(nFirst+1,nSecond - nFirst-1);

			std::wstring sTermId = sVal.substr(nSecond+1,nThird - nSecond-1);
			CString strTermId(sTermId.c_str());
			WXM_LOG(_T("received TermId = %s"),strTermId);			

			std::wstring sUserName = sVal.substr(nThird+1,nForth - nThird-1);
			CString strUserName(sUserName.c_str());
			WXM_LOG(_T("received UserName = %s"),strUserName);			

			std::wstring sCheck = sVal.substr(nForth+1,sVal.length()+1-nForth);
			CString strCheckcode(sCheck.c_str());
			WXM_LOG(_T("received Checkcode = %s"),sCheck);

			COleDateTime OleStartTime(_ttol(sStartTime.c_str()));
			COleDateTime OleEndTime(_ttol(sEndTime.c_str()));
			sStartTime = OleStartTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			sEndTime = OleEndTime.Format(_T("%Y-%m-%d %H:%M:%S"));
			CString strStartTime(sStartTime.c_str());
			CString strEndTime(sEndTime.c_str());
			WXM_LOG(_T("StartTime = %s"),strStartTime);
			WXM_LOG(_T("EndTime = %s"),strEndTime);

			//先判断表中是否有对应TermId的记录，没有则插入，有则更新		
			GxxADO myado;
			myado.Connect("DSN=netbar");
			if(!myado.IsConnect())
			{
				WXM_LOG0(_T("Connection to database not establised! failed to open database!"));
				break;
			}
			CString strSQL;	

			bool bExist(FALSE);
			CString strsqlFrom,strsqlWhere;
			strsqlFrom = _T("OccupySeat");
			strsqlWhere.Format(_T("termId = '%s'"),strTermId);
			{
				const wchar_t* lpSQLW = strsqlFrom.GetBuffer();
				const wchar_t* lpSQLW1 = strsqlWhere.GetBuffer();
				int i = WideCharToMultiByte(CP_ACP,0,lpSQLW,-1,NULL,0,NULL,NULL);
				int j = WideCharToMultiByte(CP_ACP,0,lpSQLW1,-1,NULL,0,NULL,NULL);
				char* lpSQLA = new char[i+1];
				char* lpSQLA1 = new char[j+1];
				WideCharToMultiByte(CP_ACP,0,lpSQLW,-1,lpSQLA,i,NULL,NULL);
				WideCharToMultiByte(CP_ACP,0,lpSQLW1,-1,lpSQLA1,j,NULL,NULL);

				if(myado.CountOfRecords(lpSQLA,lpSQLA1) > 0)
				{
				strSQL.Format(_T("update OccupySeat set starttime = '%s',endtime = '%s', name = '%s', checkcode = '%s' where termId = '%s'")
				,strStartTime,strEndTime,strUserName,strCheckcode,strTermId);
				}
				else
				{
				strSQL.Format(L"Insert into OccupySeat (starttime,endtime,name,termId,checkcode) values ('%s','%s','%s','%s','%s')"
				,strStartTime,strEndTime,strUserName,strTermId,strCheckcode);
				}
				delete []lpSQLA;
				delete []lpSQLA1;
				strsqlFrom.ReleaseBuffer();
				strsqlWhere.ReleaseBuffer();
			}

			WXM_LOG(_T("SQL = %s\n"),strSQL);

			const wchar_t* lpSQLW = strSQL.GetBuffer();
			int i = WideCharToMultiByte(CP_ACP, 0, lpSQLW, -1, NULL, 0, NULL, NULL);
			char* lpSQLA = new char[i+1];
			WideCharToMultiByte(CP_ACP, 0, lpSQLW, -1, lpSQLA, i, NULL, NULL);

			WXM_LOG(_T("SQLA = %s\n"),strSQL);
			myado.ExcuteSql(lpSQLA); 

			strSQL.ReleaseBuffer();
			delete []lpSQLA;	

			myado.Close();
		}
		break;
case TAG_FREESEAT:
	{
		//先取整个字串
		//按照#进行分割
		//第一个为开始时间，第二个为结束时间，第三个为终端名，第四个为check码
		//然后再打印出来，保存数据库
		//00150039         1452159904369#1452159904369#CLIENT1#hehe#46dfe613d7348035
		//					登入时间       登出时间     TermId  Username  scheck

		std::wstring sVal = str.substr(8,nValLen);
		std::wstring::size_type nFirst = sVal.find(L'#',0);
		WXM_LOG(_T("nFirst = %d"),nFirst);
		std::wstring::size_type nSecond = sVal.find(L'#',nFirst+1);
		WXM_LOG(_T("nSecond = %d"),nSecond);
		std::wstring::size_type nThird = sVal.find(L'#',nSecond+1);
		WXM_LOG(_T("nThird = %d"),nThird);
		std::wstring::size_type nForth = sVal.find(L'#',nThird+1);
		WXM_LOG(_T("nForth = %d"),nForth);
		if(nFirst == std::wstring::npos || nSecond == std::wstring::npos || nThird == std::wstring::npos || nForth == std::wstring::npos)
		{
			WXM_LOG0(_T("占座信息校验失败"));
			break;
		}
		std::wstring sStartTime = sVal.substr(0,nFirst); 

		std::wstring sEndTime = sVal.substr(nFirst+1,nSecond - nFirst-1);

		std::wstring sTermId = sVal.substr(nSecond+1,nThird - nSecond-1);
		CString strTermId(sTermId.c_str());
		WXM_LOG(_T("received TermId = %s"),strTermId);			

		std::wstring sUserName = sVal.substr(nThird+1,nForth - nThird-1);
		CString strUserName(sUserName.c_str());
		WXM_LOG(_T("received UserName = %s"),strUserName);			

		std::wstring sCheck = sVal.substr(nForth+1,sVal.length()+1-nForth);
		CString strCheckcode(sCheck.c_str());
		WXM_LOG(_T("received Checkcode = %s"),sCheck);

		COleDateTime OleStartTime(_ttol(sStartTime.c_str()));
		COleDateTime OleEndTime(_ttol(sEndTime.c_str()));
		sStartTime = OleStartTime.Format(_T("%Y-%m-%d %H:%M:%S"));
		sEndTime = OleEndTime.Format(_T("%Y-%m-%d %H:%M:%S"));
		CString strStartTime(sStartTime.c_str());
		CString strEndTime(sEndTime.c_str());
		WXM_LOG(_T("StartTime = %s"),strStartTime);
		WXM_LOG(_T("EndTime = %s"),strEndTime);

		GxxADO myado;
		myado.Connect("DSN=netbar");
		if(!myado.IsConnect())
		{
			WXM_LOG0(_T("Connection to database not establised! failed to open database!"));
			break;
		}
		CString strSQL;	

		//表中查找TermId,Name一致的记录,如果存在则删除	
		bool bExist(FALSE);
		CString strsqlFrom,strsqlWhere;
		strsqlFrom = _T("OccupySeat");
		strsqlWhere.Format(_T("termId = '%s' and name = '%s'"),strTermId,strUserName);
		{
			const wchar_t* lpSQLW = strsqlFrom.GetBuffer();
			const wchar_t* lpSQLW1 = strsqlWhere.GetBuffer();
			int i = WideCharToMultiByte(CP_ACP,0,lpSQLW,-1,NULL,0,NULL,NULL);
			int j = WideCharToMultiByte(CP_ACP,0,lpSQLW1,-1,NULL,0,NULL,NULL);
			char* lpSQLA = new char[i+1];
			char* lpSQLA1 = new char[j+1];
			WideCharToMultiByte(CP_ACP,0,lpSQLW,-1,lpSQLA,i,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,lpSQLW1,-1,lpSQLA1,j,NULL,NULL);

			WXM_LOG(_T("FreeSeat: select * from %s where %s"),strsqlFrom,strsqlWhere);

			if(myado.CountOfRecords(lpSQLA,lpSQLA1) > 0)
			{
				strSQL.Format(_T("delete from occupyseat where termId = '%s' and name = '%s'"),strTermId,strUserName);
				WXM_LOG(_T("FreeSeat Delete: %s"),strSQL);
			}
			else
			{
				WXM_LOG(_T("FreeSeat: No matched record found!"));
			}
			delete []lpSQLA;
			delete []lpSQLA1;
			strsqlFrom.ReleaseBuffer();
			strsqlWhere.ReleaseBuffer();
		}

		//WXM_LOG(_T("SQL = %s\n"),strSQL);

		const wchar_t* lpSQLW = strSQL.GetBuffer();
		int i = WideCharToMultiByte(CP_ACP, 0, lpSQLW, -1, NULL, 0, NULL, NULL);
		char* lpSQLA = new char[i+1];
		WideCharToMultiByte(CP_ACP, 0, lpSQLW, -1, lpSQLA, i, NULL, NULL);

		//WXM_LOG(_T("SQLA = %s\n"),strSQL);
		myado.ExcuteSql(lpSQLA); 

		strSQL.ReleaseBuffer();
		delete []lpSQLA;	

		myado.Close();
	}
	break;

	default:
		// 异常tag
		WXM_LOG0(_T("收到不能被识别的消息"));
		break;
	}
}

bool CConnNotice::didServerDisconnected( CG2XSocketClient* pSender )
{
	WXM_LOG0(_T("中间件连接断开，稍后尝试重新连接..."));
	// 利用定时器重新连接 ConnectNotice();
	m_nTimerIDReConn = SetTimer(NULL, 0, 1 * 60 * 1000, TimerProc);

	return false;
}