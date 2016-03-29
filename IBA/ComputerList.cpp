
#include "stdafx.h"
#include "IBA.h"
#include ".\computerlist.h"
//#include ".\json\gxx_base.h"
#include ".\Servlet\WinHttpRequest.h"
#include "NetBarConfig.h"
#include "IBAHelpper.h"
#include ".\Servlet\MachReportMethod.h"
#include ".\Socket\LocalServer.h"
#include ".\dal\IDTypeInfo.h"

using namespace NS_SERVLET;

#include <iphlpapi.h> 

#pragma comment(lib, "IPHLPAPI.lib") 

CComputerList::CComputerList(void)
{
	m_pThread = NULL;
	m_Devices.RemoveAll();
	m_nSerialNo = 0;
}

CComputerList::~CComputerList(void)
{
	Clear();
}
//生成终端号和IP组成的索引
/*
* 按照m_ComputerArray填充m_ComputerKeyMap和m_SortIndexArray 
* 清空m_ComputerKeyMap和m_SortIndexArray
* m_ComputerKeyMap添加键值对(对应于m_ComputerArray所有项的)，键值为strTermID+strComputerIP，值为其在m_ComputerArray中的索引
* m_SortIndexArray添加索引值(对应于m_ComputerArray所有项的 ) -- 其实就是从1开始的序列*/
void CComputerList::MakeComputerKeyMap()
{
	m_ComputerKeyMap.clear();
	m_SortIndexArray.RemoveAll();

	for (INT_PTR i = 0; i < m_ComputerArray.GetCount(); i++)
	{
		CComputerInfo& ComputerInfo = m_ComputerArray.GetAt(i);

		//更新关键字数组
		CString strKey;
		strKey.Format(_T("%s+%s"), ComputerInfo.GetTerminalID(), ComputerInfo.GetComputerIP());
		m_ComputerKeyMap.insert(std::make_pair(strKey, i));

		//更新排序索引数组

		BOOL bInsert = FALSE;

		for (INT_PTR j = 0; j < m_SortIndexArray.GetCount(); j++)
		{
			CComputerInfo& ComputerInfo2 = m_ComputerArray.GetAt(m_SortIndexArray.GetAt(j));

			if (ComputerInfo2.GetTerminalID().CompareNoCase(ComputerInfo.GetTerminalID()) > 0)
			{
				m_SortIndexArray.InsertAt(j, i);
				bInsert = TRUE;
				break;
			}
		}

		if (!bInsert)
		{
			m_SortIndexArray.Add(i);
		}
	}
}
//添加计算机监控信息
/*************************************************
Function:       //AddComputer
Description:    //添加computerInfo.若已存在相同TermId+IP对象,则仅更新,否则添加.更新m_UpdateIndexArray及m_ComputerKeyMap
Input:          //computer - 待添加computerInfo
Return:         //仅做了更新,返回false，做添加，返回true
*************************************************/
BOOL CComputerList::AddComputer(CComputerInfo& computer)
{
	INT_PTR nIndex = Find(computer.GetTerminalID(), computer.GetComputerIP());

	//若终端号和IP已经存在,重置computerInfo
	if (nIndex >= 0)
	{
		UpdateComputer(nIndex, computer);
		return FALSE;
	}

	nIndex = m_ComputerArray.Add(computer);
	
	m_UpdateIndexArray.Add(nIndex);

	//为了提高效率，不想每次都做更新，等第一手动调用以后才做自动调用???
	if (m_ComputerKeyMap.size() > 0)
	{
		MakeComputerKeyMap();
	}

	return TRUE;
}
//更新特定计算机监控信息
/*************************************************
Function:       //UpdateComputer
Description:    //重置nIndex位置的ComputerInfo,设m_UpdateState为EUS_CHANGE,PCClassID保持不变.添加nIndex到m_UpdateIndexArray.
Input:          //nIndex - 需重置的成员索引
                //computer - 重置来源
Return:         //索引无效返回false.
*************************************************/
BOOL CComputerList::UpdateComputer(INT_PTR nIndex, CComputerInfo& computer)
{
	if (IsInvaildIndex(nIndex))
	{
		return FALSE;
	}
	
	computer.SetUpdateState(CComputerInfo::EUS_CHANGE);
	computer.SetPCClassID(m_ComputerArray.GetAt(nIndex).GetPCClassID()); //PCClassID保持不变
	m_ComputerArray.SetAt(nIndex, computer); //重置nIndex位置ComputerInfo
		
	m_UpdateIndexArray.Add(nIndex);

	return TRUE;
}
/*************************************************
Function:       //Find
Description:    //依TermId+IP查找终端,返回索引
Input:          //CString strTermID - 终端号
				//CString strComputerIP - 终端IP
Return:         //对应终端在m_ComputerKeyMap中索引
other:			//在m_ComputerKeyMap中查找
*************************************************/
INT_PTR CComputerList::Find(CString strTermID, CString strComputerIP)
{
	INT_PTR nRet = -1;

	CString strKey;

	strKey.Format(_T("%s+%s"), strTermID, strComputerIP);

	std::map<CString, INT_PTR>::iterator iter = m_ComputerKeyMap.find(strKey); 
	
	if (iter != m_ComputerKeyMap.end()) 
	{
		nRet = iter->second;
	} 

	return nRet;
}
/*************************************************
Function:       //Find
Description:    //依MemberID查找终端,返回索引
Input:          //UINT nMemberID - 查找所依据的用户号
Return:         //对应终端在m_ComputerArray中索引
*************************************************/
INT_PTR CComputerList::Find(UINT nMemberID)
{
	for (INT_PTR i = 0; i < m_ComputerArray.GetCount(); i++)
	{
		if (m_ComputerArray.GetAt(i).GetMemberID() == nMemberID)
		{
			return i;
		}
	}

	return -1;
}
//查找信息。0//终端号查找；1//上网账号查找；2//IP查找
INT_PTR CComputerList::LookFor(UINT nKeyType, LPCTSTR lpszKeyValue)
{
	for (INT_PTR i = 0; i < m_SortIndexArray.GetCount(); i++)
	{
		CComputerInfo& ComputerInfo = m_ComputerArray.GetAt(m_SortIndexArray.GetAt(i));
		
		switch (nKeyType)
		{
		case 0: //终端号查找
			//实际上是一种模糊匹配，只要字串中有此字段的首个子串，就认为是。
			if (ComputerInfo.GetTerminalID().MakeUpper().Find(CString(lpszKeyValue).MakeUpper()) >= 0)
			{
				return m_SortIndexArray.GetAt(i);
			}

			break;

		case 1: //上网账号查找

			if (ComputerInfo.GetNetId().MakeUpper().Find(CString(lpszKeyValue).MakeUpper()) >= 0)
			{
				return m_SortIndexArray.GetAt(i);
			}

			break;

		case 2:  //IP查找

			CString strTmp(ComputerInfo.GetComputerIP());
			
			if (strTmp.GetLength() > lstrlen(lpszKeyValue))
			{
				strTmp = ComputerInfo.GetComputerIP().Right(lstrlen(lpszKeyValue));
			}

			if (strTmp.CompareNoCase(lpszKeyValue) == 0)
			{
				return m_SortIndexArray.GetAt(i);
			}

			break;
		
		}
	}

	return -1;
}
//清除
void CComputerList::Clear()
{
	m_ComputerArray.RemoveAll();
	m_UpdateIndexArray.RemoveAll();
	m_SortIndexArray.RemoveAll();
	m_ComputerKeyMap.clear();
}
//计算机数目
INT_PTR CComputerList::GetCount()
{
	return m_ComputerArray.GetCount();
}
//得到给定标号的终端信息
CComputerInfo& CComputerList::GetComputer(INT_PTR nIndex)
{
	IsInvaildIndex(nIndex);

	return m_ComputerArray.GetAt(nIndex);
}

