
#include "stdafx.h"
#include "IBA.h"
#include ".\computerlist.h"

CComputerList::CComputerList(void)
{
}

CComputerList::~CComputerList(void)
{
	Clear();
}
//生成终端号和IP组成的索引
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
BOOL CComputerList::AddComputer(CComputerInfo& computer)
{
	INT_PTR nIndex = Find(computer.GetTerminalID(), computer.GetComputerIP());

	if (nIndex >= 0)
	{
		UpdateComputer(nIndex, computer);
		return FALSE;
	}

	nIndex = m_ComputerArray.Add(computer);
	
	m_UpdateIndexArray.Add(nIndex);

	//为了提高效率，不想每次都做更新，等第一手动调用以后才做自动调用
	if (m_ComputerKeyMap.size() > 0)
	{
		MakeComputerKeyMap();
	}

	return TRUE;
}
//更新特定计算机监控信息
BOOL CComputerList::UpdateComputer(INT_PTR nIndex, CComputerInfo& computer)
{
	if (IsInvaildIndex(nIndex))
	{
		return FALSE;
	}
	
	computer.SetUpdateState(CComputerInfo::EUS_CHANGE);
	computer.SetPCClassID(m_ComputerArray.GetAt(nIndex).GetPCClassID());
	m_ComputerArray.SetAt(nIndex, computer);
		
	m_UpdateIndexArray.Add(nIndex);

	return TRUE;
}
//按终端号和IP查找
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
//按用户号查找
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

	for (INT_PTR i = 0; i < m_ComputerArray.GetCount(); i++)
	{
		if (m_ComputerArray.GetAt(i).HasUser())//只统计生日，黑户，VIP，挂机，余额不多，在线状态
		{
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