BOOL CComputerList::GetComputerInfo(UINT nMemberID, CComputerInfo& cInfo)
{
	INT_PTR iP = Find(nMemberID);
	if ( iP < 0 )
		return FALSE;

	cInfo = GetComputer(iP);
	return TRUE;
}

BOOL CComputerList::GetComputerInfo(CString strTermID, CComputerInfo& outInfo)
{
	for (int i=0; i < m_ComputerArray.GetCount(); i++)
	{
		CComputerInfo & info = m_ComputerArray.GetAt(i);
		if (info.GetTerminalID() == strTermID)
		{
			outInfo = info;
			return TRUE;
		}
	}
	return FALSE;
}
//当前更新的第一个记录
INT_PTR CComputerList::GetUpdateComputer()
{
	INT_PTR nIndex = -1;

	if (!m_UpdateIndexArray.IsEmpty())
	{
		nIndex = m_UpdateIndexArray.GetAt(0);
		m_UpdateIndexArray.RemoveAt(0);
	}
	
	return nIndex;
}
//验证取得标号的有效性
BOOL CComputerList::IsInvaildIndex(INT_PTR nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < m_ComputerArray.GetCount());

	if (nIndex < 0 || nIndex >= m_ComputerArray.GetCount())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CComputerList::GetStatInfo(CStatInfo & StatInfo)
{
	ASSERT(StatInfo.m_nOnline==0);

	StatInfo.m_nAllCount = m_ComputerArray.GetCount();

	//2015-0824 liyajun 添加网吧终端区域的信息
	//添加所有区域PCClassID
	StatInfo.m_PCClassInfoArray.RemoveAll();
	for(INT_PTR i=0; i < CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetCount();++i)
	{
		PCClassInfo ClassInfo;
		ClassInfo.nAvailableDevices = 0;
		ClassInfo.nOccupiedDevices = 0;
		ClassInfo.nPCClassID = CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetID();
		ClassInfo.nTotalDevices = 0;
		ClassInfo.strPCClassName = CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetName();
		StatInfo.m_PCClassInfoArray.Add(ClassInfo);		
	}

	////Temp
	////std::map<CString,UINT> PCClassMap =  CLocalServer::GetInstance()->GetPCClassMap();
	std::map<CString,UINT> PCClassMap =  CNetBarConfig::GetInstance()->m_TermIDPCClassMap;
	//for(std::map<CString,UINT>::iterator iter = PCClassMap.begin(); iter != PCClassMap.end(); ++iter)
	//{
	//	TRACE(_T("CommandProcess::m_PCClassMap::first:%s,second:%d\n"),iter->first,iter->second);
	//}

	//统计各区域的终端数量
	for(INT_PTR i = 0; i < StatInfo.m_PCClassInfoArray.GetCount(); ++i)
	{
		UINT nCount = 0;
		for(std::map<CString,UINT>::iterator iter = PCClassMap.begin(); iter != PCClassMap.end(); ++iter)
		{
			if(StatInfo.m_PCClassInfoArray.GetAt(i).nPCClassID == iter->second)
			{
				nCount++;
			}
		}
		StatInfo.m_PCClassInfoArray.GetAt(i).nTotalDevices = nCount;
		StatInfo.m_PCClassInfoArray.GetAt(i).nAvailableDevices = nCount;
		StatInfo.m_PCClassInfoArray.GetAt(i).strPCClassName = CNetBarConfig::GetInstance()->GetPCClassName(StatInfo.m_PCClassInfoArray.GetAt(i).nPCClassID);
	}


	for (INT_PTR i = 0; i < m_ComputerArray.GetCount(); i++)
	{
		if (m_ComputerArray.GetAt(i).HasUser())//只统计生日，黑户，VIP，挂机，余额不多，在线状态
		{
			//2015-0824 liyajun 统计各区域的占用的终端和未占用的终端数
			for(INT_PTR j = 0; j< StatInfo.m_PCClassInfoArray.GetCount(); ++j)
			{
				if(m_ComputerArray.GetAt(i).GetPCClassID() == StatInfo.m_PCClassInfoArray.GetAt(j).nPCClassID)
				{
					++StatInfo.m_PCClassInfoArray.GetAt(j).nOccupiedDevices;
					--StatInfo.m_PCClassInfoArray.GetAt(j).nAvailableDevices;
				}
			}
			StatInfo.m_nOnline++;
			//////////////包时付款方式//////////////
			switch(m_ComputerArray.GetAt(i).GetPayType())
			{
			case CActiveMember::EPayType_General:
				{
					StatInfo.m_nCommonPayCount++;
					break;
				}
			case CActiveMember::EPayType_Capped:
			case CActiveMember::EPayType_Withholding:
				{
					StatInfo.m_nBundTimePayCount++;
					break;
				}
			}
			//////////////////////统计普通卡用户及会员用户///////////////////
			CString strUserClassName = m_ComputerArray.GetAt(i).GetMemberType();
			for ( INT k = 0; k < CNetBarConfig::GetInstance()->m_UserClassArray.GetCount(); k++)
			{
				CString strTmp = CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(k).GetClassName();
				//利用级别名字来识别不同的级别
				if ( strTmp == strUserClassName )
				{	//判断是否为会员
					if (CNetBarConfig::GetInstance()->m_UserClassArray.GetAt(k).GetIsMember())
					{
						StatInfo.m_nMemberUserCount++;
					} 
					else
					{
						StatInfo.m_nCommonUserCount++;
					}

					break;
				}
			}
			////////////////////性别统计///////////////////
			if (m_ComputerArray.GetAt(i).GetSex() == 0)
			{
				StatInfo.m_ManCount++;
			}
			else
			{
				StatInfo.m_WomanCount++;
			}
		}
		else if (m_ComputerArray.GetAt(i).GetComputerStatus() == CComputerInfo::ECS_LOCKED)
		{
			StatInfo.m_nLock++;
		}
		else if (m_ComputerArray.GetAt(i).GetComputerStatus() == CComputerInfo::ECS_UNLOCK)
		{
			StatInfo.m_nUnLock++;
		}
		else if (m_ComputerArray.GetAt(i).GetComputerStatus() == CComputerInfo::ECS_THIEF)
		{
			StatInfo.m_ThiefCount++;
		}
	}

	return TRUE;
}
//更新所有计算机的状态
void CComputerList::UpdateAllComputerStatus()
{
	for (INT_PTR i = 0; i < m_ComputerArray.GetCount(); i++)
	{
		m_ComputerArray.GetAt(i).CheckStatus();
	}
}

void CComputerList::ClearComputerUserInfo(UINT nMemberId)
{
	// 2014-1-17 - qsc
	int nIndex = Find(nMemberId);

	if (nIndex >= 0) //远程下机
	{
		CComputerInfo& computer = GetComputer(nIndex);
		computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
		computer.ClearUserInfo();
		UpdateComputer(nIndex, computer);

		//CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
	}
}

std::string EncodeBase64( const char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	std::string strEncode;
	unsigned char Tmp[4]={0};
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}

UINT CComputerList::WorkThread(LPVOID lpParam)
{
	CComputerList* pThis = (CComputerList*)lpParam;
	if (NULL == pThis) return 0;

	UINT uRet = 0;

	const DWORD dwInterval = 1000*3600;
	do 
	{
		//try
		//{
		while (TRUE) 
		{
			//CIBALog::GetInstance()->Write(_T("CComputerList::WorkThread, start report..."));
			uRet = pThis->Run();

			// In order to run per 1 hour
			Sleep(dwInterval);
		}
		//}
		//catch(...) 
		//{
		//	CIBALog::GetInstance()->Write(_T("CComputerList::WorkThread, report thread thow a exception."));
		//}
	}while (TRUE);

	return uRet; 
}

UINT CComputerList::Run()
{
	//2015-0916 liyajun 不进行上报
	return 1;
	//// 重置成员
	//m_Devices.RemoveAll();

	//	// 获取网段
	//CString strLocalIp = CNetBarConfig::GetInstance()->GetLocalServerIP();
	//INT nPos = strLocalIp.ReverseFind(_T('.'));
	//if (nPos < 5) return 0;
	//CString strLocalSeg = strLocalIp.Left(nPos+1);

	//CArray<CString, CString&> vtSegments;
	////GetNetSegments(vtSegments);
	//INT nCount = vtSegments.GetCount();
	//BOOL bLackLocal = TRUE;

	//if (bLackLocal)
	//	vtSegments.Add(strLocalSeg);


	//// 获取序列号
	//m_nSerialNo = CIBAConfig::GetInstance()->GetLocalSerialNo();

	//// 开始扫描
	//nCount = vtSegments.GetCount();
	//for (INT nSeg=0; nSeg<nCount; ++nSeg)
	//{
	//	CString strCheckIp;
	//	for (int i=0; i<255; ++i)
	//	{
	//		strCheckIp.Format(_T("%s%d"), vtSegments.GetAt(nSeg).GetBuffer(), i);
	//		if (0 == strCheckIp.CompareNoCase(strLocalIp))
	//			continue;

	//		EmptyARPBuffer(strCheckIp);

	//		SendARPDgram(strCheckIp);
	//	}
	//}

	//// 比较看是否有已存在的
	//INT_PTR nPCCount = m_ComputerArray.GetCount();
	//INT_PTR nDevices = m_Devices.GetCount();

	//INT_PTR nUnknown = 0;
	//INT_PTR nRepeat = 0;
	//for (INT_PTR i=0; i<nPCCount; ++i)
	//{
	//	BOOL bIShutdown = TRUE;
	//	CComputerInfo& info = m_ComputerArray.GetAt(i);
	//	CStringA strIp = CT2A(info.GetComputerIP().GetBuffer());
	//	for (INT_PTR j=0; j<nDevices; ++j)
	//	{
	//		IPMAC& ipcmac = m_Devices.GetAt(j);
	//		if (0 == strIp.CompareNoCase(ipcmac.szIp))
	//		{
	//			// ip 相同
	//			ipcmac.bDerelict = TRUE;

	//			CString strMac = CA2T(ipcmac.szMac);
	//			info.SetMac(strMac);

	//			info.SetCheckTime(ipcmac.nTime);

	//			nRepeat += 1;

	//			// 不能查到的算已关机
	//			bIShutdown = FALSE;
	//		}
	//		else
	//			bIShutdown = TRUE;
	//	}

	//	info.SetIsShutdown(bIShutdown);

	//	if (!bIShutdown && info.GetComputerStatus() == CComputerInfo::ECS_THIEF)
	//	{
	//		nUnknown += 1;
	//	}
	//}

	//// 发送
	//nUnknown = nUnknown + nDevices - nRepeat;
	//if (ReportPCInfo(nUnknown))
	//	return 1;

	//return 0;
}

void CComputerList::StartReportPCInfo()
{
#ifndef _DEBUG
	// 创建工作线程
	m_pThread = AfxBeginThread(WorkThread, this);
	if (NULL == m_pThread)
		CIBALog::GetInstance()->Write(_T("CComputerList::StartReportPCInfo, AfxBeginThread failed."));
#endif
}

void CComputerList::EmptyARPBuffer(CString strIP)
{
	CStringA strIPA = CT2A(strIP.GetBuffer());
	strIP.ReleaseBuffer();

	// 删除指定IP的本机ARP缓存
	DWORD dwSize = 0;
	CHAR szTem = 0;
	DWORD dwDelIP = inet_addr(strIPA.GetBuffer());
	strIPA.ReleaseBuffer();
	if (ERROR_INSUFFICIENT_BUFFER == GetIpNetTable((PMIB_IPNETTABLE)&szTem, &dwSize,TRUE))
	{
		CHAR* pBuf = new CHAR[dwSize];	
		ZeroMemory(pBuf, dwSize);
		//try
		//{
		if (ERROR_SUCCESS == GetIpNetTable((PMIB_IPNETTABLE)pBuf, &dwSize, TRUE))
		{
			PMIB_IPNETTABLE pNetTable = (PMIB_IPNETTABLE)pBuf;
			for (UINT i = 0; i < pNetTable->dwNumEntries; i++)
			{
				if (dwDelIP == pNetTable->table[i].dwAddr)
				{
					MIB_IPNETROW EnterAddrRow;
					EnterAddrRow.dwAddr = pNetTable->table[i].dwAddr;
					EnterAddrRow.dwIndex = pNetTable->table[i].dwIndex;
					DeleteIpNetEntry(&EnterAddrRow);
				}
			}
		}
		//}
		//catch (...)
		//{
		//	CIBALog::GetInstance()->WriteFormat(_T("CComputerList::EmptyARPBuffer exception, IP:%s, Error:%d."),
		//		strIP, GetLastError());
		//}
		delete[] pBuf;
	}
}

BOOL CComputerList::SendARPDgram(CString strIP)
{
	CStringA strIPA = CT2A(strIP.GetBuffer());
	strIP.ReleaseBuffer();

	ULONG szMac[2] = {0};  
	ULONG uLen = 6;  

	if (NO_ERROR == SendARP(inet_addr(strIPA.GetBuffer()), 0, szMac, &uLen))
	{
		strIPA.ReleaseBuffer();

		IPMAC ipmac;
		
		strcpy(ipmac.szIp, strIPA.GetBuffer());  

		strIPA.ReleaseBuffer();

		sprintf(ipmac.szMac,"%02x-%02x-%02x-%02x-%02x-%02x",
			(unsigned char)szMac[0],
			(unsigned char)szMac[1],
			(unsigned char)szMac[2],
			(unsigned char)szMac[3],
			(unsigned char)szMac[4],
			(unsigned char)szMac[5]);

		ipmac.nTime = time(NULL);

		m_Devices.Add(ipmac);

		return TRUE;
	}

	return FALSE;
}

void CComputerList::GetNetSegments(CArray<CString, CString&>& vtSegments)
{
	vtSegments.RemoveAll();

	DWORD AdapterInfoSize = 0;   
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(0, &AdapterInfoSize)) 
	{
		PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[AdapterInfoSize];
		if (ERROR_SUCCESS == GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) 
		{
			size_t nNumber = -1;
			while (pAdapterInfo)
			{
				IP_ADDR_STRING* pIPAddrString = &pAdapterInfo->IpAddressList;
				while (pIPAddrString)
				{
					CStringA strIpA = pIPAddrString->IpAddress.String;
					CString strIp = CA2T(strIpA.GetBuffer());
					strIpA.ReleaseBuffer();

					BOOL bNeedAdd = TRUE;
					do 
					{
						nNumber = strIp.ReverseFind(_T('.'));
						if (nNumber < 5)
						{
							bNeedAdd = FALSE;
							break;
						}

						strIp = strIp.Left(nNumber+1);
						nNumber = vtSegments.GetCount();

						for (size_t i=0; i<nNumber; ++i)
						{
							if (0 == strIp.Compare(vtSegments.GetAt(i).GetBuffer()))
							{
								vtSegments.GetAt(i).ReleaseBuffer();
								bNeedAdd = FALSE;
								break;
							}
							vtSegments.GetAt(i).ReleaseBuffer();
						}
					} while (0);


					if (bNeedAdd)
						vtSegments.Add(strIp);

					pIPAddrString = pIPAddrString->Next;
				}

				pAdapterInfo = pAdapterInfo->Next;
			}
		}

		delete[] pAdapterInfo;
	}
}

CStringA CComputerList::GetHostName(CStringA strIp)
{
	CStringA strResult("");

	//try
	//{
	struct in_addr addr = { 0 };
	addr.s_addr = inet_addr(strIp.GetBuffer());
	strIp.ReleaseBuffer();
	HOSTENT* lpHostEnt = gethostbyaddr((char*)&addr, 4, AF_INET);
	if (lpHostEnt != NULL)
	{
		strResult = lpHostEnt->h_name;
	}
	else
	{
		char hostname[NI_MAXHOST] = {0};
		char servInfo[NI_MAXSERV] = {0};
		u_short port = 27015;

		// Set up sockaddr_in structure which is passed
		// to the getnameinfo function
		struct sockaddr_in saGNI;
		saGNI.sin_family = AF_INET;
		saGNI.sin_addr.s_addr = inet_addr(strIp.GetBuffer());
		strIp.ReleaseBuffer();
		saGNI.sin_port = htons(port);

		DWORD dwRetval = getnameinfo((struct sockaddr *)&saGNI,
			sizeof(struct sockaddr),
			hostname,
			NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

		if (dwRetval != 0) 
		{
			CIBALog::GetInstance()->WriteFormat(_T("CComputerList::GetHostName error, IP:%s, Error:%d."),
				CA2T(strIp.GetBuffer()), GetLastError());
			strIp.ReleaseBuffer();
		}
		else
		{
			strResult = hostname;
		}
	}
	//}
	//catch (...)
	//{
	//	CIBALog::GetInstance()->WriteFormat(_T("CComputerList::GetHostName exception, IP:%s, Error:%d."),
	//		CA2T(strIp.GetBuffer()), GetLastError());
	//	strIp.ReleaseBuffer();
	//}

	return strResult;
}

BOOL CComputerList::ReportPCInfo(INT_PTR nUnknownDevice)
{
	CComputerInfo* pInfo = NULL;
	CString strData(_T("")), strTemp(_T(""));
	INT_PTR nPCCount = m_ComputerArray.GetCount();
	INT_PTR nIndex = 0;
	for (INT_PTR i=0; i<nPCCount; ++i)
	{
		pInfo = &m_ComputerArray.GetAt(i);
		if (NULL == pInfo) continue;

		// 关机的不上报
		if (pInfo->GetIsShutdown()) continue;
		// 不是可疑电脑的不上报
		if (pInfo->GetComputerStatus() != CComputerInfo::ECS_THIEF) continue;

		nIndex += 1;

		// netbarid
		strTemp.Format(_T("%05d"), CNetBarConfig::GetInstance()->GetNetBarId());
		strData += strTemp;

		// netbarcaption
		strTemp = CNetBarConfig::GetInstance()->GetNetBarName();
		CIBAHelpper::FillString(strTemp, 50, _T(' '));
		strData += strTemp;

		// unknowncount
		strTemp.Format(_T("%06d"), nUnknownDevice);
		strData += strTemp;

		// machname
		strTemp = pInfo->GetTerminalID();
		CIBAHelpper::FillString(strTemp, 50, _T(' '));
		strData += strTemp;

		// machip
		strTemp = pInfo->GetComputerIP();
		CIBAHelpper::FillString(strTemp, 15, _T(' '));
		strData += strTemp;

		// machmac
		strTemp = pInfo->GetMac();
		CIBAHelpper::FillString(strTemp, 17, _T(' '));
		strData += strTemp;

		// machtype
		strTemp = _T("PC");
		CIBAHelpper::FillString(strTemp, 10, _T(' '));
		strData += strTemp;

		// onlineTime
		strTemp.Format(_T("%014ld"), pInfo->GetCheckTime());
		strData += strTemp;
		
		// offlinetime
		strTemp.Format(_T("%014ld"), 0);
		strData += strTemp;

		if (nIndex > 0 && 0 == nIndex % 5)
		{
			nIndex -= 5;
			
			strTemp.Format(_T("%03d%s"), 5, strData);
			strData = strTemp;

			DoReport(strData);
		}
	}

	INT_PTR nExists = nIndex;
	LPIPMAC pIpMac = NULL;
	INT_PTR nDeviceCount = m_Devices.GetCount();
	for (INT_PTR i=0; i<nDeviceCount; ++i)
	{
		pIpMac = &m_Devices.GetAt(i);
		if (NULL == pIpMac) continue;

		// 已经上报过的不再次上报
		if (pIpMac->bDerelict) continue;

		nIndex += 1;

		// netbarid
		strTemp.Format(_T("%05d"), CNetBarConfig::GetInstance()->GetNetBarId());
		strData += strTemp;

		// netbarcaption
		strTemp = CNetBarConfig::GetInstance()->GetNetBarName();
		CIBAHelpper::FillString(strTemp, 50, _T(' '));
		strData += strTemp;

		// unknowncount
		strTemp.Format(_T("%06d"), nUnknownDevice);
		strData += strTemp;

		// machname
		strTemp = CA2T(GetHostName(CStringA(pIpMac->szIp)).GetBuffer());
		GetHostName(CStringA(pIpMac->szIp)).ReleaseBuffer();
		CIBAHelpper::FillString(strTemp, 50, _T(' '));
		strData += strTemp;

		// machip
		strTemp = CA2T(pIpMac->szIp);
		CIBAHelpper::FillString(strTemp, 15, _T(' '));
		strData += strTemp;

		// machmac
		strTemp = CA2T(pIpMac->szMac);
		CIBAHelpper::FillString(strTemp, 17, _T(' '));
		strData += strTemp;

		// machtype
		strTemp = _T("PC");
		CIBAHelpper::FillString(strTemp, 10, _T(' '));
		strData += strTemp;

		// onlineTime
		strTemp.Format(_T("%014ld"), pIpMac->nTime);
		strData += strTemp;

		// offlinetime
		strTemp.Format(_T("%014ld"), 0);
		strData += strTemp;

		if (nIndex > 0 && 0 == nIndex % 5)
		{
			nIndex -= 5;

			strTemp.Format(_T("%03d%s"), 5, strData);
			strData = strTemp;

			DoReport(strData);
		}
	}

	if (nIndex > 0)
	{
		strTemp.Format(_T("%03d%s"), nIndex, strData);
		strData = strTemp;

		DoReport(strData);
	}

	CIBALog::GetInstance()->Write(_T("CComputerList::ReportPCInfo finished."));

	return TRUE;
}

BOOL CComputerList::DoReport(CString& strData)
{
	//yajun 20150721 试验状态跳闪问题是否由此接口导致，直接返回true
	/*
	CMachReportMethod MachReport;
	MachReport.SetData(strData);
	
	BOOL bRet = MachReport.DoReport();
	strData.Empty();

	if (!bRet)
	{
		CIBALog::GetInstance()->Write(_T("CComputerList::DoReport, communication with center failed.*****"));
		return FALSE;
	}

	if (0 == MachReport.GetStatusCode())
	{
		CIBALog::GetInstance()->WriteFormat(_T("CComputerList::DoReport, AddNum:%d, MergeNum:%d, ErrorNum:%d, DelNum:%d, OperateTime:%s"),
			MachReport.GetAddNum(), MachReport.GetMergeNum(), MachReport.GetErrorNum(), MachReport.GetDelNum(), MachReport.GetOperateTime().GetBuffer());
	}
	else
	{
		CIBALog::GetInstance()->WriteFormat(_T("CComputerList::DoReport, Status:%d.*****"), MachReport.GetStatusCode());
	}*/

	return TRUE;
}

//BOOL CComputerList::SendHttpData(CStringA strData, int nPCCount, int nRetryTimes /* = 5 */)
//{
//	CString strRequestIp = _T("");
//#if _DEBUG
//	strRequestIp = _T("192.168.10.253");
//#else
//	strRequestIp = CNetBarConfig::GetInstance()->GetMainCenterIP();
//#endif
//	SHORT nRequestPort = 8080;
//	CString strRequestPath = _T("/netbar/servlet/CommonRequest?command=NonSysDetail&param=");
//	CString strRequestData = CA2T(EncodeBase64(strData.GetBuffer(), strData.GetLength()).c_str());
//
//	CString strUrl(_T(""));
//	strUrl.Format(_T("http://%s:%d%s%s"),
//		strRequestIp.GetBuffer(),
//		nRequestPort,
//		strRequestPath.GetBuffer(),
//		strRequestData.GetBuffer());
//
//	static const LONG CodePage_GB2312 = 936;
//	static const LONG WinHttpRequestOption_URLCodePage = 2;
//
//	int nTempRetryTimes = nRetryTimes;
//	BOOL bOk = TRUE;
//	do 
//	{
//		CWinHttpRequest HttpRequest(FALSE); 
//		try
//		{
//			HttpRequest.Create();
//			HttpRequest.SetOption(WinHttpRequestOption_URLCodePage, COleVariant(CodePage_GB2312));
//			HttpRequest.SetTimeouts(5000, 5000, 1000 * 60, 1000 * 60);
//			HttpRequest.Open(_T("POST"), strUrl.GetBuffer());
//			HttpRequest.Send();
//
//			if (200 == HttpRequest.GetStatus())
//			{
//				CString strResp = HttpRequest.GetResponseText();
//				GxxDictionaryPtr respData = GxxDictionary::create();
//				respData->initWithJsonText(CT2A(strResp.GetBuffer()));
//
//				if (respData->isKeyExist("addNum") && 
//					respData->isKeyExist("mergeNum") &&
//					respData->isKeyExist("errorNum"))
//				{
//					INT_PTR nSucNum = respData->valueForKey("addNum")->intValue();
//					INT_PTR nMergeNum = respData->valueForKey("mergeNum")->intValue();
//					INT_PTR nErrorNum = respData->valueForKey("errorNum")->intValue();
//
//					CIBALog::GetInstance()->WriteFormat(_T("CComputerList::SendHttpData, count:%d, addNum:%d, mergeNum:%d, errorNum:%d"),
//						nPCCount, nSucNum, nMergeNum, nErrorNum);
//
//					if (nSucNum + nMergeNum == nPCCount && 0 == nErrorNum)
//						return TRUE;
//				}
//				else
//				{
//					CIBALog::GetInstance()->WriteFormat(_T("CComputerList::SendHttpData, response data:%s"), strResp.GetBuffer());
//				}
//
//				bOk = TRUE;
//			}
//			else
//			{
//				if (bOk)
//				{
//					CIBALog::GetInstance()->WriteFormat(_T("CComputerList::SendHttpData failed, status:%d."), HttpRequest.GetStatus());
//					bOk = FALSE;
//				}
//				else
//				{
//					CIBALog::GetInstance()->WriteFormat(_T("CComputerList::SendHttpData retry report failed, status:%d."), HttpRequest.GetStatus());
//				}
//			}
//		}
//		catch (...)
//		{
//			if (bOk)
//			{
//				CIBALog::GetInstance()->Write(_T("CComputerList::SendHttpData throw exception!"));
//				bOk = FALSE;
//			}
//			else
//			{
//				CIBALog::GetInstance()->Write(_T("CComputerList::SendHttpData retry report throw exception!"));
//			}
//		}
//	} while (--nTempRetryTimes > 0 && !bOk);
//	
//
//	return FALSE;
//}